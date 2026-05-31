# newton

Engine plugin.

## Layout

- `plugin.json` - plugin metadata
- `CMakeLists.txt` - standardized plugin build file
- `build.sh` - standalone plugin build helper
- `src/plugin.cpp` - required plugin entrypoints
- `src/` - plugin source files
- `include/` - plugin header files

## Build

```sh
./build.sh --core-dir /path/to/rome/core
```

The build script writes the plugin binary to `bin/`, marks the binary read-only, and removes `build/` after a successful build.
