set(CMAKE_CXX_STANDARD 17)
aux_source_directory(. SRC)
add_executable(complier ${SRC})
add_executable(asd yufashu.cpp)