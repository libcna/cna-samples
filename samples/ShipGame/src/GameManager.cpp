// SPDX-License-Identifier: MS-PL
//-----------------------------------------------------------------------------
// GameManager.cs
//
// Microsoft XNA Community Game Platform
// Copyright (C) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "GameManager.hpp"

#include <algorithm>
#include <cmath>
#include <string>

#include "Microsoft/Xna/Framework/Color.hpp"
#include "Microsoft/Xna/Framework/MathHelper.hpp"
#include "Microsoft/Xna/Framework/PlayerIndex.hpp"
#include "Microsoft/Xna/Framework/Rectangle.hpp"
#include "Microsoft/Xna/Framework/Graphics/BlendState.hpp"
#include "Microsoft/Xna/Framework/Graphics/DepthStencilState.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMesh.hpp"
#include "Microsoft/Xna/Framework/Graphics/ModelMeshPart.hpp"
#include "Microsoft/Xna/Framework/Graphics/PrimitiveType.hpp"
#include "Microsoft/Xna/Framework/Graphics/SamplerState.hpp"
#include "Microsoft/Xna/Framework/Input/GamePad.hpp"
#include "System/ArgumentNullException.hpp"

#include "BoxCollider/CollisionBox.hpp"
#include "BoxCollider/CollisionMesh.hpp"
#include "EntityList.hpp"
#include "FontManager.hpp"
#include "GameOptions.hpp"
#include "Graphics/AnimSprite.hpp"
#include "Graphics/AnimSpriteManager.hpp"
#include "Graphics/LightList.hpp"
#include "Graphics/ParticleManager.hpp"
#include "Graphics/ParticleSystem.hpp"
#include "Graphics/Powerup.hpp"
#include "Graphics/PowerupManager.hpp"
#include "Graphics/Projectile.hpp"
#include "Graphics/ProjectileManager.hpp"
#include "InputManager.hpp"
#include "PlayerShip.hpp"

namespace ShipGame {

using Microsoft::Xna::Framework::Color;
using Microsoft::Xna::Framework::MathHelper;
using Microsoft::Xna::Framework::PlayerIndex;
using Microsoft::Xna::Framework::Rectangle;
using Microsoft::Xna::Framework::Vector4;
using Microsoft::Xna::Framework::Audio::Cue;
using Microsoft::Xna::Framework::Audio::SoundBank;
using Microsoft::Xna::Framework::Content::ContentManager;
using Microsoft::Xna::Framework::Graphics::BlendState;
using Microsoft::Xna::Framework::Graphics::DepthStencilState;
using Microsoft::Xna::Framework::Graphics::Effect;
using Microsoft::Xna::Framework::Graphics::EffectParameter;
using Microsoft::Xna::Framework::Graphics::GraphicsDevice;
using Microsoft::Xna::Framework::Graphics::Model;
using Microsoft::Xna::Framework::Graphics::ModelMesh;
using Microsoft::Xna::Framework::Graphics::ModelMeshPart;
using Microsoft::Xna::Framework::Graphics::PrimitiveType;
using Microsoft::Xna::Framework::Graphics::SamplerState;
using Microsoft::Xna::Framework::Graphics::Texture2D;
using Microsoft::Xna::Framework::Input::GamePad;
using BoxCollider::CollisionBox;
using BoxCollider::CollisionMesh;

GameManager::GameManager(SoundBank* soundBank) {
    shipFile_.resize(GameOptions::MaxPlayers);
    players_.resize(GameOptions::MaxPlayers);
    vibrationTime_.resize(GameOptions::MaxPlayers);
    bones_.resize(GameOptions::MaxBonesPerModel);

    sound_ = soundBank;
    animatedSprite_ = std::make_unique<AnimSpriteManager>();
    projectile_ = std::make_unique<ProjectileManager>(this);
    particle_ = std::make_unique<ParticleManager>();
    powerup_ = std::make_unique<PowerupManager>(this);
}

GameManager::~GameManager() { Dispose(true); }

void GameManager::SetLevel(const std::string& levelFileName) { levelFile_ = levelFileName; }

void GameManager::SetShips(const std::string& shipPlayer1, const std::string& shipPlayer2,
                           std::uint32_t invertYAxis) {
    shipFile_[0] = shipPlayer1;
    shipFile_[1] = shipPlayer2;
    invertY_ = invertYAxis;
}

bool GameManager::GetInvertY(int player) const {
    return (invertY_ & (1u << player)) != 0;
}

std::string GameManager::GetPlayerShip(int player) const { return shipFile_[player]; }

int GameManager::getPlayerWinner() const {
    if (gameMode_ == GameMode::SinglePlayer) {
        return 0;
    } else {
        if (players_[0]->getScore() > players_[1]->getScore()) {
            return 0;
        } else {
            return 1;
        }
    }
}

void GameManager::LoadFiles(ContentManager& content) {
    std::string level = levelFile_ + "/" + levelFile_;

    // load level model
    levelColor_ = content.Load<Model>("levels/" + level);

    // load collision model
    Model collisionModel = content.Load<Model>("levels/" + level + "_collision");
    levelCollision_ =
        std::make_shared<CollisionMesh>(collisionModel, GameOptions::CollisionMeshSubdivisions);

    // load spawns and lights
    levelSpawns_ = EntityList::Load("content/levels/" + level + "_spawns.xml");
    levelLights_ = LightList::Load("content/levels/" + level + "_lights.xml");

    // load particle textures
    if (particleTextures_.empty()) {
        int i, j = (int)particleFiles_.size();
        particleTextures_.resize(j);
        for (i = 0; i < j; i++)
            particleTextures_[i] = content.Load<Texture2D>("particles/" + particleFiles_[i]);
    }

    // load animated sprite textures
    if (animatedSpriteTextures_.empty()) {
        int i, j = (int)animatedSpriteFiles_.size();
        animatedSpriteTextures_.resize(j);
        for (i = 0; i < j; i++)
            animatedSpriteTextures_[i] =
                content.Load<Texture2D>("explosions/" + animatedSpriteFiles_[i]);
    }

    // load projectile models
    if (projectileModels_.empty()) {
        int i, j = (int)projectileFiles_.size();
        projectileModels_.resize(j);
        for (i = 0; i < j; i++)
            projectileModels_[i] = content.Load<Model>("projectiles/" + projectileFiles_[i]);
    }

    // load powerup models
    if (powerupModels_.empty()) {
        int i, j = (int)powerupFiles_.size();
        powerupModels_.resize(j);
        for (i = 0; i < j; i++)
            powerupModels_[i] = content.Load<Model>("powerups/" + powerupFiles_[i]);
    }

    // cerate players
    for (int i = 0; i < GameOptions::MaxPlayers; i++)
        if (!shipFile_[i].empty()) {
            Model ShipModel = content.Load<Model>("ships/" + shipFile_[i]);

            std::shared_ptr<EntityList> ShipEnities =
                EntityList::Load("content/ships/" + shipFile_[i] + ".xml");

            players_[i] = std::make_shared<PlayerShip>(this, i, ShipModel, ShipEnities,
                                                       GameOptions::CollisionBoxRadius);
        } else
            players_[i] = nullptr;

    // create powerups
    std::shared_ptr<EntityList> powerups =
        EntityList::Load("content/levels/" + level + "_powerups.xml");

    for (const Entity& entity : powerups->Entities()) {
        if (entity.name == "energy") {
            AddPowerup(PowerupType::Energy, entity.transform);
        } else if (entity.name == "missile") {
            AddPowerup(PowerupType::Missile, entity.transform);
        }
    }

    // load hud textures
    if (gameMode_ == GameMode::SinglePlayer) {
        hudCrosshair_ = content.Load<Texture2D>("screens/hud_sp_crosshair");
        hudEnergy_ = content.Load<Texture2D>("screens/hud_sp_energy");
        hudMissile_ = content.Load<Texture2D>("screens/hud_sp_missile");
        hudScore_ = content.Load<Texture2D>("screens/hud_sp_score");
        hudBars_ = content.Load<Texture2D>("screens/hud_sp_bars");
    } else {
        hudCrosshair_ = content.Load<Texture2D>("screens/hud_mp_crosshair");
        hudEnergy_ = content.Load<Texture2D>("screens/hud_mp_energy");
        hudMissile_ = content.Load<Texture2D>("screens/hud_mp_missile");
        hudScore_ = content.Load<Texture2D>("screens/hud_mp_score");
        hudBars_ = content.Load<Texture2D>("screens/hud_mp_bars");
    }

    // load damage indicator texture
    damageTexture_ = content.Load<Texture2D>("screens/damage");
}

void GameManager::UnloadFiles() {
    // unload level
    levelColor_.reset();
    levelCollision_ = nullptr;
    levelSpawns_ = nullptr;
    levelLights_ = nullptr;

    // unload poasticles
    particleTextures_.clear();
    // unload animated sprites
    animatedSpriteTextures_.clear();
    // unload projectiles
    projectileModels_.clear();
    // unload powerups
    powerupModels_.clear();

    // unload players
    for (int i = 0; i < GameOptions::MaxPlayers; i++) {
        // must displose player so that it releases its effects
        if (players_[i] != nullptr)
            players_[i]->Dispose();
        players_[i] = nullptr;
    }

    // unload hud
    hudCrosshair_.reset();
    hudEnergy_.reset();
    hudMissile_.reset();
    hudScore_.reset();
    hudBars_.reset();

    // unload damage texture
    damageTexture_.reset();

    // unload powerups
    powerup_->Clear();
}

void GameManager::PlaySound(const std::string& soundName) { sound_->PlayCue(soundName); }

void GameManager::PlaySound3D(const std::string& soundName, Vector3 position) {
    // get distance from sound to closest player
    float minimumDistance = 1e10f;
    for (int i = 0; i < GameOptions::MaxPlayers; i++)
        if (players_[i] != nullptr && players_[i]->getIsAlive()) {
            float dist = (position - players_[i]->getPosition()).LengthSquared();
            if (dist < minimumDistance)
                minimumDistance = dist;
        }

    // create a new sound instance
    Cue* cue = sound_->GetCue(soundName);
    cueSounds_.emplace_back(cue);

    // set volume based on distance from closest player
    cue->SetVariable("Distance", (float)std::sqrt(minimumDistance));

    // play sound
    cue->Play();
}

void GameManager::SetVibration(int player, float duration) { vibrationTime_[player] = duration; }

void GameManager::AddDamageSplash(int attacker, float damage, Vector3 position, float radius) {
    // check all players
    for (int i = 0; i < GameOptions::MaxPlayers; i++)
        // if player is alive
        if (players_[i] != nullptr && players_[i]->getIsAlive()) {
            // get squared distance from player to splash center
            Vector3 vec = players_[i]->getPosition() - position;
            float len = vec.LengthSquared();
            // if player inside sphere
            if (len < radius * radius) {
                // get actual length
                len = (float)std::sqrt(len);

                // compute damage intensity (squared not linear inside sphere)
                float intensity = len / radius;
                intensity = 1.0f - intensity * intensity;

                // normalize vector used for pushing direction
                vec *= 1.0f / len;

                // apply damage and push player
                AddDamage(attacker, i, intensity * damage, vec);
            }
        }
}

void GameManager::AddDamage(int attacker, int defender, float damage, Vector3 pushDirection) {
    // push defender for taking the damage
    players_[defender]->AddImpulseForce(5000 * pushDirection * damage);

    // apply damage to defender
    players_[defender]->AddEnergy(-damage);

    // set vibration on the defender gamepad
    SetVibration(defender, -0.3f);

    // if defender dies
    if (players_[defender]->getIsAlive() == false) {
        // compute explosion position
        Matrix m = players_[defender]->getTransform();
        m.setTranslationProperty(m.getTranslationProperty() + 25 * m.getForwardProperty());

        // add ship explosion animated sprite
        AddAnimSprite(AnimSpriteType::Ship, m.getTranslationProperty(), 100, 0.0f, 20,
                      DrawMode::AdditiveAndGlow, defender);

        // add ship explosion particle system
        AddParticleSystem(ParticleSystemType::ShipExplode, m);

        // if suicide
        if (attacker == defender) {
            // attacker lose point
            players_[attacker]->setScore(std::max(0, players_[attacker]->getScore() - 1));
        } else {
            // attacker win point
            players_[attacker]->setScore(players_[attacker]->getScore() + 1);
        }

        // ship explosion adds splash damage
        AddDamageSplash(defender, 0.4f, m.getTranslationProperty(), 1000);

        // play explode sound
        PlaySound("ship_explode");
    }
}

void GameManager::ProcessInput(float elapsedTime, InputManager* input) {
    // process input for player 1
    players_[0]->ProcessInput(elapsedTime, input, 0);

    // if in multiplayer mode, process input for player 2
    if (gameMode_ == GameMode::MultiPlayer)
        players_[1]->ProcessInput(elapsedTime, input, 1);
}

void GameManager::Update(float elapsedTime) {
    // update player 1
    players_[0]->Update(elapsedTime, levelCollision_.get(), levelSpawns_.get());

    // if in multiplayer mode
    if (gameMode_ == GameMode::MultiPlayer) {
        // update player 2
        players_[1]->Update(elapsedTime, levelCollision_.get(), levelSpawns_.get());

        // if both players are alive
        if (players_[0]->getIsAlive() && players_[1]->getIsAlive()) {
            // test collision between players
            Vector3 position1 = players_[0]->getPosition();
            Vector3 position2 = players_[1]->getPosition();
            CollisionBox player1Box(position1 + players_[0]->box->min,
                                    position1 + players_[0]->box->max);
            CollisionBox player2Box(position2 + players_[1]->box->min,
                                    position2 + players_[1]->box->max);
            // if player boxes intersect
            if (player1Box.BoxIntersect(player2Box)) {
                // compute push direction
                Vector3 direction = Vector3::Normalize(position2 - position1);

                // push players in oposide directions
                direction *= GameOptions::ShipCollidePush;
                players_[0]->AddImpulseForce(-direction);
                players_[1]->AddImpulseForce(direction);

                // play ship collide sound
                PlaySound("ship_collide");
            }
        }
    }

    // update animated sprites
    animatedSprite_->Update(elapsedTime);

    // update animated projectiles
    projectile_->Update(elapsedTime);

    // update particle systems
    particle_->Update(elapsedTime);

    // update powerups
    powerup_->Update(elapsedTime);

    // delete any finished 3D sounds
    cueSoundsDelete_.clear();
    for (const auto& cue : cueSounds_)
        if (cue->getIsStoppedProperty())
            cueSoundsDelete_.push_back(cue.get());
    for (Cue* cue : cueSoundsDelete_) {
        cue->Dispose();
        cueSounds_.erase(std::find_if(cueSounds_.begin(), cueSounds_.end(),
                                      [cue](const std::unique_ptr<Cue>& c)
                                      { return c.get() == cue; }));
    }

    // if gamepad vibreate enabled
    if (GameOptions::UseGamepadVibrate) {
        // check vibration for each player
        for (int i = 0; i < GameOptions::MaxPlayers; i++) {
            float leftMotorAmount = 0;
            float rightMotorAmount = 0;

            // if left vibration
            if (vibrationTime_[i] > 0) {
                leftMotorAmount =
                    GameOptions::VibrationIntensity *
                    std::min(1.0f, vibrationTime_[i] / GameOptions::VibrationFadeout);
                vibrationTime_[i] = std::max(0.0f, vibrationTime_[i] - elapsedTime);
            } else
                // if right vibration
                if (vibrationTime_[i] < 0) {
                    rightMotorAmount =
                        GameOptions::VibrationIntensity *
                        std::min(1.0f, -vibrationTime_[i] / GameOptions::VibrationFadeout);
                    vibrationTime_[i] = std::min(0.0f, vibrationTime_[i] + elapsedTime);
                }

            // set vibration values
            GamePad::SetVibration((PlayerIndex)i, leftMotorAmount, rightMotorAmount);
        }
    }
}

void GameManager::Draw3D(GraphicsDevice* gd) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // clear background
    gd->Clear(Color::Black);

    // draw scene
    DrawScene(gd, RenderTechnique::NormalMapping);
}

void GameManager::DrawHud(FontManager* font, Rectangle rect, Vector3 bars, int barsLeft,
                          int barsWidth, bool crosshair) {
    Rectangle r(0, 0, 0, 0);

    // if crosshair enabled
    if (crosshair) {
        // draw crosshair hud texture
        r.X = rect.X + (rect.Width - hudCrosshair_->getWidthProperty()) / 2;
        r.Y = rect.Y + (rect.Height - hudCrosshair_->getHeightProperty()) / 2;
        r.Width = hudCrosshair_->getWidthProperty();
        r.Height = hudCrosshair_->getHeightProperty();
        font->DrawTexture(&hudCrosshair_.value(), r, Color::White, BlendState::AlphaBlend);
    }

    // draw score hud texture
    r.X = rect.X + (rect.Width - hudScore_->getWidthProperty()) / 2;
    r.Y = rect.Y;
    r.Width = hudScore_->getWidthProperty();
    r.Height = hudScore_->getHeightProperty();
    font->DrawTexture(&hudScore_.value(), r, Color::White, BlendState::AlphaBlend);

    // draw missile hud texture
    r.X = rect.X + rect.Width - hudMissile_->getWidthProperty();
    r.Y = rect.Y + rect.Height - hudMissile_->getHeightProperty();
    r.Width = hudMissile_->getWidthProperty();
    r.Height = hudMissile_->getHeightProperty();
    font->DrawTexture(&hudMissile_.value(), r, Color::White, BlendState::AlphaBlend);

    // draw energy hud texture
    r.X = rect.X;
    r.Y = rect.Y + rect.Height - hudEnergy_->getHeightProperty();
    r.Width = hudEnergy_->getWidthProperty();
    r.Height = hudEnergy_->getHeightProperty();
    font->DrawTexture(&hudEnergy_.value(), r, Color::White, BlendState::AlphaBlend);

    // get hud bars
    Rectangle s(0, 0, hudBars_->getWidthProperty(), hudBars_->getHeightProperty());

    // draw the energy bar
    r.Width = s.Width = barsLeft + (int)(barsWidth * bars.X);
    font->DrawTexture(&hudBars_.value(), r, s, Color::Red, BlendState::Additive);

    // draw the shield bar
    r.Width = s.Width = barsLeft + (int)(barsWidth * bars.Y);
    font->DrawTexture(&hudBars_.value(), r, s, Color::Green, BlendState::Additive);

    // draw the boost bar
    r.Width = s.Width = barsLeft + (int)(barsWidth * bars.Z);
    font->DrawTexture(&hudBars_.value(), r, s, Color::Blue, BlendState::Additive);
}

void GameManager::Draw2D(FontManager* font) {
    if (font == nullptr) {
        throw System::ArgumentNullException("font");
    }

    Rectangle rect = font->getScreenRectangle();

    // if in single player mode
    if (gameMode_ == GameMode::SinglePlayer) {
        if (players_[0]->getIsAlive()) {
            // draw hud
            DrawHud(font, rect, players_[0]->getBars(), 70, 120,
                    players_[0]->getCamera3rdPerson() == false);

            // draw missile count
            font->DrawText(FontType::ArialMedium, std::to_string(players_[0]->getMissileCount()),
                           Vector2((float)(rect.getRightProperty() - 138),
                                   (float)(rect.getBottomProperty() - 120)),
                           Color::LightCyan);
        }

        // draw damage indicator
        Color DamageColor = players_[0]->getDamageColor();
        if (DamageColor.getAProperty() > 0)
            font->DrawTexture(&damageTexture_.value(), rect, DamageColor, BlendState::AlphaBlend);
    } else {
        // multiplayer half horizontal screen
        rect.Width /= 2;

        // if player is alive
        if (players_[0]->getIsAlive()) {
            // draw hud
            DrawHud(font, rect, players_[0]->getBars(), 80, 100,
                    players_[0]->getCamera3rdPerson() == false);

            // draw missile count
            font->DrawText(FontType::ArialMedium, std::to_string(players_[0]->getMissileCount()),
                           Vector2((float)(rect.getRightProperty() - 138),
                                   (float)(rect.getBottomProperty() - 125)),
                           Color::LightCyan);
        }

        // draw damage indicator
        Color damageColor = players_[0]->getDamageColor();
        if (damageColor.getAProperty() > 0)
            font->DrawTexture(&damageTexture_.value(), rect, damageColor, BlendState::AlphaBlend);

        // second player on second horizontal half
        rect.X += rect.Width;

        // if player is alive
        if (players_[1]->getIsAlive()) {
            // draw hud
            DrawHud(font, rect, players_[1]->getBars(), 80, 100,
                    players_[1]->getCamera3rdPerson() == false);

            // draw missile count
            font->DrawText(FontType::ArialMedium, std::to_string(players_[1]->getMissileCount()),
                           Vector2((float)(rect.getRightProperty() - 138),
                                   (float)(rect.getBottomProperty() - 125)),
                           Color::LightCyan);
        }

        // draw damage indicator
        damageColor = players_[1]->getDamageColor();
        if (damageColor.getAProperty() > 0)
            font->DrawTexture(&damageTexture_.value(), rect, damageColor, BlendState::AlphaBlend);

        // draw score
        font->DrawText(FontType::ArialLarge, std::to_string(players_[0]->getScore()),
                       Vector2((float)(rect.Width / 2 - 20), 20), Color::LightCyan);
        font->DrawText(FontType::ArialLarge, std::to_string(players_[1]->getScore()),
                       Vector2((float)(rect.Width * 3 / 2 - 20), 20), Color::LightCyan);
    }
}

void GameManager::DrawScene(GraphicsDevice* gd, RenderTechnique technique) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    if (gameMode_ == GameMode::SinglePlayer) {
        // camera position and view projection matrix
        Vector3 cameraPosition = players_[0]->getCameraPosition();
        Matrix viewProjection = players_[0]->getViewMatrix() * projectionFull_;

        // draw the level geomery
        DrawModel(gd, &levelColor_.value(), technique, cameraPosition,
                  Matrix::getIdentityProperty(), viewProjection, levelLights_.get());

        // if in 3rd person mode draw player ship
        bool camera3rdPerson = players_[0]->getCamera3rdPerson();
        if (camera3rdPerson)
            players_[0]->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw projectiles
        projectile_->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw powerups
        powerup_->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw animated sprites
        animatedSprite_->Draw(gd, cameraPosition, players_[0]->getViewUp(), viewProjection, 0,
                              camera3rdPerson);

        // draw particle systems
        particle_->Draw(gd, viewProjection);
    } else {
        // set left viewport
        gd->setViewportProperty(viewportLeft_);

        // camera position and view projection matrix for player 1
        Vector3 cameraPosition = players_[0]->getCameraPosition();
        Matrix viewProjection = players_[0]->getViewMatrix() * projectionSplit_;

        // draw the level geomery
        DrawModel(gd, &levelColor_.value(), technique, cameraPosition,
                  Matrix::getIdentityProperty(), viewProjection, levelLights_.get());

        // draw player 2 ship
        players_[1]->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // if in 3rd person mode draw player 1 ship
        bool camera3rdPerson = players_[0]->getCamera3rdPerson();
        if (camera3rdPerson)
            players_[0]->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw projectiles
        projectile_->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw powerups
        powerup_->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw animated sprites
        animatedSprite_->Draw(gd, cameraPosition, players_[0]->getViewUp(), viewProjection, 0,
                              camera3rdPerson);

        // draw particle systems
        particle_->Draw(gd, viewProjection);

        // setup right viewport
        gd->setViewportProperty(viewportRight_);

        // camera position and view projection matrix for player 2
        cameraPosition = players_[1]->getCameraPosition();
        viewProjection = players_[1]->getViewMatrix() * projectionSplit_;

        // draw the level geomery
        DrawModel(gd, &levelColor_.value(), technique, cameraPosition,
                  Matrix::getIdentityProperty(), viewProjection, levelLights_.get());

        // draw player 1 ship
        players_[0]->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // if in 3rd person mode draw player 2 ship
        camera3rdPerson = players_[1]->getCamera3rdPerson();
        if (camera3rdPerson)
            players_[1]->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw projectiles
        projectile_->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw powerups
        powerup_->Draw(gd, technique, cameraPosition, viewProjection, levelLights_.get());

        // draw animated sprites
        animatedSprite_->Draw(gd, cameraPosition, players_[1]->getViewUp(), viewProjection, 1,
                              camera3rdPerson);

        // draw particle systems
        particle_->Draw(gd, viewProjection);
    }
}

void GameManager::LoadContent(GraphicsDevice* gd, ContentManager& content) {
    // load reflection cubemap texture
    // reflectCube = content.Load<TextureCube>("Reflect");

    // load content for animated sprite manager
    animatedSprite_->LoadContent(gd, content);

    // load content for particle system manager
    particle_->LoadContent(gd, content);

    // set up projection matrix for full and slpit screen
    float aspect = (float)gd->getViewportProperty().getWidthProperty() /
                   (float)gd->getViewportProperty().getHeightProperty();
    projectionFull_ =
        Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(60), aspect, 1.0f, 10000.0f);
    projectionSplit_ = Matrix::CreatePerspectiveFieldOfView(MathHelper::ToRadians(60),
                                                            aspect * 0.5f, 1.0f, 10000.0f);

    // viewport for split screen
    viewportLeft_ = gd->getViewportProperty();
    viewportLeft_.setWidthProperty(gd->getViewportProperty().getWidthProperty() / 2 - 1);
    viewportRight_ = viewportLeft_;
    viewportRight_.setXProperty(gd->getViewportProperty().getWidthProperty() / 2 + 1);
}

void GameManager::UnloadContent() {
    UnloadFiles();

    // unload content for animated sprite manager
    animatedSprite_->UnloadContent();

    // unload content for particle system manager
    particle_->UnloadContent();
}

int GameManager::GetPlayerAtPosition(Vector3 position) {
    for (int i = 0; i < GameOptions::MaxPlayers; i++)
        if (players_[i] != nullptr && players_[i]->getIsAlive())
            if (players_[i]->box->PointInside(position - players_[i]->getPosition()))
                return i;
    return -1;
}

PlayerShip* GameManager::GetPlayer(int playerId) { return players_[playerId].get(); }

std::shared_ptr<ParticleSystem> GameManager::AddParticleSystem(ParticleSystemType type,
                                                              Matrix transform) {
    std::shared_ptr<ParticleSystem> ps;

    switch (type) {
    case ParticleSystemType::ShipExplode:
        ps = std::make_shared<ParticleSystem>(
            ParticleSystemType::ShipExplode,
            200,                                // num particles
            0.0f,                               // emission angle (0 for omni)
            0.8f, 0.8f,                         // particle and total time
            20.0f, 50.0f,                       // min and max size
            600.0f, 1000.0f,                    // min and max vel
            Vector4(1.0f, 1.0f, 1.0f, 1.6f),    // start color
            Vector4(1.0f, 1.0f, 1.0f, 0.0f),    // end color
            &particleTextures_[(int)type].value(), // texture
            DrawMode::Additive,                 // draw mode
            transform);                         // transform
        break;
    case ParticleSystemType::ShipTrail:
        ps = std::make_shared<ParticleSystem>(
            ParticleSystemType::ShipTrail,
            100,                                // num particles
            5.0f,                               // emission angle (0 for omni)
            0.5f, 2.0f,                         // particle time and total time
            50.0f, 100.0f,                      // min and max size
            1000.0f, 1500.0f,                   // min and max vel
            Vector4(0.5f, 0.2f, 0.0f, 1.0f),    // start color
            Vector4(1.0f, 0.0f, 0.0f, 0.0f),    // end color
            &particleTextures_[(int)type].value(), // texture
            DrawMode::AdditiveAndGlow,          // draw mode
            transform);                         // transform
        break;
    case ParticleSystemType::MissileExplode:
        ps = std::make_shared<ParticleSystem>(
            ParticleSystemType::MissileExplode,
            200,                                // num particles
            0.0f,                               // emission angle (0 for omni)
            0.5f, 0.5f,                         // particle and total time
            20.0f, 60.0f,                       // min and max size
            800.0f, 1200.0f,                    // min and max vel
            Vector4(1.0f, 1.0f, 1.0f, 1.5f),    // start color
            Vector4(1.0f, 1.0f, 1.0f, -0.5f),   // end color
            &particleTextures_[(int)type].value(), // texture
            DrawMode::AdditiveAndGlow,          // draw mode
            transform);                         // transform
        break;
    case ParticleSystemType::MissileTrail:
        ps = std::make_shared<ParticleSystem>(
            ParticleSystemType::MissileTrail,
            100,                                // num particles
            10.0f,                              // emission angle (0 for omni)
            0.5f, 1.0f,                         // particle time and total time
            15.0f, 30.0f,                       // min and max size
            1000.0f, 1500.0f,                   // min and max vel
            Vector4(0.5f, 0.2f, 0.0f, 1.0f),    // start color
            Vector4(1.0f, 0.0f, 0.0f, 0.0f),    // end color
            &particleTextures_[(int)type].value(), // texture
            DrawMode::AdditiveAndGlow,          // draw mode
            transform);                         // transform
        break;
    case ParticleSystemType::BlasterExplode:
        ps = std::make_shared<ParticleSystem>(
            ParticleSystemType::BlasterExplode,
            40,                                 // num particles
            2,                                  // emission angle (0 for omni)
            0.25f, 0.25f,                       // particle time and total time
            30.0f, 40.0f,                       // min and max size
            200.0f, 800.0f,                     // min and max vel
            Vector4(1.0f, 1.0f, 1.0f, 1.5f),    // start color
            Vector4(1.0f, 1.0f, 1.0f, -0.2f),   // end color
            &particleTextures_[(int)type].value(), // texture
            DrawMode::AdditiveAndGlow,          // draw mode
            transform);                         // transform
        break;
    }

    if (ps != nullptr)
        particle_->Add(ps);

    return ps;
}

std::shared_ptr<AnimSprite> GameManager::AddAnimSprite(AnimSpriteType type, Vector3 position,
                                                       float radius, float viewOffset,
                                                       float frameRate, DrawMode mode,
                                                       int player) {
    // create animated sprite
    auto a = std::make_shared<AnimSprite>(type, position, radius, viewOffset,
                                          &animatedSpriteTextures_[(int)type].value(), 256, 256,
                                          frameRate, mode, player);

    // add it to the animated sprite manager
    animatedSprite_->Add(a);

    return a;
}

std::shared_ptr<Powerup> GameManager::AddPowerup(PowerupType type, Matrix transform) {
    // create powerup
    auto p = std::make_shared<Powerup>(type, transform, &powerupModels_[(int)type].value());

    // add it to the powerup manager
    powerup_->Add(p);

    return p;
}

std::shared_ptr<Projectile> GameManager::AddProjectile(ProjectileType type, int player,
                                                       Matrix transform, float velocity,
                                                       float damage, RenderTechnique technique) {
    // get source and destination positions for projectile
    Vector3 source = transform.getTranslationProperty();
    Vector3 destination = source + transform.getForwardProperty() * 10000;

    // ray intersect level to find out where projetile is going to hit
    float hitDist;
    Vector3 hitPos, hitNormal;
    if (levelCollision_->PointIntersect(source, destination, hitDist, hitPos, hitNormal))
        destination = hitPos;
    else
        hitNormal = transform.getBackwardProperty();

    // create projectile
    auto p = std::make_shared<Projectile>(type, &projectileModels_[(int)type].value(), player,
                                          velocity, damage, transform, destination, technique);

    // add it to the projectile manager
    projectile_->Add(p);

    return p;
}

void GameManager::DrawProjectile(GraphicsDevice* gd, ProjectileType p, RenderTechnique technique,
                                 Vector3 cameraPosition, Matrix world, Matrix viewProjection,
                                 LightList* lights) {
    DrawModel(gd, &projectileModels_[(int)p].value(), technique, cameraPosition, world,
              viewProjection, lights);
}

void GameManager::DrawModel(GraphicsDevice* gd, Model* model, RenderTechnique technique,
                            Vector3 cameraPosition, Matrix world, Matrix viewProjection,
                            LightList* lights) {
    if (gd == nullptr) {
        throw System::ArgumentNullException("gd");
    }

    // get model bones
    model->CopyAbsoluteBoneTransformsTo(bones_);

    BlendState bs = gd->getBlendStateProperty();
    DepthStencilState ds = gd->getDepthStencilStateProperty();

    gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);
    gd->setBlendStateProperty(BlendState::Additive);

    // for each mesh in model
    for (ModelMesh* mesh : model->getMeshesProperty()) {
        // get mesh world matrix
        Matrix worldBone = bones_[mesh->getParentBoneProperty()->getIndexProperty()] * world;
        Matrix worldBoneInverse = Matrix::Invert(worldBone);

        // compute camera position in object space
        Vector3 cameraObjectSpace = cameraPosition - worldBone.getTranslationProperty();
        cameraObjectSpace = Vector3::TransformNormal(cameraObjectSpace, worldBoneInverse);

        gd->getSamplerStatesProperty()[0] = SamplerState::LinearWrap;

        // for each mesh part
        for (ModelMeshPart* meshPart : mesh->getMeshPartsProperty()) {
            // if primitives to render
            if (meshPart->getPrimitiveCountProperty() > 0) {
                // setup vertices and indices
                gd->SetVertexBuffer(meshPart->getVertexBufferProperty());
                gd->setIndicesProperty(meshPart->getIndexBufferProperty());

                // setup effect
                Effect* effect = meshPart->getEffectProperty();
                effect->getParametersProperty()["WorldViewProj"]->SetValue(worldBone *
                                                                          viewProjection);
                effect->getParametersProperty()["CameraPosition"]->SetValue(cameraObjectSpace);

                // setup technique
                effect->setCurrentTechniqueProperty(
                    &meshPart->getEffectProperty()->getTechniquesProperty()[(int)technique]);

                // if not lights specified
                if (lights == nullptr) {
                    // begin effect
                    effect->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
                    // draw with plain mapping
                    gd->DrawIndexedPrimitives(PrimitiveType::TriangleList,
                                              meshPart->getVertexOffsetProperty(), 0,
                                              meshPart->getNumVerticesProperty(),
                                              meshPart->getStartIndexProperty(),
                                              meshPart->getPrimitiveCountProperty());
                    gd->SetVertexBuffer(nullptr);
                    gd->setIndicesProperty(nullptr);
                } else {
                    gd->setDepthStencilStateProperty(DepthStencilState::Default);
                    gd->setBlendStateProperty(BlendState::Opaque);

                    // get light effect parameters
                    EffectParameter* effectLightPosition = &effect->getParametersProperty()[1];
                    EffectParameter* effectLightColor = &effect->getParametersProperty()[2];
                    EffectParameter* effectLightAmbient = &effect->getParametersProperty()[3];

                    // ambient light
                    Vector3 ambient = lights->ambient;

                    // for each light
                    for (const Light& light : lights->lights) {
                        // setup light in effect
                        effectLightAmbient->SetValue(ambient);
                        light.SetEffect(effectLightPosition, effectLightColor, worldBoneInverse);

                        // begin effect
                        effect->getCurrentTechniqueProperty()->getPassesProperty()[0].Apply();
                        // draw primitives
                        gd->DrawIndexedPrimitives(PrimitiveType::TriangleList,
                                                  meshPart->getVertexOffsetProperty(), 0,
                                                  meshPart->getNumVerticesProperty(),
                                                  meshPart->getStartIndexProperty(),
                                                  meshPart->getPrimitiveCountProperty());

                        // setup additive blending with no depth write
                        gd->setDepthStencilStateProperty(DepthStencilState::DepthRead);
                        gd->setBlendStateProperty(BlendState::Additive);

                        // clear ambinet light (applied in first pass only)
                        ambient = Vector3::Zero;
                    }

                    // clear vertices and indices
                    gd->SetVertexBuffer(nullptr);
                    gd->setIndicesProperty(nullptr);
                }
            }
        }
    }
    gd->setDepthStencilStateProperty(ds);
    gd->setBlendStateProperty(bs);
}

void GameManager::Dispose() { Dispose(true); }

void GameManager::Dispose(bool disposing) {
    if (disposing && !isDisposed_) {
        if (animatedSprite_ != nullptr) {
            animatedSprite_->Dispose();
            animatedSprite_ = nullptr;
        }
        if (particle_ != nullptr) {
            particle_->Dispose();
            particle_ = nullptr;
        }
    }
}

} // namespace ShipGame
