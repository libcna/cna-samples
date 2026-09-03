# Racing Game Android build

This project packages the unchanged CNA Racing Game port as an SDL3 Android
application using the `OPENGLES3` renderer. It does not contain converted or
replacement game assets. `stageRacingContent` copies the authentic XNA Game
Studio output from `RACING_CONTENT_ROOT` into the APK as `Content/...` and
fails if the canonical `Models/Car.xnb` is absent.

The `org.libsdl.app` Java sources and launcher images are byte-identical copies
from the pinned CNA SDL checkout (`cbe3fbe9f367340dcd924de29c225c9f4ffea1f5`).
Their zlib licence is retained in `SDL-LICENSE.txt`; `RacingGameActivity` is the
only game-specific Java class.

Build from the sample root with:

```text
scripts/build-android.sh
```

This default `debug` variant keeps native assertions and debugging information.
For representative device frame-time measurements, build an optimized but
debug-key-signed qualification APK with:

```text
RACING_ANDROID_VARIANT=benchmark scripts/build-android.sh
```

The benchmark APK is emitted as
`app/outputs/apk/benchmark/app-benchmark.apk`. It is installable for local
qualification but is not a production-signed release artifact.

The default ABI is `arm64-v8a`. Select an emulator build explicitly with:

```text
RACING_ANDROID_ABI=x86_64 \
RACING_ANDROID_BUILD_ROOT=/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master/cna-android-x86_64/gradle \
scripts/build-android.sh
```

Generated Gradle, native and staged-content outputs stay below
`/rv/tmp/samples/SAMPLE-152-XNA-4-Racing-Game-Kit-master` by default. Override
`RACING_ARTIFACT_ROOT`, `RACING_CONTENT_ROOT` or
`RACING_ANDROID_BUILD_ROOT` when reproducing the build elsewhere. The build
requires Android SDK 35, Build Tools 36.1.0, CMake 4.1.2 and NDK
29.0.14206865.

The manifest deliberately requests OpenGL ES 3 and landscape orientation. It
does not request network access. Authentic game content remains a local
qualification input until the release asset-rights gate is resolved.

Menus use direct touch through the existing pointer/click actions. During a race,
the safe-area-aware overlay provides analog steering, analog throttle and brake,
handbrake, camera and pause/back controls with stable multi-touch ownership. The
same provider preserves keyboard, genuine mouse and connected gamepad input. The
provider suppresses duplicate SDL mouse-driving values only while a real touch
owns the current or immediately preceding race frame, so one finger is not
consumed twice while genuine mouse use remains available. Accelerometer steering
is enabled by default in the Android product, calibrates the current device angle
on every race entry/resume, and falls back to touch if the CNA sensor cannot
start. Touching the steering pad temporarily overrides tilt.

These APKs are qualification artifacts, not a completed Android release. A
supported build still requires a complete touch-only race on representative
physical devices, suspend/resume and graphics-context recreation, XACT listening,
storage/gamepad checks, and measured GPU memory, load time and thermal behavior.
Do not replace the canonical XNB/XACT products with lower-quality assets merely to
reduce APK size; first measure residency and remove only proven unused packaging.
