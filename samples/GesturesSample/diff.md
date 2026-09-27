# SAMPLE-079 — Deliberate C++ and desktop-input differences

## Named-color palette initialization

Original `Sprite.cs` declares a `static readonly Color[] Colors` field containing
`Color.White`, `Color.Red`, `Color.Blue` and `Color.Green`. The port uses the same
four named CNA colors in that order. C++ initializes its function-local static
array on the first `Sprite` use, after framework static initialization has
finished. This preserves the palette values and progression while avoiding
unspecified initialization order between translation units. The earlier port's
numeric RGBA replacements are gone. This is C++ initialization mechanics, not
a second color path or a change to CNA color values.

## Owner-requested mouse input for a touch game

On 2026-09-27 the owner asked that desktop mouse-to-touch emulation be retained
for touch samples, specifically correcting SAMPLE-077. SAMPLE-079 follows that
preference with one `CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true)`
call in `Game1`'s constructor. CNA's general bridge remains off by default.
The game still reads only `TouchPanel`; Hold, Tap, DoubleTap, FreeDrag, Flick
and Pinch keep their original code and ordering. A mouse supplies one touch
contact, so two-contact Pinch still requires genuine touch input. This opt-in
adds desktop operability and does not alter original Phone touch events.
