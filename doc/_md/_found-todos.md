---
Generator: Umba Brief Scanner
---

# _libs/marty_format/impl

- [ ] `[_libs/marty_format/impl/internal_processFormatValuesImpl.h:437]`
  Если есть BigInt, то форматируем через него, если у нас "целый" форматный
  символ

- [ ] `[_libs/marty_format/impl/internal_processFormatValuesImpl.h:442]`
  Если BigInt'а нет, то форматируем через std::uint64_t getAsUint64() const, не
  забывая про знак

- [ ] `[_libs/marty_format/impl/internal_processFormatValuesImpl.h:1275]`
  Если задано сделать escape для строки то надо сделать escape, но не длиннее,
  чем макс ширина

- [ ] `[_libs/marty_format/impl/internal_processFormatValuesImpl.h:1287]`
  Тут используем UTF итераторы явно, если нам не нужен UTF, надо будет обрубить
  эту возможность



# _libs/marty_rich_text

- [ ] `[_libs/marty_rich_text/Para.h:232]`
  isImage - содержит только img/image и ничего больше. Также завести ещё одно
  значение EParaType для Image

- [ ] `[_libs/marty_rich_text/PugixmlFb2Parser_DocumentBody.h:100]`
  Пока изображение представляем, как пустой параграф, но надо доделать

- [ ] `[_libs/marty_rich_text/PugixmlFb2Parser_DocumentBody.h:632]`
  std::vector<Block>          annotation

- [ ] `[_libs/marty_rich_text/PugixmlFb2Parser_DocumentHeader.h:149]`
  Тут надо бы распарсить содержимое document-info, но это пока никому не нужно

- [ ] `[_libs/marty_rich_text/PugixmlGenericRichParaWalker.h:231]`
  Отпроцессить следующие тэги

- [ ] `[_libs/marty_rich_text/PugixmlGenericRichParaWalker.h:252]`
  Тут может быть нюанс, что идут два одинаковых тэга с доп параметрами, их надо
  проверить, пока не сделано (теперь вроде сделано) Или вложенные тэги - хотя,
  вложенность, по идее, схлопывается

- [ ] `[_libs/marty_rich_text/PugixmlGenericRichParaWalker.h:308]`
  Тут может быть нюанс, что идут два одинаковых тэга с доп параметрами, их надо
  проверить, пока не сделано (теперь вроде сделано) Или вложенные тэги - хотя,
  вложенность, по идее, схлопывается



# _src/umba-md-pp

- [ ] `[_src/umba-md-pp/code-options-database.h:117]`
  Пока у нас строки начала/окончания блока односимвольные, но надо переделать,
  желательно везде

- [ ] `[_src/umba-md-pp/processing.h:245]`
  Надо бы ещё принудительно флэттенизировать урлы, которые выходят за пределы
  каталога с документом UMBA_USED(flattenImageLinks);

- [ ] `[_src/umba-md-pp/processing.h:274]`
  Надо бы сделать какой-то null_insert_iterator и null_inserter

- [ ] `[_src/umba-md-pp/processing.h:1011]`
  Нужно добавить обрамление

- [ ] `[_src/umba-md-pp/processing_utils.h:471]`
  Доделать



# _src/umba-md-pp/code-processing

- [ ] `[_src/umba-md-pp/code-processing/utils.h:1328]`
  тут надо проверять, является ли коментарий коментарием спец вида. Пока
  складываем любые коментарии

- [ ] `[_src/umba-md-pp/code-processing/utils.h:1427]`
  тут надо проверять, является ли коментарий коментарием спец вида. Пока
  складываем любые коментарии

- [ ] `[_src/umba-md-pp/code-processing/utils.h:1602]`
  Не поддерживаются C++20 requires

- [ ] `[_src/umba-md-pp/code-processing/utils.h:1604]`
  При бэк-сканировании до начала блока стоп производится в том числе по
  закрывающей блочной скобке При использовании requires requires могут быть
  блочные скобки в заголовке функции, и блок будет выцеплен некорректно Надо
  что-то сделать, возможно, отключать опцией остановку по символу блока

- [ ] `[_src/umba-md-pp/code-processing/utils.h:1740]`
  Тут, наверное, надо удалить символы коментария в начале и в конце, а также доп
  символы, которые использует доксиген: '*'/'!'/'<'

- [ ] `[_src/umba-md-pp/code-processing/utils.h:1819]`
  Тут, наверное, надо удалить символы коментария в начале и в конце, а также доп
  символы, которые использует доксиген: '*'/'!'/'<'

