# complier
一个编译器
词法分析器用的是语法分析器解析正则生成nfa转dfa
语法分析器用的是lr(1)文法
后端使用的是llvm,但不依赖于llvm的库，只需要有llc编译工具即可正常运行，直接编译asd为编译器，qwe为asd.c 和test.txt编译成的可执行文件(这一步编译需要clang)

此项目的编译只依赖于c++的标准库，没有任何额外依赖，但是需要c++17标准的支持
请使用足够新版的编译器,目前已测试最新的vs2017,gcc7.3.1,clang5.0.1可以正常使用。
