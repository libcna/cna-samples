// SPDX-License-Identifier: MS-PL

using System;
using System.Collections.Generic;
using System.Globalization;
using System.IO;
using Microsoft.Xna.Framework;
using Microsoft.Xna.Framework.Graphics;

namespace RacingEffectOracle
{
    internal struct TangentVertex : IVertexType
    {
        internal Vector3 Position;
        internal Vector2 TextureCoordinate;
        internal Vector3 Normal;
        internal Vector3 Tangent;

        internal static readonly VertexDeclaration Declaration = new VertexDeclaration(
            new VertexElement(0, VertexElementFormat.Vector3, VertexElementUsage.Position, 0),
            new VertexElement(12, VertexElementFormat.Vector2, VertexElementUsage.TextureCoordinate, 0),
            new VertexElement(20, VertexElementFormat.Vector3, VertexElementUsage.Normal, 0),
            new VertexElement(32, VertexElementFormat.Vector3, VertexElementUsage.Tangent, 0));

        internal TangentVertex(Vector3 position, Vector2 textureCoordinate)
        {
            Position = position;
            TextureCoordinate = textureCoordinate;
            Normal = Vector3.Forward;
            Tangent = Vector3.Right;
        }

        VertexDeclaration IVertexType.VertexDeclaration { get { return Declaration; } }
    }

    internal sealed class OracleGame : Game
    {
        private const int Size = 32;
#if FNA_ORACLE
        private const string OracleFormat = "racing-fna-effect-oracle-v1";
        private const string OutputPrefix = "fna";
#else
        private const string OracleFormat = "racing-xna4-effect-oracle-v1";
        private const string OutputPrefix = "xna";
#endif
        private readonly GraphicsDeviceManager graphics;
        private readonly string outputDirectory;
        private readonly List<string> report = new List<string>();
        private bool completed;
        private int failures;

        internal OracleGame(string outputDirectory)
        {
            this.outputDirectory = outputDirectory;
            graphics = new GraphicsDeviceManager(this);
            graphics.GraphicsProfile = GraphicsProfile.HiDef;
            graphics.PreferredBackBufferWidth = Size;
            graphics.PreferredBackBufferHeight = Size;
            graphics.SynchronizeWithVerticalRetrace = false;
            Content.RootDirectory = "Content";
            IsFixedTimeStep = false;
        }

        private void Check(bool condition, string message)
        {
            report.Add((condition ? "PASS " : "FAIL ") + message);
            if (!condition) ++failures;
        }

        private static string Names(EffectTechniqueCollection techniques)
        {
            var names = new List<string>();
            foreach (EffectTechnique technique in techniques) names.Add(technique.Name);
            return String.Join(",", names.ToArray());
        }

        private static string PassNames(EffectTechnique technique)
        {
            var names = new List<string>();
            foreach (EffectPass pass in technique.Passes) names.Add(pass.Name);
            return String.Join(",", names.ToArray());
        }

        private Effect LoadOracleEffect(string assetName)
        {
#if FNA_ORACLE
            string effectRoot = Environment.GetEnvironmentVariable("RACING_FNA_EFFECT_ROOT");
            if (String.IsNullOrEmpty(effectRoot))
                throw new InvalidOperationException("RACING_FNA_EFFECT_ROOT is required for the FNA oracle");
            string filename = Path.GetFileName(assetName) + ".efb";
            return new Effect(GraphicsDevice, File.ReadAllBytes(Path.Combine(effectRoot, filename)));
#else
            return Content.Load<Effect>(assetName);
#endif
        }

        private static void SetNormalParameters(Effect effect, Texture2D diffuse, Texture2D normal)
        {
            effect.Parameters["world"].SetValue(Matrix.Identity);
            effect.Parameters["viewProj"].SetValue(Matrix.Identity);
            effect.Parameters["viewInverse"].SetValue(Matrix.CreateTranslation(0.0f, 0.0f, 2.0f));
            effect.Parameters["lightDir"].SetValue(Vector3.Forward);
            effect.Parameters["ambientColor"].SetValue(new Vector4(0.1f, 0.1f, 0.1f, 1.0f));
            effect.Parameters["diffuseColor"].SetValue(new Vector4(0.6f, 0.6f, 0.6f, 1.0f));
            effect.Parameters["specularColor"].SetValue(new Vector4(0.2f, 0.2f, 0.2f, 1.0f));
            effect.Parameters["shininess"].SetValue(4.0f);
            effect.Parameters["UseAlpha"].SetValue(false);
            effect.Parameters["diffuseTexture"].SetValue(diffuse);
            effect.Parameters["normalTexture"].SetValue(normal);
            effect.CurrentTechnique = effect.Techniques["Specular20"];
        }

        private Color[] DrawNormalEffect(Effect effect, VertexBuffer quad)
        {
            using (var target = new RenderTarget2D(GraphicsDevice, Size, Size, false,
                SurfaceFormat.Color, DepthFormat.None, 0, RenderTargetUsage.PreserveContents))
            {
                GraphicsDevice.SetRenderTarget(target);
                GraphicsDevice.Clear(Color.Black);
                GraphicsDevice.BlendState = BlendState.Opaque;
                GraphicsDevice.DepthStencilState = DepthStencilState.None;
                GraphicsDevice.RasterizerState = RasterizerState.CullNone;
                GraphicsDevice.SetVertexBuffer(quad);
                effect.CurrentTechnique.Passes[0].Apply();
                GraphicsDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);
                GraphicsDevice.SetRenderTarget(null);
                var pixels = new Color[Size * Size];
                target.GetData(pixels);
                return pixels;
            }
        }

        private static int DifferentPixelCount(Color[] left, Color[] right)
        {
            int count = 0;
            for (int i = 0; i < left.Length; ++i)
                if (left[i].PackedValue != right[i].PackedValue) ++count;
            return count;
        }

        private static int LitCount(Color[] pixels)
        {
            int count = 0;
            foreach (Color pixel in pixels)
                if (pixel.R != 0 || pixel.G != 0 || pixel.B != 0) ++count;
            return count;
        }

        private void RunNormalMapping(Effect normalEffect)
        {
            Check(normalEffect.Techniques["Specular20"] != null, "NormalMapping exposes Specular20");
            Check(normalEffect.Techniques["Specular20"].Passes.Count == 1, "Specular20 has one pass");
            Check(normalEffect.Parameters["diffuseTexture"] != null, "NormalMapping exposes diffuseTexture");
            Check(normalEffect.Parameters["normalTexture"] != null, "NormalMapping exposes normalTexture");
            Check(Math.Abs(normalEffect.Parameters["shininess"].GetValueSingle() - 16.0f) < 0.0001f,
                "NormalMapping preserves authored shininess default");

            using (var diffuse = new Texture2D(GraphicsDevice, 2, 2, false, SurfaceFormat.Color))
            using (var normal = new Texture2D(GraphicsDevice, 2, 2, false, SurfaceFormat.Color))
            using (var quad = new VertexBuffer(GraphicsDevice, TangentVertex.Declaration, 4, BufferUsage.WriteOnly))
            using (Effect clone = normalEffect.Clone())
            {
                diffuse.SetData(new[] { new Color(120, 60, 30, 255), new Color(120, 60, 30, 255),
                    new Color(120, 60, 30, 255), new Color(120, 60, 30, 255) });
                normal.SetData(new[] { new Color(0, 128, 255, 128), new Color(0, 128, 255, 128),
                    new Color(0, 128, 255, 128), new Color(0, 128, 255, 128) });
                quad.SetData(new[] {
                    new TangentVertex(new Vector3(-1.0f, -1.0f, 0.5f), new Vector2(0.0f, 1.0f)),
                    new TangentVertex(new Vector3(-1.0f,  1.0f, 0.5f), new Vector2(0.0f, 0.0f)),
                    new TangentVertex(new Vector3( 1.0f, -1.0f, 0.5f), new Vector2(1.0f, 1.0f)),
                    new TangentVertex(new Vector3( 1.0f,  1.0f, 0.5f), new Vector2(1.0f, 0.0f)) });

                SetNormalParameters(normalEffect, diffuse, normal);
                SetNormalParameters(clone, diffuse, normal);
                Color[] originalPixels = DrawNormalEffect(normalEffect, quad);
                Color[] equalClonePixels = DrawNormalEffect(clone, quad);
                Check(DifferentPixelCount(originalPixels, equalClonePixels) == 0,
                    "equal Effect clone parameters produce identical pixels");

                clone.Parameters["ambientColor"].SetValue(new Vector4(0.7f, 0.1f, 0.1f, 1.0f));
                Check(Math.Abs(normalEffect.Parameters["ambientColor"].GetValueVector4().X - 0.1f) < 0.0001f,
                    "mutating clone leaves original parameter storage isolated");
                Color[] changedClonePixels = DrawNormalEffect(clone, quad);
                int changed = DifferentPixelCount(originalPixels, changedClonePixels);
                Check(LitCount(originalPixels) == Size * Size, "Specular20 renders the complete normal-mapped quad");
                Check(changed == Size * Size, "clone parameter change affects every rendered quad pixel");
                Color center = originalPixels[(Size / 2) * Size + Size / 2];
                report.Add(String.Format(CultureInfo.InvariantCulture,
                    "NORMAL center={0},{1},{2},{3} lit={4} cloneChanged={5}",
                    center.R, center.G, center.B, center.A, LitCount(originalPixels), changed));
                WritePpm(Path.Combine(outputDirectory, OutputPrefix + "-normal.ppm"), Size, Size, originalPixels);
                WritePpm(Path.Combine(outputDirectory, OutputPrefix + "-normal-clone.ppm"), Size, Size, changedClonePixels);
            }
        }

        private void RunBlur(Effect blurEffect)
        {
            EffectTechnique technique = blurEffect.Techniques["ScreenAdvancedBlur20"];
            Check(technique != null, "PostScreenShadowBlur exposes ScreenAdvancedBlur20");
            Check(technique.Passes.Count == 2, "ScreenAdvancedBlur20 has exactly two passes");
            Check(PassNames(technique) == "AdvancedBlurHorizontal,AdvancedBlurVertical",
                "ScreenAdvancedBlur20 preserves authored pass order");
            Check(blurEffect.Parameters["windowSize"] != null && blurEffect.Parameters["sceneMap"] != null &&
                  blurEffect.Parameters["blurMap"] != null, "PostScreenShadowBlur exposes all runtime parameters");

            var sourcePixels = new Color[Size * Size];
            for (int y = 14; y <= 17; ++y)
                for (int x = 14; x <= 17; ++x) sourcePixels[y * Size + x] = Color.White;

            using (var source = new Texture2D(GraphicsDevice, Size, Size, false, SurfaceFormat.Color))
            using (var horizontal = new RenderTarget2D(GraphicsDevice, Size, Size, false,
                SurfaceFormat.Color, DepthFormat.None, 0, RenderTargetUsage.PreserveContents))
            using (var vertical = new RenderTarget2D(GraphicsDevice, Size, Size, false,
                SurfaceFormat.Color, DepthFormat.None, 0, RenderTargetUsage.PreserveContents))
            using (var quad = new VertexBuffer(GraphicsDevice, typeof(VertexPositionTexture), 4, BufferUsage.WriteOnly))
            {
                source.SetData(sourcePixels);
                quad.SetData(new[] {
                    new VertexPositionTexture(new Vector3(-1.0f, -1.0f, 0.5f), new Vector2(0.0f, 1.0f)),
                    new VertexPositionTexture(new Vector3(-1.0f,  1.0f, 0.5f), new Vector2(0.0f, 0.0f)),
                    new VertexPositionTexture(new Vector3( 1.0f, -1.0f, 0.5f), new Vector2(1.0f, 1.0f)),
                    new VertexPositionTexture(new Vector3( 1.0f,  1.0f, 0.5f), new Vector2(1.0f, 0.0f)) });

                blurEffect.CurrentTechnique = technique;
                blurEffect.Parameters["windowSize"].SetValue(new Vector2(Size, Size));
                blurEffect.Parameters["sceneMap"].SetValue(source);
                GraphicsDevice.BlendState = BlendState.Opaque;
                GraphicsDevice.DepthStencilState = DepthStencilState.None;
                GraphicsDevice.RasterizerState = RasterizerState.CullNone;
                GraphicsDevice.SetVertexBuffer(quad);

                GraphicsDevice.SetRenderTarget(horizontal);
                GraphicsDevice.Clear(Color.Black);
                technique.Passes[0].Apply();
                GraphicsDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);
                GraphicsDevice.SetRenderTarget(null);
                var horizontalPixels = new Color[Size * Size];
                horizontal.GetData(horizontalPixels);

                blurEffect.Parameters["blurMap"].SetValue(horizontal);
                GraphicsDevice.SetRenderTarget(vertical);
                GraphicsDevice.Clear(Color.Black);
                technique.Passes[1].Apply();
                GraphicsDevice.DrawPrimitives(PrimitiveType.TriangleStrip, 0, 2);
                GraphicsDevice.SetRenderTarget(null);
                var verticalPixels = new Color[Size * Size];
                vertical.GetData(verticalPixels);

                int horizontalRow = CountLitOnRow(horizontalPixels, Size / 2);
                int horizontalColumn = CountLitOnColumn(horizontalPixels, Size / 2);
                int verticalRow = CountLitOnRow(verticalPixels, Size / 2);
                int verticalColumn = CountLitOnColumn(verticalPixels, Size / 2);
                Check(horizontalRow > horizontalColumn, "first pass spreads the source horizontally");
                Check(verticalColumn > horizontalColumn, "second pass adds the authored vertical spread");
                Check(LitCount(verticalPixels) > LitCount(sourcePixels), "two-pass blur expands the lit footprint");
                report.Add(String.Format(CultureInfo.InvariantCulture,
                    "BLUR sourceLit={0} horizontalLit={1} verticalLit={2} horizontalRow={3} horizontalColumn={4} verticalRow={5} verticalColumn={6}",
                    LitCount(sourcePixels), LitCount(horizontalPixels), LitCount(verticalPixels),
                    horizontalRow, horizontalColumn, verticalRow, verticalColumn));
                WritePpm(Path.Combine(outputDirectory, OutputPrefix + "-blur-horizontal.ppm"), Size, Size, horizontalPixels);
                WritePpm(Path.Combine(outputDirectory, OutputPrefix + "-blur-vertical.ppm"), Size, Size, verticalPixels);
            }
        }

        private static int CountLitOnRow(Color[] pixels, int row)
        {
            int count = 0;
            for (int x = 0; x < Size; ++x) if (pixels[row * Size + x].R != 0) ++count;
            return count;
        }

        private static int CountLitOnColumn(Color[] pixels, int column)
        {
            int count = 0;
            for (int y = 0; y < Size; ++y) if (pixels[y * Size + column].R != 0) ++count;
            return count;
        }

        private static void WritePpm(string path, int width, int height, Color[] pixels)
        {
            using (var output = new BinaryWriter(File.Create(path)))
            {
                byte[] header = System.Text.Encoding.ASCII.GetBytes(
                    String.Format(CultureInfo.InvariantCulture, "P6\n{0} {1}\n255\n", width, height));
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
            report.Add("FORMAT " + OracleFormat);
            report.Add("FRAMEWORK " + typeof(Game).Assembly.FullName);
            try
            {
                Effect normalEffect = LoadOracleEffect("Shaders/NormalMapping");
                Effect blurEffect = LoadOracleEffect("Shaders/PostScreenShadowBlur");
                report.Add("NORMAL_TECHNIQUES " + Names(normalEffect.Techniques));
                report.Add("BLUR_TECHNIQUES " + Names(blurEffect.Techniques));
                RunNormalMapping(normalEffect);
                RunBlur(blurEffect);
            }
            catch (Exception exception)
            {
                ++failures;
                report.Add("FAIL exception=" + exception);
            }

            report.Add(failures == 0 ? "RESULT PASS" : "RESULT FAIL count=" + failures);
            File.WriteAllLines(Path.Combine(outputDirectory, OutputPrefix + "-effects.txt"), report.ToArray());
            Exit();
            base.Draw(gameTime);
        }

        internal int FailureCount { get { return failures; } }
    }

    internal static class Program
    {
        [STAThread]
        private static int Main(string[] args)
        {
            string executableDirectory = AppDomain.CurrentDomain.BaseDirectory;
            Directory.SetCurrentDirectory(executableDirectory);
            string outputDirectory = args.Length == 0
                ? Path.Combine(executableDirectory, "output")
                : args[0];
            using (var game = new OracleGame(outputDirectory))
            {
                game.Run();
                return game.FailureCount == 0 ? 0 : 1;
            }
        }
    }
}
