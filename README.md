# HTTP Server
This is a small HTTP/1.1 server I built to explore multithreading and low-level networking; it sends the client's headers back as JSON response.

## Requirements
- POSIX-compatible environment (Linux, macOS, WSL, etc.).
- `make` plus a C compiler such as GCC or Clang with pthread support.
- Optional: [`bear`](https://github.com/rizsotto/Bear) if you want to regenerate `compile_commands.json` for clangd.

## Build
- `make` or `make debug` (default target) builds with AddressSanitizer/UndefinedBehaviorSanitizer enabled for easier debugging.
- `make release` builds an optimized binary (`-O3 -s`) with `NODEBUG` defined.
- `make clean` removes every artifact under `build/`.

## Run
After compiling, you can start the server. By default it will listen on port `3490`:    
```bash
make run
```

To run the server on a different port, invoke it directly:    
```bash
./build/target.out [port]
```

## Linting
Regenerate the compilation database that clangd rely on with:    
```bash
bear -- make
```
