cmake_minimum_required(VERSION 3.16)

project(metaparse CXX)


#message(STATUS $ENV{LLVM_PATH})

#link_directories($ENV{LLVM_PATH}/lib)
set(CMAKE_MESSAGE_LOG_LEVEL TRACE)
set(LLVM_COMPONENTS "codegen")
set(LLVM_FLAGS "")
set(LLVM_LIBS "")

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    message(STATUS "Generating LLVM build info for GCC")
    set(CXX_VERSION_STRING gnu++2a)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
    endif()
    execute_process(COMMAND llvm-config --cxxflags --ldflags --system-libs --libs ${LLVM_COMPONENTS} OUTPUT_VARIABLE LLVM_FLAGS)
    string(REGEX REPLACE "[\n\r]" " " LLVM_FLAGS ${LLVM_FLAGS})
    string(REGEX REPLACE "(-std=)([^ ]*) " "\\1${CXX_VERSION_STRING}" LLVM_FLAGS ${LLVM_FLAGS})

    message(STATUS "LLVM Flags: ${LLVM_FLAGS}")

elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    message(STATUS YOooooo)
    include_directories($ENV{LLVM_PATH}/include)
    set(LLVM_FLAGS "`llvm-config --cxxflags --ldflags --system-libs --libs ${LLVM_COMPONENTS}`")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:preprocessor")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_FLAGS}")

set(CMAKE_CXX_STANDARD 20)

add_executable(metaparse main.cpp metafunction.h include/meta/attribute.h ASTBase.h include/logic/axiom.h include/logic/operators.h include/logic/boolean.h include/meta/concepts.h)
add_executable(tests test/test_defs.inl test/meta/concepts.ii test/run_tests.cpp)
enable_language(CXX)
add_test(unit_tests COMMAND ./tests)


target_include_directories(metaparse PUBLIC ./include)
target_include_directories(tests PUBLIC ./include)
#target_link_libraries(metaparse LLVMCore LLVMSupport)
#set_target_properties(PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -v ${LLVM_CONFIG_FLAGS}")