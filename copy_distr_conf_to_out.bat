@rem call tests\_set_pp.bat
@rem echo PPPATH: %PPPATH%

@set PPPATH_MSVC2019_X86=%~dp0\.out\msvc2019\x86
@set PPPATH_MSVC2019_X64=%~dp0\.out\msvc2019\x64
@set PPPATH_MSVC2022_X86=%~dp0\.out\msvc2022\x86
@set PPPATH_MSVC2022_X64=%~dp0\.out\msvc2022\x64

@copy /Y _distr_conf\conf\umba-md-pp.options    _distr_conf\conf\umba-md-pp-view.options
@copy /Y _distr_conf\conf\umba-md-pp.options    _distr_conf\conf\umba-md-pp-view-console.options

@rem xcopy /Y /S /E /I /F /R _distr_conf\conf\*  %PPPATH%\..\conf

@xcopy /Y /S /E /I /F /R _distr_conf\conf\*  %PPPATH_MSVC2019_X86%\conf
@xcopy /Y /S /E /I /F /R _distr_conf\conf\*  %PPPATH_MSVC2019_X64%\conf
@xcopy /Y /S /E /I /F /R _distr_conf\conf\*  %PPPATH_MSVC2022_X86%\conf
@xcopy /Y /S /E /I /F /R _distr_conf\conf\*  %PPPATH_MSVC2022_X64%\conf
