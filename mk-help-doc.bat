@call %~dp0.bat\setup_out_root_paths.bat
@rem set UMBA_MD_PP=%OUTROOTPATH%\Debug\umba-md-pp
@set UMBA_MD_PP=umba-md-pp

@rem %UMBA_MD_PP%         --help         > doc\umba-md-pp-options.txt
%UMBA_MD_PP% --md    --help         > doc\umba-md-pp-options.md_
@rem %UMBA_MD_PP%         --version      > doc\md_\umba-md-pp-version.txt
%UMBA_MD_PP%         --version-info > doc\umba-md-pp-version-info.txt

@rem %UMBA_MD_PP% --bash  --help > umba-md-pp-help.bash
@rem %UMBA_MD_PP% --clink --help > umba-md-pp-help.clink



