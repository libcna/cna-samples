# Racing Game Milestone 1 harness

This is the permanent, non-game diagnostic target required by `plan_racing.md` Milestone 1. It is
not the Racing port and deliberately contains no translated gameplay, screen, content-loader or
asset code. The eventual product target will be added separately after the harness gates the
pinned Linux `OPENGL33` environment.

The harness exercises a real `Game` loop and SDL/OpenGL window on an isolated Xvfb display. Its
qualification script proves:

- `Initialize` / `LoadContent` / `Update` / `Draw` / `UnloadContent` and graphics-device events;
- deterministic clear, backbuffer readback, multiple successful presents and a retained capture;
- resize, fullscreen round-trip, viewport and presentation state;
- real X11 events reaching CNA's public `Keyboard` and `Mouse` snapshots;
- `RenderTarget2D` bind/clear/unbind/readback;
- independent upload/readback of all six `TextureCube` faces;
- byte-exact upload/readback and GPU consumption of the original Racing
  `Position + TextureCoordinate + Normal + Tangent` 44-byte vertex layout;
- Debug and ASan/UBSan execution on the exact `OPENGL33` renderer, with backend/profile evidence.

No host desktop window is opened: `scripts/qualify-milestone1.sh` always runs the executable under
its own Xvfb server and injects input only into that virtual display.
