@call mk-help.bat
@call %~dp0\tests\_setup.bat

@set PROCESSING_OPTIONS=--processing-options=numeric-sections,generateSectionId,generate-toc
@set SEC_LEVELS=--numeric-sections-max-level=2 --toc-max-level=3
@set SNIPPETS_PATHS=%~dp0\..;%~dp0\snippets
@set INSERT_OPTIONS=--set-insert-options=filename,path,filenameLineNo,fail,trim-arround,no-snippet-options
@set OPTS=--overwrite --add-examples-path=%SNIPPETS_PATHS% %PROCESSING_OPTIONS% %SEC_LEVELS% %INSERT_OPTIONS%

@%PP% %OPTS% %OPTS% README.md_ README.md

@call clean-help.bat