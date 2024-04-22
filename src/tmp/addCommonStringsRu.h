
        addMdCommonStr("flags-section-title"                 , "$(Name) флаги"        );
        addMdCommonStr("enum-section-title"                  , "$(Name) перечисление" );

        addMdCommonStr("class-section-title"                 , "$(Name) класс"     );
        addMdCommonStr("class-members-section-title"         , "Поля класса"       );
        addMdCommonStr("class-methods-section-title"         , "Методы"            );

        addMdCommonStr("namespace-section-title"             , "$(Name) пространство имён" );
        addMdCommonStr("namespace-vars-section-title"        , "Переменные"        );
        addMdCommonStr("namespace-functions-section-title"   , "Функции"           );
        addMdCommonStr("namespace-classes-section-title"     , "Классы"            );
        
        addMdCommonStr("method-section-title"                , "$(Name) метод"     );
        addMdCommonStr("function-section-title"              , "$(Name) функция"   );

        addMdCommonStr("method-parameters-section-title"     , "Параметры"         );
        addMdCommonStr("method-returns-section-title"        , "Возвращаемое значение" );
        addMdCommonStr("method-no-parameters-text"           , "Данный метод не имеет параметров.");
        addMdCommonStr("method-return-type-text"             , "Возвращаемый тип: $(TypeInfo)");
        addMdCommonStr("function-parameters-section-title"   , "Параметры"         );
        addMdCommonStr("function-returns-section-title"      , "Возвращаемое значение"      );
        addMdCommonStr("function-no-parameters-text"         , "Данная функция не имеет параметров."  );
        addMdCommonStr("function-return-type-text"           , "Возвращаемый тип: $(TypeInfo)");

        addMdCommonStr("remarks-subsection-title"            , "Примечания"            );
        addMdCommonStr("return-subsection-title"             , "Возвращаемое значение" );
        addMdCommonStr("example-subsection-title"            , "Пример использования"  );


        addMdCommonStr("index-section-title"                 , "Индекс"             );
        addMdCommonStr("enum-index-text"                     , "$(RefText) перечисление");
        addMdCommonStr("flags-index-text"                    , "$(RefText) флаги");
        addMdCommonStr("namespace-index-text"                , "$(RefText) пространство имён");
        addMdCommonStr("class-index-text"                    , "$(RefText) класс");


        addSqCommonStr("_set"     , "Поддержка оператора индексирования ('[]'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#set) для получения подробностей.");
        addSqCommonStr("_get"     , "Поддержка оператора индексирования ('[]'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#get) для получения подробностей.");
        addSqCommonStr("_add"     , "Поддержка оператора 'plus' ('+'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#add) для получения подробностей.");
        addSqCommonStr("_sub"     , "Поддержка оператора 'minus' ('-'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#sub) для получения подробностей.");
        addSqCommonStr("_mul"     , "Поддержка оператора 'multiply' ('*'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#mul) для получения подробностей.");
        addSqCommonStr("_div"     , "Поддержка оператора 'divide' ('/'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#div) для получения подробностей.");
        addSqCommonStr("_unm"     , "Поддержка оператора 'unary minus' ('-'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#unm) для получения подробностей.");
        addSqCommonStr("_cmp"     , "Поддержка операторов сравнения (<, >, <=, >=, ==, !=). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#cmp) для получения подробностей.");
        addSqCommonStr("_modulo"  , "Поддержка оператора 'modulo' ('%'). См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#modulo) для получения подробностей.");
        addSqCommonStr("_tostring", "Поддержка преобразования в строку. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#tostring) для получения подробностей.");


        // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html
        addSqCommonStr("tointeger"   , "Преобразует значение в integer и возвращает его, см. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#integer) для получения подробностей. Реализован для совместимости со встроенными типами. Метод toInteger() также реализован для соответствия соглашениям об именовании camelCase.");
        addSqCommonStr("tofloat"     , "Преобразует значение во float и возвращает его, см. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#float) для получения подробностей. Реализован для совместимости со встроенными типами. Метод  toFloat() также реализован для соответствия соглашениям об именовании camelCase.");
        addSqCommonStr("tostring"    , "Преобразует значение в string и возвращает его, см. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#string) для получения подробностей. Реализован для совместимости со встроенными типами. Метод toString() также реализован для соответствия соглашениям об именовании camelCase.");
        addSqCommonStr("tointeger64" , "Преобразует значение в DotNut.Integer64 и возвращает его. Метод toInteger64() также реализован для соответствия соглашениям об именовании camelCase.");


        // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#string
        addSqCommonStr("len"     , "Возвращает количество элементов контейнера. String/Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.len) для получения подробностей." );
        addSqCommonStr("pop"     , "Удаляет последний элемент контейнера. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.pop) для получения подробностей." );
        addSqCommonStr("top"     , "Возвращает последний элемент контейнера. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.top) для получения подробностей." );
        addSqCommonStr("slice"   , "Возвращает часть контейнера ('срез') как новый контейнер. String/Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.slice) для получения подробностей." );
        addSqCommonStr("push"    , "Добавляет элемент в конец контейнера. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.push) для получения подробностей." );
        addSqCommonStr("append"  , "Добавляет элемент в конец контейнера. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.append) для получения подробностей." );
        addSqCommonStr("extend"  , "Расширяет контейнер, добавляя контейнер, переданный в аргументе, в конец текущего. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.extend) для получения подробностей." );
        addSqCommonStr("insert"  , "Вставляет значение в контейнер в позицию ‘idx’. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.insert) для получения подробностей." );
        addSqCommonStr("remove"  , "Удаляет элемент контейнера в позиции ‘idx’. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.remove) для получения подробностей." );
        addSqCommonStr("resize"  , "Изменяет размер контейнера, удаляя или добавляя элементы. Значение параметра ‘fill’ используется для добавления новых элементов при расширении. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.resize) для получения подробностей." );
        addSqCommonStr("reverse" , "Измеряет порядок элементов текущего контейнера на обратный. Array-совместимый метод. См. [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.reverse) для получения подробностей." );
    
        addSqCommonStr("size"    , "Возвращает количество элементов в контейнере." );
        addSqCommonStr("empty"   , "Возвращает true если контейнер пуст." );


        //addSqCommonStr(, "");

        //addMdCommonStr("", "");
