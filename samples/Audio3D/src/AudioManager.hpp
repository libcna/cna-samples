// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AudioManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include <memory>
#include <string>

#include "CNA/CNAHelper.hpp"
#include "IAudioEmitter.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioEmitter.hpp"
#include "Microsoft/Xna/Framework/Audio/AudioListener.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "SharpRuntime/SharpRuntimeHelper.hpp"
#include "System/Collections/Generic/Dictionary.hpp"
#include "System/Collections/Generic/List.hpp"

namespace Audio3D
{
    /** @brief Tracks active 3D sounds, updates their spatial settings, and disposes completed instances. */
    class AudioManager : public Microsoft::Xna::Framework::GameComponent
    {
    public:
        /**
         * @brief Constructs an audio manager for a game.
         * @param game Game that owns the component.
         */
        explicit AudioManager(Microsoft::Xna::Framework::Game& game);

        /**
         * @brief Gets the listener that describes the ear hearing 3D sounds.
         * @return A reference to the shared audio listener.
         */
        [[nodiscard]] Microsoft::Xna::Framework::Audio::AudioListener& getListenerProperty();

        /** @brief Sets the global 3D scale and loads every sound effect. */
        void Initialize() override;

        /**
         * @brief Updates active spatial sounds and disposes instances that have stopped.
         * @param gameTime Timing information for the current frame.
         */
        void Update(Microsoft::Xna::Framework::GameTime& gameTime) override;

        /**
         * @brief Creates and starts a sound attached to an emitter.
         * @param soundName Content name of the sound effect.
         * @param isLooped Whether the sound should loop.
         * @param emitter Entity whose spatial properties drive the sound.
         * @return The newly started sound instance.
         */
        Microsoft::Xna::Framework::Audio::SoundEffectInstance* Play3DSound(
            const SharpRuntime::String& soundName,
            bool isLooped,
            IAudioEmitter* emitter);

        /**
         * @brief Returns the fully qualified logical type name.
         * @return `Audio3D.AudioManager`.
         */
        CNAEXT [[nodiscard]] const std::string& GetTypeName() const override;

    protected:
        /**
         * @brief Disposes loaded sound effects before disposing the component.
         * @param disposing Whether managed-style resources should be disposed.
         */
        void Dispose(bool disposing) override;

    private:
        struct ActiveSound
        {
            std::unique_ptr<Microsoft::Xna::Framework::Audio::SoundEffectInstance> Instance;
            IAudioEmitter* Emitter = nullptr;
        };

        static const std::array<SharpRuntime::String, 4> soundNames;

        Microsoft::Xna::Framework::Audio::AudioListener listener;
        Microsoft::Xna::Framework::Audio::AudioEmitter emitter;
        System::Collections::Generic::Dictionary<
            SharpRuntime::String,
            std::shared_ptr<Microsoft::Xna::Framework::Audio::SoundEffect>> soundEffects;
        System::Collections::Generic::List<std::shared_ptr<ActiveSound>> activeSounds;

        void Apply3D(ActiveSound& activeSound);
    };
}
