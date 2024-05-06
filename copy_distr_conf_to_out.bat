@call tests\_set_pp.bat
@copy /Y _distr_conf\conf\umba-md-pp.options    _distr_conf\conf\umba-md-pp-view.options
@copy /Y _distr_conf\conf\umba-md-pp.options    _distr_conf\conf\umba-md-pp-view-console.options
xcopy /Y /S /E /I /F /R _distr_conf\conf\*  %PPPATH%\..\conf