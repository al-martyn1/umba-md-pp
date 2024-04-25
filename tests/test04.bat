@if exist %~dp0\_setup.bat @call %~dp0\_setup.bat
%PP% %CMNOPTS% --processing-options=numeric-sections,generateSectionId test04.md_ test04.md

