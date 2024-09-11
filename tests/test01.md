# Препроцессор для Markdown 01

Test number one


# Title

Test01
Link to [test02.md](test02.md_)
Image - ![Image](img/filled-96.png)

Link to document which is out of the hierarchy [upper_inc.md_](../../upper_inc.md_)
Image from out of the  hierarchy- ![Image](../../upper-level-filled-96.png)



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

# Other Title

## Other Subtitle

Bla-bla.

  - [Title](#user-content-title)
    - [Subtitle](#user-content-subtitle)
      - [Русский заголовок третьего уровня](#user-content-русский-заголовок-третьего-уровня)
  - [Other Title](#user-content-other-title)
    - [Other Subtitle](#user-content-other-subtitle)


