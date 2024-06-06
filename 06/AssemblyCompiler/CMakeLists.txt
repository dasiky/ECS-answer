project(AssemblyCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AssemblyCompiler main.cpp Code.cpp Parser.cpp SymbolTable.cpp)