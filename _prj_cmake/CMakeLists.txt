cmake_minimum_required(VERSION 3.27.0)
project(umba-md-pp-view-console VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Полезные ключики GCC - https://codeforces.com/blog/entry/15547?locale=ru

#add_compile_definitions("_WIN32" "WIN32" "_UNICODE" "UNICODE" "YAML_CPP_STATIC_DEFINE")
add_compile_definitions("_WIN32" "WIN32" "YAML_CPP_STATIC_DEFINE")

#add_link_options("-municode")
add_link_options("-mconsole")
add_link_options("-Wl,--subsystem,console")
#add_link_options("--subsystem,console")

#set(CMAKE_GENERATOR_PLATFORM "x64")

set(CMAKE_VERBOSE_MAKEFILE on )
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(X64 ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif()

#set(CMAKE_COLOR_DIAGNOSTICS ON)

#-O0 
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    #-fdiagnostics-color=always
	#set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -flto -flto-report  -fno-rtti -Wall -Wextra -Werror -pedantic-errors -Wctor-dtor-privacy -Wextra-semi -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls -Wsign-promo")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  #-fignore-unknown-pragmas
  set(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wno-ctor-dtor-privacy -Wsign-promo")
  #add_compile_options(-fdiagnostics-color=always)
  add_compile_options(-Wa,-mbig-obj)
  # -fno-rtti
  add_compile_options(-Wall -Wno-unknown-pragmas -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wredundant-decls -Wno-unused-function)
  add_compile_options(-Wno-unknown-pragmas)
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

set(PRJ_ROOT ${PROJECT_SOURCE_DIR}/..)
set(SRC_ROOT ${PRJ_ROOT}/src)
set(LIBS_ROOT ${PRJ_ROOT}/_libs)


include_directories(
  "${LIBS_ROOT}"
  "${LIBS_ROOT}/encoding"
  "${LIBS_ROOT}/sfmt/inc"
  "${LIBS_ROOT}/nlohmann/include"
  "${LIBS_ROOT}/yaml-cpp/include"
)

#set (SOURCES
#	imgui_impl_glfw.h
#	imgui_impl_opengl2.h
#    )

set (YAML_CPP_SOURCES
    "${LIBS_ROOT}/yaml-cpp/src/binary.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/convert.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/depthguard.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/directives.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/emit.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/emitfromevents.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/emitter.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/emitterstate.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/emitterutils.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/exceptions.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/exp.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/memory.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/node.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/node_data.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/nodebuilder.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/nodeevents.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/null.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/ostream_wrapper.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/parse.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/parser.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/regex_yaml.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/scanner.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/scanscalar.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/scantag.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/scantoken.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/simplekey.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/singledocparser.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/stream.cpp"
    "${LIBS_ROOT}/yaml-cpp/src/tag.cpp"
    )



#set_source_files_properties("${LIBS_ROOT}/encoding/compact_enc_det/compact_enc_det.cc"           PROPERTIES LANGUAGE C )
#set_source_files_properties("${LIBS_ROOT}/encoding/compact_enc_det/compact_enc_det_hint_code.cc" PROPERTIES LANGUAGE C )
#set_source_files_properties("${LIBS_ROOT}/encoding/util/encodings/encodings.cc"                  PROPERTIES LANGUAGE C )
#set_source_files_properties("${LIBS_ROOT}/encoding/util/languages/languages.cc"                  PROPERTIES LANGUAGE C )


set (UMBA_MD_PP_COMMON_SOURCES
    "${LIBS_ROOT}/sfmt/umba_lout.cpp"
    "${LIBS_ROOT}/encoding/encoding.cpp"
    "${LIBS_ROOT}/encoding/compact_enc_det/compact_enc_det.cc"
    "${LIBS_ROOT}/encoding/compact_enc_det/compact_enc_det_hint_code.cc"
    "${LIBS_ROOT}/encoding/util/encodings/encodings.cc"
    "${LIBS_ROOT}/encoding/util/languages/languages.cc"
    )

add_compile_definitions("USE_UMBA_LOUT_COUT")



add_executable(umba-md-pp
    ${YAML_CPP_SOURCES}
    ${UMBA_MD_PP_COMMON_SOURCES}
    "${SRC_ROOT}/umba-md-pp/umba-md-pp.cpp"
    )
target_link_libraries(umba-md-pp ws2_32 iphlpapi shell32)


add_executable(umba-md-pp-view
    ${YAML_CPP_SOURCES}
    ${UMBA_MD_PP_COMMON_SOURCES}
    "${SRC_ROOT}/umba-md-pp/umba-md-pp-view.cpp"
    )
target_link_libraries(umba-md-pp-view ws2_32 iphlpapi shell32)
#target_compile_definitions(umba-md-pp-view "UMBA_MD_PP_VIEW_CONSOLE")


add_executable(umba-md-pp-view-console
    ${YAML_CPP_SOURCES}
    ${UMBA_MD_PP_COMMON_SOURCES}
    "${SRC_ROOT}/umba-md-pp/umba-md-pp-view.cpp"
    )
target_link_libraries(umba-md-pp-view-console ws2_32 iphlpapi shell32)
target_compile_definitions(umba-md-pp-view-console PRIVATE "UMBA_MD_PP_VIEW_CONSOLE")
#https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements


#target_compile_options(Framework PUBLIC -Wa,-mbig-obj)
#-flto




#include(CPack)
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

