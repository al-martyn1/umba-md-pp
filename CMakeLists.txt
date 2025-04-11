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
set(UMBA_USE_BOOST      OFF)

include(${CMAKE_CURRENT_LIST_DIR}/.cmake/umba.cmake)



# message("BOOST_ROOT: ${BOOST_ROOT}")
# message("Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
# MinGW GCC 7.3 - Compiler version: 7.3.0
# MinGW GCC 13.2 - Compiler version: 13.2.0


include_directories(${LIB_ROOT})

add_subdirectory(${LIB_ROOT}/encoding)
add_subdirectory(${LIB_ROOT}/marty_cpp)
# add_subdirectory(${LIB_ROOT}/marty_pugixml)
add_subdirectory(${LIB_ROOT}/marty_format)
add_subdirectory(${LIB_ROOT}/marty_tr)
add_subdirectory(${LIB_ROOT}/marty_yaml_toml_json)
add_subdirectory(${LIB_ROOT}/nlohmann)    # https://json.nlohmann.me/integration/cmake/#external
#add_subdirectory(${LIB_ROOT}/pugixml)
add_subdirectory(${LIB_ROOT}/sfmt)
add_subdirectory(${LIB_ROOT}/umba)
add_subdirectory(${LIB_ROOT}/umba_tokenizer)
add_subdirectory(${LIB_ROOT}/yaml-cpp)

set(COMMON_LIBS encoding::encoding marty::cpp sfmt::sfmt umba::umba umba::tokenizer marty::format)
#set(PUGIXML_LIB pugixml::pugixml marty_pugixml::marty_pugixml)
set(JSON_YAML_LIB nlohmann_json::nlohmann_json yaml-cpp::yaml-cpp)


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
# add_compile_definitions("UMBA_TOKEN_TRIE_FIND_NEXT_BINARY_SEARCH_DISABLED")



file(GLOB_RECURSE sources   "${SRC_ROOT}/*.cpp")
file(GLOB_RECURSE headers   "${SRC_ROOT}/*.h")
file(GLOB_RECURSE resources "${SRC_ROOT}/*.rc")
umba_make_sources_tree("${SRC_ROOT}" "${sources}" "${headers}" "${resources}")


set(PLATFORM_LIBS)
if(WIN32) # Всё нормально, это про таргет
    list(APPEND PLATFORM_LIBS ws2_32 iphlpapi shell32)
endif()


set(LIBS_BASE                 "${PLATFORM_LIBS}" "${COMMON_LIBS}")
set(LIBS_BASE_JSON_YAML       "${PLATFORM_LIBS}" "${COMMON_LIBS}" "${JSON_YAML_LIB}")
#set(LIBS_BASE_JSON_YAML_XML   "${PLATFORM_LIBS}" "${COMMON_LIBS}" "${JSON_YAML_LIB}" "${PUGIXML_LIB}")
#set(LIBS_BASE_XML             "${PLATFORM_LIBS}" "${COMMON_LIBS}" "${PUGIXML_LIB}")

set(COMMON_CON_OPTIONS "BIGOBJ" "UTF8" "CONSOLE" "PEDANTIC" "WERROR" "ALLOW_UNUSED")
set(COMMON_GUI_OPTIONS "BIGOBJ" "UTF8" "WINDOWS" "PEDANTIC" "WERROR" "ALLOW_UNUSED")

set(MD_PP_SRC "${SRC_ROOT}/umba-md-pp")
set(SMPD_SRC  "${SRC_ROOT}/simple-drawing")

#---

add_executable(gen-lang-detection-tr "${MD_PP_SRC}/gen-lang-detection-tr.cpp" "${headers}")
target_link_libraries(gen-lang-detection-tr PRIVATE "${LIBS_BASE_JSON_YAML}")
umba_add_target_options(gen-lang-detection-tr ${COMMON_CON_OPTIONS}) # "${UNDEFINED_VAR}" # https://stackoverflow.com/questions/38117708/turn-list-into-arguments-for-cmake-function


add_executable(umba-md-pp "${MD_PP_SRC}/umba-md-pp.cpp" "${headers}")
target_link_libraries(umba-md-pp PRIVATE "${LIBS_BASE_JSON_YAML}")
umba_add_target_options(umba-md-pp ${COMMON_CON_OPTIONS})


add_executable(umba-md-pp-view "${MD_PP_SRC}/umba-md-pp-view.cpp" "${MD_PP_SRC}/_umba-md-pp-view.rc" "${headers}")
target_link_libraries(umba-md-pp-view PRIVATE "${LIBS_BASE_JSON_YAML}")
umba_add_target_options(umba-md-pp-view ${COMMON_GUI_OPTIONS})
target_compile_definitions(umba-md-pp-view PRIVATE "UMBA_MD_PP_VIEW" ) # "UMBA_EVENTS_LOG_ENABLE"


add_executable(umba-md-pp-view-console "${MD_PP_SRC}/umba-md-pp-view.cpp" "${MD_PP_SRC}/_umba-md-pp-view.rc" "${headers}")
target_link_libraries(umba-md-pp-view-console PRIVATE "${LIBS_BASE_JSON_YAML}")
umba_add_target_options(umba-md-pp-view-console ${COMMON_CON_OPTIONS})
target_compile_definitions(umba-md-pp-view-console PRIVATE "UMBA_MD_PP_VIEW" "UMBA_MD_PP_VIEW_CONSOLE")


add_executable(test_rtf_workarounds "${MD_PP_SRC}/test_rtf_workarounds.cpp" "${headers}")
target_link_libraries(test_rtf_workarounds PRIVATE "${LIBS_BASE}")
umba_add_target_options(test_rtf_workarounds ${COMMON_CON_OPTIONS})

#---

add_executable(smpd-test-001 "${SMPD_SRC}/smpd-test-001.cpp" "${headers}")
target_link_libraries(smpd-test-001 PRIVATE "${LIBS_BASE}")
umba_add_target_options(smpd-test-001 ${COMMON_CON_OPTIONS})




#https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#target-usage-requirements


#target_compile_options(Framework PUBLIC -Wa,-mbig-obj)
#-flto




#include(CPack)
#set(CPACK_PROJECT_NAME ${PROJECT_NAME})
#set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

