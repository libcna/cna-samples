// SPDX-License-Identifier: MS-PL

#pragma once

#include <tuple>

#include "CNA/CNAHelper.hpp"
#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Microsoft/Xna/Framework/Vector3.hpp"
#include "System/Xml/Serialization/detail/XmlMember.hpp"

namespace Microsoft::Xna::Framework
{
    CNAEXT inline constexpr const char* SharpXmlRootName(const Vector3*)
    {
        return "Vector3";
    }

    CNAEXT inline constexpr auto SharpXmlMembers(const Vector3*)
    {
        using System::Xml::Serialization::detail::MakeMember;
        return std::make_tuple(MakeMember("X", &Vector3::X),
                               MakeMember("Y", &Vector3::Y),
                               MakeMember("Z", &Vector3::Z));
    }

    CNAEXT inline constexpr const char* SharpXmlRootName(const Matrix*)
    {
        return "Matrix";
    }

    CNAEXT inline constexpr auto SharpXmlMembers(const Matrix*)
    {
        using System::Xml::Serialization::detail::MakeMember;
        return std::make_tuple(
            MakeMember("M11", &Matrix::M11), MakeMember("M12", &Matrix::M12),
            MakeMember("M13", &Matrix::M13), MakeMember("M14", &Matrix::M14),
            MakeMember("M21", &Matrix::M21), MakeMember("M22", &Matrix::M22),
            MakeMember("M23", &Matrix::M23), MakeMember("M24", &Matrix::M24),
            MakeMember("M31", &Matrix::M31), MakeMember("M32", &Matrix::M32),
            MakeMember("M33", &Matrix::M33), MakeMember("M34", &Matrix::M34),
            MakeMember("M41", &Matrix::M41), MakeMember("M42", &Matrix::M42),
            MakeMember("M43", &Matrix::M43), MakeMember("M44", &Matrix::M44));
    }
}
