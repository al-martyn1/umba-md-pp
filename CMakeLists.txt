cmake_minimum_required(VERSION 3.21.0)
project(umba-md-pp VERSION 0.1.0 LANGUAGES C CXX)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(UMBA_STATIC_RUNTIME ON)
set(UMBA_USE_BOOST      ON)

include(${CMAKE_CURRENT_LIST_DIR}/.cmake/umba.cmake)



# message("BOOST_ROOT: ${BOOST_ROOT}")
# message("Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
# MinGW GCC 7.3 - Compiler version: 7.3.0
# MinGW GCC 13.2 - Compiler version: 13.2.0


include_directories(${LIB_ROOT})

add_subdirectory(${LIB_ROOT}/encoding)
add_subdirectory(${LIB_ROOT}/marty_cpp)
# add_subdirectory(${LIB_ROOT}/marty_pugixml)
add_subdirectory(${LIB_ROOT}/marty_tr)
add_subdirectory(${LIB_ROOT}/marty_yaml_toml_json)
add_subdirectory(${LIB_ROOT}/nlohmann)    # https://json.nlohmann.me/integration/cmake/#external
#add_subdirectory(${LIB_ROOT}/pugixml)
add_subdirectory(${LIB_ROOT}/sfmt)
add_subdirectory(${LIB_ROOT}/umba)
add_subdirectory(${LIB_ROOT}/yaml-cpp)


umba_configure_boost("STATIC_LIBS" "MULTITHREADED" "${STATIC_RUNTIME}")


#-O0 
#set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
#set(CMAKE_CXX_FLAGS_RELEASE "-O3")

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    #-fdiagnostics-color=always
	#set(CMAKE_CXX_FLAGS "-Wa,-mbig-obj -flto -flto-report  -fno-rtti -Wall -Wextra -Werror -pedantic-errors -Wctor-dtor-privacy -Wextra-semi -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls -Wsign-promo")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  #-fignore-unknown-pragmas
  set(CMAKE_CXX_FLAGS "-Woverloaded-virtual -Wno-ctor-dtor-privacy -Wsign-promo")
  #add_compile_options(-fdiagnostics-color=always)
  #add_compile_options(-Wa,-mbig-obj)
  # -fno-rtti
  #add_compile_options(-Wall -Wno-unknown-pragmas -Wduplicated-branches -Wduplicated-cond -Wlogical-op -Wredundant-decls -Wno-unused-function)
  #add_compile_options(-Wno-unknown-pragmas)
  #-Wno-unknown-pragmas
  #-Wnon-virtual-dtor -Wctor-dtor-privacy
  #add_compile_options(-w)
  #add_compile_options(-w -Wa,-mbig-obj)
  #-fdiagnostics-color=always 
  #-Wall   
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
  # using Intel C++
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  # using Visual Studio C++
endif()



##set_source_files_properties("${LIBS_ROOT}/encoding/compact_enc_det/compact_enc_det.cc"           PROPERTIES LANGUAGE C )


add_compile_definitions("YAML_CPP_STATIC_DEFINE")
add_compile_definitions("USE_UMBA_LOUT_COUT")
add_compile_definitions("UMBA_TOKENIZER_DISABLE_UMBA_TOKENIZER_GET_CHAR_CLASS_FUNCTION")



file(GLOB_RECURSE sources   "${SRC_ROOT}/*.cpp")
file(GLOB_RECURSE headers   "${SRC_ROOT}/*.h")
file(GLOB_RECURSE resources "${SRC_ROOT}/*.rc")
umba_make_sources_tree("${SRC_ROOT}" "${sources}" "${headers}" "${resources}")


set(PLATFORM_LIBS)
if(WIN32) # Всё нормально, это про таргет
    list(APPEND PLATFORM_LIBS ws2_32 iphlpapi shell32)
endif()

set(COMMON_SRC encoding::encoding marty_cpp::marty_cpp sfmt::sfmt umba::umba)
#set(PUGIXML_SRC pugixml::pugixml marty_pugixml::marty_pugixml)
set(JSON_YAML_SRC nlohmann_json::nlohmann_json yaml-cpp::yaml-cpp)


add_executable(gen-lang-detection-tr "${SRC_ROOT}/umba-md-pp/gen-lang-detection-tr.cpp" "${headers}")
umba_add_target_options(gen-lang-detection-tr "${UNDEFINED_VAR}" "BIGOBJ" "CONSOLE" "UTF8" )
target_link_libraries(gen-lang-detection-tr PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}" "${JSON_YAML_SRC}")


add_executable(umba-md-pp "${SRC_ROOT}/umba-md-pp/umba-md-pp.cpp" "${headers}")
umba_add_target_options(umba-md-pp "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(umba-md-pp PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}" "${JSON_YAML_SRC}")


add_executable(umba-md-pp-view "${SRC_ROOT}/umba-md-pp/umba-md-pp-view.cpp" "${SRC_ROOT}/umba-md-pp/_umba-md-pp-view.rc" "${headers}")
umba_add_target_options(umba-md-pp-view "BIGOBJ" "WINDOWS" "UTF8")
target_compile_definitions(umba-md-pp-view PRIVATE "UMBA_MD_PP_VIEW")
target_link_libraries(umba-md-pp-view PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}" "${JSON_YAML_SRC}")


add_executable(umba-md-pp-view-console "${SRC_ROOT}/umba-md-pp/umba-md-pp-view.cpp" "${SRC_ROOT}/umba-md-pp/_umba-md-pp-view.rc" "${headers}")
umba_add_target_options(umba-md-pp-view-console "BIGOBJ" "CONSOLE" "UTF8")
target_compile_definitions(umba-md-pp-view-console PRIVATE "UMBA_MD_PP_VIEW" "UMBA_MD_PP_VIEW_CONSOLE")
target_link_libraries(umba-md-pp-view-console PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}" "${JSON_YAML_SRC}")


add_executable(test_rtf_workarounds "${SRC_ROOT}/umba-md-pp/test_rtf_workarounds.cpp" "${headers}")
umba_add_target_options(test_rtf_workarounds "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_rtf_workarounds PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


add_executable(test_001 "${SRC_ROOT}/umba-md-pp/test_001.cpp" "${headers}")
umba_add_target_options(test_001 "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_001 PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


add_executable(test_002 "${SRC_ROOT}/umba-md-pp/test_002.cpp" "${headers}")
umba_add_target_options(test_002 "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_002 PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


add_executable(test_003 "${SRC_ROOT}/umba-md-pp/test_003.cpp" "${headers}")
umba_add_target_options(test_003 "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_003 PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


add_executable(test_004 "${SRC_ROOT}/umba-md-pp/test_004.cpp" "${headers}")
umba_add_target_options(test_004 "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_004 PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


add_executable(test_005 "${SRC_ROOT}/umba-md-pp/test_005.cpp" "${headers}")
umba_add_target_options(test_005 "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_005 PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


add_executable(test_006 "${SRC_ROOT}/umba-md-pp/test_006.cpp" "${headers}")
umba_add_target_options(test_006 "BIGOBJ" "CONSOLE" "UTF8")
target_link_libraries(test_006 PRIVATE "${PLATFORM_LIBS}" "${COMMON_SRC}")


#https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements


#target_compile_options(Framework PUBLIC -Wa,-mbig-obj)
#-flto




#include(CPack)
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

