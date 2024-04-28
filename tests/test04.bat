@if exist %~dp0\_setup.bat @call %~dp0\_setup.bat
@set OPTS=--processing-options=numeric-sections,generateSectionId,generate-toc --numeric-sections-max-level=2 --toc-max-level=3
%PP% %CMNOPTS% %OPTS% test04.md_ test04.md




