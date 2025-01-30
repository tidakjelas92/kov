# kov

Project for Global Game Jam 2025

## Building

Pull dependencies:
```
git submodule update --init --recursive
```

### Windows

Requirements:
- Visual Studio 2022 Build Tools

```batch
:: make sure cl is in the path by running vcvarsall.bat or this project's devenv.bat
devenv

:: first time building, build raylib too.
build raylib main

:: subsequent builds only need to build the default target (main)
build

:: release build
build raylib main release
```

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
