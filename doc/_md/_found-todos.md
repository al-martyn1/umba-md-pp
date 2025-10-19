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



# _src/umba-md-pp

- [ ] `[_src/umba-md-pp/code-options-database.h:117]`
  Пока у нас строки начала/окончания блока односимвольные, но надо переделать,
  желательно везде

- [ ] `[_src/umba-md-pp/processing.h:237]`
  Надо бы ещё принудительно флэттенизировать урлы, которые выходят за пределы
  каталога с документом UMBA_USED(flattenImageLinks);

- [ ] `[_src/umba-md-pp/processing.h:266]`
  Надо бы сделать какой-то null_insert_iterator и null_inserter

- [ ] `[_src/umba-md-pp/processing.h:1003]`
  Нужно добавить обрамление

- [ ] `[_src/umba-md-pp/processing_utils.h:455]`
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

