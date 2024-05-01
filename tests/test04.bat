@if exist %~dp0\_setup.bat @call %~dp0\_setup.bat
@set METAOPTS=--serialize-meta-tags=title,descripion
@rem author
@set OPTS=--processing-options=meta-data,numeric-sections,generateSectionId,generate-toc --numeric-sections-max-level=2 --toc-max-level=3
%PP% %CMNOPTS% %OPTS% %METAOPTS% test04.md_ test04.md




