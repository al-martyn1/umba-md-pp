@rem Copy MDPP configs to MDPP View configs
@call "%~dp0\update_md-pp-view-conf-options.bat"
@rem
@rem Configure OUTROOTPATHS var
@call "%~dp0\.bat\setup_out_root_paths.bat"

@echo OUTROOTPATHS=%OUTROOTPATHS%

@FOR %%i IN (%OUTROOTPATHS%) DO @(
    @FOR %%j IN (%PLATFORMS%) DO @(
        @IF EXIST "%OUTROOTPATH_BASE%\%%~i\%%~j" @(
            echo Coying "%~dp0\conf\*" to "%%~i\%%~j\\conf"
            xcopy /Y /S /E /I /F /R "%~dp0\conf\*"  "%OUTROOTPATH_BASE%\%%~i\%%~j\\conf"
)
)
)
