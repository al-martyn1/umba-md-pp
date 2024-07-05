include_guard(GLOBAL)


if(STATIC_RUNTIME)
    # For use as ${STATIC_RUNTIME} when calling umba_add_target_options
    set(STATIC_RUNTIME "STATIC_RUNTIME")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
else()
    message("Default Dynamic runtime used")
endif()


### Boost
# Defaults for Boost
set(Boost_USE_STATIC_LIBS OFF) 
set(Boost_USE_MULTITHREADED ON)  
set(Boost_USE_STATIC_RUNTIME OFF) 


# D:\CMake\share\cmake-3.29\Modules\FindBoost.cmake
# https://cmake.org/cmake/help/latest/module/FindBoost.html
# https://stackoverflow.com/questions/6646405/how-do-you-add-boost-libraries-in-cmakelists-txt

# Install
# https://www.youtube.com/watch?v=D640-ZJmBWM
# How to link C++ program with Boost using CMake - https://stackoverflow.com/questions/3897839/how-to-link-c-program-with-boost-using-cmake
# Using Boost with CMake - https://discourse.cmake.org/t/using-boost-with-cmake/6299
# Работа с Boost и CMake под Windows - https://zhitenev.ru/rabota-s-boost-i-cmake-pod-windows/
# Boost CMake support infrastructure - https://github.com/boostorg/cmake
#   git clone --recurse-submodules https://github.com/boostorg/boost  
# Building and Installing the Library - https://www.boost.org/doc/libs/1_85_0/libs/regex/doc/html/boost_regex/install.html

# Cross-compilation - https://www.boost.org/build/doc/html/bbv2/tasks/crosscompile.html
# Builtin tools - https://www.boost.org/build/doc/html/bbv2/reference/tools.html
# Or, Build Custom Binaries - https://www.boost.org/doc/libs/1_56_0/more/getting_started/unix-variants.html#or-build-custom-binaries

# How do I use a different compiler - https://gitlab.kitware.com/cmake/community/-/wikis/FAQ#how-do-i-use-a-different-compiler
# #set(CMAKE_CXX_LINK_EXECUTABLE ${CMAKE_CXX}) # не нужно
# set(CMAKE_LINKER ${CMAKE_CXX})



# STATIC_LIBS
# MULTITHREADED - no mean, option is set to ON by default
# SINGLETHREADED - single threaded not supported by MSVC?
# STATIC_RUNTIME
function(umba_configure_boost)

    math(EXPR maxArgN "${ARGC} - 1")

    foreach(index RANGE 0 ${maxArgN} 1)

        if(${ARGV${index}} STREQUAL "STATIC_LIBS")
            set(Boost_USE_STATIC_LIBS ON) 
        elseif(${ARGV${index}} STREQUAL "MULTITHREADED")
            # no mean, option is set to ON by default
        elseif(${ARGV${index}} STREQUAL "SINGLETHREADED")
            set(Boost_USE_MULTITHREADED OFF)  
        elseif(${ARGV${index}} STREQUAL "STATIC_RUNTIME")
            set(Boost_USE_STATIC_RUNTIME ON) 
        endif()

    endforeach()

endfunction()


### Target options

# UNICODE/MBCS/DBCS
# https://learn.microsoft.com/ru-ru/cpp/text/support-for-multibyte-character-sets-mbcss?view=msvc-170
# https://learn.microsoft.com/ru-ru/cpp/text/mbcs-support-in-visual-cpp?view=msvc-170
# https://learn.microsoft.com/ru-ru/cpp/text/mbcs-programming-tips?view=msvc-170

# "UNICODE" "CONSOLE" "WINDOWS" "BIGOBJ" "UTF8"
# "SRCUTF8"/"UTF8SRC"/"SRC_UTF8"/"UTF8_SRC"
# "STATIC_RUNTIME"
function(umba_add_target_options TARGET)

    # https://cmake.org/cmake/help/latest/variable/CMAKE_LANG_COMPILER_ID.html

    # https://jeremimucha.com/2021/02/cmake-functions-and-macros/

    #math(EXPR indices "${ARGC} - 1")
    #foreach(index RANGE ${indices})

    math(EXPR maxArgN "${ARGC} - 1")
    foreach(index RANGE 1 ${maxArgN} 1)

        #message("  ARGV${index}: ${ARGV${index}}")

        if(${ARGV${index}} STREQUAL "UNICODE")
            if(WIN32)

                # Common for all
                target_compile_definitions(${TARGET} PRIVATE "UNICODE" "_UNICODE")

                if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                    message(NOTICE "Add UNICODE options for Clang")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                    target_compile_options(${TARGET} PRIVATE "-municode")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                    message(NOTICE "Add UNICODE options for Intel")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    #message(NOTICE "Add UNICODE options for MSVC")
                endif()
            endif()

        elseif(${ARGV${index}} STREQUAL "SRCUTF8" OR ${ARGV${index}} STREQUAL "UTF8SRC" OR ${ARGV${index}} STREQUAL "UTF8_SRC" OR ${ARGV${index}} STREQUAL "SRC_UTF8")
            if(WIN32)
                if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                    message(NOTICE "Add SRCUTF8 options for Clang")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                    # !!! conversion from UTF-8 to UTF-8 -finput-charset=UTF-8 not supported by iconv
                    # target_compile_options(${TARGET} PRIVATE "-finput-charset=UTF-8")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                    message(NOTICE "Add SRCUTF8 options for Intel")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    # /utf-8, that sets both /source-charset:utf-8 and /execution-charset:utf-8.
                    target_compile_options(${TARGET} PRIVATE "/source-charset:utf-8")
                endif()
            endif()

        elseif(${ARGV${index}} STREQUAL "UTF8")
            if(WIN32)
                if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                    message(NOTICE "Add UTF8 options for Clang")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                    # !!! conversion from UTF-8 to UTF-8 -finput-charset=UTF-8 not supported by iconv
                    # target_compile_options(${TARGET} PRIVATE "-fexec-charset=UTF-8 -finput-charset=UTF-8")  # https://gcc.gnu.org/onlinedocs/gcc/Option-Summary.html
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                    message(NOTICE "Add UTF8 options for Intel")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    target_compile_options(${TARGET} PRIVATE "/utf-8")
                endif()
            endif()

        elseif(${ARGV${index}} STREQUAL "CONSOLE")
            if(WIN32)

                # Common for all
                target_compile_definitions(${TARGET} PRIVATE "CONSOLE" "_CONSOLE")

                if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                    message(NOTICE "Add CONSOLE options for Clang")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                    target_compile_options(${TARGET} PRIVATE "-mconsole")
                    target_link_options(${TARGET} PRIVATE "-Wl,--subsystem,console")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                    message(NOTICE "Add CONSOLE options for Intel")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    target_link_options(${TARGET} PRIVATE "/SUBSYSTEM:CONSOLE")
                endif()
            endif()

        elseif(${ARGV${index}} STREQUAL "WINDOWS")
            if(WIN32)

                # Common for all
                target_compile_definitions(${TARGET} PRIVATE "WINDOWS" "_WINDOWS")

                if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                    message(NOTICE "Add WINDOWS options for Clang")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                    target_compile_options(${TARGET} PRIVATE "-mwindows")
                    target_link_options(${TARGET} PRIVATE "-Wl,--subsystem,windows")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                    message(NOTICE "Add WINDOWS options for Intel")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    target_link_options(${TARGET} PRIVATE "/SUBSYSTEM:WINDOWS")
                endif()
            endif()

        elseif(${ARGV${index}} STREQUAL "BIGOBJ")
            if(WIN32)
                if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                    message(NOTICE "Add BIGOBJ options for Clang")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                    target_compile_options(${TARGET} PRIVATE "-Wa,-mbig-obj")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                    message(NOTICE "Add BIGOBJ options for Intel")
                elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                    target_compile_options(${TARGET} PRIVATE "/bigobj")
                endif()
            endif()

        elseif(${ARGV${index}} STREQUAL "STATIC_RUNTIME")
            if(WIN32)
                # Под винду разве не все кмпиляторы используют MSVC ABI?
                set_property(TARGET ${TARGET} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

                #if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
                #    message(NOTICE "Add BIGOBJ options for Clang")
                #elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
                #    target_compile_options(${TARGET} PRIVATE "-Wa,-mbig-obj")
                #elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Intel")
                #    message(NOTICE "Add BIGOBJ options for Intel")
                #elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
                #    set_property(TARGET ${TARGET} PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
                #endif()
            endif()

        endif() # if(${ARGV${index}}

    endforeach()

endfunction()


