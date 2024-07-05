cd %~dp0\boost
call bootstrap.bat gcc
cd %~dp0\

@rem https://www.boost.org/doc/libs/1_56_0/more/getting_started/unix-variants.html#or-build-custom-binaries
@rem  b2 --build-dir=/tmp/build-boost toolset=gcc stage
@rem That will build static and shared non-debug multi-threaded variants of the libraries. To build all variants, pass the additional option, “--build-type=complete”.
