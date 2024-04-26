@if exist %~dp0\_setup.bat @call %~dp0\_setup.bat
%PP% %CMNOPTS% --processing-options=numeric-sections,generateSectionId test04.md_ test04.md
copy /Y test04.md generated_toc_sample.md



