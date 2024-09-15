@call %~dp0.bat\setup_out_root_paths.bat

@rem %OUTROOTPATH%\Debug\umba-md-pp         --help         > doc\md_\umba-md-pp-options.txt
%OUTROOTPATH%\Debug\umba-md-pp --md    --help         > doc\umba-md-pp-options.md_
@rem %OUTROOTPATH%\Debug\umba-md-pp         --version      > doc\md_\umba-md-pp-version.txt
@rem %OUTROOTPATH%\Debug\umba-md-pp         --version-info > doc\md_\umba-md-pp-version-info.txt

@rem %OUTROOTPATH%\Debug\umba-md-pp --bash  --help > umba-md-pp-help.bash
@rem %OUTROOTPATH%\Debug\umba-md-pp --clink --help > umba-md-pp-help.clink



