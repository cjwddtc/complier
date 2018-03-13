project(compiler CXX)
set(CMAKE_CXX_STANDARD 17)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # using regular Clang or AppleClang

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "-lc++abi")
endif()
aux_source_directory(. SRC)
add_executable(asd yufashu.cpp dfa.cpp main.cpp)
#add_executable(run run.cpp)