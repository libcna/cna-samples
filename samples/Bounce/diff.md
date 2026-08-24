# Differences from the XNA 4.0 original

There are no known active port differences after the `SAMPLE-016` audit. See
`missing.md` for the reference limitation and complete verification evidence.

The previous port intentionally diverged from the original in several places.
These historical adaptations are now resolved:

- the persistent desktop tilt model was replaced with the exact per-frame
  Windows Phone emulator accelerometer behavior;
- orientation compensation and the original shake/floor update order were
  restored;
- the port again preserves the original shake-speed assignment, including its
  overwritten upper clamp;
- fullscreen and the 30 Hz target were restored;
- the custom 24-byte `VertexPositionNormal` type and named XNA colors were
  restored;
- the invented tilt indicator and runtime help overlay were removed;
- the sample-local DirectionalLight diffuse-color workaround was removed after
  correcting CNA's general Microsoft XNA 4.0 constructor defaults.

The original is a Windows Phone 7-only project. CNA maps the phone accelerometer
to a supported real sensor or to the original emulator keyboard branch, and maps
the phone fullscreen request to each target platform's faithful fullscreen mode.
Those are platform realizations of the original API behavior, not sample-local
gameplay substitutions.
