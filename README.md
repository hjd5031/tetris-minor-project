# Tetris Minor Project

A console-based Tetris game implemented in C as a minor project.

This project runs in the Windows console and was built with Visual Studio. It implements the core mechanics of Tetris, including block movement, rotation, collision detection, line clearing, scoring, next-block preview, and increasing difficulty.

## Features

- Console-based Tetris gameplay
- Seven standard tetromino block types
- Block rotation
- Left, right, and downward movement
- Hard drop
- Collision detection with walls, floor, and stacked blocks
- Line clear detection
- Score system
- Level system with increasing fall speed
- Next block preview
- Game over detection and message

## Controls

| Key | Action |
| --- | --- |
| Left Arrow | Move block left |
| Right Arrow | Move block right |
| Down Arrow | Move block down |
| Up Arrow | Rotate block |
| Spacebar | Hard drop |

## Scoring

- Move block down manually: +1 point
- Hard drop: +2 points per dropped row
- Clear one line: +100 points
- Level increases every 500 points
- Maximum level: 9

## Technologies

- C
- Windows Console API
- Visual Studio 2022
- MSVC v143 toolset

## Project Structure

```text
tetris-minor-project/
├── Tetris.sln
├── Tetris/
│   ├── Tetris.vcxproj
│   ├── 소스.c
│   └── block.h
└── README.md
```

## Main Files

| File | Description |
| --- | --- |
| `Tetris/소스.c` | Main game loop, rendering, input handling, scoring, collision logic |
| `Tetris/block.h` | Tetromino block shape data |
| `Tetris/Tetris.vcxproj` | Visual Studio C/C++ project file |
| `Tetris.sln` | Visual Studio solution file |

## How to Build and Run

1. Clone the repository.

```bash
git clone https://github.com/hjd5031/tetris-minor-project.git
```

2. Open `Tetris.sln` in Visual Studio 2022.

3. Select a build configuration, such as `Debug x64`.

4. Build and run the project.

## Notes

This project uses Windows-specific headers such as `Windows.h` and `conio.h`, so it is intended to run on Windows.

If the project fails to build because `block.h` cannot be found, check the include path in `Tetris.vcxproj`. The repository already contains `Tetris/block.h`, so the project should reference that local file.

Generated folders such as `.vs/`, `x64/`, and build output files do not need to be committed to the repository.

## Author

- GitHub: [hjd5031](https://github.com/hjd5031)
- Repository: [tetris-minor-project](https://github.com/hjd5031/tetris-minor-project)
```

