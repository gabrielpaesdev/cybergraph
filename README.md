# CyberGraph: Tactical Topology

> A turn-based tactical game built on graph theory, where two players fight for control of a network.



## About

CyberGraph is a 1v1 strategy game where **Router** and **Saboteur** take turns fighting over a procedurally generated network topology. The Router reinforces edges to build a connected path between source and target; the Saboteur cuts edges to isolate them. The core mechanic is inspired by the **Shannon Switching Game**, reframed as a network infrastructure battle, with procedural map generation and an optional chess clock for added pressure.

## Features

- Procedural topology generation (LAN, Corporate WAN, Global Backbone)
- Optional chess clock (untimed, 1, 3, 5, or 10 minutes per player)
- Light/dark mode
- Multi-language support (Portuguese/English)
- Custom bitmap font rendering (no SDL_ttf dependency)
- Fullscreen toggle and resizable window

## How to play

- **Router** clicks an edge to **reinforce** it (green, thicker)
- **Saboteur** clicks an edge to **cut** it (red, thinner)
- Router wins if a fully reinforced path connects source to target
- Saboteur wins if no path remains between source and target
- Running out of time on the clock also decides the match

| Key | Action |
|---|---|
| Left click | Reinforce / Cut edge |
| `R` | Regenerate map |
| `ESC` | Back to menu |
| `F11` | Fullscreen |

## Build & run

**Dependencies:** `g++` (C++17), `SDL2` dev libraries

```bash
sudo apt install build-essential libsdl2-dev

git clone https://github.com/your-username/cybergraph.git
cd cybergraph
make run
```

## Architecture

Kept deliberately simple, single-module engine:

- **`engine.h` / `engine.cpp`** — global game state (nodes, edges, turn, status), procedural graph generation, BFS-based path checking, per-screen input handling
- **`font8x8.h`** — embedded 8x8 bitmap font, avoiding an SDL_ttf dependency
- **`main.cpp`** — main loop, delta time, event dispatch, per-state rendering

Global state was a deliberate choice over a more elaborate OOP architecture — for a single local-play mode with a handful of screens, it keeps the code direct without unnecessary abstraction overhead. Win conditions run BFS twice per move: once ignoring cut edges (checks if Saboteur already won) and once considering only reinforced edges (checks if Router already won).

## License

Licensed under [AGPL-3.0](LICENSE).

---

Developed by **Gabriel Paes** — 2026
