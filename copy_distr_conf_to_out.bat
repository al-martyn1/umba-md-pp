@call "%~dp0\update_md-pp-view-conf-options.bat"
@call "%~dp0\.bat\setup_out_root_paths.bat"

@echo OUTROOTPATHS=%OUTROOTPATHS%

@FOR %%i IN (%OUTROOTPATHS%) DO @(
        @IF EXIST "%%~i\" @(
            echo Coying "%~dp0\_distr_conf\conf\*" to "%%~i\\conf"
            xcopy /Y /S /E /I /F /R "%~dp0\_distr_conf\conf\*"  "%%~i\\conf"
)
)
