@call %~dp0.bat\setup_out_root_paths.bat

@rem %OUTROOTPATH%\Debug\umba-md-pp         --help > umba-md-pp-help.txt
%OUTROOTPATH%\Debug\umba-md-pp --md    --help > doc\umba-md-pp-help.md_
@rem %OUTROOTPATH%\Debug\umba-md-pp --bash  --help > umba-md-pp-help.bash
@rem %OUTROOTPATH%\Debug\umba-md-pp --clink --help > umba-md-pp-help.clink



