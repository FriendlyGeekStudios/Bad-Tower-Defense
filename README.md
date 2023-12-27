# BadTowerDefense

This game is a Procedural Tower Defense game currently in the Alpha stages

Players will build towers to protect their base against increasingly difficult waves of enemies

## Engineering Details
Between rounds of the game, players will have the opportunity to increase their defenses before another wave begins through the purchase of additional towers

When players complete a wave, a new Map Chunk will be spawned. Each chunk is uniquely generated and produced in such a way that the map will never loop back on itself or trap itself in a corner using a custom random walk algorithm.

Additionally, when a new chunk is generated a dedicated path is generated within the chunk that the enemy units will follow. This is accomplished using a variant of Dijkstra's pathfinding algorithm and using random weights to encourage unique paths per chunk. Each path will always connect to the end of the previous chunk to produce a continuous, uninterrupted road.

### Enemy Spawning
Enemies are spawned psuedo-randomly with certain enemies locked behind round requirements. An Enemy Overseer is responsible for generating enemies to fight each round which is determined by the current round. Additionally, I plan to implement a scaling mechanism so that the more towers and defenses the player has, the harder the rounds will be

### Assets
This is largely a kit-bashed game with assets being used from the Fantastic Battle Pack from Tidal Flask Studios as well as characters and animations from Adobe's Mixamo.

## Roadmap
Here are some additional features that I would like to add to the game:
- [] Tower Upgrade Feature
- [] Explosion Animations/effects
- [] Better AI pathfinding and recovery
- [] Custom first Chunk
- [] Detailed Headquarters Asset
- [] More Enemy Types
- [] More Tower Types
- [] Temporary, Activatible abilities
- [] Boss Enemies
- [] Meta-Progression System (For a more rogue-like experience)