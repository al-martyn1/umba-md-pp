umba-2c -h > umba-2c-help.txt
umba-tr -h > umba-tr-help.txt

umba-tr @translations.rsp src/umba-md-pp/tr/translations.json
umba-2c -Y --text --string --size --header src/umba-md-pp/tr/translations.json src/umba-md-pp/tr/translations.json.h
rem --filename
rem --compress-ws
@rem --header