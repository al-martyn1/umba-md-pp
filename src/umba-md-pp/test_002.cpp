/*! \file
    \brief Генерируем CharClass таблицу
 */

#include "umba/umba.h"
#include "umba/tokeniser.h"



int main(int argc, char* argv[])
{

    umba::tokeniser::CharClass charClasses[128] = { umba::tokeniser::CharClass::none };
    umba::tokeniser::generation::generateCharClassTable(charClasses);

    umba::tokeniser::generation::CommentType  commentType  = umba::tokeniser::generation::CommentType ::cppLike;
    umba::tokeniser::generation::ArrayType    arrayType    = umba::tokeniser::generation::ArrayType   ::stdArray;
    umba::tokeniser::generation::TypeNameType typeNameType = umba::tokeniser::generation::TypeNameType::cppLike;

    if (argc>1)
    {
        commentType  = umba::tokeniser::generation::CommentType ::cLike;
        arrayType    = umba::tokeniser::generation::ArrayType   ::cLike;
        typeNameType = umba::tokeniser::generation::TypeNameType::cLike;
    }

    umba::tokeniser::generation::printCharClassArray( charClasses, commentType, arrayType, typeNameType /* , const std::string &varName="charClassesTable" */ );

    return 0;
}

