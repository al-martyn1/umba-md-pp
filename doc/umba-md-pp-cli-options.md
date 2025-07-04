**Информация о документе**

**URL**: https://github.com/al-martyn1/umba-md-pp/blob/main/doc/umba-md-pp-cli-options.md

---

# Umba Markdown Preprocessor command line options

- [Usage](#usage)
- [Options](#options)


# Usage

```
umba-md-pp [OPTIONS] input_file [output_file]
```

If output_file not taken, STDOUT used.


# Options


**-q**,
**--quet** - Operate quetly.

**--info=info-type1[,+info-type2,-info-type]** - Make info messages enabled/disabled, '+' (or nothing) - enable message, '-' - disable it. Type is one of: `auto-url`, `config`, `graphviz`, `meta-tags`, `opt-files`, `page-index`, `plantuml`, `snippet-lookup`, `strip-extentions`.

**--warning=warn-type1[,+warn-type2,-warn-type]** - Make warning messages enabled/disabled, '+' (or nothing) - enable message, '-' - disable it. Type is one of: `graphviz`, `img-copy`, `img-copy-exist`, `plantuml`, `same-file`.

**--home** - Open homepage.

**--no-builtin-options** - Don't parse predefined options from main distribution options file.

**--no-custom-builtin-options** - Don't parse predefined options from custom global options file.

**--no-user-builtin-options** - Don't parse predefined options from user local options file.

**-v**,
**--version** - Show version number.

**--version-info** - Show version info - app name, version, host platform, build date and time.

**--builtin-options-info** - Show builtin options files location.

**--where** - Show where the executable file is.

**--color=CLR** - Force set console output coloring.
Allowed values: `file`/`no`/`none`, `ansi`/`term` or `cmd`/`console`/`win`/`win32`/`windows`, initial value: `<AUTO>`.

**--verbose[=MODE]** - Verbose mode on/off.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**-Y[=MODE]**,
**--overwrite[=MODE]** - Allow overwrite existing file.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**-L=LINEFEED**,
**--LF=LINEFEED**,
**--linefeed=LINEFEED** - Output linefeed. LINEFEED is one of: `CR`/`LF`/`CRLF`/`LFCR`/`DETECT`.Default is `CRLF`.

**-I=PATH**,
**--add-examples-path=PATH** - Add paths list for examples searching.

**-E=LANG:EXT[,EXT...]**,
**--add-code-file-extention=LANG:EXT[,EXT...]**,
**--add-code-file-extentions=LANG:EXT[,EXT...]** - Add file extention for the code for lang detection.

**-P=LANG:PREFIX**,
**--add-code-cut-prefix=LANG:PREFIX** - Add prefix for the cut labels in the code files.

**--add-code-comment-marker=LANG:MARKER_PAIR** - Add marker for comments, signle line marker or pair in form `BEGIN|END`.

**--add-code-separator-prefix=LANG:PREFIX** - Add lang prefix for the separator line.

**--set-code-case-sens=LANG:CASE_SENS** - Set code case sensitivity.

**--set-code-prototype-remove=LANG:KWD_COMMA_LIST** - Set code case sensitivity.

**--set-code-assign-op=LANG:ASSIGN_OP**,
**--set-code-assign-operator=LANG:ASSIGN_OP** - Set code assign operator.

**--set-code-block-chars=LANG:{}** - Set code block open and close characters, like "{}" for C/C++.

**--set-code-statement-separator=LANG:SEP** - Set code statement separator, like ";" (semicolon) for C/C++.

**-T=LANG:TAG**,
**--set-code-listing-tag=LANG:TAG** - Set target markdown tag for the code section.

**--set-code-processing-handler=LANG:HANDLER_TYPE:HANDLER_NAME** - Set code processing handler for language `LANG`.
`HANDLER_TYPE` can be one of the: `fn-prototype-extract`, `cls-prototype-extract`, `fn-prototype-format`, `cls-prototype-format`
`HANDLER_NAME` can be one of the: `cc-fn-ptt-extract`, `cc-cls-ptt-extract`, `cc-fn-ptt-format`, `cc-cls-ptt-format`.

**-O=OPTS**,
**--set-insert-options=OPTS** - Set default insert options.

**--processing-options=OPTS** - Set processing (output generation) options.

**-C=VAR:VAL**,
**--set-var=VAR:VAL**,
**--set-condition-var=VAR:VAL** - Set variable valie for conditions and substitutions.

**-R=NAME**,
**--target-renderer=NAME** - Set target renderer (`github`/`doxygen`).

**--target-format=NAME** - Set target format (md/html/rtf/pdf).

**--target-name=NAME**,
**--rendering-target-name=NAME** - Set target name.

**-m=TAG:REPLACETO**,
**--meta-tag-replace=TAG:REPLACETO** - Add meta tag name replacement.

**-M=TAG:SERIALIZETO**,
**--meta-tag-serialize=TAG:SERIALIZETO**,
**--meta-tag-serialize-as=TAG:SERIALIZETO** - Add meta tag name for serialization.

**--meta-tag-set-type=TYPE:TAG[,TAG...]**,
**--set-meta-tag-type=TYPE:TAG[,TAG...]** - Set meta tag type (`TextFirst`/`TextReplace`/`TextMerge`/`List`/`CommaList`/`Set`/`CommaSet`/`UniqueList`/`UniqueCommaList`/`RootOnly`).

**--serialize-meta-tags=TAG[,TAG...]**,
**--meta-tag-serialize-list=TAG[,TAG...]** - Set/append meta tag serialize list to add to meta section. Use `+` sign at first position to append tags to list.

**--document-meta-tags=TAG[,TAG...]** - Set/append list of meta tags to add to document body/text. Use `+` sign at first position to append tags to list.

**--numeric-sections-max-level=LEVEL** - Set max level for sections numeration.
Allowed values: `0`/`inf`/`infinite`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `11`, `12`, `13`, `14`, `15` or `16`, initial value: `infinite`.

**--toc-max-level=LEVEL** - Set max section level for table of contents (TOC).
Allowed values: `0`/`inf`/`infinite`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `10`, `11`, `12`, `13`, `14`, `15` or `16`, initial value: `infinite`.

**--restrict-path-raise=RAISE** - Raise insert restriction path up to 4 levels max.
Min value: `0`, max value: `4`, initial value: ` `.

**--document-language=LANG** - Set default document language, which used if no language tag in document meta info.

**--force-document-language=LANG** - Force set document language, override document meta info.

**--batch-output-path=PATH**,
**--batch-output-root=PATH** - Set output root path for batch mode.

**-X=MASK,...**,
**--batch-exclude-files=MASK,...** - Exclude files from parsing in the batch mode. The `MASK` parameter is a simple file mask, where `*` means any number of any chars, and `?` means exact one of any char. In addition, symbol `^` in front and/or back of the mask means that the mask will be bound to beginning/ending of the tested file name.
Also, regular expresion syntax allowed in form `{*?regex?*}YOURREGEX`. The regular expresions supports
See also: C++ Modified ECMA Script regular expression grammar - https://en.cppreference.com/w/cpp/regex/ecmascript.

**--batch-exclude-dir=DIRNAME,...**,
**--batch-exclude-dirs=DIRNAME,...** - Exclude dirs from scaning in the batch mode. The 'DIRNAME' parameter is a simple directory name, not a mask.

**--batch-scan=DIRNAME,...** - Perform batch job on taken directories.

**--batch-rscan=DIRNAME,...**,
**--batch-scan-recurse=DIRNAME,...** - Perform batch job on taken directories with recursion scan.

**--batch-page-index-file=FILE**,
**--batch-pages-index-file=FILE** - Set index of processed pages file name.

**--batch-generate-page-index[=MODE]**,
**--batch-generate-page-index-file[=MODE]** - Generate index of processed pages.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--batch-git-add-file=FILE**,
**--batch-generate-git-add-file=FILE** - Generate bat/shell script with git add commands for all generated files.

**--batch-split-page-index-file[=MODE]**,
**--batch-split-pages-index-file[=MODE]** - Generate index of processed pages.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--copy-images[=MODE]**,
**--copy-image-files[=MODE]** - Copy image files to output folder, if it taken. No effect if output path not taken (see --batch-output-root) nor single file output path folder not equal to source path.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--flatten-image-links[=MODE]** - Flatten image links: 'some/path/to/image.png' becomes to 'some_path_to_image.png'. No effect if output path not taken (see --batch-output-root) nor single file output path folder not equal to source path.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--viewer-copy-to-source[=MODE]**,
**--viewer-copy-render-to-source-location[=MODE]** - Copy generated (rendered) file to source file location.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--viewer-copy-use-title[=MODE]**,
**--viewer-render-copy-use-doc-title[=MODE]** - Copy generated (rendered) file to source file location with file name obtained from the document title.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--strip-ext[=MODE]**,
**--strip-extention[=MODE]**,
**--strip-extentions[=MODE]** - Strip supported extentions in the local links.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--arg-list-value-style=STYLE** - Set default value style for <arg-list> tag values. Available values are `normal`, `bold`, `italic`, `bold-italic`, `italic-bold`, `backtick` and `backtick-quote`.

**--arg-list-type=TYPE** - Set default type for <arg-list> tag. Available values are `table` and `text`.

**--arg-list-title=TITLE** - Set default titles for <arg-list> tag in table mode.

**--val-list-value-style=STYLE** - Set default value style for <val-list> tag values. Available values are `normal`, `bold`, `italic`, `bold-italic`, `italic-bold`, `backtick` and `backtick-quote`.

**--val-list-type=TYPE** - Set default type for <val-list> tag. Available values are `table` and `text`.

**--val-list-title=TITLE** - Set default titles for <val-list> tag in table mode.

**--gv-dpi=DPI**,
**--gviz-dpi=DPI**,
**--graphviz-dpi=DPI** - Set DPI for Graphviz tools output.

**--gv-output-format=FORMAT**,
**--gviz-output-format=FORMAT**,
**--graphviz-output-format=FORMAT** - Set Graphviz tools output format (SVG/PNG).

**--gv-output-path=PATH**,
**--gv-output-root=PATH**,
**--gviz-output-path=PATH**,
**--gviz-output-root=PATH**,
**--graphviz-output-path=PATH**,
**--graphviz-output-root=PATH** - Set Graphviz tools output root path.

**--gv-show-labels[=MODE]**,
**--gviz-show-labels[=MODE]**,
**--graphviz-show-labels[=MODE]** - Show labels on Graphviz graphs.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--gv-keep-temp-dot-files[=MODE]**,
**--gviz-keep-temp-dot-files[=MODE]**,
**--graphviz-keep-temp-dot-files[=MODE]** - Keep temporary dot files.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--add-mdpp-extention=EXT[,EXT...]**,
**--add-mdpp-extentions=EXT[,EXT...]** - Add file extentions of the MD-PP files.

**--dont-lookup-for-doxygen[=MODE]** - Do not lookup for Doxygen (in registry).
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--dont-lookup-for-graphviz[=MODE]** - Do not lookup for Graphviz (in registry).
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--java=JAVA_EXE** - Set Java executable full path name.

**--java-home=JAVA_HOME** - Set Java home. Java executable must exist as $(JAVA_HOME)/bin/java.

**--plantuml=PLANTUML_JAR**,
**--plant-uml=PLANTUML_JAR** - Set Plant UML jar full path name.

**--puml-output-format=FORMAT**,
**--plantuml-output-format=FORMAT**,
**--plant-uml-output-format=FORMAT** - Set PlantUML output format (SVG/PNG).

**--puml-output-path=PATH**,
**--puml-output-root=PATH**,
**--plantuml-output-path=PATH**,
**--plantuml-output-root=PATH**,
**--plant-uml-output-path=PATH**,
**--plant-uml-output-root=PATH** - Set PlantUML output root path.

**--puml-show-labels[=MODE]**,
**--plantuml-show-labels[=MODE]**,
**--plant-uml-show-labels[=MODE]** - Show labels on PlantUML raphs.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--generated-output-path=PATH**,
**--generated-output-root=PATH** - Set output root path for generated files (same as `--graphviz-output-path=PATH --plant-uml-output-path=PATH`).

**--clear-generation-cache[=MODE]** - Clear cached information of the generated files - force regenerate all images on next call. No other actions will be performed if this option was taken.
Flag option, allowed values: `+`/`1`/`Y(es)`/`T(rue)` or /`-`/`0`/`N(o)`/`F(alse)`, default value: `TRUE`.

**--gcc** - GCC messages format instead of MSVC format.

**--autocomplete-install** - Install autocompletion to bash/clink(cmd).

**--autocomplete-uninstall** - Remove autocompletion from bash/clink(cmd).


