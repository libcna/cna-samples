// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// AudioManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "AudioManager.hpp"

#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundState.hpp"

namespace Audio3D
{
    using Microsoft::Xna::Framework::Game;
    using Microsoft::Xna::Framework::GameComponent;
    using Microsoft::Xna::Framework::GameTime;
    using Microsoft::Xna::Framework::Audio::AudioListener;
    using Microsoft::Xna::Framework::Audio::SoundEffect;
    using Microsoft::Xna::Framework::Audio::SoundEffectInstance;
    using Microsoft::Xna::Framework::Audio::SoundState;
    using SharpRuntime::intcs;
    using SharpRuntime::String;

    const std::array<String, 4> AudioManager::soundNames = {
        "CatSound0",
        "CatSound1",
        "CatSound2",
        "DogSound",
    };

    AudioManager::AudioManager(Game& game)
        : GameComponent(game)
    {
    }

    AudioListener& AudioManager::getListenerProperty()
    {
        return listener;
    }

    void AudioManager::Initialize()
    {
        SoundEffect::setDistanceScaleProperty(2000.0f);
        SoundEffect::setDopplerScaleProperty(0.1f);

        for (const String& soundName : soundNames)
        {
            auto soundEffect = std::make_shared<SoundEffect>(
                getGameProperty().getContentProperty().Load<SoundEffect>(soundName));
            soundEffects.Add(soundName, soundEffect);
        }

        GameComponent::Initialize();
    }

    void AudioManager::Dispose(bool disposing)
    {
        try
        {
            if (disposing)
            {
                for (const auto& soundEffect : soundEffects.getValuesProperty())
                {
                    soundEffect->Dispose();
                }

                soundEffects.Clear();
            }
        }
        catch (...)
        {
            GameComponent::Dispose(disposing);
            throw;
        }

        GameComponent::Dispose(disposing);
    }

    void AudioManager::Update(GameTime& gameTime)
    {
        intcs index = 0;

        while (index < activeSounds.getCountProperty())
        {
            const auto activeSound = activeSounds.getItem(index);

            if (activeSound->Instance->getStateProperty() == SoundState::Stopped)
            {
                activeSound->Instance->Dispose();
                activeSounds.RemoveAt(index);
            }
            else
            {
                Apply3D(*activeSound);
                ++index;
            }
        }

        GameComponent::Update(gameTime);
    }

    SoundEffectInstance* AudioManager::Play3DSound(
        const String& soundName,
        bool isLooped,
        IAudioEmitter* sourceEmitter)
    {
        auto activeSound = std::make_shared<ActiveSound>();

        const auto& sounds = static_cast<const decltype(soundEffects)&>(soundEffects);
        activeSound->Instance = std::make_unique<SoundEffectInstance>(
            sounds[soundName]->CreateInstance());
        activeSound->Instance->setIsLoopedProperty(isLooped);
        activeSound->Emitter = sourceEmitter;

        Apply3D(*activeSound);
        activeSound->Instance->Play();

        SoundEffectInstance* instance = activeSound->Instance.get();
        activeSounds.Add(activeSound);
        return instance;
    }

    void AudioManager::Apply3D(ActiveSound& activeSound)
    {
        emitter.setPositionProperty(activeSound.Emitter->getPositionProperty());
        emitter.setForwardProperty(activeSound.Emitter->getForwardProperty());
        emitter.setUpProperty(activeSound.Emitter->getUpProperty());
        emitter.setVelocityProperty(activeSound.Emitter->getVelocityProperty());

        activeSound.Instance->Apply3D(listener, emitter);
    }

    const std::string& AudioManager::GetTypeName() const
    {
        static const std::string name = "Audio3D.AudioManager";
        return name;
    }
}
