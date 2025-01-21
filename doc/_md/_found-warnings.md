---
Generator: Umba Brief Scanner
---

# _libs/encoding

- `[_libs/encoding/disable_warnings_begin.h:6]`
  C4668: '__LP64__' is not defined as a preprocessor macro,
  replacing with '0' for '#if/#elif'

- `[_libs/encoding/disable_warnings_begin.h:7]`
  C4838: conversion from 'int' to 'char' requires a narrowing
  conversion

- `[_libs/encoding/disable_warnings_begin.h:8]`
  C4310: cast truncates constant value

- `[_libs/encoding/disable_warnings_begin.h:9]`
  C4309: 'initializing': truncation of constant value

- `[_libs/encoding/disable_warnings_begin.h:10]`
  C4365: 'initializing': conversion from 'int' to 'size_t',
  signed/unsigned mismatch

- `[_libs/encoding/disable_warnings_begin.h:11]`
  C4244: 'initializing': conversion from '__int64' to 'int',
  possible loss of data

- `[_libs/encoding/disable_warnings_begin.h:12]`
  C4061: enumerator 'CompactEncDet::NUM_CORPA' in switch of enum
  'CompactEncDet::TextCorpusType' is not explicitly handled by a
  case label

- `[_libs/encoding/disable_warnings_begin.h:13]`
  C4267: 'initializing': conversion from 'size_t' to 'int',
  possible loss of data

- `[_libs/encoding/disable_warnings_begin.h:14]`
  C4996: 'strncpy': This function or variable may be unsafe.
  Consider using strncpy_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.

- `[_libs/encoding/disable_warnings_begin.h:15]`
  C5039: 'qsort': pointer or reference to potentially throwing
  function passed to 'extern "C"' function under -EHc. Undefined
  behavior may occur if this function throws an exception

- `[_libs/encoding/disable_warnings_begin.h:16]`
  C5045: Compiler will insert Spectre mitigation for memory load
  if /Qspectre switch specified

- `[_libs/encoding/encoding.cpp:11]`
  C5045: Compiler will insert Spectre mitigation for memory load
  if /Qspectre switch specified

- `[_libs/encoding/encoding.cpp:223]`
  C4505: 'encoding::toLower': unreferenced function with internal
  linkage has been removed

- `[_libs/encoding/encoding.h:51]`
  C4820: 'encoding::WindowsEncodingInfo': '4' bytes padding added
  after data member 'encoding::WindowsEncodingInfo::codePage'



# _libs/encoding/util

- `[_libs/encoding/util/basictypes.h:281]`
  if Dest or Source is a non-POD type, the result of the memcpy
  is likely to surprise you.



# _libs/encoding/util/encodings

- `[_libs/encoding/util/encodings/encodings.cc:19]`
  C4711: function 'char const * __ptr64 __cdecl LanguageCode(enum
  Language)' selected for automatic inline expansion

- `[_libs/encoding/util/encodings/encodings.cc:20]`
  C4738: storing 32-bit float result in memory, possible loss of
  performance

- `[_libs/encoding/util/encodings/encodings.h:115]`
  This function does not currently return true for all encodings
  that are supersets of Ascii 7-bit.

- `[_libs/encoding/util/encodings/encodings.h:122]`
  This function does not currently return true for all encodings
  that are 8-bit encodings.



# _libs/encoding/util/languages

- `[_libs/encoding/util/languages/languages.cc:19]`
  C4711: function 'char const * __ptr64 __cdecl LanguageCode(enum
  Language)' selected for automatic inline expansion

- `[_libs/encoding/util/languages/languages.cc:20]`
  C4738: storing 32-bit float result in memory, possible loss of
  performance

- `[_libs/encoding/util/languages/languages.h:106]`
  This function provides only a simple test on the values of the
  two Language arguments. It returns false if either language is
  invalid. It returns true if the language arguments are equal,
  or if they are both Chinese languages, both Norwegian
  languages, or both Portuguese languages, as defined by
  IsChineseLanguage, IsNorwegianLanguage, and
  IsPortugueseLanguage. Otherwise it returns false.



# _libs/marty_cpp

- `[_libs/marty_cpp/marty_cpp.h:2784]`
  C4996: 'sprintf': This function or variable may be unsafe.
  Consider using sprintf_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.

- `[_libs/marty_cpp/marty_cpp.h:2847]`
  C4996: 'sprintf': This function or variable may be unsafe.
  Consider using sprintf_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.



# _libs/marty_cpp/warnings

- `[_libs/marty_cpp/warnings/disable_C5045.h:4]`
  C5045: Compiler will insert Spectre mitigation for memory load
  if /Qspectre switch specified

- `[_libs/marty_cpp/warnings/pop.h:2]`
  C5031: #pragma warning(pop): likely mismatch, popping warning
  state pushed in different file

- `[_libs/marty_cpp/warnings/pop.h:3]`
  C4193: #pragma warning(pop) : no matching '#pragma
  warning(push)'



# _libs/marty_decimal

- `[_libs/marty_decimal/marty_bcd_decimal_impl.h:185]`
  C4996: This function or variable may be unsafe. Consider using
  sprintf_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.

- `[_libs/marty_decimal/marty_bcd_decimal_impl.h:747]`
  C4702: unreachable code

- `[_libs/marty_decimal/marty_decimal.h:60]`
  C28159: Consider using 'GetTickCount64' instead of
  'GetTickCount'



# _libs/marty_decimal/warnings

- `[_libs/marty_decimal/warnings/disable_C5045.h:4]`
  C5045: Compiler will insert Spectre mitigation for memory load
  if /Qspectre switch specified

- `[_libs/marty_decimal/warnings/pop.h:2]`
  C5031: #pragma warning(pop): likely mismatch, popping warning
  state pushed in different file

- `[_libs/marty_decimal/warnings/pop.h:3]`
  C4193: #pragma warning(pop) : no matching '#pragma
  warning(push)'



# _libs/marty_tr

- `[_libs/marty_tr/locales.h:43]`
  C4623: 'class': default constructor was implicitly defined as
  deleted



# _libs/nlohmann/test/thirdparty/doctest

- `[_libs/nlohmann/test/thirdparty/doctest/doctest.h:4558]`
  is just a message in this context so we don't treat it as an
  assert



# _libs/nlohmann/test/thirdparty/Fuzzer

- `[_libs/nlohmann/test/thirdparty/Fuzzer/FuzzerInterface.h:17]`
  keep the interface in C.



# _libs/umba

- `[_libs/umba/errors.h:258]`
  C4774: 'sprintf' : format string expected in argument 2 is not
  a string literal

- `[_libs/umba/errors.h:259]`
  C4996: 'sprintf': This function or variable may be unsafe.
  Consider using sprintf_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.

- `[_libs/umba/errors.h:323]`
  C4774: 'sprintf' : format string expected in argument 2 is not
  a string literal

- `[_libs/umba/errors.h:324]`
  C4996: 'sprintf': This function or variable may be unsafe.
  Consider using sprintf_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.









# _libs/umba/warnings

- `[_libs/umba/warnings/disable_C4061.h:2]`
  C4061: enumerator in switch of enum is not explicitly handled
  by a case label

- `[_libs/umba/warnings/disable_C4062.h:2]`
  C4062: enumerator in switch of enum is not handled

- `[_libs/umba/warnings/disable_C4127.h:2]`
  C4127: conditional expression is constant

- `[_libs/umba/warnings/disable_C4191.h:2]`
  C4191: 'type cast': unsafe conversion from 'FARPROC' to '...'

- `[_libs/umba/warnings/disable_C4265.h:2]`
  C4265: class has virtual functions, but its non-trivial
  destructor is not virtual; instances of this class may not be
  destructed correctly #pragma warning(disable:5204) // warning
  C5204: class has virtual functions, but its trivial destructor
  is not virtual; instances of objects derived from this class
  may not be destructed correctly

- `[_libs/umba/warnings/disable_C4355.h:2]`
  C4355: 'this': used in base member initializer list

- `[_libs/umba/warnings/disable_C4365.h:2]`
  C4365: 'argument': conversion from 'int' to 'UINT',
  signed/unsigned mismatch

- `[_libs/umba/warnings/disable_C4464.h:2]`
  C4465: relative include path contains '..'

- `[_libs/umba/warnings/disable_C4555.h:2]`
  C4555: result of expression not used

- `[_libs/umba/warnings/disable_C4623.h:3]`
  C4623: default constructor was implicitly defined as deleted

- `[_libs/umba/warnings/disable_C4820.h:2]`
  C4820: 'N' bytes padding added after data member

- `[_libs/umba/warnings/disable_C4840.h:3]`
  C4840: non-portable use of class
  'std::basic_string<char,std::char_traits<char>,std::allocator<char>>'
  as an argument to a variadic function

- `[_libs/umba/warnings/disable_C4996.h:3]`
  C4996: This function or variable may be unsafe. Consider using
  sprintf_s instead. To disable deprecation, use
  _CRT_SECURE_NO_WARNINGS. See online help for details.

- `[_libs/umba/warnings/disable_C5045.h:4]`
  C5045: Compiler will insert Spectre mitigation for memory load
  if /Qspectre switch specified

- `[_libs/umba/warnings/disable_C5204.h:3]`
  C5204: class has virtual functions, but its trivial destructor
  is not virtual; instances of objects derived from this class
  may not be destructed correctly

- `[_libs/umba/warnings/disable_C5267.h:3]`
  C5267: definition of implicit copy constructor for 'i_base' is
  deprecated because it has a user-provided destructor

- `[_libs/umba/warnings/disable_common.h:5]`
  C4464: relative include path contains '..'

- `[_libs/umba/warnings/disable_unsafe_conversion.h:1]`
  C4191: 'type cast': unsafe conversion from 'FARPROC' to '...'

- `[_libs/umba/warnings/pop.h:2]`
  C5031: #pragma warning(pop): likely mismatch, popping warning
  state pushed in different file

- `[_libs/umba/warnings/pop.h:3]`
  C4193: #pragma warning(pop) : no matching '#pragma
  warning(push)'



# _libs/yaml-cpp/test/gtest-1.11.0/googletest/test

- `[_libs/yaml-cpp/test/gtest-1.11.0/googletest/test/googletest-death-test-test.cc:650]`
  in opt mode.

- `[_libs/yaml-cpp/test/gtest-1.11.0/googletest/test/googletest-death-test-test.cc:682]`
  in opt mode.

