#pragma once

// RolePlayingGameDataContentReaders.hpp -- CNAEXT.
//
// XNA finds a ContentTypeReader by the assembly-qualified name the .xnb records, reflecting over
// RolePlayingGameDataWindows at load time. C++ has no reflection, so the game says once which
// reader answers to each of those names. The readers themselves are faithful translations of the
// nested *Reader classes the original data types declare; only the way they are found differs.
// See diff.md.

namespace RolePlayingGameData {

// Registers every content type reader this sample's .xnb corpus names. Idempotent: CNA ignores a
// repeat registration of the same key, exactly as FNA's own AddTypeCreator does.
void RegisterContentTypeReaders();

} // namespace RolePlayingGameData
