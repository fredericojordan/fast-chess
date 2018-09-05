# fast-chess

![Chess Game](http://i.imgur.com/O6rcSqu.png)

Simple chess game (intended to be faster than my [python version](https://github.com/fredericojordan/chess), hence the name). GUI uses [SDL 2.0](https://www.libsdl.org/) library.

Many thanks to [Chess Programming Wiki](http://chessprogramming.wikispaces.com) and [TSCP](http://www.tckerrigan.com/Chess/TSCP/) for some ideas and tuning values.

## Features

- Board Representation:
    - Bitboards
- Search:
    - Iterative Deepening
    - Alpha-Beta Pruning
    - Parallel Processing
    - Quiescence Search
    - Static Exchange Evaluation
- Evaluation:
    - Material Balance
    - Piece-Square Tables
    - Pawn Structure
- Graphical User Interface:
    - Attack Heatmap
    - Edit Board Mode

## Quickstart

1. Download [latest release](https://github.com/fredericojordan/fast-chess/releases)
2. Extract file contents
3. Run `chess.exe` (Windows) or `./chess` (macOS/Linux)
4. ???
5. Profit

## GUI Hotkeys

|   Hotkey    |                     Usage                           |
| :---------: | --------------------------------------------------- |
|  **▲ / ▼**  | Increase/decrease AI search depth                   |
|    **A**    | Toggle [A]I opponent                                |
|    **I**    | [I]nvert player's colors                            |
|    **H**    | Toggle attack [H]eatmap                             |
|    **C**    | Change board [c]olor scheme                         |
|    **R**    | [R]andom board color                                |
|    **T**    | Random [t]inted board color                         |
|    **E**    | [E]dit mode (drag to move or click to cycle pieces) |
|    **P**    | Export to [P]GN                                     |
|    **D**    | [D]ump game info to file                            |
|    **M**    | List legal [M]oves (console mode only)              |
|    **V**    | Static board e[v]aluation (console mode only)       |
|    **Q**    | [Q]uit                                              |
|    **U**    | [U]ndo move                                         |

## Screenshots

![Chess Heatmap](http://i.imgur.com/qvwbINN.png)
