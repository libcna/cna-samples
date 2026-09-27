# SAMPLE-080 — Owner-requested desktop mouse input

The original Windows Phone game reads raw `TouchPanel` contacts and has no
mouse input. On 2026-09-27 the owner requested that mouse-to-touch emulation be
retained for touch samples. This port opts into CNA's general, default-off
bridge with one `CNAEXT TouchPanel::setMouseTouchEmulationEnabledEXT(true)` call
in the game constructor. The game's virtual-stick logic still reads only
`TouchPanel`; it has no second input implementation.

The left mouse button supplies one touch contact. A drag in the left half can
move the player, and a drag in the right half can aim and fire. Operating both
sticks simultaneously still needs two real touch contacts. The opt-in does
not change the original Phone touch path or CNA's default behavior for other
games.
