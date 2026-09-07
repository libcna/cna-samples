#pragma once

// AudioManager.hpp -- C++ port of Yacht/Misc/AudioManager.cs.

#include <map>
#include <memory>
#include <optional>
#include <string>

#include "Microsoft/Xna/Framework/Audio/SoundEffect.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundEffectInstance.hpp"
#include "Microsoft/Xna/Framework/Audio/SoundState.hpp"
#include "Microsoft/Xna/Framework/Game.hpp"
#include "Microsoft/Xna/Framework/GameComponent.hpp"
#include "Microsoft/Xna/Framework/Media/MediaPlayer.hpp"
#include "Microsoft/Xna/Framework/Media/MediaState.hpp"
#include "Microsoft/Xna/Framework/Media/Song.hpp"
#include "System/Random.hpp"

namespace Yacht {

using Microsoft::Xna::Framework::Game;
using Microsoft::Xna::Framework::GameComponent;
using Microsoft::Xna::Framework::Audio::SoundEffect;
using Microsoft::Xna::Framework::Audio::SoundEffectInstance;
using Microsoft::Xna::Framework::Audio::SoundState;
using Microsoft::Xna::Framework::Media::MediaPlayer;
using Microsoft::Xna::Framework::Media::MediaState;
using Microsoft::Xna::Framework::Media::Song;

/**
 * @brief The game's one place for sound, and a game component so it shares the game's lifetime.
 *
 * Sounds are loaded once into instances and replayed from the bank rather than created per
 * play, which is what lets a die-selection tick fire on every tap without allocating.
 *
 * @note The music half is empty on purpose, not missing: the original's LoadMusic says "No
 * music for this game" and this game never calls PlayMusic. The methods are here because they
 * are part of the type the sample ships, and a port that quietly dropped them would be
 * describing a different class.
 */
class AudioManager : public GameComponent {
public:
    /**
     * @brief The single audio manager.
     *
     * @return The instance, or null before Initialize.
     */
    [[nodiscard]] static AudioManager* getInstanceProperty() { return audioManager_.get(); }

    /**
     * @brief Creates the audio manager and adds it to the game's components.
     *
     * @param game The game that owns it.
     */
    static void Initialize(Game& game)
    {
        audioManager_.reset(new AudioManager(game));
        game.getComponentsProperty().Add(audioManager_.get());
    }

    /**
     * @brief Loads a sound effect and files it under a short name.
     *
     * @param contentName The asset name under Sounds/.
     * @param alias       The name the game plays it by.
     */
    static void LoadSound(const std::string& contentName, const std::string& alias)
    {
        SoundEffect soundEffect =
            audioManager_->getGameProperty().getContentProperty().template Load<SoundEffect>(
                soundAssetLocation_ + contentName);
        SoundEffectInstance soundEffectInstance = soundEffect.CreateInstance();

        if (audioManager_->soundBank_.find(alias) == audioManager_->soundBank_.end()) {
            audioManager_->soundBank_.emplace(alias, std::move(soundEffectInstance));
        }
    }

    /**
     * @brief Loads a song and files it under a short name.
     *
     * @param contentName The asset name under Sounds/.
     * @param alias       The name the game plays it by.
     */
    static void LoadSong(const std::string& contentName, const std::string& alias)
    {
        Song song = audioManager_->getGameProperty().getContentProperty().template Load<Song>(
            soundAssetLocation_ + contentName);

        if (audioManager_->musicBank_.find(alias) == audioManager_->musicBank_.end()) {
            audioManager_->musicBank_.emplace(alias, std::move(song));
        }
    }

    /** @brief Loads every sound the game plays. */
    static void LoadSounds()
    {
        LoadSound("DiceRoll 1", "Roll1");
        LoadSound("DiceRoll 2", "Roll2");
        LoadSound("DiceRoll 3", "Roll3");
        LoadSound("DiceRoll 4", "Roll4");
        LoadSound("Pencil 1", "Pencil1");
        LoadSound("Pencil 2", "Pencil2");
        LoadSound("Pencil 3", "Pencil3");
        LoadSound("DiceSelection 1", "DieSelect1");
        LoadSound("DiceSelection 2", "DieSelect2");
        LoadSound("Score Select", "ScoreSelect");
        LoadSound("Turn Change 1", "TurnChange1");
        LoadSound("Turn Change 2", "TurnChange2");
        LoadSound("Winner", "Winner");
        LoadSound("Loss", "Loss");
    }

    /** @brief Loads the game's music. There is none. */
    static void LoadMusic()
    {
        // No music for this game
    }

    /**
     * @brief The sound filed under a name.
     *
     * @param soundName The name the sound was loaded under.
     * @return The sound, or null when nothing is filed under that name.
     */
    [[nodiscard]] SoundEffectInstance* operator[](const std::string& soundName) const
    {
        const auto found = audioManager_->soundBank_.find(soundName);
        return found == audioManager_->soundBank_.end() ? nullptr : &found->second;
    }

    /**
     * @brief Plays a sound.
     *
     * @param soundName The name the sound was loaded under.
     */
    static void PlaySound(const std::string& soundName)
    {
        // If the sound exists, start it
        const auto found = audioManager_->soundBank_.find(soundName);
        if (found != audioManager_->soundBank_.end()) {
            found->second.Play();
        }
    }

    /**
     * @brief Plays one of a numbered family of sounds, chosen at random.
     *
     * The dice roll has four recordings and the selection tick two; picking between them is
     * what keeps a repeated action from sounding mechanical.
     *
     * @param soundName The family's name, without the number.
     * @param maxNumber How many recordings the family has.
     */
    static void PlaySoundRandom(const std::string& soundName, int maxNumber)
    {
        PlaySound(soundName + std::to_string(audioManager_->random_.Next(1, maxNumber + 1)));
    }

    /**
     * @brief Plays a sound, looped or not.
     *
     * @param soundName The name the sound was loaded under.
     * @param isLooped  True to repeat it until stopped.
     */
    static void PlaySound(const std::string& soundName, bool isLooped)
    {
        // If the sound exists, start it
        const auto found = audioManager_->soundBank_.find(soundName);
        if (found != audioManager_->soundBank_.end()) {
            if (found->second.getIsLoopedProperty() != isLooped) {
                found->second.setIsLoopedProperty(isLooped);
            }
            found->second.Play();
        }
    }

    /**
     * @brief Plays a sound, looped or not, at a given volume.
     *
     * @param soundName The name the sound was loaded under.
     * @param isLooped  True to repeat it until stopped.
     * @param volume    The volume, from zero to one.
     */
    static void PlaySound(const std::string& soundName, bool isLooped, float volume)
    {
        // If the sound exists, start it
        const auto found = audioManager_->soundBank_.find(soundName);
        if (found != audioManager_->soundBank_.end()) {
            if (found->second.getIsLoopedProperty() != isLooped) {
                found->second.setIsLoopedProperty(isLooped);
            }
            found->second.setVolumeProperty(volume);
            found->second.Play();
        }
    }

    /**
     * @brief Stops a sound.
     *
     * @param soundName The name the sound was loaded under.
     */
    static void StopSound(const std::string& soundName)
    {
        // If the sound exists, stop it
        const auto found = audioManager_->soundBank_.find(soundName);
        if (found != audioManager_->soundBank_.end()) {
            found->second.Stop();
        }
    }

    /** @brief Stops every sound that is playing. */
    static void StopSounds()
    {
        for (auto& [name, sound] : audioManager_->soundBank_) {
            if (sound.getStateProperty() != SoundState::Stopped) {
                sound.Stop();
            }
        }
    }

    /**
     * @brief Pauses every playing sound, or resumes every paused one.
     *
     * @param resumeSounds True to resume, false to pause.
     */
    static void PauseResumeSounds(bool resumeSounds)
    {
        const SoundState state = resumeSounds ? SoundState::Paused : SoundState::Playing;

        for (auto& [name, sound] : audioManager_->soundBank_) {
            if (sound.getStateProperty() == state) {
                if (resumeSounds) {
                    sound.Resume();
                } else {
                    sound.Pause();
                }
            }
        }
    }

    /**
     * @brief Plays a song on repeat, stopping whatever was playing.
     *
     * @param musicSoundName The name the song was loaded under.
     */
    static void PlayMusic(const std::string& musicSoundName)
    {
        // If the music sound exists
        const auto found = audioManager_->musicBank_.find(musicSoundName);
        if (found != audioManager_->musicBank_.end()) {
            // Stop the old music sound
            if (MediaPlayer::getStateProperty() != MediaState::Stopped) {
                MediaPlayer::Stop();
            }

            MediaPlayer::setIsRepeatingProperty(true);
            MediaPlayer::Play(&found->second);
        }
    }

    /** @brief Stops the song that is playing. */
    static void StopMusic()
    {
        if (MediaPlayer::getStateProperty() != MediaState::Stopped) {
            MediaPlayer::Stop();
        }
    }

    /** @brief The type's name. @return "AudioManager". */
    [[nodiscard]] const std::string& GetTypeName() const override
    {
        static const std::string name = "AudioManager";
        return name;
    }

    /** @brief Releases the sound bank. */
    ~AudioManager() override;

private:
    explicit AudioManager(Game& game) : GameComponent(game) {}

    static inline const std::string soundAssetLocation_ = "Sounds/";

    // Declared here and defined after the class: a unique_ptr member of its own type needs the
    // type to be complete before the deleter is instantiated.
    static std::unique_ptr<AudioManager> audioManager_;

    // Audio Data
    std::map<std::string, SoundEffectInstance> soundBank_;
    std::map<std::string, Song> musicBank_;
    System::Random random_;
};

inline AudioManager::~AudioManager() = default;
inline std::unique_ptr<AudioManager> AudioManager::audioManager_;

} // namespace Yacht
