# SAMPLE-083 intentional translation seams

There are no known observable behavior or content differences from either original project
configuration. The following are C#-to-C++ representation choices, not sample workarounds.

## Shared Windows and Phone source

The original has two project files but only one shared source/content product. The C++ target uses
the Windows branch by default and retains each Phone-only statement under the same
`WINDOWS_PHONE` conditional. The audit's separate Phone compile supplies that definition; no
second copied game class or divergent asset tree is introduced.

## Object and collection representation

Nullable C# graphics/audio references are represented by `optional` and the owned `SpriteBatch`
by `unique_ptr`. The private `List<Snowflake>` is a `vector` because snowflakes have no identity
outside their enclosing game. C# accumulates colliding references in a temporary removal list;
the C++ loop erases the same encountered elements safely through its returned iterator. Collision
order, score, sound calls and surviving snowflake order are unchanged.

The two C# `Random` objects remain two independent `System::Random` instances. Explicit local
temporaries preserve C#'s defined left-to-right random-consumption order where a multi-argument
C++ construction would otherwise leave it unspecified.
