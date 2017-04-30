# wasm-malloc

A simple implementation of malloc/free for WebAssembly. The heap is a doubly linked lists, with blocks being merged or split if necessary/possible.

At the moment, realloc is not supported. malloc/calloc do not correctly align the memory.

Because the WebAssembly MVP does not support shrinking the linear memory, freed memory can not be released.

## Building

This requires llvm (compiled with the wasm32 target), wabt and binaryen. The path to the llvm bin directory has to be adjusted in the compile script. wabt and 
binaryen should be accessible through the path.

```
mkdir build
CFLAGS=-O2 ./compile build/main.wasm src/mm.c src/main.c
```

Debugging information can be enabled with the preprocessor macro `MM_DEBUG`. This also adds the function `print_heap()` which prints the current layout of the 
heap to the browser console.

## Usage

The functions can be invoked in JavaScript as `instance.exports.malloc()/calloc()/free()`.

In C these functions can be included with `mm.h`, they have the exact same signature as in the C standard library.
