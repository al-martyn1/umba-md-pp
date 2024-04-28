  - [1 umba-md-pp - Markdown препроцессор](#user-content-1-umba-md-pp---markdown-препроцессор)
  - [2 Основные возможности](#user-content-2-основные-возможности)
  - [3 Справочник по аргументам командной строки](#user-content-3-справочник-по-аргументам-командной-строки)

# 1 umba-md-pp - Markdown препроцессор

**umba-md-pp** - препроцессор для подготовки документации с использованием markdown-разметки простого текста.

# 2 Основные возможности

umba-md-pp:

  - умеет автоматически нумеровать разделы документа;
  - умеет автоматически формировать раздел "Содержание" и включать его в любом месте документа по команде `#!toc`/`#$toc`, или в начале документа, если положение TOC не задано в документе;
  - умеет вставлять внешние документы из отдельных файлов, как часть текущего документа, при этом изменяя уровень заголовков вставляемого документа;
  - умеет вставлять в документ фрагменты кода из внешних файлов, по тэгам и по номерам строк, при этом добавляя номера строк исходного файла, и имя файла, откуда было сделано включение;
  - умеет задавать опции включения документов и фрагментов кода глобально, или локально, при каждом включении;
  - умеет задавать условные переменные для проверки их при включении файлов или фрагментов кода;
  - умеет при включении файлов или фрагментов кода из внешних файлов проверять заданные условия и производить включение документов/фрагментов кода по условию;
  - умеет в условное включение документов и/или фрагментов кода при наличии файла документа/кода: при ошибке включения документа можно подавить сообщения об ошибках;
  - умеет однострочные коментарии вида `#//`;
  - умеет многострочные коментарии вида `#/*`/`#*/`;
  - умеет в метаданные **Markdown**;




# 3 Справочник по аргументам командной строки


**%%-q%%**,
**%%--quet%%** - Operate quetly.

**%%--home%%** - Open homepage.

**%%--no-builtin-options%%** - Don't parse predefined options from main distribution options file 'F:/_github/umba-tools/umba-md-pp/.out/msvc2019/x86/conf/umba-md-pp.options'.

**%%--no-custom-builtin-options%%** - Don't parse predefined options from custom global options file 'F:/_github/umba-tools/umba-md-pp/.out/msvc2019/x86/conf/umba-md-pp.custom.options'.

**%%--no-user-builtin-options%%** - Don't parse predefined options from user local options file 'C:/Users/martinov/.umba-md-pp.options'.

**%%-v%%**,
**%%--version%%** - Show version info.

**%%--where%%** - Show where the executable file is.

**%%--color=CLR%%** - Force set console output coloring.
Allowed values: 'file'/'no'/'none', 'ansi'/'term' or 'cmd'/'console'/'win'/'win32'/'windows', initial value: <AUTO>.

**%%-Y[=MODE]%%**,
**%%--overwrite[=MODE]%%** - Allow overwrite existing file.
Flag option, allowed values: '+'/'1'/'Y(es)'/'True' or '-'/'0'/'N(o)'/'False', default value: TRUE.

**%%-L=LINEFEED%%**,
**%%--LF=LINEFEED%%**,
**%%--linefeed=LINEFEED%%** - Output linefeed. LINEFEED is one of: CR/LF/CRLF/LFCR/DETECT. Default is CRLF.

**%%-I=PATH%%**,
**%%--add-examples-path=PATH%%** - Add paths list for examples searching.

**%%-E=LANG:EXT[,EXT...]%%**,
**%%--add-lang-file-extention=LANG:EXT[,EXT...]%%**,
**%%--add-lang-file-extentions=LANG:EXT[,EXT...]%%** - Add file extention for the lang for lang detection.

**%%-P=LANG:PREFIX%%**,
**%%--set-lang-cut-prefix=LANG:PREFIX%%** - Set prefix for the cut labels in the lang files.

**%%-T=LANG:TAG%%**,
**%%--set-lang-listing-tag=LANG:TAG%%** - Set target markdown tag for the code section.

**%%-O=OPTS%%**,
**%%--set-insert-options=OPTS%%** - Set default insert options.

**%%--processing-options=OPTS%%** - Set processing (output generation) options.

**%%-C=VAR:VAL%%**,
**%%--set-condition-var=VAR:VAL%%** - Set condition variable.

**%%-R=NAME%%**,
**%%--target-renderer=NAME%%** - Set target renderer (github/doxygen).

**%%-m=TAG:REPLACETO%%**,
**%%--meta-tag-replace=TAG:REPLACETO%%** - Add meta tag name replacement.

**%%-M=TAG:SERIALIZETO%%**,
**%%--meta-tag-serialize=TAG:SERIALIZETO%%** - Add meta tag name for serialization.

**%%--numeric-sections-max-level=LEVEL%%** - Set max level for numeration.
Allowed values: '0'/'inf'/'infinite', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15' or '16', initial value: 'infinite'.

**%%--toc-max-level=LEVEL%%** - Set max level for table of contents (TOC).
Allowed values: '0'/'inf'/'infinite', '1', '2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12', '13', '14', '15' or '16', initial value: 'infinite'.

**%%--gcc%%** - GCC messages format instead of MSVC format.

**%%--autocomplete-install%%** - Install autocompletion to bash/clink(cmd).

**%%--autocomplete-uninstall%%** - Remove autocompletion from bash/clink(cmd).

