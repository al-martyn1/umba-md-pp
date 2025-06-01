---
Generator: Umba Brief Scanner
---

# _libs/marty_format/impl

- [ ] `[_libs/marty_format/impl/internal_processFormatValuesImpl.h:847]`
  Если задано сделать escape для строки то надо сделать escape, но не длиннее,
  чем макс ширина

- [ ] `[_libs/marty_format/impl/internal_processFormatValuesImpl.h:859]`
  Тут используем UTF итераторы явно, если нам не нужен UTF, надо будет обрубить
  эту возможность



# _src/umba-md-pp

- [ ] `[_src/umba-md-pp/processing.h:71]`
  Набить парсеры для graph/csv

- [ ] `[_src/umba-md-pp/processing.h:144]`
  Набить процессоры для graph/csv

- [ ] `[_src/umba-md-pp/processing.h:930]`
  Надо бы ещё принудительно флэттенизировать урлы, которые выходят за пределы
  каталога с документом UMBA_USED(flattenImageLinks);

- [ ] `[_src/umba-md-pp/processing.h:959]`
  Надо бы сделать какой-то null_insert_iterator и null_inserter

- [ ] `[_src/umba-md-pp/processing.h:1674]`
  Нужно добавить обрамление

- [ ] `[_src/umba-md-pp/processing_utils.h:454]`
  Доделать

