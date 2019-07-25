cmake_minimum_required(VERSION 3.0)

project(LibToolingExample
  VERSION 0.1
  LANGUAGES C CXX)

find_package(LLVM 7.0.0 REQUIRED CONFIG)
include(CTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message("-- LLVM_PACKAGE_VERSION is: ${LLVM_PACKAGE_VERSION}")
message("-- Using LLVMConfig.cmake in: ${LLVM_DIR}")
message("-- LLVM_LIBRARY_DIRS is: ${LLVM_LIBRARY_DIRS}")

set(CLANG_LIBS
  clangTooling
  clangFrontendTool
  clangFrontend
  clangDriver
  clangSerialization
  clangCodeGen
  clangParse
  clangSema
  clangStaticAnalyzerFrontend
  clangStaticAnalyzerCheckers
  clangStaticAnalyzerCore
  clangAnalysis
  clangARCMigrate
  clangRewriteFrontend
  clangRewrite
  clangEdit
  clangAST
  clangLex
  clangBasic
  clangASTMatchers
)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter -fno-strict-aliasing -fno-exceptions -fno-rtti")

llvm_map_components_to_libnames(llvm_libs
  Analysis
  Support
  MC
  Option
)

set(SOURCES
  Example.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES})
target_link_libraries(${CMAKE_PROJECT_NAME}
  ${CLANG_LIBS}
  ${llvm_libs}
)

add_test(NAME test1
         COMMAND ${CMAKE_PROJECT_NAME} ${PROJECT_SOURCE_DIR}/test.c --
)

