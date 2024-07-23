# Препроцессор для Markdown

Утилита, добавляющая некоторые возможности к Markdown

Test number one


# umba-md-pp - Markdown препроцессор

**umba-md-pp** - препроцессор для подготовки документации с использованием markdown-разметки
простого текста.

# Важные ссылки

 - [Перечень страниц (индекс документации)](pages.md)
 - [TODO](doc/todo.md)

## Тест макроподстановки в именах включаемых файлов

Этот файл подключен со значением макроса `INSERT_DOC_FILENAME_MACRO_TEST`, равным `by_macro2`.



# Основные возможности

Основные возможности **umba-md-pp**:

  - умеет автоматически нумеровать разделы документа (`--processing-options=numeric-sections`);
  - умеет автоматически формировать раздел "Содержание" и включать его в любом месте документа
    по команде `#!toc`/`#$toc`,
    или в начале документа, если положение TOC не задано в документе (`--processing-options=generate-toc`);
  - умеет вставлять внешние документы из отдельных файлов, как часть текущего документа,
    при этом изменяя уровень заголовков вставляемого документа (`#!insert{doc,raise=+-N} included_doc.md`);
  - умеет вставлять в документ фрагменты кода из внешних файлов, по тэгам и по номерам
    строк, при этом добавляя номера строк исходного файла, и имя файла, откуда было сделано
    включение (`#!insert{line-no} snippets.cpp#CODE_FRAGMENT_NAME`);
  - умеет задавать условные переменные для проверки их при включении файлов или фрагментов кода (`--set-condition-var=ALLOW_EXTERNAL_INC:1`);
  - умеет при включении файлов или фрагментов кода из внешних файлов проверять заданные условия и производить
    включение документов/фрагментов кода по условию (`#!insert{ifdef:ALLOW_EXTERNAL_INC,if:$ALLOW_EXTERNAL_INC!=0,if:$google_inc!=0} google_inc.md`);
  - умеет в условное включение документов и/или фрагментов кода при наличии файла документа/кода: при ошибке
    включения документа можно подавить сообщения об ошибках (`#!insert{no-fail} ...`);
  - умеет однострочные коментарии вида `#//`;
  - умеет многострочные коментарии вида `#/*`/`#*/`;
  - умеет в метаданные **Markdown**;


Пытаемся в картинки

![Todo 1](doc/icons/todo-list-50.png)

[![Todo 2](doc/icons/todo-list-50.png) TODO](doc/todo.md)




Включаем по сигнатуре, блок, но сигнатура в одну строку при ссылке

```cpp
2: inline
3: void simpleDoNothing( int v1)
4: {
5:     // Первая версия, до isUrlAbsolute
6: }
```

Включаем по сигнатуре, блок, но сигнатура в одну строку в файле

```cpp
43: inline void simpleNothing2( char v3)
44: {
45:     // Третья версия, сигнатура в одну строку
46: }
```

Включаем по сигнатуре, блок, шаблонная функция

```cpp
 9: template<typename StringType>
10: bool isUrlAbsolute(const StringType &strUrl)
11: {
12:     using CharType = typename StringType::value_type;
13: 
14:     // looking for scheme: "protocol://..."
15:     // "://..." - is invalid, missing protocol
16: 
17: 
18:     auto colonPos = strUrl.find((CharType)':');
19: 
20:     if (colonPos==strUrl.npos)
21:         return false;
22: 
23:     if (colonPos==0)
24:         return false;
25: 
26:     if ((colonPos+1u)>=strUrl.size() || (colonPos+2u)>=strUrl.size())
27:         return false;
28: 
29:     if (strUrl[colonPos+1u]==(CharType)'/' && strUrl[colonPos+2u]==(CharType)'/')
30:         return true;
31: 
32:     return false;
33: }
```

Включаем по сигнатуре, блок, нешаблонная функция, первая версия (без использования сигнатурного пути)

```cpp
2: inline
3: void simpleDoNothing( int v1)
4: {
5:     // Первая версия, до isUrlAbsolute
6: }
```

Включаем по сигнатуре, блок, нешаблонная функция, вторая версия (с использованием сигнатурного пути)

```cpp
36: inline
37: void simpleDoNothing( char v2)
38: {
39:     // Вторая версия, после isUrlAbsolute
40: }
```

Включаем по сигнатуре, шаблонная функция, до разделителя

```cpp
 9: template<typename StringType>
10: bool isUrlAbsolute(const StringType &strUrl)
11: {
12:     using CharType = typename StringType::value_type;
13: 
14:     // looking for scheme: "protocol://..."
15:     // "://..." - is invalid, missing protocol
16: 
17: 
18:     auto colonPos = strUrl.find((CharType)':');
19: 
20:     if (colonPos==strUrl.npos)
21:         return false;
22: 
23:     if (colonPos==0)
24:         return false;
25: 
26:     if ((colonPos+1u)>=strUrl.size() || (colonPos+2u)>=strUrl.size())
27:         return false;
28: 
29:     if (strUrl[colonPos+1u]==(CharType)'/' && strUrl[colonPos+2u]==(CharType)'/')
30:         return true;
31: 
32:     return false;
33: }
```

Включаем по сигнатуре, шаблонная функция, до двух пустых строк

```cpp
 9: template<typename StringType>
10: bool isUrlAbsolute(const StringType &strUrl)
11: {
12:     using CharType = typename StringType::value_type;
13: 
14:     // looking for scheme: "protocol://..."
15:     // "://..." - is invalid, missing protocol
```

Включаем по сигнатуре, от одной сигнатуры до другой

```cpp
 2: inline
 3: void simpleDoNothing( int v1)
 4: {
 5:     // Первая версия, до isUrlAbsolute
 6: }
 7: 
 8: //----------------------------------------------------------------------------
 9: template<typename StringType>
10: bool isUrlAbsolute(const StringType &strUrl)
11: {
12:     using CharType = typename StringType::value_type;
13: 
14:     // looking for scheme: "protocol://..."
15:     // "://..." - is invalid, missing protocol
16: 
17: 
18:     auto colonPos = strUrl.find((CharType)':');
19: 
20:     if (colonPos==strUrl.npos)
21:         return false;
22: 
23:     if (colonPos==0)
24:         return false;
25: 
26:     if ((colonPos+1u)>=strUrl.size() || (colonPos+2u)>=strUrl.size())
27:         return false;
28: 
29:     if (strUrl[colonPos+1u]==(CharType)'/' && strUrl[colonPos+2u]==(CharType)'/')
30:         return true;
31: 
32:     return false;
33: }
34: 
35: //----------------------------------------------------------------------------
```

Включаем по номерам строк

```cpp
 6: // source parsing errors
 7: // requires std::stringr curFile, unsigned lineNo in log scope
 8: #define LOG_ERR                      UMBA_LOG_ERR_INPUT
 9: #define LOG_WARN(warnType)           UMBA_LOG_WARN_INPUT(warnType)
10: 
11: // options and other errors
12: #define LOG_ERR_OPT                  UMBA_LOG_ERR
```





# 3 Подключенные для примера результаты тестов


Скопировать данный файл на уровень выше.

tests/img/filled-96.png на уровень выше.

Картинко - ![Картинко from upper level](../upper-level-filled-96.png)


## Title

Test01
Link to [test02.md](tests/test02.md)
Image - ![Image](tests/img/filled-96.png)

Link to document which is out of the hierarchy [upper_inc.md_](../upper_inc.md)
Image from out of the  hierarchy- ![Image](../upper-level-filled-96.png)



// Print lines, noKeepCutTags #print_ver_all

```cpp
```


// Print lines, keepCutTags   #print_ver_all

```cpp
```


// No lines, keepCutTags   #print_ver_all

```cpp
```


// printCommitHash

```cpp
```


// printOnlyVersion

```cpp
```


### Subtitle
#### Русский заголовок третьего уровня

```
lst existing
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12 - this command not processed due it in listing
```

https://moonbase59.github.io/gh-toc/
https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
Github tips, notes, warnings and so on - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#alerts
todo's - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#task-lists

```cpp
 6: // source parsing errors
 7: // requires std::stringr curFile, unsigned lineNo in log scope
 8: #define LOG_ERR                      UMBA_LOG_ERR_INPUT
 9: #define LOG_WARN(warnType)           UMBA_LOG_WARN_INPUT(warnType)
10: 
11: // options and other errors
12: #define LOG_ERR_OPT                  UMBA_LOG_ERR
```

```cpp
```

```cpp
```

## Other Title

### Other Subtitle

Bla-bla.

  - [umba-md-pp - Markdown препроцессор](#user-content-umba-md-pp---markdown-препроцессор)
  - [Важные ссылки](#user-content-важные-ссылки)
    - [Тест макроподстановки в именах включаемых файлов](#user-content-тест-макроподстановки-в-именах-включаемых-файлов)
  - [Основные возможности](#user-content-основные-возможности)
  - [3 Подключенные для примера результаты тестов](#user-content-3-подключенные-для-примера-результаты-тестов)
    - [Title](#user-content-title)
      - [Subtitle](#user-content-subtitle)
        - [Русский заголовок третьего уровня](#user-content-русский-заголовок-третьего-уровня)
    - [Other Title](#user-content-other-title)
      - [Other Subtitle](#user-content-other-subtitle)
    - [3.1 Title](#user-content-31-title)
      - [3.1.1 Subtitle](#user-content-311-subtitle)
        - [Русский заголовок третьего уровня](#user-content-русский-заголовок-третьего-уровня-1)
    - [3.2 Other Title](#user-content-32-other-title)
      - [3.2.1 Other Subtitle](#user-content-321-other-subtitle)




## 3.1 Title

Test01

// Print lines, noKeepCutTags #print_ver_all

!!! File not found

```cpp
```


// Print lines, keepCutTags   #print_ver_all

!!! File not found

```cpp
```


// No lines, keepCutTags   #print_ver_all

!!! File not found

```cpp
```


// printCommitHash

!!! File not found

```cpp
```


// printOnlyVersion

!!! File not found

```cpp
```


### 3.1.1 Subtitle
#### Русский заголовок третьего уровня

```
lst existing
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12 - this command not processed due it in listing
```

https://moonbase59.github.io/gh-toc/
https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
Github tips, notes, warnings and so on - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#alerts
todo's - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#task-lists


!!! File not found

```cpp
```

!!! File not found

```cpp
```

## 3.2 Other Title

### 3.2.1 Other Subtitle

Bla-bla.

  - [1 umba-md-pp - Markdown препроцессор](user-content-1-umba-md-pp---markdown-препроцессор)[1 umba-md-pp - Markdown препроцессор](#)
  - [2 Основные возможности](user-content-2-основные-возможности)[2 Основные возможности](#)
  - [3 Подключенные для примера результаты тестов](user-content-3-подключенные-для-примера-результаты-тестов)[3 Подключенные для примера результаты тестов](#)
    - [3.1 Title](user-content-31-title)[3.1 Title](#)
      - [3.1.1 Subtitle](user-content-311-subtitle)[3.1.1 Subtitle](#)
    - [3.2 Other Title](user-content-32-other-title)[3.2 Other Title](#)
      - [3.2.1 Other Subtitle](user-content-321-other-subtitle)[3.2.1 Other Subtitle](#)




