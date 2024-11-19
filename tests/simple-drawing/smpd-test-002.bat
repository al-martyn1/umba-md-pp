@call %~dp0find_test_exe.bat smpd-test-002
@rem call %~dp0setup_test_files.bat
@set TEST_FILES=%~dp0test02.ptg
%TEST_EXE% %TEST_FILES%
