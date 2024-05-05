@if exist %~dp0\_setup.bat @call %~dp0\_setup.bat
@set GEN=%PPPATH%\gen-lang-detection-tr.exe
%GEN% >gen-lang-detection-tr.json

