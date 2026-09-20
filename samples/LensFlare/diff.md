# LensFlare — approved product difference

On 2026-09-20 the owner expressly approved publishing SAMPLE-041 to the gallery
with a known OpenGL ES 3 / WebGL 2 limitation, deferring the substantial
general CNA fallback. This approval changes the **accepted output**, not the
sample's logic. No boolean-query multiplier, forced coverage, fabricated sun,
or other workaround was added to the port.

The terrain, its directional lighting and fog, camera controls and content
render. The sun glow and ten lens-flare sprites that appear in the original
XNA 4.0 product are effectively invisible in both mandated CNA renderers:
the XNA count query tallies covered fragments, whereas EasyGL must use a
boolean occlusion query on ES3/WebGL2, yielding `PixelCount=1` for about 9788
covered pixels. The faithful sample divides by its 10000-pixel query area,
reducing alpha to 0.0001. Fresh original XNA has 11,178 near-white pixels in
the measured sun region; fresh CNA native and browser captures have zero.

This exception does **not** mean visual parity, a working flare effect, or a
framework fix. The gallery displays a visible warning, and `missing.md`
specifies the deferred CNA-wide fallback and required tests. The browser
gate still reports `glowVisible=false` and accepts that only when explicitly
run with the owner-approved limitation expectation.
