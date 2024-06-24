project(JackCompiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(JackCompiler JackCompiler.cpp JackTokenizer.cpp CompilationEngine.cpp SymbolTabel.cpp VMWriter.cpp)