if (LLVM_COMPILER_IS_GCC_COMPATIBLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -Wl,--wrap=memcpy")
endif ()

if (ZAPCC_DEBUG)
  string (REGEX REPLACE "[/-]O2" "-O0"
    CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO}") 
endif()

set( LLVM_LINK_COMPONENTS
  ${LLVM_TARGETS_TO_BUILD}
  Analysis
  CodeGen
  Core
  IPO
  InstCombine
  Instrumentation
  MC
  MCParser
  ObjCARCOpts
  Option
  ScalarOpts
  Support
  TransformUtils
  Vectorize
  )

include_directories(BEFORE
  ${CMAKE_CURRENT_SOURCE_DIR}/../zapccs
  ${CMAKE_CURRENT_BINARY_DIR}/../clang/include
  ${CMAKE_CURRENT_SOURCE_DIR}/../clang/include
)

add_clang_executable(zapcc
  Client.cpp
  ErrorCouldNot.cpp
  FindBaseName.cpp
  TerminateAllCopiesOf.cpp
  tinycthread.c
  zapcc1_main.cpp
  ../clang/tools/driver/cc1_main.cpp
  ../clang/tools/driver/cc1as_main.cpp
  ../clang/tools/driver/driver.cpp
  ../zapccs/CommandLine.cpp
  ../zapccs/DisableStreamsBuffering.cpp
  ../zapccs/FindOption.cpp
  ../zapccs/GetExePath.cpp
  ../zapccs/memcpy_compat.c
  ../zapccs/StringConst.cpp
  ../zapccs/StopWatch.cpp
  ../zapccs/TextFile.cpp
)

target_link_libraries(zapcc
  clangBasic
  clangCodeGen
  clangDriver
  clangFrontend
  clangFrontendTool
  )

set_target_properties(zapcc PROPERTIES FOLDER "Ceemple")

set(ZAPCC_LINKS_TO_CREATE zapcc++ zapcc-cl)
foreach(link ${ZAPCC_LINKS_TO_CREATE})
  add_clang_symlink(${link} zapcc)
endforeach()  
