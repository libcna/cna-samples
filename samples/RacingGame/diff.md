# Racing Game intentional C++ mechanics

The original XNA game remains the behavioral authority. This file records only
language mechanics that the C# source cannot need.

- `TrackData` explicitly registers its serializable members and the XNA
  `Vector3`/`Matrix` value shapes with sharp-runtime's compile-time
  `XmlSerializer` customization points. .NET discovers the same public
  properties through reflection; reflection is deliberately unavailable in
  sharp-runtime. The registration changes no element name, order, value or
  wire representation.
- `TangentVertex` is a trivially-copyable 44-byte C++ struct with a static
  `VertexDeclaration`. It does not inherit CNA's polymorphic `IVertexType`,
  because that would insert a vtable pointer into the bytes uploaded to the GPU.
  Its four fields and offsets remain exactly the original
  `Position@0, TextureCoordinate@12, Normal@20, Tangent@32` layout.
