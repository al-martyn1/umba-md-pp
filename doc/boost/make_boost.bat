@set TOLCHAIN=GCC 13.2.0 x86_64-w64-mingw32
@set CC=C:\w64devkit\bin\gcc.exe
@set CXX=C:\w64devkit\bin\g++.exe
@set LINKER=C:\w64devkit\bin\g++.exe
@set CXX_STD=17
@call %~dp0\make_boost_impl.bat
@set CXX_STD=20
@call %~dp0\make_boost_impl.bat


@set TOLCHAIN=
@set CC=
@set CXX=
@set LINKER="${CMAKE_CXX}"
@set TOOLCHAIN_FILE=C:\work\github_3dp\stm32-cmake\cmake\stm32_gcc.cmake
