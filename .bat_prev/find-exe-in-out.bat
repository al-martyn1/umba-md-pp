@call %~dp0\setup_out_pp_root_paths.bat
@set LOOKUP_FOR=%1
@rem https://superuser.com/questions/1569197/what-is-the-meaning-of-tilde-in-batch-variables
@FOR %%i IN (%PPROOTPATHS%) DO @(
    @FOR %%j IN (Debug Release) DO @(
        @rem echo %%~i\%%j
        @IF EXIST "%%~i\%%j\%LOOKUP_FOR%.exe" @(
            @set FOUND_EXE="%%~i\%%j\%LOOKUP_FOR%.exe"
            @goto FOUND
)
)
)
:FOUND
