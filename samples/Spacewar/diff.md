<!-- SPDX-License-Identifier: MS-PL -->

# SAMPLE-014 — Spacewar: required C++ serialization declaration

The original `Settings.Save` and `Settings.Load` pass the `Settings` type to
`System.Xml.Serialization.XmlSerializer`. On .NET, the serializer discovers the public fields of
`Settings`, its three nested value types, the XNA vectors and the `Keys` enum through reflection.
C++ has no equivalent runtime field enumeration, so the port declares each serializable member
once with `SHARP_XML_SERIALIZABLE` in `src/Settings.hpp`. The reusable XML traversal, parsing and
writing remain in sharp-runtime's `XmlSerializer`; the vector and complete `Keys` registrations
are shared CNA adapters, not Spacewar-specific readers.

This is a language-required declaration, not a behavior change or an owner-approved feature.
`src/Settings.cpp` calls `XmlSerializer` on `File::OpenRead`/`File::Create` streams just as the
original does. It contains no XML element traversal, value parsing, enum switch or handwritten
document writer. The original `content\` media path is retained byte-for-byte and handled by
CNA's existing content and XACT path normalization.

The original C# array fields (`Ships`, `Weapons`, `ShipLights`) are represented by C++ vectors so
`XmlSerializer` can assign the complete array from a document, including a changed element count.
This preserves the XML wire form and the C# load semantics: an absent array field keeps its
original defaults, while a present array replaces them with the document's elements.
