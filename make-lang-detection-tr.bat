@call %~dp0\.bat\find-gen-lang-detection-tr.bat

@if "%GEN_LANG_DETECTION_TR%"=="" goto ERR
%GEN_LANG_DETECTION_TR% >%~dp0\src\umba-md-pp\tr\lang-detection-tr.json

:ERR
@exit /B 1

:OK
