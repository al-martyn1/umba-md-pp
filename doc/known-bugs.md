
  - [X] проверить, относительно чего считаются пути в опции --add-examples-path - если опция встречается 
    в .options-файле, то относительные пути должны отсчитываться от него
  - [X] по умолчанию в --set-insert-options не стоит опция fail, и проблемы молча съедаются. Может, я давно не обновлял дистр?
  - [X] по умолчанию в --set-insert-options не стоит опция path - неудобно
  - [X] по умолчанию в --set-insert-options не стоит опция filenameLineNo - неудобно
  - [ ] сделать тип генерируемого конента - --generation-target=rtf, и в этом режиме, если --target-renderer=doxygen, устанавливать
    опцию ProcessingOptions::parse-github-alerts (но её можно установить и ручками). Данная опция должна делать обычный параграф, и вставлять
    слова Note, Tip, Important, Warning, Caution или их локализованные варианты.
    https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#alerts
    https://docs.github.com/en/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#alerts
  - [ ] Уметь парсить обозначения языков, включая их естественные названия (без учета регистра), превращая их в идентификаторы. По этим идентификаторам
    мы и будем вставлять локализованные Note, Tip, Important, Warning, Caution (будет группа github-alerts).
  - [ ] По этому lang-id также будем задавать язык для доксигена
  - [ ] Язык документа можно определять по тэгу Language в документе, или новой опцией --document-language=ru, --lang=ru
  - [ ] если не задан выходной файл, проверяем входной файл. Если начинается на точку, то просто убираем лидирующую точку. Если расширение начинается 
    или заканчивается на '_' - убираем этот символ
  - [ ] бага в генерации id для секций под доксиген - вместо подчеркивания появляется символ 0x7F
  - [ ] генерация имени файла по имени документа - не забываем, что char у нас знаковый



