@call %~dp0find_test_exe.bat smpd-test-003
@call %~dp0setup_test_files.bat

%TEST_EXE% %TEST_FILES%
