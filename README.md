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

Основная идея была в том, чтобы писать и хранить документацию по софтовым проектам непосредственно в проекте,
не используя никакие дополнительные конфлюэнцы и тп.

Doxygen - имеет право на жизнь, но, в основном, как средство создания полновесной справочной документации.
Такая документация не всегда нужна, да и сложно заставить разработчиков документировать каждую функцию.
При этом, даже если каждая функция задокументированна, общее предствление, как всем этим добром пользоваться,
справочная документация не даёт.

Хорошим вариантом видится написание статей по тем или иным аспектам использования кода/библиотек, которые бы лежали
непосредственно рядом с исходным кодом.

Язык разметки markdown выглядит для этого довольно неплохо, и он из коробки поддерживается GitHub'ом и GitLab'ом - 
можно зайти в репозиторий через браузер и изучать документацию прямо там.

У markdown есть недостаток - всё содержимое должно быть включено в MD-документ. Это неудобно, когда
мы описываем вещи и явления, не зависящие от нас - нам приходится следить, чтобы содержимое MD-файлов 
соответствовало реальному положению дел.

Это довольно трудозатратно. Но что если попробовать научить markdown брать текст документа из внешних документов 
и исходных тестов программ? Тогда нам нужен препроцессор.

Вторая проблема при ведении документации в формате markdown или каком-то аналогичном - 
это то, что нет никаких средств для локального просмотра. Я попытался решить и эту проблему, сделав локальный рендерер
(пока он работает на базе Doxygen). Локальный рендерер при вызове создаёт временную папку в домашнем каталоге пользователя, 
подготавливает файл для генерации конечного документа, вызывает Doxygen для генерации RTF, и открывает полученный RTF
в ассоциированной с ним программе (обычно это MS Word). Локальный рендерер умеет интегрироваться в систему, и просмотр 
`.MD` файлов запускается по клику из проводника (бонусов идёт то, что `.MD` отображаются с соответствующей иконкой в проводнике).
Локальный рендерер реализован пока только под ОС Windows.

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


# Пакетный режим

В пакетном режиме утилита просматривает все файлы `.md_` (на самом деле входных расширений больше),
и генерирует рядом процессированные `.md` файлы (расширение зависит от входного).

Процессинг может производится не "по месту", а в отдельную папку - так можно генерировать содержимое
GitHub/GitLab wiki, например.


# Локальная утилита просмотра .md_ и .md файлов

Локальная утилита просмотра умеет регистрироваться в системе под текущим пользователем для поддерживаемых расширений,
и по клику в проводнике открывать RTF, сгенерированный из markdown'а.

Для рендеринга в текущий момент используется Doxygen, в далёких планах - сделать свой рендерер.


# Поддержка GraphViz

Утилита позволяет в расширенном markdown'е вставлять графы на языке GraphViz как непосредственно
в теле докумена, так и из внешнего файла. Требуется установленный GraphViz (обычно ставится вместе с Doxygen).
Используются тэги `<graph>`/`</graph>`.


# Поддержка PlantUML

В ближайшее время планируется поддержка PlantUML для рендеринга UML из языка PlantUML, по аналогии с GraphViz.


# Вставка таблиц CSV

Предполагается генерация таблиц из внедрённого или внешнего CSV.


# Вставка сниппетов

На фрагменты кода можно ссылаться через имя файла и якорь: `src/somefile.cpp#400-410`.

Якоря имеют диапазон: стартовый якорь, и конечный якорь.

Стартовый якорь:

 - тэг. Для каждого языка задается префикс тэга, например, для C++ это `//#!`. После префикса идёт имя тэга без пробелов.
   Окончание фрагмента ищется как пустой префикс ил префикс с таким же тэгом. Учитывается вложенность. Вложенные тэги при вставке
   могут удаляться или заменятся пустой строкой (чтобы не ломать количество строк).
 - номер строки.
 - сигнатура кода. Пробелы не участвуют в сравнении. В якоре сигнатуру можно задать в одну строку, в исходниках, где она ищется, 
   сигнатура может быть размазана по нескольким строкам. В случае сигнатур можно задавать последовательность - 
   "вставить фрагмент с сигнатурой, которая появляется после сигнатуры1 и сигнатуры2"

Конечный якорь. Допустимы стартовые якоря, а также:
  - якорь `{}` - означает вставку блока, ограниченного блоковыми скобками, 
    с учетом вложенности (но строковые литералы не разбираются, на них может происходить сбой, если они содержат блоковые символы).
    Блоковые символы задаются для каждого языка отдельно.
  - якорь `(N)`, где N - число пустых строк, по обнаружению которых детектится конец фрагмента кода.
  - якорь `(-)` - линия-разделитель, задаётся для каждого языка отдельно в произвольном количестве вариантов, для C++ это, например,
    `//---` или `/***`. Задаются только префиксы, т.е. `//------------` - это тоже линия разделитель

Извлекаем фрагмент кода по сигнатуре:
```
#!insert{nolineno,noKeepCutTags} umba-md-pp/extern_tools.h#`template<typename StringType> StringType makeSystemFunctionCommandString`-{}
```

```cpp
template<typename StringType>
StringType makeSystemFunctionCommandString(const StringType &cmd, std::vector<StringType> cmdArgs)
{
    cmdArgs.insert(cmdArgs.begin(), cmd);
    for(auto &cmdArg : cmdArgs)
    {
        cmdArg = escapeCommandLineArgument(cmdArg);
    }

    using CharType = typename StringType::value_type;

    return umba::string_plus::merge<std::string, typename std::vector<StringType>::const_iterator>( cmdArgs.begin(), cmdArgs.end(), (CharType)' '/*, [](auto s) { return s; }*/ );
}
```



# Стандартный конфиг

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{nolineno,noKeepCutTags} conf/umba-md-pp.options


# Любите ли вы документацию так, как люблю её я?

Думаю ещё сделать всяких штук типа эпиграфов, и тп.


Пытаемся в графы

[Node Shapes](https://graphviz.org/doc/info/shapes.html)

[Undirected Graphs](https://graphviz.org/Gallery/undirected/)

[Directed Graphs examples](https://graphviz.org/Gallery/directed/)

[Neato Spring Graphs eamples](https://graphviz.org/Gallery/neato/)

[Twopi Radial Graphs](https://graphviz.org/Gallery/twopi/)


![Trie](.doc.generated_images/doc/trie.svg)
![Flat Trie](.doc.generated_images/doc/trie_flat.svg)

![Entity relation diagram (Dot)](.doc.generated_images/simple-er-dot.svg)

![Entity relation diagram (Neato)](.doc.generated_images/simple-er-neato.svg)

![Entity relation diagram (FDP)](.doc.generated_images/simple-er-fdp.svg)

![Entity relation diagram (Twopi)](.doc.generated_images/simple-er-twopi.svg)

![Entity relation diagram (Circo)](.doc.generated_images/simple-er-circo.svg)




![Left to right graph flow](.doc.generated_images/left-to-right.svg)

![A-B Graph](.doc.generated_images/doc/dot/test001.svg)

![Vinse](.doc.generated_images/doc/dot/test004.svg)

![Life flow](.doc.generated_images/life.svg)

![Like a table](.doc.generated_images/like-table.svg)

![XHTML genealogy](.doc.generated_images/xhtml-genealogy.svg)

![Subgraph (cluster) sample](.doc.generated_images/subgraph-cluster-sample.svg)


![Node Shapes - digraph R](.doc.generated_images/node-shapes-digraph_r.svg)

![Node Shapes - digraph structs](.doc.generated_images/node-shapes-digraph_structs.svg)

![Node Shapes - digraph G](.doc.generated_images/node-shapes-digraph_g.svg)


Пытаемся в картинки

![Todo 1](doc/icons/todo-list-50.png)

[![Todo 2](doc/icons/todo-list-50.png) TODO](doc/todo.md)




Включаем по сигнатуре, блок, но сигнатура в одну строку при ссылке

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`inline void simpleDoNothing`-{}

Включаем по сигнатуре, блок, но сигнатура в одну строку в файле

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`inline\nvoid simpleNothing2`-{}

Включаем по сигнатуре, блок, шаблонная функция

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`template<typename StringType>\nbool isUrlAbsolute`-{}

Включаем по сигнатуре, блок, нешаблонная функция, первая версия (без использования сигнатурного пути)

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`inline\nvoid simpleDoNothing`-{}

Включаем по сигнатуре, блок, нешаблонная функция, вторая версия (с использованием сигнатурного пути)

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`template<typename StringType>\nbool isUrlAbsolute`/`inline\nvoid simpleDoNothing`-{}

Включаем по сигнатуре, шаблонная функция, до разделителя

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`template<typename StringType>\nbool isUrlAbsolute`-(-)

Включаем по сигнатуре, шаблонная функция, до двух пустых строк

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`template<typename StringType>\nbool isUrlAbsolute`-(2)

Включаем по сигнатуре, от одной сигнатуры до другой

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp\signature_test.cpp#`inline\nvoid simpleDoNothing`-`inline\nvoid simpleDoNothing`

Включаем по номерам строк

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} umba-md-pp\log.h#6-12





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

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print_ver_all


// Print lines, keepCutTags   #print_ver_all

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print_ver_all


// No lines, keepCutTags   #print_ver_all

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{nolineno,noKeepCutTags} cpp/print_ver.h#print_ver_all


// printCommitHash

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#printCommitHash


// printOnlyVersion

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#printOnlyVersion


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

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#print_ver_all

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#printOnlyVersion

## Other Title

### Other Subtitle

Bla-bla.

  - [umba-md-pp - Markdown препроцессор](#user-content-umba-md-pp---markdown-препроцессор)
  - [Важные ссылки](#user-content-важные-ссылки)
    - [Тест макроподстановки в именах включаемых файлов](#user-content-тест-макроподстановки-в-именах-включаемых-файлов)
  - [Основные возможности](#user-content-основные-возможности)
  - [Пакетный режим](#user-content-пакетный-режим)
  - [Локальная утилита просмотра .md_ и .md файлов](#user-content-локальная-утилита-просмотра-md_-и-md-файлов)
  - [Поддержка GraphViz](#user-content-поддержка-graphviz)
  - [Поддержка PlantUML](#user-content-поддержка-plantuml)
  - [Вставка таблиц CSV](#user-content-вставка-таблиц-csv)
  - [Вставка сниппетов](#user-content-вставка-сниппетов)
  - [Стандартный конфиг](#user-content-стандартный-конфиг)
  - [Любите ли вы документацию так, как люблю её я?](#user-content-любите-ли-вы-документацию-так-как-люблю-её-я)
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

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print_ver_all


// Print lines, keepCutTags   #print_ver_all

!!! File not found

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print_ver_all


// No lines, keepCutTags   #print_ver_all

!!! File not found

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{nolineno,noKeepCutTags} cpp/print_ver.h#print_ver_all


// printCommitHash

!!! File not found

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#printCommitHash


// printOnlyVersion

!!! File not found

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
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


!!! File not found

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#print_ver_all

!!! File not found

!!! File not found in: C:\work\github\umba-tools\umba-md-pp\doc\doc, C:\work\github\umba-tools\umba-md-pp\_distr_conf\_distr_conf, C:\work\github\umba-tools\umba-md-pp\tests\snippets\tests\snippets, C:\work\github\umba-tools\umba-md-pp\src\src
#!insert{lineno} cpp/print_ver.h#printOnlyVersion

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




