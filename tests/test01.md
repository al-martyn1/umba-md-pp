
# 1. Title

// Print lines, noKeepCutTags #print-ver-all

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

cpp/print_ver.h:7
```cpp
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
```


// Print lines, keepCutTags   #print-ver-all

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

cpp/print_ver.h:7
```cpp
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
```


// No lines, keepCutTags   #print-ver-all

Fail,Filename,FilenameLineNo,Path,TrimArround

cpp/print_ver.h:6
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

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

cpp/print_ver.h:25
```cpp
25: void printCommitHash( const std::string &indent = "" )      // 23
26: {                                                           // 24
27:     if (appCommitHash.empty())                              // 25
28:         return;                                             // 26
29:     std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
30: }                                                           // 28
```


// printOnlyVersion

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

cpp/print_ver.h:7
```cpp
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
```


## 1.1. Subtitle
### 1.1.1. Русский заголовок третьего уровня

```
lst existing
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12 - this command not processed due it in listing
```

https://moonbase59.github.io/gh-toc/
https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax
Github tips, notes, warnings and so on - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#alerts
todo's - https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#task-lists

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

!!! File not found
#!insert{lineno} ..\src\umba-md-pp\log.h#6-12

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

cpp/print_ver.h:7
```cpp
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
```

Fail,Filename,FilenameLineNo,LineNo,Path,TrimArround

cpp/print_ver.h:7
```cpp
 7: void printOnlyVersion()                                     // 7
 8: {                                                           // 8
 9:     std::cout<<appVersion<<"\n";                            // 9
10: }                                                           // 10
```

# 2. Other Title

## 2.1. Other Subtitle

Bla-bla.

