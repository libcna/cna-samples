# CustomModelClass — the one line the original does not have

Everything in `src/` is a direct C++ translation of `CustomModel.cs` and
`CustomModelSampleGame.cs` except one constructor call:

```cpp
CNAEXT CustomModelContentReaderRegistrationEXT::RegisterEXT();
```

## Why it exists

The sample's `CustomModelProcessor` emits a root `ReflectiveReader<CustomModel>`, a
`ListReader<CustomModel.ModelPart>` and a nested `ReflectiveReader<ModelPart>`. XNA discovers the
two serialized field lists by reflecting over the game assembly at load time. C++ has no runtime
field reflection, so the port declares those same lists once and CNA reads the unchanged XNA XNB.

The declaration includes `ModelPart.Effect` as a shared resource, exactly matching its original
`[ContentSerializer(SharedResource = true)]` attribute. CNA therefore assigns the two shared
`BasicEffect` instances only after the root and shared-resource table have been read, as XNA does.

## Why it is not a workaround

The call disappears if C++ gains the reflection XNA uses. It does not change the data, bypass the
content system or add behavior: `Content.Load<CustomModel>("tank")` still consumes the exact three
files produced by the sample's own content processor. The C++ representation uses
`std::shared_ptr<CustomModel>` because the original is a C# reference type and because its delayed
shared-resource fixups require stable object identity.
