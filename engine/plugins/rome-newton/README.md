# newton

Engine plugin.

## Layout

- `plugin.json` - plugin metadata
- `CMakeLists.txt` - standardized plugin build file
- `build.sh` - standalone plugin build helper
- `clean.sh` - standalone plugin clean helper
- `src/plugin.cpp` - required plugin entrypoints
- `src/` - plugin source files
- `include/` - plugin header files

## Build

```sh
./build.sh --core /path/to/rome/core
```

The build script writes the plugin binary to `bin/`.

## Clean

```sh
./clean.sh
```
