umba-2c -h > umba-2c-help.txt
umba-tr -h > umba-tr-help.txt

@call make-lang-detection-tr.bat

umba-tr @%~dp0\translations.rsp %~dp0\_src/umba-md-pp/tr/translations.json  2>%~dp0\make_translations.log
umba-2c -Y --text --string --size --header %~dp0\_src/umba-md-pp/tr/translations.json %~dp0\_src/umba-md-pp/tr/translations.json.h
rem --filename
rem --compress-ws
@rem --header