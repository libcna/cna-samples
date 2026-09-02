// SPDX-License-Identifier: MS-PL

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;
using RacingGame.Graphics;
using RacingGame.Tracks;
using RacingTrackOracle;

namespace RacingStaticSceneOracle
{
    internal sealed class GpuMesh : IDisposable
    {
        internal readonly VertexBuffer VertexBuffer;
        internal readonly IndexBuffer IndexBuffer;
        internal readonly int VertexCount;
        internal readonly int PrimitiveCount;

        internal GpuMesh(GraphicsDevice device, StaticSceneMesh source)
        {
            VertexCount = source.Vertices.Length;
            PrimitiveCount = source.Indices.Length / 3;
            if (VertexCount == 0 || source.Indices.Length == 0) return;
            VertexBuffer = new VertexBuffer(device, TangentVertex.VertexDeclaration,
                VertexCount, BufferUsage.WriteOnly);
            VertexBuffer.SetData(source.Vertices);
            IndexBuffer = new IndexBuffer(device, IndexElementSize.ThirtyTwoBits,
                source.Indices.Length, BufferUsage.WriteOnly);
            IndexBuffer.SetData(source.Indices);
        }

        public void Dispose()
        {
            if (IndexBuffer != null) IndexBuffer.Dispose();
            if (VertexBuffer != null) VertexBuffer.Dispose();
        }
    }

    internal sealed class OracleGame : Game
    {
        private const int Width = 320;
        private const int Height = 180;
        private readonly GraphicsDeviceManager graphics;
        private readonly string contentRoot;
        private readonly string effectRoot;
        private readonly string outputDirectory;
        private readonly List<string> report = new List<string>();
        private readonly List<GpuMesh> meshes = new List<GpuMesh>();
        private StaticSceneGeometry scene;
        private GpuMesh landscapeMesh;
        private GpuMesh roadMesh;
        private GpuMesh roadBackMesh;
        private GpuMesh tunnelMesh;
        private GpuMesh leftGuardMesh;
        private GpuMesh rightGuardMesh;
        private GpuMesh columnsMesh;
        private Effect normalEffect;
        private Effect landscapeEffect;
        private Effect skyEffect;
        private Model skyCubeModel;
        private TextureCube skyCubeTexture;
        private Texture2D landscapeDiffuse;
        private Texture2D landscapeNormal;
        private Texture2D landscapeDetail;
        private Texture2D roadDiffuse;
        private Texture2D roadNormal;
        private Texture2D roadBackDiffuse;
        private Texture2D roadBackNormal;
        private Texture2D tunnelDiffuse;
        private Texture2D tunnelNormal;
        private Texture2D guardDiffuse;
        private Texture2D guardNormal;
        private Texture2D columnDiffuse;
        private Texture2D columnNormal;
        private bool completed;
        private int failures;

        internal OracleGame(string contentRoot, string effectRoot, string outputDirectory)
        {
            this.contentRoot = contentRoot;
            this.effectRoot = effectRoot;
            this.outputDirectory = outputDirectory;
            graphics = new GraphicsDeviceManager(this);
            graphics.GraphicsProfile = GraphicsProfile.HiDef;
            graphics.PreferredBackBufferWidth = Width;
            graphics.PreferredBackBufferHeight = Height;
            graphics.SynchronizeWithVerticalRetrace = false;
            Content.RootDirectory = "Content";
            IsFixedTimeStep = false;
        }

        private void Check(bool condition, string message)
        {
            report.Add((condition ? "PASS " : "FAIL ") + message);
            if (!condition) ++failures;
        }

        private GpuMesh Upload(StaticSceneMesh source)
        {
            var mesh = new GpuMesh(GraphicsDevice, source);
            meshes.Add(mesh);
            return mesh;
        }

        private Effect LoadBehaviorEffect(string name)
        {
            return new Effect(GraphicsDevice,
                File.ReadAllBytes(Path.Combine(effectRoot, name + ".efb")));
        }

        protected override void LoadContent()
        {
            Directory.SetCurrentDirectory(Directory.GetParent(contentRoot).FullName);
            scene = RacingTrackOracle.Program.BuildStaticSceneGeometry(
                contentRoot, "TrackBeginner");
            landscapeMesh = Upload(scene.LandscapeMesh);
            roadMesh = Upload(scene.Road);
            roadBackMesh = Upload(scene.RoadBack);
            tunnelMesh = Upload(scene.Tunnel);
            leftGuardMesh = Upload(scene.LeftGuard);
            rightGuardMesh = Upload(scene.RightGuard);
            columnsMesh = Upload(scene.Columns);

            normalEffect = LoadBehaviorEffect("NormalMapping");
            landscapeEffect = LoadBehaviorEffect("LandscapeNormalMapping");
            skyEffect = LoadBehaviorEffect("PreScreenSkyCubeMapping");
            skyCubeModel = Content.Load<Model>("Models/Cube");
            skyCubeTexture = Content.Load<TextureCube>("Textures/SkyCubeMap");
            landscapeDiffuse = Content.Load<Texture2D>("Textures/Landscape");
            landscapeNormal = Content.Load<Texture2D>("Textures/LandscapeNormal");
            landscapeDetail = Content.Load<Texture2D>("Textures/LandscapeDetail");
            roadDiffuse = Content.Load<Texture2D>("Textures/Road");
            roadNormal = Content.Load<Texture2D>("Textures/RoadNormal");
            roadBackDiffuse = Content.Load<Texture2D>("Textures/RoadBack");
            roadBackNormal = Content.Load<Texture2D>("Textures/RoadBackNormal");
            tunnelDiffuse = Content.Load<Texture2D>("Textures/RoadTunnel");
            tunnelNormal = Content.Load<Texture2D>("Textures/RoadTunnelNormal");
            guardDiffuse = Content.Load<Texture2D>("Textures/Leitplanke");
            guardNormal = Content.Load<Texture2D>("Textures/LeitplankeNormal");
            columnDiffuse = Content.Load<Texture2D>("Textures/RoadCement");
            columnNormal = Content.Load<Texture2D>("Textures/RoadCementNormal");
            base.LoadContent();
        }

        private static void SetCommonParameters(
            Effect effect, Matrix view, Matrix projection, TextureCube reflection)
        {
            Matrix viewProjection = view * projection;
            if (effect.Parameters["world"] != null)
                effect.Parameters["world"].SetValue(Matrix.Identity);
            if (effect.Parameters["worldViewProj"] != null)
                effect.Parameters["worldViewProj"].SetValue(viewProjection);
            if (effect.Parameters["viewProj"] != null)
                effect.Parameters["viewProj"].SetValue(viewProjection);
            if (effect.Parameters["viewInverse"] != null)
                effect.Parameters["viewInverse"].SetValue(Matrix.Invert(view));
            if (effect.Parameters["lightDir"] != null)
                effect.Parameters["lightDir"].SetValue(Vector3.Normalize(
                    new Vector3(8500.0f, -7250.0f, 15000.0f)));
            if (effect.Parameters["reflectionCubeTexture"] != null)
                effect.Parameters["reflectionCubeTexture"].SetValue(reflection);
        }

        private static void SetMaterialParameters(
            Effect effect, Texture2D diffuse, Texture2D normal,
            Color ambient, Color diffuseColor, Color specular)
        {
            if (effect.Parameters["ambientColor"] != null)
                effect.Parameters["ambientColor"].SetValue(ambient.ToVector4());
            if (effect.Parameters["diffuseColor"] != null)
                effect.Parameters["diffuseColor"].SetValue(diffuseColor.ToVector4());
            if (effect.Parameters["specularColor"] != null)
                effect.Parameters["specularColor"].SetValue(specular.ToVector4());
            if (effect.Parameters["shininess"] != null)
                effect.Parameters["shininess"].SetValue(24.0f);
            if (effect.Parameters["UseAlpha"] != null)
                effect.Parameters["UseAlpha"].SetValue(false);
            if (effect.Parameters["diffuseTexture"] != null)
                effect.Parameters["diffuseTexture"].SetValue(diffuse);
            if (effect.Parameters["normalTexture"] != null)
                effect.Parameters["normalTexture"].SetValue(normal);
        }

        private void DrawMesh(GpuMesh mesh, Effect effect, string techniqueName)
        {
            if (mesh.VertexBuffer == null || mesh.IndexBuffer == null) return;
            EffectTechnique technique = effect.Techniques[techniqueName];
            if (technique == null)
                throw new InvalidDataException("Missing Racing technique " + techniqueName);
            effect.CurrentTechnique = technique;
            GraphicsDevice.SetVertexBuffer(mesh.VertexBuffer);
            GraphicsDevice.Indices = mesh.IndexBuffer;
            foreach (EffectPass pass in technique.Passes)
            {
                pass.Apply();
                GraphicsDevice.DrawIndexedPrimitives(PrimitiveType.TriangleList,
                    0, 0, mesh.VertexCount, 0, mesh.PrimitiveCount);
            }
        }

        private void DrawSky(Matrix view, Matrix projection)
        {
            skyEffect.Parameters["view"].SetValue(view);
            skyEffect.Parameters["projection"].SetValue(projection);
            skyEffect.Parameters["ambientColor"].SetValue(
                new Color(232, 232, 232).ToVector4());
            skyEffect.Parameters["diffuseTexture"].SetValue(skyCubeTexture);
            skyEffect.CurrentTechnique = skyEffect.Techniques["SkyCubeMap"];
            GraphicsDevice.DepthStencilState = DepthStencilState.None;
            GraphicsDevice.RasterizerState = RasterizerState.CullNone;
            GraphicsDevice.BlendState = BlendState.Opaque;
            foreach (ModelMesh mesh in skyCubeModel.Meshes)
            {
                foreach (ModelMeshPart part in mesh.MeshParts)
                {
                    GraphicsDevice.SetVertexBuffer(part.VertexBuffer);
                    GraphicsDevice.Indices = part.IndexBuffer;
                    foreach (EffectPass pass in skyEffect.CurrentTechnique.Passes)
                    {
                        pass.Apply();
                        GraphicsDevice.DrawIndexedPrimitives(
                            PrimitiveType.TriangleList, part.VertexOffset, 0,
                            part.NumVertices, part.StartIndex, part.PrimitiveCount);
                    }
                }
            }
        }

        private void DrawScene(Matrix view, Matrix projection)
        {
            DrawSky(view, projection);
            GraphicsDevice.BlendState = BlendState.Opaque;
            GraphicsDevice.DepthStencilState = DepthStencilState.Default;
            GraphicsDevice.RasterizerState = RasterizerState.CullCounterClockwise;

            SetCommonParameters(landscapeEffect, view, projection, skyCubeTexture);
            SetMaterialParameters(landscapeEffect, landscapeDiffuse, landscapeNormal,
                new Color(88, 88, 88), new Color(234, 234, 234),
                new Color(33, 33, 33));
            landscapeEffect.Parameters["detailTexture"].SetValue(landscapeDetail);
            DrawMesh(landscapeMesh, landscapeEffect, "DiffuseWithDetail20");

            SetCommonParameters(normalEffect, view, projection, skyCubeTexture);
            SetMaterialParameters(normalEffect, roadDiffuse, roadNormal,
                new Color(40, 40, 40), new Color(210, 210, 210), Color.White);
            DrawMesh(roadMesh, normalEffect, "SpecularRoad20");
            SetMaterialParameters(normalEffect, roadBackDiffuse, roadBackNormal,
                new Color(40, 40, 40), new Color(210, 210, 210), Color.White);
            DrawMesh(roadBackMesh, normalEffect, "SpecularRoad20");

            GraphicsDevice.RasterizerState = RasterizerState.CullNone;
            SetMaterialParameters(normalEffect, tunnelDiffuse, tunnelNormal,
                new Color(182, 182, 182), new Color(80, 80, 80),
                new Color(64, 64, 64));
            DrawMesh(tunnelMesh, normalEffect, "Diffuse20");
            GraphicsDevice.RasterizerState = RasterizerState.CullCounterClockwise;

            SetMaterialParameters(normalEffect, guardDiffuse, guardNormal,
                new Color(72, 72, 72), new Color(182, 182, 182),
                new Color(225, 225, 225));
            DrawMesh(leftGuardMesh, normalEffect, "Specular20");
            DrawMesh(rightGuardMesh, normalEffect, "Specular20");
            SetMaterialParameters(normalEffect, columnDiffuse, columnNormal,
                new Color(40, 40, 40), new Color(210, 210, 210), Color.White);
            DrawMesh(columnsMesh, normalEffect, "Specular20");
            GraphicsDevice.SetVertexBuffer(null);
            GraphicsDevice.Indices = null;
        }

        private static void WritePpm(string path, Color[] pixels)
        {
            using (var output = new BinaryWriter(File.Create(path)))
            {
                byte[] header = System.Text.Encoding.ASCII.GetBytes(
                    String.Format(CultureInfo.InvariantCulture,
                        "P6\n{0} {1}\n255\n", Width, Height));
                output.Write(header);
                foreach (Color pixel in pixels)
                {
                    output.Write(pixel.R);
                    output.Write(pixel.G);
                    output.Write(pixel.B);
                }
            }
        }

        protected override void Draw(GameTime gameTime)
        {
            if (completed) return;
            completed = true;
            Directory.CreateDirectory(outputDirectory);
            report.Add("FORMAT racing-fna-static-scene-v1");
            report.Add("FRAMEWORK " + typeof(Game).Assembly.FullName);
            try
            {
                TrackVertex start = scene.Track.Points[0];
                Vector3 camera = start.pos - start.dir * 42.0f +
                    start.up * 18.0f + start.right * 8.0f;
                Vector3 target = start.pos + start.dir * 38.0f + start.up * 2.0f;
                Matrix view = Matrix.CreateLookAt(camera, target, start.up);
                Matrix projection = Matrix.CreatePerspectiveFieldOfView(
                    0.78539816f, Width / (float)Height, 0.5f, 4000.0f);
                GraphicsDevice.Viewport = new Viewport(0, 0, Width, Height);
                GraphicsDevice.Clear(new Color(71, 112, 156));
                DrawScene(view, projection);
                var pixels = new Color[Width * Height];
                GraphicsDevice.GetBackBufferData(pixels);
                int minLuma = 255 * 3;
                int maxLuma = 0;
                foreach (Color pixel in pixels)
                {
                    int luma = pixel.R + pixel.G + pixel.B;
                    minLuma = Math.Min(minLuma, luma);
                    maxLuma = Math.Max(maxLuma, luma);
                }
                Check(maxLuma - minLuma >= 24,
                    "static scene produces a meaningful luminance range");
                Check(scene.LeftHolders.Length == 224 &&
                    scene.RightHolders.Length == 231 &&
                    scene.ColumnPositions.Length == 101,
                    "Beginner representative-object placements remain authentic");
                report.Add(String.Format(CultureInfo.InvariantCulture,
                    "SCENE width={0} height={1} lumaRange={2} roadVertices={3} " +
                    "leftGuardVertices={4} rightGuardVertices={5} columns={6}",
                    Width, Height, maxLuma - minLuma, scene.Road.Vertices.Length,
                    scene.LeftGuard.Vertices.Length, scene.RightGuard.Vertices.Length,
                    scene.ColumnPositions.Length));
                WritePpm(Path.Combine(outputDirectory, "fna-static-scene.ppm"), pixels);
            }
            catch (Exception exception)
            {
                ++failures;
                report.Add("FAIL exception=" + exception);
            }
            report.Add(failures == 0 ? "RESULT PASS" : "RESULT FAIL count=" + failures);
            File.WriteAllLines(Path.Combine(outputDirectory, "fna-static-scene.txt"),
                report.ToArray());
            Exit();
            base.Draw(gameTime);
        }

        protected override void UnloadContent()
        {
            foreach (GpuMesh mesh in meshes) mesh.Dispose();
            if (normalEffect != null) normalEffect.Dispose();
            if (landscapeEffect != null) landscapeEffect.Dispose();
            if (skyEffect != null) skyEffect.Dispose();
            base.UnloadContent();
        }

        internal int FailureCount { get { return failures; } }
    }

    internal static class Program
    {
        [STAThread]
        private static int Main(string[] args)
        {
            if (args.Length != 3)
            {
                Console.Error.WriteLine(
                    "usage: StaticSceneOracle <Content dir> <FNA effect dir> <output dir>");
                return 2;
            }
            string contentRoot = Path.GetFullPath(args[0]);
            string effectRoot = Path.GetFullPath(args[1]);
            string outputDirectory = Path.GetFullPath(args[2]);
            using (var game = new OracleGame(contentRoot, effectRoot, outputDirectory))
            {
                game.Run();
                return game.FailureCount == 0 ? 0 : 1;
            }
        }
    }
}
