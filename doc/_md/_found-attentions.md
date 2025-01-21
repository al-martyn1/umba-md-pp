---
Generator: Umba Brief Scanner
---

# _libs/encoding

- `[_libs/encoding/encoding.h:315]`
  Надо бы тут разобраться с файлами, которые UTF-16 static const
  UINT cpid_UTF16   = 1200; // LE static const UINT cpid_UTF16BE
  = 1201;



# _libs/marty_cpp

- `[_libs/marty_cpp/marty_cpp.h:3225]`
  название?

- `[_libs/marty_cpp/marty_cpp.h:4798]`
  std::make_tuple

- `[_libs/marty_cpp/marty_cpp.h:4979]`
  заменить std::stoll, чтоб умело парсить двоичные константы и
  разделители try



# _libs/marty_decimal

- `[_libs/marty_decimal/marty_bcd.h:377]`
  Оно не нужно



# _libs/marty_tr

- `[_libs/marty_tr/locales.h:1182]`
  ar-DZ Arabic (Algeria)

- `[_libs/marty_tr/locales.h:1185]`
  az-Cyrl Azerbaijani (Cyrillic) (_)

- `[_libs/marty_tr/locales.h:1194]`
  chr-Cher Cherokee (_)

- `[_libs/marty_tr/locales.h:1199]`
  de-AT German (Austria)

- `[_libs/marty_tr/locales.h:1204]`
  en-AU English (Australia)

- `[_libs/marty_tr/locales.h:1277]`
  ru-MD Russian (Moldova)

- `[_libs/marty_tr/macros.h:620]`
  Чего-то с прокси не срослось - компилятор помирает от
  вложенности шаблонов Порешал, сделав getter нешаблонным
  параметром с виртуальным оператором ()

- `[_libs/marty_tr/macros.h:624]`
  Пока не будем ничего делать, потом разберёмся



# _libs/umba

- `[_libs/umba/assert.h:81]`
  Сделать как надо



- `[_libs/umba/cmd_line.h:1647]`
  std::map<std::string, int>::const_iterator eit =
  optInfo.enumVals.find(optArgCopy);
    if (eit == optInfo.enumVals.end())
    {
    errMsg = std::string("Invalid option value taken. Option value can be one of: ") + optInfo.getAllEnumNames(", ", " or ") + std::string(" (") + optInfo.getAllOptionNames("/") + std::string(")");
    return false;
    }

- `[_libs/umba/cmd_line.h:3002]`
  Надо ProgramLocation проверить на юникод

- `[_libs/umba/filecache.h:238]`
  Not found

- `[_libs/umba/macros.h:417]`
  Чего-то с прокси не срослось - компилятор помирает от
  вложенности шаблонов Порешал, сделав getter нешаблонным
  параметром с виртуальным оператором ()

- `[_libs/umba/macros.h:421]`
  Пока не будем ничего делать, потом разберёмся

- `[_libs/umba/shellapi.h:333]`
  тут нужен замут через dl*, но пока лень и не особо нужно

- `[_libs/umba/time_service.cpp:183]`
  Сделать как надо

- `[_libs/umba/time_service.cpp:195]`
  Сделать как надо

- `[_libs/umba/time_service.cpp:204]`
  Сделать как надо

- `[_libs/umba/time_service.cpp:231]`
  Сделать как надо

- `[_libs/umba/time_service.cpp:257]`
  Сделать как надо

- `[_libs/umba/variadic_helpers.h:138]`
  Usage
  
  template< typename... SomeTypeListItems >
  
  inline
  
  AdcInjected adcInitInject( ADC_TypeDef* ADCx, AdcSamplingSpeed
  samplingSpeed
    , AdcInitHwOption doInitAdc // set to true to add injected channels over regular
    , SomeTypeListItems... structs)
  
  {
    getVariadicArgsType< 0, SomeTypeListItems... > buf[sizeof...(structs)];
    size_t sz = buildSomeTypeArray< sizeof...(structs) > (buf, structs...);
    return adcInitInject( ADCx, samplingSpeed, doInitAdc, buf, sz );
  
  }
    auto
    adc1 = umba::periph::adcInitInject( ADC1, umba::periph::AdcSamplingSpeed::high
    , umba::periph::AdcInitHwOption::init
    , PA1, PA2, PA3 // here is the list
    );

- `[_libs/umba/zz_detect_environment.h:683]`
  ???

- `[_libs/umba/zz_mcu_low_level.h:30]`
  Сделать как надо

- `[_libs/umba/zz_mcu_low_level.h:54]`
  Сделать как надо

- `[_libs/umba/zz_mcu_low_level.h:72]`
  Сделать как надо

- `[_libs/umba/zz_mcu_low_level.h:123]`
  Сделать как надо

- `[_libs/umba/zz_mcu_low_level.h:139]`
  Сделать как надо

- `[_libs/umba/zz_mcu_low_level.h:148]`
  Сделать как надо



# _src/simple-drawing

- `[_src/simple-drawing/smpd-test-001.cpp:323]`
  Inform about exception

- `[_src/simple-drawing/smpd-tokenizer.h:203]`
  Фильтры, установленные позже, отрабатывают раньше

- `[_src/simple-drawing/smpd-tokenizer.h:250]`
  Фильтры, установленные позже, отрабатывают раньше



# _src/umba-md-pp

- `[_src/umba-md-pp/document.h:104]`
  Тут надо добавить что-то типа хидера, или break line Или - не
  надо?

- `[_src/umba-md-pp/processing.h:1017]`
  Не понятно, с чего этот варнинг вылезает, надо разобраться

- `[_src/umba-md-pp/processing.h:1574]`
  Какое-то оформление надо

- `[_src/umba-md-pp/processing.h:1782]`
  JSON-META У нас универсальный парсер YAML/JSON и мета данные
  могут быть в формате JSON При вставке в JSON могут быть
  проблемы

- `[_src/umba-md-pp/snippet_options.h:864]`
  Тут бы надо предусмотреть, что в первой строке поиска надо
  пропустить окончание сигнатуры, но пока сойдёт и так

- `[_src/umba-md-pp/snippet_options.h:910]`
  - или тут надо уменьшить на размер конечной сигнатуры в
  строках?

- `[_src/umba-md-pp/umba-md-pp-view.cpp:501]`
  Fix RTF here

- `[_src/umba-md-pp/umba-md-pp-view.cpp:537]`
  Это вылезает, если предыдущая версия файла уже открыта в ворде

- `[_src/umba-md-pp/umba_md_processing_utils.h:572]`
  Не понятно, что за состояние, и почему я его не обработал

- `[_src/umba-md-pp/utils.h:357]`
  надо обдумать для других возможных путей

