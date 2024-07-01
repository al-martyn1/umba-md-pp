@rem call tests\_set_pp.bat
@rem echo PPPATH: %PPPATH%

@set PPPATH_MSVC2019_X86=%~dp0\.out\msvc2019\x86
@set PPPATH_MSVC2019_X64=%~dp0\.out\msvc2019\x64
@set PPPATH_MSVC2022_X86=%~dp0\.out\msvc2022\x86
@set PPPATH_MSVC2022_X64=%~dp0\.out\msvc2022\x64

@set PPPATH_GCC_73_X86=%~dp0\.out\GCC 7.3.0 i686-w64-mingw32\win32-x86
@rem set PPPATH_GCC_73_X64=%~dp0\.out\msvc2022\x64

@call %~dp0\update_md-pp-view-conf-options.bat

@if exist %PPPATH_MSVC2019_X86%   xcopy /Y /S /E /I /F /R %~dp0\_distr_conf\conf\*  %PPPATH_MSVC2019_X86%\conf
@if exist %PPPATH_MSVC2019_X64%   xcopy /Y /S /E /I /F /R %~dp0\_distr_conf\conf\*  %PPPATH_MSVC2019_X64%\conf
@if exist %PPPATH_MSVC2022_X86%   xcopy /Y /S /E /I /F /R %~dp0\_distr_conf\conf\*  %PPPATH_MSVC2022_X86%\conf
@if exist %PPPATH_MSVC2022_X64%   xcopy /Y /S /E /I /F /R %~dp0\_distr_conf\conf\*  %PPPATH_MSVC2022_X64%\conf
@if exist "%PPPATH_GCC_73_X86%"   xcopy /Y /S /E /I /F /R %~dp0\_distr_conf\conf\*  "%PPPATH_GCC_73_X86%\conf"
