## UE Box

[UE](https://github.com/EpicGames/UnrealEngine) box of goodies

## Build

Generate project files on Linux

```
setup.sh - setup demo project files

  -u Unreal Engine installation directory
  -h help
```

Build with `cmake`

```
cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_BUILD_TYPE=Debug .
cmake --build . --target UEBoxEditor
cmake --build . --target UEBox-Linux-Test
```

Run editor `./editor.sh`
