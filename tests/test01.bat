@if exist %~dp0\_setup.bat @call %~dp0\_setup.bat
%PP% %CMNOPTS% test01.md_ test01.md > test01.log

