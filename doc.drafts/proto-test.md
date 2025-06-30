**Информация о документе**

**URL**: https://github.com/al-martyn1/umba-md-pp/blob/main/doc.drafts/proto-test.md

---

- [Прототипы функций](#прототипы-функций)
  - [Общие сведения](#общие-сведения)
    - [Извлечение блоков кода](#извлечение-блоков-кода)
      - [Бэктрейс по сигнатуре](#бэктрейс-по-сигнатуре)
    - [Извлечение прототипов функций](#извлечение-прототипов-функций)
    - [Стили форматирования прототипов функций](#стили-форматирования-прототипов-функций)
    - [Прототипы классов](#прототипы-классов)
  - [Примеры форматирования прототипов функций](#примеры-форматирования-прототипов-функций)
  - [Извлечение прототипа - директива prototype](#извлечение-прототипа---директива-prototype)
- [Извлекаем код по сигнатуре - makeConditionOperatorsInfoVec](#извлекаем-код-по-сигнатуре---makeconditionoperatorsinfovec)
  - [Код функции полностью](#код-функции-полностью)
  - [Директива извлечения прототипа](#директива-извлечения-прототипа)
  - [Стиль MSDN](#стиль-msdn)
  - [Стиль Unix man](#стиль-unix-man)
  - [Стиль Unix man (table)](#стиль-unix-man-table)
  - [Стиль Umba](#стиль-umba)
- [Функция select](#функция-select)
  - [Код функции](#код-функции)
  - [Стиль MSDN](#стиль-msdn-1)
  - [Стиль Unix man](#стиль-unix-man-1)
  - [Стиль Unix man (table)](#стиль-unix-man-table-1)
  - [Стиль Umba](#стиль-umba-1)
- [Функция CreateFileA](#функция-createfilea)
- [Функция CloseHandle](#функция-closehandle)
- [Функция GetLastError](#функция-getlasterror)
- [Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype, из блока, вложенный шаблон, угловые скобки вместе):](#извлекаем-фрагмент-кода-по-сигнатуре-прототип-директива-prototype-из-блока-вложенный-шаблон-угловые-скобки-вместе)

## Прототипы функций

### Общие сведения

#### Извлечение блоков кода

Блоки кода могут извлекаться из исходных текстов проекта различными способами.
Сейчас мы рассматриваем способ извлечения фрагмента кода по сигнатуре.

##### Бэктрейс по сигнатуре

Сигнатура блока кода может быть не полной. Так сделано, чтобы не нужно было задавать сигнатуры
полностью с учетом шаблонов и коментариев, и прочего подобного. 
Для извлечения полной сигнатуры производится бэктрейс после нахождения явно заданной сигнатуры.
Бэктрейс производится до нахождения строки коментария, пустой строки, или закрывающей блок скобки.

Бэктрейс может ломаться на коде `C++` с концептами, использующими `requires requires { ... }`.
Этот момент требует доработки.
Но для других языков всё в целом гораздо проще.


#### Извлечение прототипов функций

В языке `C++` функция может быть `inline`, при этом у неё нет прототипа, а есть только её тело.

Функция извлечения прототипа функции из блока кода умеет извлекать прототип функции 
как из блока кода функции,
так и найдя отдельный прототип функции.
В случае блока кода функции тело функции удаляется.

Во многих языках нет понятия прототипа функции, но в документацию
тело функции не должно добавляться. Функция извлечения прототипа функции
позволяет добавить в документацию только прототип/заголовок функции даже при 
отсутствии отдельного прототипа в коде.


#### Стили форматирования прототипов функций

При форматировании прототипов функций могут быть использованы стили:

- `'msdn'`/`'ms'` - стиль, используемый в MSDN/документации Microsoft по MS API;

- `'man'`/`'linux'`/`'unix'`/`'bsd'` - стиль вывода документации в формате команды `'man'`;

- `'tab-man'`/`'table-man'`/... - стиль вывода документации в формате команды `'man'` с "табличным" форматированием;

- `'umba'`/`'mdr'`/`'dotnut'` - оригинальный MDR стиль, схожий со стилем `'msdn'`.

Оригинальные стили `'msdn'` и `'man'` не полностью копируются: так, спецификаторы
и возвращаемое значение располагаются на отдельных строках, а не в той же строке, как
в оригинальном формате; отступы соблюдены не совсем так же.


#### Прототипы классов

Прототипы классов - это извлечение классов целиком из исходного кода с удалением тел всех 
`inline` функций. Актуально для `C++` классов с `inline` функциями, а также для языков,
у которых тело функции всегда задаётся в классе и нет разделения на прототип и реализацию
(`C#` или `PHP`, как пример).

На текущий момент не реализовано.


### Примеры форматирования прототипов функций

- MSDN - [https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea](https://learn.microsoft.com/en-us/windows/win32/api/fileapi/nf-fileapi-createfilea)

- MAN - [https://man7.org/linux/man-pages/man2/select.2.html](https://man7.org/linux/man-pages/man2/select.2.html)


### Извлечение прототипа - директива prototype

Прототип извлекается из блока, найденного по сигнатуре. Но, также, прототип может быть обнаружен в виде прототипа. Задание блока 
для извлечения прототипа функции обычно осуществляется маркером окончания `'{}'`, 
а задание блока или прототипа
для извлечения прототипа функции осуществляется маркером окончания `'{};'`, 

При извлечении прототипов из них могут удаляться незначащие ключевые слова/спецификаторы, такие как `'inline'` или `'WINAPI'` (в примере удаляется `'inline'`, 
это зависит от настроек, а не закодировано жестко).

```
#!prototype{proto-fmt=FMT} tests/cc/templates/03_function_template.h#`std::vector< std::pair<ConditionOperators, StringType> > makeConditionOperatorsInfoVec`-{};
where FMT is one of msdn|man|umba
```







## Извлекаем код по сигнатуре - makeConditionOperatorsInfoVec

### Код функции полностью

Директива извлечения фрагмента кода:
```
#!insert tests/cc/templates/03_function_template.h#`std::vector< std::pair<ConditionOperators, StringType> > makeConditionOperatorsInfoVec`-{};
```

Код функции:

```cpp
template< typename CharType = char, // Тип символа
typename StringType = std::string<CharType, std::char_traits<CharType>, std::allocator<CharType> > //!< Тип строки
>
inline
std::vector< std::pair<ConditionOperators, StringType> >
makeConditionOperatorsInfoVec( void (__stdcall *)(const std::string<char> &) handler
, StringType str, const std::string<char> &str2 = std::string("aaa") /* AAA */
) //! Создаёт вектор условных операторов
const = 0
{
    const auto &condInfoVec = getConditionOperatorsInfoVec();
    for(const auto &condInfo : condInfoVec)
    {
        auto pos = str.find(condInfo.second);
        if (pos!=str.npos)
        {
            left .assign(str, 0, pos);
            return condInfo.first;
        }
    }

    return ConditionOperators::unknown;
}
```


### Директива извлечения прототипа

```
#!prototype{proto-fmt=FMT} tests/cc/templates/03_function_template.h#`std::vector< std::pair<ConditionOperators, StringType> > makeConditionOperatorsInfoVec`-{};
where FMT is one of msdn|man|tab-man|umba
```

### Стиль MSDN

```cpp
template<
  typename CharType   = char,
  typename StringType = std::string<CharType,std::char_traits<CharType>,std::allocator<CharType>>
>
std::vector<std::pair<ConditionOperators,StringType>>
makeConditionOperatorsInfoVec(
  void (__stdcall*) (const std::string<char>&) handler,
  StringType                                   str,
  const std::string<char>                      &str2   = std::string("aaa")
)
const = 0;
```

### Стиль Unix man

```cpp
template<typename CharType = char,
         typename StringType = std::string<CharType,std::char_traits<CharType>,std::allocator<CharType>>>
std::vector<std::pair<ConditionOperators,StringType>>
makeConditionOperatorsInfoVec(void (__stdcall*) (const std::string<char>&) handler,
                              StringType str,
                              const std::string<char>   &str2 = std::string("aaa"))
const = 0;
```

### Стиль Unix man (table)

```cpp
template<typename CharType   = char,
         typename StringType = std::string<CharType,std::char_traits<CharType>,std::allocator<CharType>>>
std::vector<std::pair<ConditionOperators,StringType>>
makeConditionOperatorsInfoVec(void (__stdcall*) (const std::string<char>&) handler,
                              StringType                                   str,
                              const std::string<char>                      &str2   = std::string("aaa"))
const = 0;
```

### Стиль Umba

```cpp
template< typename CharType   = char
        , typename StringType = std::string<CharType,std::char_traits<CharType>,std::allocator<CharType>>
        >
std::vector<std::pair<ConditionOperators,StringType>>
makeConditionOperatorsInfoVec( void (__stdcall*) (const std::string<char>&) handler
                             , StringType                                   str
                             , const std::string<char>                      &str2   = std::string("aaa")
                             )
const = 0;
```



## Функция select

### Код функции

```cpp
inline
int select(int nfds, fd_set *_Nullable restrict readfds,
                  fd_set *_Nullable restrict writefds,
                  fd_set *_Nullable restrict exceptfds,
                  struct timeval *_Nullable restrict timeout)
{
    return nfds;
}
```

### Стиль MSDN

```cpp
int
select(
  int            nfds,
  fd_set         *_Nullable restrict readfds,
  fd_set         *_Nullable restrict writefds,
  fd_set         *_Nullable restrict exceptfds,
  struct timeval *_Nullable restrict timeout
);
```

### Стиль Unix man

```cpp
int
select(int nfds,
       fd_set *_Nullable restrict readfds,
       fd_set *_Nullable restrict writefds,
       fd_set *_Nullable restrict exceptfds,
       struct timeval *_Nullable restrict timeout);
```

### Стиль Unix man (table)

```cpp
int
select(int            nfds,
       fd_set         *_Nullable restrict readfds,
       fd_set         *_Nullable restrict writefds,
       fd_set         *_Nullable restrict exceptfds,
       struct timeval *_Nullable restrict timeout);
```

### Стиль Umba

```cpp
int
select( int            nfds
      , fd_set         *_Nullable restrict readfds
      , fd_set         *_Nullable restrict writefds
      , fd_set         *_Nullable restrict exceptfds
      , struct timeval *_Nullable restrict timeout
      );
```


## Функция CreateFileA

```cpp
HANDLE CreateFileA(
  LPCSTR                lpFileName,
  DWORD                 dwDesiredAccess,
  DWORD                 dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD                 dwCreationDisposition,
  DWORD                 dwFlagsAndAttributes,
  HANDLE                hTemplateFile
);
```

```cpp
HANDLE
CreateFileA(
  LPCSTR                lpFileName,
  DWORD                 dwDesiredAccess,
  DWORD                 dwShareMode,
  LPSECURITY_ATTRIBUTES lpSecurityAttributes,
  DWORD                 dwCreationDisposition,
  DWORD                 dwFlagsAndAttributes,
  HANDLE                hTemplateFile
);
```

```cpp
HANDLE
CreateFileA(LPCSTR lpFileName,
            DWORD dwDesiredAccess,
            DWORD dwShareMode,
            LPSECURITY_ATTRIBUTES lpSecurityAttributes,
            DWORD dwCreationDisposition,
            DWORD dwFlagsAndAttributes,
            HANDLE hTemplateFile);
```

```cpp
HANDLE
CreateFileA( LPCSTR                lpFileName
           , DWORD                 dwDesiredAccess
           , DWORD                 dwShareMode
           , LPSECURITY_ATTRIBUTES lpSecurityAttributes
           , DWORD                 dwCreationDisposition
           , DWORD                 dwFlagsAndAttributes
           , HANDLE                hTemplateFile
           );
```


## Функция CloseHandle

```cpp
BOOL CloseHandle(
  HANDLE hObject
);
```

```cpp
BOOL
CloseHandle(
  HANDLE hObject
)
;
```

```cpp
BOOL
CloseHandle(HANDLE hObject)
;
```

```cpp
BOOL
CloseHandle( HANDLE hObject
           )
;
```


## Функция GetLastError

```cpp
DWORD GetLastError();
```

```cpp
DWORD
GetLastError(

)
;
```

```cpp
DWORD
GetLastError( )
;
```

```cpp
DWORD
GetLastError(
            )
;
```



## Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype, из блока, вложенный шаблон, угловые скобки вместе):
```
#!prototype umba-md-pp/snippet_options.h#`const std::vector< std::pair<ConditionOperators, std::string>>& getConditionOperatorsInfoVec`-{}
```

```cpp
const
std::vector<std::pair<ConditionOperators,std::string>>>> &
getConditionOperatorsInfoVec(

)
;
```


Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype, из блока, вложенный шаблон, угловые скобки раздельно):
```
#!prototype umba-md-pp/snippet_options.h#`std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec`-{}
```

```cpp
std::vector<std::pair<ConditionOperators,std::string>>
makeConditionOperatorsInfoVec(

)
;
```


Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype, из блока, вложенный шаблон, угловые скобки раздельно, форматируем):
```
#!prototype{format} umba-md-pp/snippet_options.h#`std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec`-{}
```

!!! Options parsing error

Fail,FilenameLineNo,Path,TrimArround,PrototypeFormat=1
#!prototype{format} umba-md-pp/snippet_options.h#`std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec`-{}


Извлекаем фрагмент кода по сигнатуре (полный блок):
```
#!insert umba-md-pp/snippet_options.h#`std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec`-{}
```

```cpp
inline
std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec() //! Создаёт вектор условных операторов
{
    return std::vector< std::pair<ConditionOperators, std::string> >{ { ConditionOperators::equal       , "==" }
                                                                    , { ConditionOperators::notEqual    , "!=" }
                                                                    , { ConditionOperators::greaterEqual, ">=" }
                                                                    , { ConditionOperators::lessEqual   , "<=" }
                                                                    , { ConditionOperators::greater     , ">" }
                                                                    , { ConditionOperators::less        , "<" }
                                                                    };
}
```


Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype, из блока):
```
#!prototype umba-md-pp/extern_tools.h#`inline std::string findDoxygenBinPathImpl`-{}
```

```cpp
std::string
findDoxygenBinPathImpl(
  bool dontTouchSystem
)
;
```


Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype):
```
#!prototype umba-md-pp/extern_tools.h#`inline std::string findDoxygenBinPathImpl`
```

```cpp
std::string
findDoxygenBinPathImpl(
  bool dontTouchSystem
)
;
```


Извлекаем фрагмент кода по сигнатуре (прототип, директива prototype, с форматированием):
```
#!prototype{fmt} umba-md-pp/extern_tools.h#`inline std::string findDoxygenBinPathImpl`
```

!!! Options parsing error

Fail,FilenameLineNo,Path,TrimArround,PrototypeFormat=1
#!prototype{fmt} umba-md-pp/extern_tools.h#`inline std::string findDoxygenBinPathImpl`


Извлекаем фрагмент кода по сигнатуре:
```
#!insert{nolineno,noKeepCutTags} umba-md-pp/extern_tools.h#`inline std::string findDoxygenBinPathImpl`-{}
```

```cpp
inline std::string findDoxygenBinPathImpl(bool dontTouchSystem)
{
    if (dontTouchSystem)
        return std::string();

#if defined(WIN32) && defined(_WIN32)

    std::wstring p;
    if (!umba::win32_utils::regQueryAppInstallLocationBin(std::wstring(L"doxygen_is1"), p))
        return std::string();

    return umba::toUtf8(p);

#else

    return std::string();

#endif
}
```

