# kov

Project for Global Game Jam 2025

## Building

Pull dependencies:
```
git submodule update --init --recursive
```

### Windows

Requirements:
- Visual Studio 2022 community (for build tools only)

Steps:
1. Can try to build raylib or just download the precompiled binaries from the releases section.
2. Put `raylib.lib` in `vendor\raylib\src`.
3. Run `build.bat`

### Linux

Requirements:
- llvm

Steps:
1. `cd` to `vendor/raylib/src`
2. `make`
3. `cd` back to project root.
3. Run `build.sh`


## Credits

- [Raylib 5.5](https://github.com/raysan5/raylib)
- [Kenney's 1bit input prompts](https://kenney.nl/assets/1-bit-input-prompts-pixel-16)
- [Kenney interface sounds](https://kenney.nl/assets/interface-sounds)
- [Kenney's Monochrome RPG](https://kenney.nl/assets/monochrome-rpg)
