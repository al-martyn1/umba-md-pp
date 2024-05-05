@if exist %~dp0\tests\_setup.bat @call %~dp0\tests\_setup.bat
@remset GEN=.out\msvc2019\x86\Debug\gen-lang-detection-tr.exe
@rem     .out\msvc2019\x86\Debug\gen-lang-detection-tr.exe
@set GEN=%PPPATH%\gen-lang-detection-tr.exe
%GEN% >src\umba-md-pp\tr\lang-detection-tr.json

