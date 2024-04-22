
        addMdCommonStr("flags-section-title"                 , "$(Name) flags"     );
        addMdCommonStr("enum-section-title"                  , "$(Name) enum"      );

        addMdCommonStr("class-section-title"                 , "$(Name) class"     );
        addMdCommonStr("class-members-section-title"         , "Member fields"     );
        addMdCommonStr("class-methods-section-title"         , "Methods"           );

        addMdCommonStr("namespace-section-title"             , "$(Name) namespace" );
        addMdCommonStr("namespace-vars-section-title"        , "Variables"         );
        addMdCommonStr("namespace-functions-section-title"   , "Functions"         );
        addMdCommonStr("namespace-classes-section-title"     , "Classes"           );
        
        addMdCommonStr("method-section-title"                , "$(Name) method"    );
        addMdCommonStr("function-section-title"              , "$(Name) function"  );

        addMdCommonStr("method-parameters-section-title"     , "Parameters"        );
        addMdCommonStr("method-returns-section-title"        , "Return value"      );
        addMdCommonStr("method-no-parameters-text"           , "This method has no parameters.");
        addMdCommonStr("method-return-type-text"             , "Return type: $(TypeInfo)");
        addMdCommonStr("function-parameters-section-title"   , "Parameters"        );
        addMdCommonStr("function-returns-section-title"      , "Return value"      );
        addMdCommonStr("function-no-parameters-text"         , "This function has no parameters."  );
        addMdCommonStr("function-return-type-text"           , "Return type: $(TypeInfo)");

        addMdCommonStr("remarks-subsection-title"            , "Remarks"           );
        addMdCommonStr("return-subsection-title"             , "Return value"      );
        addMdCommonStr("example-subsection-title"            , "Example"           );


        addMdCommonStr("index-section-title"                 , "Index"             );
        addMdCommonStr("enum-index-text"                     , "$(RefText) enumeration");
        addMdCommonStr("flags-index-text"                    , "$(RefText) flags");
        addMdCommonStr("namespace-index-text"                , "$(RefText) namespace");
        addMdCommonStr("class-index-text"                    , "$(RefText) class");


        addSqCommonStr("_set"     , "Support for the index ('[]') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#set) for details.");
        addSqCommonStr("_get"     , "Support for the index ('[]') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#get) for details.");
        addSqCommonStr("_add"     , "Support for the 'plus' ('+') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#add) for details.");
        addSqCommonStr("_sub"     , "Support for the 'minus' ('-') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#sub) for details.");
        addSqCommonStr("_mul"     , "Support for the 'multiply' ('*') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#mul) for details.");
        addSqCommonStr("_div"     , "Support for the 'divide' ('/') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#div) for details.");
        addSqCommonStr("_unm"     , "Support for the 'unary minus' ('-') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#unm) for details.");
        addSqCommonStr("_cmp"     , "Support for the compare (<, >, <=, >=, ==, !=) operators. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#cmp) for details.");
        addSqCommonStr("_modulo"  , "Support for the 'modulo' ('%') operator. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#modulo) for details.");
        addSqCommonStr("_tostring", "Conversion to string support. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/metamethods.html#tostring) for details.");


        // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html
        addSqCommonStr("tointeger"   , "Convert the value to integer and returns it, see [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#integer) for details. Exposed for compatibility with built-in types. The toInteger() method has also been implemented to comply with camelCase naming conventions.");
        addSqCommonStr("tofloat"     , "Convert the value to float and returns it, see [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#float) for details. Exposed for compatibility with built-in types. The toFloat() method has also been implemented to comply with camelCase naming conventions.");
        addSqCommonStr("tostring"    , "Convert the value to string and returns it, see [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#string) for details. Exposed for compatibility with built-in types. The toString() method has also been implemented to comply with camelCase naming conventions.");
        addSqCommonStr("tointeger64" , "Convert the value to DotNut.Integer64 and returns it. The toInteger64() method has also been implemented to comply with camelCase naming conventions.");


        // http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#string
        addSqCommonStr("len"     , "Returns the container length. String/Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.len) for details." );
        addSqCommonStr("pop"     , "Removes a value from the back of the container. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.pop) for details." );
        addSqCommonStr("top"     , "Returns the value of the container with the higher index. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.top) for details." );
        addSqCommonStr("slice"   , "Returns a section of the container as new container. String/Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.slice) for details." );
        addSqCommonStr("push"    , "Appends the value at the end of the container. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.push) for details." );
        addSqCommonStr("append"  , "Appends the value at the end of the container. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.append) for details." );
        addSqCommonStr("extend"  , "Extends the container by appending all the items in the given container. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.extend) for details." );
        addSqCommonStr("insert"  , "Inserts the value at the position ‘idx’ in the container. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.insert) for details." );
        addSqCommonStr("remove"  , "Removes the value at the position ‘idx’ in the container. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.remove) for details." );
        addSqCommonStr("resize"  , "Resizes the container. Value of the parameter ‘fill’ will be used to fill the new container’s slots when the size specified is bigger than the previous size. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.resize) for details." );
        addSqCommonStr("reverse" , "Reverse the elements of the container in place. Array compatible method. See [squirrel manual](http://squirrel-lang.org/squirreldoc/reference/language/builtin_functions.html#array.reverse) for details." );
    
        addSqCommonStr("size"    , "Returns number of elements in the container." );
        addSqCommonStr("empty"   , "Returns true if there is no items in the container." );


        //addSqCommonStr(, "");

        //addMdCommonStr("", "");
