cmake_minimum_required(VERSION 3.27.0)
project(Sm23900_WIN32 VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


add_compile_definitions("_WIN32" "WIN32" "_UNICODE" "UNICODE")

add_link_options("-municode")
#set(CMAKE_GENERATOR_PLATFORM "x64")

set(CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 18)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(X64 ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()
set(CMAKE_COLOR_DIAGNOSTICS ON)

set(CMAKE_CXX_FLAGS_DEBUG "-g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	#set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -flto -flto-report -fdiagnostics-color=always -fno-rtti -Wall -Wextra -Werror -pedantic-errors -Wctor-dtor-privacy -Wextra-semi -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls -Wsign-promo")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
  add_compile_options(-Wa,-mbig-obj -fno-rtti)
  add_compile_options(-Wall -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Woverloaded-virtual -Wredundant-decls -Wsign-promo)
  add_compile_options(-fignore-unknown-pragmas -Wno-unknown-pragmas -Wno-ctor-dtor-privacy)
  #-Wno-unknown-pragmas
  #-Wnon-virtual-dtor -Wctor-dtor-privacy
  #add_compile_options(-w)
  #add_compile_options(-w -Wa,-mbig-obj)
  #-fdiagnostics-color=always 
	#set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -fno-rtti")
  #-Wall   
  #-flto -flto-report
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()

include_directories(
  "../_libs"
)

add_executable(${PROJECT_NAME}
    "../src/umba-md-pp/umba-md-pp.cpp"
    "../libs/sfmt\umba_lout.cpp"
    "../libs/encoding\encoding.cpp"
    "../libs/encoding\compact_enc_det\compact_enc_det.cc"
    "../libs/encoding\compact_enc_det\compact_enc_det_hint_code.cc"
    "../libs/encoding\util\encodings\encodings.cc"
    "../libs/encoding\util\languages\languages.cc"
    )
    #target_compile_options(Framework PUBLIC -Wa,-mbig-obj)
    #-flto


target_link_libraries(${PROJECT_NAME} ws2_32 iphlpapi shell32)


#include(CPack)
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

