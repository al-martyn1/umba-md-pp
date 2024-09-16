@if exist "%~dp0\set_sln.bat"    @call "%~dp0\set_sln.bat"
@call "%~dp0\update_md-pp-view-conf-options.bat"
@rem
@if "%DEPLOY_CONFIG%"=="" @set DEPLOY_CONFIG=Release
@if "%DEPLOY_ROOT%"==""   @set DEPLOY_ROOT=%UMBA_TOOLS%
@rem
@if "%DEPLOY_ROOT%"=="" goto DEPLOY_ROOT_NOT_SET
@goto DEPLOY_ROOT_IS_SET
:DEPLOY_ROOT_NOT_SET
@echo DEPLOY_ROOT environmetnt variable is not set, UMBA_TOOLS not set too
@exit /B 1
:DEPLOY_ROOT_IS_SET
@rem
@set DEPLOY_BINS=%SLN% umba-md-pp-view
@rem
@call "%~dp0.bat\deploy_impl.bat"
@rem
@rem
@if "%UMBA_TOOLS%"=="" goto DONT_REG_HANDLER
%UMBA_TOOLS%\bin\umba-md-pp-view.exe --register-view-handler
:DONT_REG_HANDLER
