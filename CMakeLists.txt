cmake_minimum_required(VERSION 3.16)

project(metaparse CXX)



include_directories($ENV{LLVM_PATH}/include)
#link_directories($ENV{LLVM_PATH}/lib)
set(CMAKE_MESSAGE_LOG_LEVEL TRACE)


set(LLVM_CONFIG_FLAGS "`llvm-config --cxxflags --ldflags --system-libs --libs core`")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
    message(STATUS YOooooo)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /experimental:preprocessor")
endif()

set(CMAKE_CXX_STANDARD 20)

add_executable(metaparse main.cpp metafunction.h meta.h ASTBase.h)

#target_link_libraries(metaparse LLVMCore LLVMSupport)
set_target_properties(PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -v ${LLVM_CONFIG_FLAGS}")
