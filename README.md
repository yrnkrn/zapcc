![ZAPCC Logo](/docs/zapcc/zapcc-logo.png)

zapcc is a caching C++ compiler based on clang, designed to perform faster compilations.
zapcc uses in-memory compilation cache in client-server architecture, remembering all compilation information between runs. 
zapcc is the client while zapccs is the server. Each zapcc run will reuse an existing server or if none was available will start a new one.

## License

This open source release is licensed under the LLVM Release License (University of Illinois/NCSA).

## Building

The prerequisites and build process are identical to [building LLVM](https://llvm.org/docs/CMake.html).

    git clone https://github.com/yrnkrn/zapcc.git llvm
    mkdir build
    cd build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DLLVM_ENABLE_WARNINGS=OFF ../llvm
    ninja
    
## Running the tests
    
    ninja check-all

## Using

zapcc command syntax is identical to [clang](https://clang.llvm.org/docs/UsersManual.html) with the command being zapcc.
   
    zapcc ...
   
## Killing the zapccs server

    pkill zapcc

To kill the zapccs server to free memory or replace with newly-built zapcc
    
## FAQ

### What is the typical acceleration of Zapcc?

Full builds are 2x-5x faster, see [WebKit](https://www.zapcc.com/demo-webkit) or [MKVToolNix ](https://www.bunkus.org/blog/2018/06/speeding-up-mkvtoolnix-compilation-speed-with-zapcc/).
Typically re-compilation of one modified source file is 10x-50x faster, see [Boost.Math](https://www.zapcc.com/demo-incremental-build/). 

Acceleration depends on the complexity of the header files vs. the complexity of the source files. It can range from no acceleration at all for plain C projects where caching is disabled to x2-x5 for build-all of heavily templated projects, up to cases of x50 speedups in developer-mode incremental change of one source file.

As a reference number, Zapcc builds LLVM build-all about x2 faster compared to building LLVM using clang.

### Is Zapcc Clang compatible?

Yes, zapcc is based on heavily-modified clang code.

### Is Zapcc GCC compatible?

Yes, to the extent clang is gcc compatible.

### My project does not compile with Zapcc!

Please make sure first your project compiles successfully with Clang. If your project does not compile with Clang, Zapcc, being based on Clang, will not be able to compile any more than clang.

### Are the sanitizers supported?

Currently, no.

### Which operating systems are supported?

zapcc builds on 
* Linux x64 using gcc, clang or zapcc
* Windows using Visual C++ or mingw-w64
* On MacOS using clang

zapcc was thoroughly tested on Linux x64 targetting Linux x64 and minimally on Windows targetting mingw-w64.
Rest are experimental, please share your experience.

### How zapcc is different from precompiled headers?

Precompiled headers requires building your project to the exact precompiled headers rules. Most projects do not bother with using precompiled headers. Even then, precompiled  headers do not cache as much as zapcc. Zapcc works within your existing build.

Precompiled headers are currently ignored by Zapcc.

### How zapcc is different from C++ modules?

Modules are not yet standard, rarely used and do not support well legacy code and macros found in most existing C++ code, such as Boost. Modules require significant code refactoring in bottom-up approach everything or are slow. Even then, modules do not cache template instantiations and generated code that are specific to your code like zapcc does.

### How much memory does Zapcc use?

To avoid killing the server by using endless memory, Zapcc server has a memory limit and will automatically reset after reaching it, restarting with an empty cache and low memory usage. The memory limit is set under [MaxMemory] at bin/zapccs.config, and you can change it to optimize memory usage and the number of servers you plan to use. Usually, you should not set the -j parameter to more than the number of physical cores + 2. This is especially important for Intel CPU with hyper-threading enabled, which report twice the number of physical cores. In such cases, Zapcc may run faster with fewer servers, each using a higher memory limit.

### Does it use ccache, distcc, warp or the like?

No.

### When was the source last merged with LLVM trunk?

This open-source release was last merged with LLVM r307021 on 2017-07-03.

### Where is the zapcc code?

There are patches all around LLVM & clang.
Additional zapcc-only code in

    tools/zapcc
    tools/zapccs
    tools/clang/test/zapcc
