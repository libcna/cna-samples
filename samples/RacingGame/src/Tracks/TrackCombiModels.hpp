// SPDX-License-Identifier: MS-PL

#pragma once

#include <utility>

#include "Microsoft/Xna/Framework/Matrix.hpp"
#include "Serialization/XmlSerializationAdapters.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Collections/Generic/List.hpp"
#include "System/Xml/Serialization/XmlSerializer.hpp"

namespace RacingGame::Landscapes
{
    class Landscape;
}

namespace RacingGame::Tracks
{
    using SharpRuntime::String;

    /**
     * @brief Loads one authored combination of landscape objects.
     */
    class TrackCombiModels
    {
    public:
        /** @brief Directory containing combination model files. */
        static constexpr const char* Directory = "Content";
        /** @brief Extension used by combination model files. */
        static constexpr const char* Extension = "CombiModel";

        /**
         * @brief One model and its local transform in a combination.
         */
        class CombiObject
        {
        public:
            /** @brief Content model name. */
            String modelName;
            /** @brief Local model transform. */
            Microsoft::Xna::Framework::Matrix matrix;

            /** @brief Constructs an empty object for XML deserialization. */
            CombiObject() = default;

            /**
             * @brief Constructs a combination object.
             *
             * @param setModelName Content model name.
             * @param setMatrix Local model transform.
             */
            CombiObject(String setModelName,
                        Microsoft::Xna::Framework::Matrix setMatrix)
                : modelName(std::move(setModelName)), matrix(setMatrix)
            {
            }

            SHARP_XML_SERIALIZABLE(
                CombiObject, "CombiObject",
                SHARP_XML_M(CombiObject, modelName),
                SHARP_XML_M(CombiObject, matrix))
        };

        /**
         * @brief Loads a combination model by content filename.
         *
         * @param filename Filename without the `.CombiModel` suffix.
         */
        explicit TrackCombiModels(const String& filename);

        /**
         * @brief Gets the combination name extracted from the filename.
         *
         * @return Combination name.
         */
        [[nodiscard]] const String& getNameProperty() const;

        /**
         * @brief Gets the placement size assigned by the original sample.
         *
         * @return Combination placement size.
         */
        [[nodiscard]] float getSizeProperty() const;

        /**
         * @brief Adds every model transformed by a parent matrix to a landscape.
         *
         * @param landscape Destination landscape.
         * @param parentMatrix Parent transform applied after each local transform.
         */
        void AddAllModels(
            Landscapes::Landscape& landscape,
            Microsoft::Xna::Framework::Matrix parentMatrix) const;

    private:
        using CombiObjectList =
            System::Collections::Generic::List<CombiObject>;

        CombiObjectList objects;
        String name;
        float size = 10.0f;
    };
}
