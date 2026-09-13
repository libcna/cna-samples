# SAMPLE-008 C++ and publication adaptations

There is no gameplay or rendering deviation from the selected original XNA Debug product.

## Conditional method calls

C#'s `[Conditional("DEBUG")]` removes each call to a marked method at the caller when `DEBUG` is
absent, even though the method itself still exists. C++ has no matching standard attribute. The
port therefore guards the same seven game call sites with `SHAPE_RENDERING_SAMPLE_DEBUG`, which the
sample target normally defines only for CMake Debug. A sample-specific name is required because a
raw `DEBUG` macro would collide with CNA's `LogLevel::DEBUG` token.

This produces the same observable pair verified from the original IL and CNA runtimes: Debug draws
the five colored shapes; Release clears to CornflowerBlue without invoking the renderer. It neither
adds a control nor bypasses a framework path.

## Static gallery build profile

The sample's useful demonstration is explicitly its Debug semantic configuration, while the public
gallery requires an optimized, symbol-free, non-threaded CMake Release bundle. The retained gallery
product therefore uses `CMAKE_BUILD_TYPE=Release` and defines only
`SHAPE_RENDERING_SAMPLE_DEBUG`, selecting the original Debug call sites without enabling framework
assertions or compiler debug information.

This is a build-profile mapping, not a sample workaround: the browser executes the same calls and
produces the exact six-color output as the unchanged XNA Debug product. The audit also builds and
runs ordinary CNA Release without that symbol, proving the original blank Release behavior remains
unchanged. The exact command is in the artifact root's `scripts/fresh-build-web-site.sh`.
