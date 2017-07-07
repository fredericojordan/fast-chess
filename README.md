# fast-chess
Simple chess game (developed to be faster than my [python version](https://github.com/fredericojordan/chess)). GUI uses [SDL](https://www.libsdl.org/) library, dlls are available for Windows.

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

## Instructions
1. Download [latest release](https://github.com/fredericojordan/fast-chess/releases)
2. Extract file contents
3. Run chess.exe (or chess-console.exe for command prompt info)
4. ???
5. Profit

## GUI Hotkeys
### AI
- **\<UP\>/\<DOWN\>** - Change AI difficulty
- **A** - Toggle [A]I opponent
- **I** - [I]nvert player's colors

### Colors
- **H** - Toggle attack [H]eatmap
- **C** - Change board [c]olor scheme
- **R** - [R]andom board color
- **T** - Random [t]inted board color

### Miscelaneous
- **E** - [E]dit mode (click and drag to move or click to cycle pieces)
- **P** - Export to [P]GN
- **D** - [D]ump game info to file
- **M** - List legal [M]oves (console mode only)
- **V** - Static board e[v]aluation (console mode only)
- **Q** - [Q]uit
- **U** - [U]ndo move

## Screenshots
![Chess Game](http://i.imgur.com/O6rcSqu.png)

![Chess Heatmap](http://i.imgur.com/qvwbINN.png)
