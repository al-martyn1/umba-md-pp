@call %~dp0\..\..\.bat\find-exe-in-out.bat test_006
@echo FOUND_EXE: %FOUND_EXE%
@set TEST_EXE=%FOUND_EXE%
@set LIB_ROOT=%~dp0\..\..\_libs
@set LIB_UMBA=%LIB_ROOT%\umba
@set LIB_MARTY_DECIMAL=%LIB_ROOT%\marty_decimal
@%TEST_EXE% %LIB_UMBA%\the.h                                      >the.txt
@%TEST_EXE% %LIB_UMBA%\stl_keil_initializer_list.h                >stl_keil_initializer_list.txt
@%TEST_EXE% %LIB_UMBA%\stl_keil_type_traits.h                     >stl_keil_type_traits.txt
@%TEST_EXE% %LIB_UMBA%\string_plus.h                              >string_plus.txt
@%TEST_EXE% %LIB_UMBA%\rgbquad.h                                  >rgbquad.txt

@%TEST_EXE% %LIB_MARTY_DECIMAL%\tests\src\regression_tests.cpp    >regression_tests.txt
@rem %TEST_EXE% %LIB_MARTY_DECIMAL%\                             >
@rem %TEST_EXE% %LIB_MARTY_DECIMAL%\                             >

@rem %TEST_EXE% %LIB_UMBA%\                             >
@rem %TEST_EXE% %LIB_UMBA%\                             >
