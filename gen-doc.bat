@rem set BATCH_ROOT=--batch-output-path=C:\work\temp\mdpp-test
umba-md-pp --overwrite %BATCH_ROOT% --copy-images --batch-exclude-dir=_libs,libs,_lib,lib,tests,test --batch-exclude-files=*upper_inc.md* --batch-scan-recurse=.

@rem C:\work\temp\mdpp-test\ 

