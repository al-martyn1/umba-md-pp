# Препроцессор для Markdown 01


# Title

Test01

// Print lines, noKeepCutTags #print-ver-all

!!! File not found in:
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print-ver-all


// Print lines, keepCutTags   #print-ver-all

!!! File not found in:
#!insert{lineno,noKeepCutTags} cpp/print_ver.h#print-ver-all


// No lines, keepCutTags   #print-ver-all

!!! File not found in:
#!insert{nolineno,noKeepCutTags} cpp/print_ver.h#print-ver-all


// printCommitHash

!!! File not found in:
#!insert{lineno} cpp/print_ver.h#printCommitHash


// printOnlyVersion

!!! File not found in:
#!insert{lineno} cpp/print_ver.h#printOnlyVersion


## Subtitle
### Русский заголовок третьего уровня

```
lst existing
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12 - this command not processed due it in listing
```

https://moonbase59.github.io/gh-toc/
https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
Github tips, notes, warnings and so on - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#alerts
todo's - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#task-lists

!!! File not found in:
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12

!!! File not found in:
#!insert{lineno} cpp/print_ver.h#print-ver-all

!!! File not found in:
#!insert{lineno} cpp/print_ver.h#printOnlyVersion

# Other Title

## Other Subtitle

Bla-bla.

  - [Title](#user-content-title)
    - [Subtitle](#user-content-subtitle)
      - [Русский заголовок третьего уровня](#user-content-русский-заголовок-третьего-уровня)
  - [Other Title](#user-content-other-title)
    - [Other Subtitle](#user-content-other-subtitle)


