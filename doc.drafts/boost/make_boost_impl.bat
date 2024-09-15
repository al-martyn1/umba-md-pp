@set BOOST_EXCLUDE_LIBRARIES=-D BOOST_EXCLUDE_LIBRARIES=simple_event_log

@set TOOLCHAIN_OPTIONS=
@if "%TOOLCHAIN_FILE%" neq "" set TOOLCHAIN_OPTIONS=%TOOLCHAIN_OPTIONS% --toolchain "%TOOLCHAIN_FILE%"
@if "%CC%" neq ""             set TOOLCHAIN_OPTIONS=%TOOLCHAIN_OPTIONS% -D CMAKE_C_COMPILER="%CC%"
@if "%CXX%" neq ""            set TOOLCHAIN_OPTIONS=%TOOLCHAIN_OPTIONS% -D CMAKE_CXX_COMPILER="%CXX%"
@if "%LINKER%" neq ""         set TOOLCHAIN_OPTIONS=%TOOLCHAIN_OPTIONS% -D CMAKE_LINKER="%LINKER%"
@if "%CXX_STD%" neq ""         set TOOLCHAIN_OPTIONS=%TOOLCHAIN_OPTIONS% -D CMAKE_CXX_STANDARD="%CXX_STD%"


@set BUILD_PATH=__build\%TOLCHAIN%
@if "%CXX_STD%" neq "" @set BUILD_PATH=%BUILD_PATH%\Cxx%CXX_STD%


if not exist boost\__build mkdir boost\__build
cd boost
cmake %TOOLCHAIN_OPTIONS% -G "MinGW Makefiles" -B "%BUILD_PATH%" %BOOST_EXCLUDE_LIBRARIES%
cmake --build "%BUILD_PATH%" --config Debug   --target install 
cmake --build "%BUILD_PATH%" --config Release --target install 
cd ..


@rem toolset https://cmake.org/cmake/help/latest/variable/CMAKE_GENERATOR_TOOLSET.html

@rem :PERFORM_GENERATE
@rem cmake -G %GENERATOR%  -A %ARCH% -B .out\%TOOLSET%\%ARCH_PATH%
@rem @if ERRORLEVEL 1 goto ERR
@rem @goto END
@rem 
@rem @rem https://ss64.com/nt/errorlevel.html
@rem :PERFORM_BUILD
@rem cmake --build .out\%TOOLSET%\%ARCH_PATH% --config Debug   --target ALL_BUILD -j 8 -- 
@rem @rem echo %errorlevel%
@rem @if ERRORLEVEL 1 goto ERR
@rem cmake --build .out\%TOOLSET%\%ARCH_PATH% --config Release --target ALL_BUILD -j 8 --
@rem @if ERRORLEVEL 1 goto ERR
@rem @goto END



