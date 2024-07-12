@rem set BATCH_ROOT=--batch-output-path=C:\work\temp\mdpp-test
@set VARS=--set-var=INSERT_DOC_FILENAME_MACRO_TEST:by_macro2

umba-md-pp --overwrite %BATCH_ROOT% %VARS% --copy-images --batch-exclude-dir=_libs,libs,_lib,lib,tests,test --batch-exclude-files=*upper_inc.md* --batch-scan-recurse=.

@rem C:\work\temp\mdpp-test\ 

