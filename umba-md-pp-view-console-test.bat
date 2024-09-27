@set CONFIG=Debug
@rem set CONFIG=Release
@call %~dp0.bat\setup_out_root_paths.bat
@rem echo %OUTROOTPATH%
@set MDPPVIEWCON="%OUTROOTPATH%\%CONFIG%\umba-md-pp-view-console.exe"
rem "C:\work\github\umba-tools\umba-md-pp\.out\GCC 7.3 x86 MinGW\win32-x86\Debug\"
@%MDPPVIEWCON% --info=+all,-plantuml,-opt-files README.md_ >umba-md-pp-view-console-test.log 2>&1