// SPDX-License-Identifier: MS-PL

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameWindow.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/BufferUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/CubeMapFace.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsAdapter.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsDevice.hpp"
#include "Microsoft/Xna/Framework/Graphics/GraphicsProfile.hpp"
#include "Microsoft/Xna/Framework/Graphics/IndexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/RasterizerState.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTarget2D.hpp"
#include "Microsoft/Xna/Framework/Graphics/RenderTargetUsage.hpp"
#include "Microsoft/Xna/Framework/Graphics/ShaderEffect.hpp"
#include "Microsoft/Xna/Framework/Graphics/SurfaceFormat.hpp"
#include "Microsoft/Xna/Framework/Graphics/TextureCube.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexBuffer.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexDeclaration.hpp"
#include "Microsoft/Xna/Framework/Graphics/VertexElement.hpp"
#include "Microsoft/Xna/Framework/GraphicsDeviceManager.hpp"
#include "Microsoft/Xna/Framework/Input/ButtonState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "Microsoft/Xna/Framework/Input/Keyboard.hpp"
#include "Microsoft/Xna/Framework/Input/Keys.hpp"
#include "Microsoft/Xna/Framework/Input/Mouse.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace Microsoft::Xna::Framework;
using namespace Microsoft::Xna::Framework::Graphics;
using namespace Microsoft::Xna::Framework::Input;

namespace {
constexpr int kInitialWidth = 256;
constexpr int kInitialHeight = 144;
constexpr int kCaptureWidth = 320;
constexpr int kCaptureHeight = 180;

struct HarnessOptions {
  std::filesystem::path executablePath;
  std::optional<std::filesystem::path> capturePath;
  bool requireInput = false;
};

#pragma pack(push, 1)
// This is the exact byte order and stride of RacingGame.Graphics.TangentVertex:
// Position, TextureCoordinate, Normal, Tangent.
struct ProbeTangentVertex {
  float px;
  float py;
  float pz;
  float u;
  float v;
  float nx;
  float ny;
  float nz;
  float tx;
  float ty;
  float tz;
};
#pragma pack(pop)

static_assert(sizeof(ProbeTangentVertex) == 44);
static_assert(offsetof(ProbeTangentVertex, u) == 12);
static_assert(offsetof(ProbeTangentVertex, nx) == 20);
static_assert(offsetof(ProbeTangentVertex, tx) == 32);

constexpr std::string_view kVertexShader = R"(#version 330 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec3 aTangent;
out vec2 vTexCoord;
out vec3 vNormal;
out vec3 vTangent;
void main() {
    gl_Position = vec4(aPosition, 1.0);
    vTexCoord = aTexCoord;
    vNormal = aNormal;
    vTangent = aTangent;
}
)";

constexpr std::string_view kFragmentShader = R"(#version 330 core
in vec2 vTexCoord;
in vec3 vNormal;
in vec3 vTangent;
out vec4 FragColor;
void main() {
    FragColor = vec4(vTexCoord.x, vNormal.z, vTangent.x, 1.0);
}
)";

bool NearChannel(int actual, int expected, int tolerance = 6) {
  return actual >= expected - tolerance && actual <= expected + tolerance;
}

bool NearColor(const Color &actual, const Color &expected, int tolerance = 6) {
  return NearChannel(actual.getRProperty(), expected.getRProperty(),
                     tolerance) &&
         NearChannel(actual.getGProperty(), expected.getGProperty(),
                     tolerance) &&
         NearChannel(actual.getBProperty(), expected.getBProperty(), tolerance);
}

HarnessOptions ParseOptions(int argc, char **argv) {
  HarnessOptions options;
  options.executablePath = std::filesystem::absolute(argv[0]);

  for (int i = 1; i < argc; ++i) {
    const std::string_view argument(argv[i]);
    if (argument == "--require-input") {
      options.requireInput = true;
    } else if (argument.starts_with("--capture=")) {
      const std::string value(
          argument.substr(std::string_view("--capture=").size()));
      if (value.empty()) {
        throw std::invalid_argument("--capture requires a non-empty path");
      }
      options.capturePath = std::filesystem::absolute(value);
    } else {
      throw std::invalid_argument("unknown Racing harness argument: " +
                                  std::string(argument));
    }
  }

  return options;
}

class RacingGameHarness final : public Game {
public:
  explicit RacingGameHarness(HarnessOptions options)
      : options_(std::move(options)),
        graphics_(std::make_unique<GraphicsDeviceManager>(this)) {
    graphics_->setGraphicsProfileProperty(GraphicsProfile::HiDef);
    graphics_->setPreferredBackBufferWidthProperty(kInitialWidth);
    graphics_->setPreferredBackBufferHeightProperty(kInitialHeight);
    graphics_->setPreferredPresentationModeProperty(
        PresentationMode::NativeBackBuffer);
    graphics_->setSynchronizeWithVerticalRetraceProperty(false);

    getWindowProperty().setTitleProperty("Racing CNA Milestone 1 Harness");
    getWindowProperty().setAllowUserResizingProperty(true);
    setIsFixedTimeStepProperty(false);
    setIsMouseVisibleProperty(true);

    graphics_->DeviceCreated +=
        [this](System::Object *, const System::EventArgs &) {
          ++deviceCreatedCount_;
        };
    graphics_->DeviceResetting +=
        [this](System::Object *, const System::EventArgs &) {
          ++deviceResettingCount_;
        };
    graphics_->DeviceReset +=
        [this](System::Object *, const System::EventArgs &) {
          ++deviceResetCount_;
        };
    graphics_->DeviceDisposing +=
        [this](System::Object *, const System::EventArgs &) {
          ++deviceDisposingCount_;
        };
    getWindowProperty().ClientSizeChanged +=
        [this](System::Object *, const System::EventArgs &) {
          ++clientSizeChangedCount_;
        };
  }

  int FinalizeAfterDispose() {
    Check(initializeCount_ == 1, "Initialize ran exactly once");
    Check(loadContentCount_ == 1, "LoadContent ran exactly once");
    Check(unloadContentCount_ == 1,
          "UnloadContent ran exactly once during disposal");
    Check(deviceCreatedCount_ == 1,
          "GraphicsDeviceManager raised DeviceCreated once");
    Check(deviceDisposingCount_ == 1,
          "GraphicsDeviceManager raised DeviceDisposing once during disposal");
    Check(deviceResettingCount_ > 0 &&
              deviceResettingCount_ == deviceResetCount_,
          "resize/fullscreen changes raised paired DeviceResetting/DeviceReset "
          "events");
    Check(updateCount_ >= 3, "at least three Update calls completed");
    Check(drawCount_ >= 3, "at least three Draw/present cycles completed");

    std::printf(
        "[INFO] lifecycle initialize=%d load=%d update=%d draw=%d unload=%d "
        "created=%d resetting=%d reset=%d disposing=%d sizeChanged=%d\n",
        initializeCount_, loadContentCount_, updateCount_, drawCount_,
        unloadContentCount_, deviceCreatedCount_, deviceResettingCount_,
        deviceResetCount_, deviceDisposingCount_, clientSizeChangedCount_);
    std::printf("=== Racing M1: %d/%d PASS ===\n", passCount_,
                passCount_ + failCount_);
    std::fflush(stdout);
    return failCount_ == 0 ? 0 : 1;
  }

protected:
  void Initialize() override {
    ++initializeCount_;
    auto &device = getGraphicsDeviceProperty();

    std::printf("[INFO] executable=%s\n",
                options_.executablePath.string().c_str());
    std::printf("[INFO] cwd=%s\n",
                std::filesystem::current_path().string().c_str());
    std::printf("[INFO] capture=%s\n",
                options_.capturePath ? options_.capturePath->string().c_str()
                                     : "disabled");
    std::printf("[INFO] requireInput=%s\n",
                options_.requireInput ? "true" : "false");
    std::printf("[INFO] renderer=%.*s graphicsProfile=%s adapter=%s\n",
                static_cast<int>(device.GetGraphicsRendererName().size()),
                device.GetGraphicsRendererName().data(),
                device.getGraphicsProfileProperty() == GraphicsProfile::HiDef
                    ? "HiDef"
                    : "Reach",
                device.getAdapterProperty().getDescriptionProperty().c_str());
    std::printf(
        "[INFO] capabilities:\n%.*s\n",
        static_cast<int>(device.GetRendererCapabilityReportEXT().size()),
        device.GetRendererCapabilityReportEXT().data());

    Check(std::filesystem::exists(std::filesystem::current_path()),
          "working directory exists");
    Check(device.GetGraphicsRendererName() == "OPENGL33",
          "harness is running on the pinned OPENGL33 renderer");
    Check(device.getGraphicsProfileProperty() == GraphicsProfile::HiDef,
          "GraphicsProfile is HiDef");
    Check(!device.getAdapterProperty().getDescriptionProperty().empty(),
          "graphics adapter description is available");

    const auto &initial = device.getPresentationParametersProperty();
    Check(initial.getBackBufferWidthProperty() == kInitialWidth &&
              initial.getBackBufferHeightProperty() == kInitialHeight,
          "initial 256x144 backbuffer was applied");

    graphics_->setPreferredBackBufferWidthProperty(kCaptureWidth);
    graphics_->setPreferredBackBufferHeightProperty(kCaptureHeight);
    graphics_->ApplyChanges();
    const auto &resized = device.getPresentationParametersProperty();
    Check(resized.getBackBufferWidthProperty() == kCaptureWidth &&
              resized.getBackBufferHeightProperty() == kCaptureHeight,
          "resize applied a 320x180 backbuffer");
    Check(device.getViewportProperty().getWidthProperty() == kCaptureWidth &&
              device.getViewportProperty().getHeightProperty() ==
                  kCaptureHeight,
          "NativeBackBuffer viewport follows the resized backbuffer");

    inputDeadline_ =
        std::chrono::steady_clock::now() + std::chrono::seconds(12);
    Game::Initialize();
  }

  void LoadContent() override {
    ++loadContentCount_;
    layoutEffect_ = std::make_unique<ShaderEffect>(
        getGraphicsDeviceProperty(), std::string(kVertexShader),
        std::string(kFragmentShader));
    Check(layoutEffect_->IsEffectValid(),
          "OPENGL33 compiled the custom tangent-layout diagnostic shaders");
    if (!layoutEffect_->IsEffectValid()) {
      std::printf("[INFO] shader compile log: %s\n",
                  layoutEffect_->GetCompileErrorEXT().c_str());
    }
  }

  void UnloadContent() override {
    ++unloadContentCount_;
    layoutEffect_.reset();
  }

  void Update(GameTime &gameTime) override {
    ++updateCount_;

    const KeyboardState keyboard = Keyboard::GetState();
    const MouseState mouse = Mouse::GetState();
    const GamePadState gamePad = GamePad::GetState(PlayerIndex::One);

    sawRKey_ = sawRKey_ || keyboard.IsKeyDown(Keys::R);
    sawLeftMouse_ =
        sawLeftMouse_ || mouse.getLeftButtonProperty() == ButtonState::Pressed;
    if (!haveMouseBaseline_) {
      initialMouseX_ = mouse.getXProperty();
      initialMouseY_ = mouse.getYProperty();
      haveMouseBaseline_ = true;
    } else {
      sawMouseMove_ = sawMouseMove_ || mouse.getXProperty() != initialMouseX_ ||
                      mouse.getYProperty() != initialMouseY_;
    }

    if (!inputSnapshotLogged_) {
      inputSnapshotLogged_ = true;
      std::printf(
          "[INFO] input keyboardPressed=%zu mouse=(%d,%d) mouseWindow=%zu "
          "gamePadOneConnected=%s\n",
          keyboard.GetPressedKeys().size(), mouse.getXProperty(),
          mouse.getYProperty(), Mouse::getWindowHandleProperty(),
          gamePad.getIsConnectedProperty() ? "true" : "false");
      Check(Mouse::getWindowHandleProperty() != 0,
            "mouse polling is associated with the Game window");
    }

    Game::Update(gameTime);
  }

  void Draw(const GameTime &) override {
    ++drawCount_;
    auto &device = getGraphicsDeviceProperty();

    if (drawCount_ == 1) {
      RunGraphicsProbes(device);
      return;
    }

    if (drawCount_ == 2) {
      Check(true, "the frame after the first present was reached");
      RunFullscreenProbe(device);
      device.Clear(Color(21, 41, 61, 255));
      return;
    }

    if (!secondPresentObserved_) {
      secondPresentObserved_ = true;
      Check(true, "the frame after the second present was reached");
    }

    device.Clear(Color(9, 19, 29, 255));

    const bool inputComplete =
        !options_.requireInput || (sawRKey_ && sawLeftMouse_ && sawMouseMove_);
    const bool inputTimedOut =
        std::chrono::steady_clock::now() >= inputDeadline_;
    if (inputComplete || inputTimedOut) {
      if (options_.requireInput) {
        Check(sawRKey_,
              "real X11/SDL keyboard input reached Keyboard.GetState");
        Check(sawLeftMouse_,
              "real X11/SDL mouse button input reached Mouse.GetState");
        Check(sawMouseMove_,
              "real X11/SDL mouse motion reached Mouse.GetState");
      }
      Exit();
    }
  }

private:
  void Check(bool condition, const char *label) {
    std::printf("[%s] %s\n", condition ? "PASS" : "FAIL", label);
    if (condition) {
      ++passCount_;
    } else {
      ++failCount_;
    }
    std::fflush(stdout);
  }

  void RunGraphicsProbes(GraphicsDevice &device) {
    const Color firstClear(37, 83, 149, 255);
    device.Clear(firstClear);
    Color clearPixel(0, 0, 0, 0);
    const Rectangle centre(kCaptureWidth / 2, kCaptureHeight / 2, 1, 1);
    device.GetBackBufferData(&centre, &clearPixel, 0, 1);
    Check(NearColor(clearPixel, firstClear),
          "deterministic backbuffer clear is visible through readback");

    const Color renderTargetColor(11, 67, 193, 255);
    RenderTarget2D target(device, 32, 32, false, SurfaceFormat::Color,
                          DepthFormat::Depth24, 0,
                          RenderTargetUsage::PreserveContents);
    device.SetRenderTarget(&target);
    device.Clear(renderTargetColor);
    device.SetRenderTarget(static_cast<RenderTarget2D *>(nullptr));
    Color targetPixel(0, 0, 0, 0);
    const Rectangle targetCentre(16, 16, 1, 1);
    target.GetData(0, &targetCentre, &targetPixel, 0, 1);
    Check(NearColor(targetPixel, renderTargetColor),
          "RenderTarget2D clear survives unbind and CPU readback");

    TextureCube cube(device, 2, false, SurfaceFormat::Color);
    const std::array<std::pair<CubeMapFace, Color>, 6> faces = {{
        {CubeMapFace::PositiveX, Color::Red},
        {CubeMapFace::NegativeX, Color::Lime},
        {CubeMapFace::PositiveY, Color::Blue},
        {CubeMapFace::NegativeY, Color::Yellow},
        {CubeMapFace::PositiveZ, Color::Cyan},
        {CubeMapFace::NegativeZ, Color::Magenta},
    }};
    bool cubeMatches = true;
    for (const auto &[face, color] : faces) {
      std::array<Color, 4> source = {color, color, color, color};
      cube.SetData(face, source.data(), static_cast<int>(source.size()));
    }
    for (const auto &[face, color] : faces) {
      std::array<Color, 4> result = {Color::Black, Color::Black, Color::Black,
                                     Color::Black};
      cube.GetData(face, result.data(), static_cast<int>(result.size()));
      for (const Color &pixel : result) {
        cubeMatches = cubeMatches && NearColor(pixel, color, 0);
      }
    }
    Check(cubeMatches, "all six TextureCube faces round-trip independently");

    const VertexDeclaration declaration(
        44, {
                VertexElement(0, VertexElementFormat::Vector3,
                              VertexElementUsage::Position, 0),
                VertexElement(12, VertexElementFormat::Vector2,
                              VertexElementUsage::TextureCoordinate, 0),
                VertexElement(20, VertexElementFormat::Vector3,
                              VertexElementUsage::Normal, 0),
                VertexElement(32, VertexElementFormat::Vector3,
                              VertexElementUsage::Tangent, 0),
            });
    const std::array<ProbeTangentVertex, 4> vertices = {{
        {-0.75f, 0.75f, 0.0f, 0.0f, 1.0f, 0.10f, 0.20f, 0.75f, 0.25f, 0.50f,
         0.80f},
        {-0.75f, -0.75f, 0.0f, 0.0f, 0.0f, 0.10f, 0.20f, 0.75f, 0.25f, 0.50f,
         0.80f},
        {0.75f, -0.75f, 0.0f, 1.0f, 0.0f, 0.10f, 0.20f, 0.75f, 0.25f, 0.50f,
         0.80f},
        {0.75f, 0.75f, 0.0f, 1.0f, 1.0f, 0.10f, 0.20f, 0.75f, 0.25f, 0.50f,
         0.80f},
    }};

    VertexBuffer vertexBuffer(device, declaration,
                              static_cast<int>(vertices.size()),
                              BufferUsage::None);
    vertexBuffer.SetData(vertices.data(), static_cast<int>(vertices.size()));
    std::array<ProbeTangentVertex, 4> readback{};
    vertexBuffer.GetDataRawEXT(0, readback.data(),
                               static_cast<int>(readback.size()),
                               sizeof(ProbeTangentVertex));
    Check(
        std::memcmp(vertices.data(), readback.data(), sizeof(vertices)) == 0,
        "exact 44-byte Racing tangent vertices survive buffer upload/readback");

    const std::array<std::uint16_t, 6> indices = {0, 1, 2, 0, 2, 3};
    IndexBuffer indexBuffer(device, static_cast<int>(indices.size()));
    indexBuffer.SetData(indices.data(), static_cast<int>(indices.size()));

    device.Clear(Color(16, 24, 32, 255));
    device.SetDepthTestEnabled(false);
    device.setBlendStateProperty(BlendState::Opaque);
    device.setRasterizerStateProperty(RasterizerState::CullNone);
    if (layoutEffect_ && layoutEffect_->IsEffectValid()) {
      layoutEffect_->Apply();
      device.SetVertexBuffer(&vertexBuffer);
      device.setIndicesProperty(&indexBuffer);
      device.DrawIndexedPrimitives(PrimitiveType::TriangleList, 0, 0,
                                   static_cast<int>(vertices.size()), 0, 2);
      device.SetVertexBuffer(nullptr);
      device.setIndicesProperty(nullptr);
    }

    Color layoutPixel(0, 0, 0, 0);
    device.GetBackBufferData(&centre, &layoutPixel, 0, 1);
    const Color expectedLayout(128, 191, 64, 255);
    Check(NearColor(layoutPixel, expectedLayout, 8),
          "GPU draw consumed UV, normal and tangent fields at their exact "
          "offsets");
    std::printf("[INFO] customLayoutCentre=(%d,%d,%d,%d)\n",
                layoutPixel.getRProperty(), layoutPixel.getGProperty(),
                layoutPixel.getBProperty(), layoutPixel.getAProperty());

    if (options_.capturePath) {
      WriteCapture(device, *options_.capturePath);
    }
  }

  void RunFullscreenProbe(GraphicsDevice &device) {
    graphics_->ToggleFullScreen();
    Check(graphics_->getIsFullScreenProperty() &&
              device.getPresentationParametersProperty()
                  .getIsFullScreenProperty(),
          "fullscreen toggle entered fullscreen in manager and presentation "
          "state");
    graphics_->ToggleFullScreen();
    Check(!graphics_->getIsFullScreenProperty() &&
              !device.getPresentationParametersProperty()
                   .getIsFullScreenProperty(),
          "second fullscreen toggle restored windowed presentation state");

    // Force a physical resize in both directions. Re-applying an unchanged
    // preference is intentionally a no-op, while a virtual display without a
    // window manager may retain its fullscreen drawable size until it receives
    // a real windowed resize request.
    graphics_->setPreferredBackBufferWidthProperty(kCaptureWidth - 1);
    graphics_->ApplyChanges();
    graphics_->setPreferredBackBufferWidthProperty(kCaptureWidth);
    graphics_->ApplyChanges();
    Check(device.getPresentationParametersProperty()
                      .getBackBufferWidthProperty() == kCaptureWidth &&
              device.getPresentationParametersProperty()
                      .getBackBufferHeightProperty() == kCaptureHeight,
          "windowed backbuffer dimensions were restored after fullscreen");
  }

  void WriteCapture(GraphicsDevice &device, const std::filesystem::path &path) {
    std::filesystem::create_directories(path.parent_path());
    std::vector<Color> pixels(
        static_cast<std::size_t>(kCaptureWidth * kCaptureHeight),
        Color::Transparent);
    device.GetBackBufferData(pixels.data(), static_cast<int>(pixels.size()));

    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    output << "P6\n" << kCaptureWidth << ' ' << kCaptureHeight << "\n255\n";
    for (const Color &pixel : pixels) {
      const char rgb[3] = {
          static_cast<char>(pixel.getRProperty()),
          static_cast<char>(pixel.getGProperty()),
          static_cast<char>(pixel.getBProperty()),
      };
      output.write(rgb, sizeof(rgb));
    }
    output.close();

    Check(
        output.good() && std::filesystem::exists(path) &&
            std::filesystem::file_size(path) >
                static_cast<std::uintmax_t>(kCaptureWidth * kCaptureHeight * 3),
        "deterministic backbuffer capture was written as a complete PPM");
  }

  HarnessOptions options_;
  std::unique_ptr<GraphicsDeviceManager> graphics_;
  std::unique_ptr<ShaderEffect> layoutEffect_;
  std::chrono::steady_clock::time_point inputDeadline_{};
  int passCount_ = 0;
  int failCount_ = 0;
  int initializeCount_ = 0;
  int loadContentCount_ = 0;
  int unloadContentCount_ = 0;
  int updateCount_ = 0;
  int drawCount_ = 0;
  int deviceCreatedCount_ = 0;
  int deviceResettingCount_ = 0;
  int deviceResetCount_ = 0;
  int deviceDisposingCount_ = 0;
  int clientSizeChangedCount_ = 0;
  int initialMouseX_ = 0;
  int initialMouseY_ = 0;
  bool haveMouseBaseline_ = false;
  bool inputSnapshotLogged_ = false;
  bool sawRKey_ = false;
  bool sawLeftMouse_ = false;
  bool sawMouseMove_ = false;
  bool secondPresentObserved_ = false;
};
} // namespace

int main(int argc, char **argv) {
  try {
    RacingGameHarness game(ParseOptions(argc, argv));
    game.Run();
    game.Dispose();
    return game.FinalizeAfterDispose();
  } catch (const std::exception &exception) {
    std::fprintf(stderr, "[FAIL] Racing M1 unhandled exception: %s\n",
                 exception.what());
    return 1;
  }
}
