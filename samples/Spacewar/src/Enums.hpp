// SPDX-License-Identifier: MS-PL
#pragma once

namespace Spacewar
{
    enum class GameState
    {
        None,
        Started,
        LogoSplash,
        PlayEvolved,
        PlayRetro,
        ShipSelection,
        ShipUpgrade,
        Victory,
    };

    enum class ProjectileType
    {
        Peashooter,
        MachineGun,
        DoubleMachineGun,
        Rocket,
        BFG,
    };

    enum class ShipClass
    {
        Pencil,
        Saucer,
        Wedge,
    };

    enum class LightingType
    {
        InGame,
        Menu,
    };
}
