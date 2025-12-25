# Terminal Chess (C++ OOP)

A simple terminal-based chess game written in C++ in one file. Play as White and Black in the same terminal window; the board renders after each move.

## Features
- Simple design: `Piece` struct, `Board` class, single-file program.
- ASCII board rendering (uppercase = White, lowercase = Black).
- Move input: `e2e4` or `e2 e4`.
- Basic legality: piece movement rules, path blocking, captures.
- Prevents moves that leave own king in check (basic check detection).
- No castling, en passant, or promotion yet.

## Build (Windows)

Using g++ (MinGW or similar):

```bat
cd "d:\4 Project\Project_2_OOPS_Chess_simple"
g++ -std=c++17 -O2 src\chess.cpp -o chess.exe
```

Using MSVC `cl` (Developer Command Prompt):

```bat
cd "d:\4 Project\Project_2_OOPS_Chess_simple"
cl /EHsc /std:c++17 src\chess.cpp /Fe:chess.exe
```

Using CMake (optional):

```bat
cd "d:\4 Project\Project_2_OOPS_Chess_simple"
cmake -S . -B build
cmake --build build --config Release
```

## Run

```bat
chess.exe
```

Enter moves like `e2e4` or `g8 f6`. Type `quit` to exit.

## Notes
- The board shows ranks on the left and files at the bottom.
- White starts; turns alternate automatically.
- If your terminal supports Unicode, we can add chess glyphs on request.
