cna_add_sample(Particles3D
    SOURCES
        src/Particle3DSampleGame.cpp
        src/ParticleEmitter.cpp
        src/ParticleSystem.cpp
        src/ParticleVertex.cpp
        src/Projectile.cpp
        src/ParticleSystems/ExplosionParticleSystem.cpp
        src/ParticleSystems/ExplosionSmokeParticleSystem.cpp
        src/ParticleSystems/FireParticleSystem.cpp
        src/ParticleSystems/ProjectileTrailParticleSystem.cpp
        src/ParticleSystems/SmokePlumeParticleSystem.cpp
        src/Program.cpp
        src/Properties/AssemblyInfo.cpp
    CONTENT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Content
)
