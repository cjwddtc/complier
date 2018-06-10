set(CMAKE_CXX_STANDARD 17)
aux_source_directory(. SRC) 
add_executable(asd ${SRC})
add_custom_command(OUTPUT	${CMAKE_SOURCE_DIR}/qwe.s
				   COMMAND	asd ${CMAKE_SOURCE_DIR}/test.txt|llc -o ${CMAKE_SOURCE_DIR}/qwe.s
				   DEPENDS	"${CMAKE_SOURCE_DIR}/test.txt" asd)
add_custom_command(OUTPUT	${CMAKE_SOURCE_DIR}/qwe.exe
				   COMMAND	clang ${CMAKE_SOURCE_DIR}/asd.c ${CMAKE_SOURCE_DIR}/qwe.s -o ${CMAKE_SOURCE_DIR}/qwe.exe
				   DEPENDS	"${CMAKE_SOURCE_DIR}/qwe.s")
add_custom_target(qwe ALL DEPENDS ${CMAKE_SOURCE_DIR}/qwe.exe)