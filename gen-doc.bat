@rem set BATCH_ROOT=--batch-output-path=C:\work\temp\mdpp-test
@set VARS=--set-var=INSERT_DOC_FILENAME_MACRO_TEST:by_macro2
@set EXCLUDE_DIRS=--batch-exclude-dir=_libs,libs,_lib,lib,tests,test,rc,_generators,_distr_conf,src,.msvc2019,boost,icons
@set EXCLUDE=%EXCLUDE_DIRS% --batch-exclude-files=*upper_inc.md*
@set COMMON=--overwrite --copy-images

umba-md-pp %COMMON% %BATCH_ROOT% %VARS% %EXCLUDE% --batch-scan-recurse=.

@rem C:\work\temp\mdpp-test\ 

