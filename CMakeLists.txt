cmake_minimum_required(VERSION 3.15)

project(metaparse CXX)


#message(STATUS $ENV{LLVM_PATH})

#link_directories($ENV{LLVM_PATH}/lib)
set(CMAKE_MESSAGE_LOG_LEVEL TRACE)
set(LLVM_COMPONENTS "core")
set(LLVM_FLAGS)
set(LLVM_LIBS)



#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LLVM_FLAGS}")

set(CMAKE_CXX_STANDARD 20)

message(STATUS "INCLUDES: ${CMAKE_INCLUDE_PATH}")

add_executable(metaparse main.cpp metafunction.h include/meta/attribute.h ASTBase.h include/logic/axiom.h include/logic/operators.h include/logic/boolean.h include/meta/concepts.h include/logic/set.h include/logic/variable.h include/meta/metavalue.h include/logic/statement.h include/logic/expression.h include/meta/rtti.h include/util/atomic_ref.h include/meta/traits.h)
add_executable(tests test/test_defs.inl test/meta/concepts.ii test/run_tests.cpp)
enable_language(CXX)
#add_test(unit_tests COMMAND ./tests)

if(CMAKE_CXX_COMPILER_ID MATCHES GNU)
    message(STATUS "Generating LLVM build info for GCC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    set(CXX_VERSION_STRING gnu++2a)
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10.0.0)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fconcepts")
    endif()
    execute_process(COMMAND llvm-config --cxxflags --ldflags --system-libs --libs ${LLVM_COMPONENTS} OUTPUT_VARIABLE LLVM_FLAGS)
    string(REGEX REPLACE "[\n\r]" " " LLVM_FLAGS ${LLVM_FLAGS})
    string(REGEX REPLACE "(-std=)([^ ]*) " "\\1${CXX_VERSION_STRING}" LLVM_FLAGS ${LLVM_FLAGS})

    message(STATUS "LLVM Flags: ${LLVM_FLAGS}")

elseif(CMAKE_CXX_COMPILER_ID MATCHES MSVC OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(STATUS YOooooo)
    set(CXX_VERSION_STRING "c++latest")
    include_directories($ENV{LLVM_PATH}/include)
    execute_process(COMMAND llvm-config --cxxflags OUTPUT_VARIABLE LLVM_CXX_FLAGS)
    execute_process(COMMAND llvm-config --ldflags --system-libs --libnames ${LLVM_COMPONENTS} OUTPUT_VARIABLE LLVM_LINK_FLAGS)
    execute_process(COMMAND llvm-config --system-libs --libnames ${LLVM_COMPONENTS} OUTPUT_VARIABLE LLVM_LIBS)
    separate_arguments(LLVM_LIBS NATIVE_COMMAND ${LLVM_LIBS})
    string(REGEX REPLACE "[\n\r]" " " LLVM_LINK_FLAGS ${LLVM_LINK_FLAGS})
    string(REGEX REPLACE "[\n\r]" " " LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
    string(REGEX REPLACE "(-std:)([^ ]*) " "/std:${CXX_VERSION_STRING}" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
    set(INC_PATH)
    set(LINK_PATH)
    string(REGEX MATCH "(C:([^L]*)LLVM([^ ]*))" INC_PATH ${LLVM_CXX_FLAGS})
    target_include_directories(metaparse PUBLIC ${INC_PATH})
    target_include_directories(tests PUBLIC ${INC_PATH})
    string(REGEX MATCH "C:([^L]*)LLVM([^ ]*)" "\"C:\\1LLVM\\2\"" LINK_PATH ${LLVM_LINK_FLAGS})
    target_link_directories(metaparse PUBLIC ${LINK_PATH})
    target_link_directories(tests PUBLIC ${LINK_PATH})
    target_link_libraries(metaparse PUBLIC ${LLVM_LIBS})
    target_link_libraries(tests PUBLIC ${LLVM_LIBS})
    string(REGEX REPLACE "(-IC:([^L]*)LLVM([^ ]*))" "" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
    separate_arguments(LLVM_FLAGS NATIVE_COMMAND ${LLVM_CXX_FLAGS})
    target_compile_options(metaparse PUBLIC ${LLVM_FLAGS})
    target_compile_options(tests PUBLIC ${LLVM_FLAGS})
    #string(REGEX REPLACE "-([ID])" "/\\1" LLVM_CXX_FLAGS ${LLVM_CXX_FLAGS})
    #string(REGEX REPLACE "-LIBPATH" "/LIBPATH" LLVM_LINK_FLAGS ${LLVM_LINK_FLAGS})
    #separate_arguments(LLVM_FLAGS NATIVE_COMMAND "${LLVM_CXX_FLAGS} /link:${LLVM_LINK_FLAGS}")
    #message(STATUS "LLVM Flags: ${LLVM_FLAGS}")
endif()









target_include_directories(metaparse PUBLIC ./include)
target_include_directories(tests PUBLIC ./include)
#target_compile_options(metaparse PRIVATE ${LLVM_FLAGS})
#target_link_libraries(metaparse LLVMCore LLVMSupport)
#set_target_properties(PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -v ${LLVM_CONFIG_FLAGS}")

#[[set(FLAG_LIST)
foreach(FLAG ${LLVM_FLAGS})
    message(STATUS "\t-> ${FLAG}")
endforeach()]]
#message(STATUS "${COMP_FEAT}")