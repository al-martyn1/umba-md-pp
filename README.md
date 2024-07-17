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
 6: 
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
11: 
12:                                                             // 12
13: 
14: void printNameVersion( const std::string &indent = "" )     // 14
15: {                                                           // 15
16:     std::cout<<indent << appFullName << " version ";        // 16
17:     printOnlyVersion();                                     // 17
18:     //<<rdlcVersion<<"\n";                                  // 18
19: }                                                           // 19
20: 
21:                                                             // 21
22: 
23: void printCommitHash( const std::string &indent = "" )      // 23
24: {                                                           // 24
25:     if (appCommitHash.empty())                              // 25
26:         return;                                             // 26
27:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
28: }                                                           // 28
29: 
30:                                                             // 30
31: 
32: void printBuildDateTime( const std::string &indent = "" )   // 32
33: {                                                           // 33
34:     std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
35: }                                                           // 35
36: 
37: 
38: 
```


// Print lines, keepCutTags   #print_ver_all

```cpp
 6: 
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
11: 
12:                                                             // 12
13: 
14: void printNameVersion( const std::string &indent = "" )     // 14
15: {                                                           // 15
16:     std::cout<<indent << appFullName << " version ";        // 16
17:     printOnlyVersion();                                     // 17
18:     //<<rdlcVersion<<"\n";                                  // 18
19: }                                                           // 19
20: 
21:                                                             // 21
22: 
23: void printCommitHash( const std::string &indent = "" )      // 23
24: {                                                           // 24
25:     if (appCommitHash.empty())                              // 25
26:         return;                                             // 26
27:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
28: }                                                           // 28
29: 
30:                                                             // 30
31: 
32: void printBuildDateTime( const std::string &indent = "" )   // 32
33: {                                                           // 33
34:     std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
35: }                                                           // 35
36: 
37: 
38: 
```


// No lines, keepCutTags   #print_ver_all

```cpp
void printOnlyVersion()                                     // 7
{                                                           // 8
    std::cout<<appVersion<<"\n";                            // 9
}                                                           // 10
                                                            // 12
void printNameVersion( const std::string &indent = "" )     // 14
{                                                           // 15
    std::cout<<indent << appFullName << " version ";        // 16
    printOnlyVersion();                                     // 17
    //<<rdlcVersion<<"\n";                                  // 18
}                                                           // 19
                                                            // 21
void printCommitHash( const std::string &indent = "" )      // 23
{                                                           // 24
    if (appCommitHash.empty())                              // 25
        return;                                             // 26
    std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
}                                                           // 28
                                                            // 30
void printBuildDateTime( const std::string &indent = "" )   // 32
{                                                           // 33
    std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
}                                                           // 35



```


// printCommitHash

```cpp
23: 
24: 
25: void printCommitHash( const std::string &indent = "" )      // 23
26: {                                                           // 24
27:     if (appCommitHash.empty())                              // 25
28:         return;                                             // 26
29:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
30: }                                                           // 28
```


// printOnlyVersion

```cpp
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
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
 6: 
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
11: 
12:                                                             // 12
13: 
14: void printNameVersion( const std::string &indent = "" )     // 14
15: {                                                           // 15
16:     std::cout<<indent << appFullName << " version ";        // 16
17:     printOnlyVersion();                                     // 17
18:     //<<rdlcVersion<<"\n";                                  // 18
19: }                                                           // 19
20: 
21:                                                             // 21
22: 
23: void printCommitHash( const std::string &indent = "" )      // 23
24: {                                                           // 24
25:     if (appCommitHash.empty())                              // 25
26:         return;                                             // 26
27:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
28: }                                                           // 28
29: 
30:                                                             // 30
31: 
32: void printBuildDateTime( const std::string &indent = "" )   // 32
33: {                                                           // 33
34:     std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
35: }                                                           // 35
36: 
37: 
38: 
```

```cpp
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
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
 6: 
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
11: 
12:                                                             // 12
13: 
14: void printNameVersion( const std::string &indent = "" )     // 14
15: {                                                           // 15
16:     std::cout<<indent << appFullName << " version ";        // 16
17:     printOnlyVersion();                                     // 17
18:     //<<rdlcVersion<<"\n";                                  // 18
19: }                                                           // 19
20: 
21:                                                             // 21
22: 
23: void printCommitHash( const std::string &indent = "" )      // 23
24: {                                                           // 24
25:     if (appCommitHash.empty())                              // 25
26:         return;                                             // 26
27:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
28: }                                                           // 28
29: 
30:                                                             // 30
31: 
32: void printBuildDateTime( const std::string &indent = "" )   // 32
33: {                                                           // 33
34:     std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
35: }                                                           // 35
36: 
37: 
38: 
```


// Print lines, keepCutTags   #print_ver_all

!!! File not found

```cpp
 6: 
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
11: 
12:                                                             // 12
13: 
14: void printNameVersion( const std::string &indent = "" )     // 14
15: {                                                           // 15
16:     std::cout<<indent << appFullName << " version ";        // 16
17:     printOnlyVersion();                                     // 17
18:     //<<rdlcVersion<<"\n";                                  // 18
19: }                                                           // 19
20: 
21:                                                             // 21
22: 
23: void printCommitHash( const std::string &indent = "" )      // 23
24: {                                                           // 24
25:     if (appCommitHash.empty())                              // 25
26:         return;                                             // 26
27:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
28: }                                                           // 28
29: 
30:                                                             // 30
31: 
32: void printBuildDateTime( const std::string &indent = "" )   // 32
33: {                                                           // 33
34:     std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
35: }                                                           // 35
36: 
37: 
38: 
```


// No lines, keepCutTags   #print_ver_all

!!! File not found

```cpp
void printOnlyVersion()                                     // 7
{                                                           // 8
    std::cout<<appVersion<<"\n";                            // 9
}                                                           // 10
                                                            // 12
void printNameVersion( const std::string &indent = "" )     // 14
{                                                           // 15
    std::cout<<indent << appFullName << " version ";        // 16
    printOnlyVersion();                                     // 17
    //<<rdlcVersion<<"\n";                                  // 18
}                                                           // 19
                                                            // 21
void printCommitHash( const std::string &indent = "" )      // 23
{                                                           // 24
    if (appCommitHash.empty())                              // 25
        return;                                             // 26
    std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
}                                                           // 28
                                                            // 30
void printBuildDateTime( const std::string &indent = "" )   // 32
{                                                           // 33
    std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
}                                                           // 35



```


// printCommitHash

!!! File not found

```cpp
23: 
24: 
25: void printCommitHash( const std::string &indent = "" )      // 23
26: {                                                           // 24
27:     if (appCommitHash.empty())                              // 25
28:         return;                                             // 26
29:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
30: }                                                           // 28
```


// printOnlyVersion

!!! File not found

```cpp
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
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
 6: // source parsing errors
 7: // requires std::stringr curFile, unsigned lineNo in log scope
 8: #define LOG_ERR                      UMBA_LOG_ERR_INPUT
 9: #define LOG_WARN(warnType)           UMBA_LOG_WARN_INPUT(warnType)
10: 
11: // options and other errors
12: #define LOG_ERR_OPT                  UMBA_LOG_ERR
```

!!! File not found

```cpp
 6: 
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
11: 
12:                                                             // 12
13: 
14: void printNameVersion( const std::string &indent = "" )     // 14
15: {                                                           // 15
16:     std::cout<<indent << appFullName << " version ";        // 16
17:     printOnlyVersion();                                     // 17
18:     //<<rdlcVersion<<"\n";                                  // 18
19: }                                                           // 19
20: 
21:                                                             // 21
22: 
23: void printCommitHash( const std::string &indent = "" )      // 23
24: {                                                           // 24
25:     if (appCommitHash.empty())                              // 25
26:         return;                                             // 26
27:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
28: }                                                           // 28
29: 
30:                                                             // 30
31: 
32: void printBuildDateTime( const std::string &indent = "" )   // 32
33: {                                                           // 33
34:     std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
35: }                                                           // 35
36: 
37: 
38: 
```

!!! File not found

```cpp
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
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




