# wasm-malloc

A simple implementation of malloc/free for WebAssembly. The heap is a doubly linked lists, with blocks being merged or split if necessary/possible.

At the moment, realloc is not supported. malloc/calloc do not correctly align the memory.

Because the WebAssembly MVP does not support shrinking the linear memory, freed memory can not be released.

## Building

This requires the LLVM toolchain (compiled to include the experimental wasm32 target, see [yurydelendik/wasmception](https://github.com/yurydelendik/wasmception)). The path to the llvm binary directory has to be passed to the compilation script with the `LLVM_ROOT` environment variable.

Example invocation of the compilation script:
```
mkdir build
LLVM_ROOT=$HOME/ CFLAGS=-O2 ./compile build/main.wasm src/malloc.c src/test.c
```

Debug logging can be enabled with the preprocessor macro `MALLOC_DEBUG`. This also adds the function `print_heap()` which prints the current layout of the heap to the browser console.

## Usage

In C these functions can be included with `malloc.h`, they have the exact same signature as in the C standard library.

The file `test.c` exports some test functions that you can call from JavaScript as `instance.exports.test_malloc/free/...(...)`.
