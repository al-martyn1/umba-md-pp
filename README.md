# 1 umba-md-pp - Markdown препроцессор

**umba-md-pp** - препроцессор для подготовки документации с использованием markdown-разметки 
простого текста.

# 2 Основные возможности

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
    включение документов/фрагментов кода по условию (`#!insert{ifdef:$ALLOW_EXTERNAL_INC,if:$ALLOW_EXTERNAL_INC!=0,if:$google_inc!=0} google_inc.md`);
  - умеет в условное включение документов и/или фрагментов кода при наличии файла документа/кода: при ошибке 
    включения документа можно подавить сообщения об ошибках (`#!insert{no-fail} ...`);
  - умеет однострочные коментарии вида `#//`;
  - умеет многострочные коментарии вида `#/*`/`#*/`;
  - умеет в метаданные **Markdown**;







# 3 Подключенные для примера результаты тестов


## 3.1 Title

Test01

// Print lines, noKeepCutTags #print-ver-all

!!! File not found
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print-ver-all


// Print lines, keepCutTags   #print-ver-all

!!! File not found
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print-ver-all


// No lines, keepCutTags   #print-ver-all

!!! File not found
#!insert{nolineno,noKeepCutTags} cpp/print_ver.h#print-ver-all


// printCommitHash

!!! File not found
#!insert{lineno} cpp/print_ver.h#printCommitHash


// printOnlyVersion

!!! File not found
#!insert{lineno} cpp/print_ver.h#printOnlyVersion


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

../src/umba-md-pp/log.h:6
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
#!insert{lineno} cpp/print_ver.h#print-ver-all

!!! File not found
#!insert{lineno} cpp/print_ver.h#printOnlyVersion

## 3.2 Other Title

### 3.2.1 Other Subtitle

Bla-bla.

  - [1 umba-md-pp - Markdown препроцессор](#user-content-1-umba-md-pp---markdown-препроцессор)
  - [2 Основные возможности](#user-content-2-основные-возможности)
  - [3 Подключенные для примера результаты тестов](#user-content-3-подключенные-для-примера-результаты-тестов)
    - [3.1 Title](#user-content-31-title)
      - [3.1.1 Subtitle](#user-content-311-subtitle)
    - [3.2 Other Title](#user-content-32-other-title)
      - [3.2.1 Other Subtitle](#user-content-321-other-subtitle)
    - [3.3 Уровень 1 [test-id]](#user-content-33-уровень-1-test-id)
      - [3.3.1 Следующий уровень 2](#user-content-331-следующий-уровень-2)
      - [3.3.2 Следующий уровень 2](#user-content-332-следующий-уровень-2)
      - [3.3.3 Следующий уровень 2](#user-content-333-следующий-уровень-2)
    - [3.4 Уровень 1 #2](#user-content-34-уровень-1-2)
      - [3.4.1 Уровень два](#user-content-341-уровень-два)
      - [3.4.2 Уровень два](#user-content-342-уровень-два)
      - [3.4.3 Level 2](#user-content-343-level-2)
    - [3.5 Level 1 #3](#user-content-35-level-1-3)
    - [3.6 We need to go longer](#user-content-36-we-need-to-go-longer)
      - [3.6.1 We need to go longer 1](#user-content-361-we-need-to-go-longer-1)
      - [3.6.2 We need to go longer 2](#user-content-362-we-need-to-go-longer-2)
      - [3.6.3 We need to go longer 3](#user-content-363-we-need-to-go-longer-3)
      - [3.6.4 We need to go longer 4](#user-content-364-we-need-to-go-longer-4)
      - [3.6.5 We need to go longer 5](#user-content-365-we-need-to-go-longer-5)
      - [3.6.6 We need to go longer 6](#user-content-366-we-need-to-go-longer-6)
      - [3.6.7 We need to go longer 7](#user-content-367-we-need-to-go-longer-7)
      - [3.6.8 We need to go longer 8](#user-content-368-we-need-to-go-longer-8)
      - [3.6.9 We need to go longer 9](#user-content-369-we-need-to-go-longer-9)
      - [3.6.10 We need to go longer 10](#user-content-3610-we-need-to-go-longer-10)
      - [3.6.11 We need to go longer 11](#user-content-3611-we-need-to-go-longer-11)
      - [3.6.12 We need to go longer 12](#user-content-3612-we-need-to-go-longer-12)
      - [3.6.13 We need to go longer 13](#user-content-3613-we-need-to-go-longer-13)
      - [3.6.14 We need to go longer 14](#user-content-3614-we-need-to-go-longer-14)
      - [3.6.15 We need to go longer 15](#user-content-3615-we-need-to-go-longer-15)
      - [3.6.16 We need to go longer 16](#user-content-3616-we-need-to-go-longer-16)
      - [3.6.17 We need to go longer 17](#user-content-3617-we-need-to-go-longer-17)
      - [3.6.18 We need to go longer 18](#user-content-3618-we-need-to-go-longer-18)
      - [3.6.19 We need to go longer 19](#user-content-3619-we-need-to-go-longer-19)
      - [3.6.20 We need to go longer 20](#user-content-3620-we-need-to-go-longer-20)
      - [3.6.21 We need to go longer 21](#user-content-3621-we-need-to-go-longer-21)
      - [3.6.22 We need to go longer 22](#user-content-3622-we-need-to-go-longer-22)
      - [3.6.23 We need to go longer 23](#user-content-3623-we-need-to-go-longer-23)
    - [3.7 We need to go deeper](#user-content-37-we-need-to-go-deeper)
      - [3.7.1 More deeper](#user-content-371-more-deeper)




## 3.3 Уровень 1 [test-id]

  - [ открывающая квадратная скобка
  - ] закрывающая квадратная скобка
  - [] просто две квадратных скобки
  - [с чем-то] просто две квадратных скобки с чем-то
  - []( просто две квадратных скобки и открывающая круглая
  - [с чем-то]( просто две квадратных скобки с чем-то и открывающая круглая
  - пабам, ссылка на раздел, с его названием (но забыли решетку): [$](Параграф 3)
  - пабам, ссылка на раздел, с его номером (но забыли решетку): [#](Параграф 3)
  - пабам, ссылка на раздел, с его названием и номером (но забыли решетку): [#$](Параграф 3)
  - пабам, ссылка на раздел, с произвольным текстом (но забыли решетку): [ссылка](Параграф 3)
  - пабам, ссылка на раздел, с его названием: [Параграф 3](#user-content-параграф-3)
  - пабам, ссылка на раздел, с его номером: [Параграф 3](#user-content-параграф-3)
  - пабам, ссылка на раздел, с его названием и номером: [Параграф 3](#user-content-параграф-3)
  - пабам, ссылка на раздел, с произвольным текстом: [ссылка](#user-content-параграф-3)
  - ссылка в гугель, для теста: [вас завалят рекламой](google.com)
  - ссылка в гугель, для теста (с http): [вас завалят рекламой](http://google.com)

[Level 3](#user-content-level-3) Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

### 3.3.1 Следующий уровень 2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

### 3.3.2 Следующий уровень 2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

### 3.3.3 Следующий уровень 2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Параграф 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

##### Параграф 4/1

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

##### Параграф 4/2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Параграф 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

## 3.4 Уровень 1 #2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

### 3.4.1 Уровень два

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

### 3.4.2 Уровень два

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

### 3.4.3 Level 2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Level 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Level 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

##### Level 4

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

##### Level 4/2

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Level 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Level 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

#### Level 3

Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.
Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla. Bla-bla.

## 3.5 Level 1 #3

Bla-bla.

## 3.6 We need to go longer
### 3.6.1 We need to go longer 1
### 3.6.2 We need to go longer 2
### 3.6.3 We need to go longer 3
### 3.6.4 We need to go longer 4
### 3.6.5 We need to go longer 5
### 3.6.6 We need to go longer 6
### 3.6.7 We need to go longer 7
### 3.6.8 We need to go longer 8
### 3.6.9 We need to go longer 9
### 3.6.10 We need to go longer 10
### 3.6.11 We need to go longer 11
### 3.6.12 We need to go longer 12
### 3.6.13 We need to go longer 13
### 3.6.14 We need to go longer 14
### 3.6.15 We need to go longer 15
### 3.6.16 We need to go longer 16
### 3.6.17 We need to go longer 17
### 3.6.18 We need to go longer 18
### 3.6.19 We need to go longer 19
### 3.6.20 We need to go longer 20
### 3.6.21 We need to go longer 21
### 3.6.22 We need to go longer 22
### 3.6.23 We need to go longer 23

## 3.7 We need to go deeper

### 3.7.1 More deeper

#### Deeper and deeper

##### Deeper, deeper and deeper

###### Deeper, deeper, deeper and deeper

####### Deeper, deeper, deeper, deeper and deeper

######## Deeper, deeper, deeper, deeper, deeper and deeper

######### Deeper, deeper, deeper, deeper, deeper, deeper and deeper

########## Deeper, deeper, deeper, deeper, deeper, deeper, deeper and deeper

########### Deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper and deeper

############ Deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper and deeper

############# Deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper and deeper

############## Deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper, deeper and deeper




