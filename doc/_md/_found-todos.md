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

- [ ] `[_src/umba-md-pp/md_pp_tag_utils.h:21]`
  Набить парсеры для graph/csv

- [ ] `[_src/umba-md-pp/md_pp_tag_utils.h:94]`
  Набить процессоры для graph/csv

- [ ] `[_src/umba-md-pp/processing.h:237]`
  Надо бы ещё принудительно флэттенизировать урлы, которые выходят за пределы
  каталога с документом UMBA_USED(flattenImageLinks);

- [ ] `[_src/umba-md-pp/processing.h:266]`
  Надо бы сделать какой-то null_insert_iterator и null_inserter

- [ ] `[_src/umba-md-pp/processing.h:1003]`
  Нужно добавить обрамление

- [ ] `[_src/umba-md-pp/processing_utils.h:455]`
  Доделать

