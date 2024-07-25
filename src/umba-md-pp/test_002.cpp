/*! \file
    \brief Генерируем CharClass таблицу
 */

#include "umba/umba.h"
#include "umba/tokeniser.h"


#include <iostream>
#include <iomanip>
#include <exception>
#include <stdexcept>

using std::cout;
using std::cerr;


template< std::size_t N >
void setCharClassFlags( umba::tokeniser::CharClass (&charClasses)[N], char ch, umba::tokeniser::CharClass setClasses)
{
    std::size_t idx = umba::tokeniser::charToCharClassTableIndex(ch);
    if (idx>=N)
    {
        cerr << "Bad index" << std::endl;
        throw std::runtime_error("Bad index");
    }

    charClasses[idx] |= setClasses;
}

template< std::size_t N >
void setCharClassFlags( umba::tokeniser::CharClass (&charClasses)[N], const std::string &chars, umba::tokeniser::CharClass setClasses)
{
    for(auto ch : chars)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}

template< std::size_t N >
void setCharClassFlags( umba::tokeniser::CharClass (&charClasses)[N], char ch1, char ch2, umba::tokeniser::CharClass setClasses)
{
    for(auto ch=ch1; ch<=ch2; ++ch)
    {
        setCharClassFlags(charClasses, ch, setClasses);
    }
}

template< std::size_t N >
void setCharClassFlagsForBracePair( umba::tokeniser::CharClass (&charClasses)[N], const std::string &braceChars)
{
    if (braceChars.size()!=2)
    {
        cerr << "Braces def invalid size" << std::endl;
        throw std::runtime_error("Braces def invalid size");
    }

    setCharClassFlags(charClasses, braceChars[0], umba::tokeniser::CharClass::brace | umba::tokeniser::CharClass::open );
    setCharClassFlags(charClasses, braceChars[1], umba::tokeniser::CharClass::brace | umba::tokeniser::CharClass::close);
}






int main(int argc, char* argv[])
{

    /*
    Делаем таблицу, из которой получаем класс символа
    Таблица - это 128 позиций на базоые ASCII-символы
    Делаем это не вручную.
    Таблица генерируется не константная, надо уметь менять её в рантайме - например, 
    чтобы управлять поведением символов $/@ в зависимости от контекста - то ли они могутт быть в идентификаторе, то ли нет
 
    а) Имеем список операторов с названиями - "!==" -> "STRICT_NEQ"
       Все операторы разбираем посимвольно, для каждого символа ставим флаг CharClass::opchar
 
    Всё, что меньше пробела - флаг nonprintable, а также 0x7F
    [X] \r, \n - linefeed
    [X] \t     - tab
    [X] \r, \n, \t, ' ' - space
 
    [X] в) кавычка, двойная кавычка, бэктик - quot
    [X] г) Для {}()<>[] - расставляем флаги brace, open, close
    [X] д) ! ? , . ( ) - punctuation (что ещё?) - можно добавлять/менять в рантайме
    [X] е) A-Za-z - alpha, identifier, identifier_first
    [X] ж) 0-9    - digit, identifier
    [ ] з) '-'    - hyphen, opchar
    [ ] и) '_'    - underscore, identifier, identifier_first

    enum class CharClass : char_class_underlying_uint_t
    {
        // unknown       = 1,
        nonprintable     = 0x0001,
        linefeed         = 0x0002,
        space            = 0x0004,
        tab              = 0x0008,
        quot             = 0x0010,
        brace            = 0x0020,
        open             = 0x0040, // Для парных символов
        close            = 0x0080, // Для парных символов
        opchar           = 0x0100,
        operator_char    = 0x0100,
        punctuation      = 0x0200,
        digit            = 0x0400,
        alpha            = 0x0800,
        identifier       = 0x1000,
        identifier_first = 0x2000,
        hyphen           = 0x4000,
        underscore       = 0x8000
    };

    */

    // umba::char_class_underlying_uint_t

    umba::tokeniser::CharClass charClasses[128] = { umba::tokeniser::CharClass::none };

    // pairs
    setCharClassFlagsForBracePair( charClasses, "{}");
    setCharClassFlagsForBracePair( charClasses, "()");
    setCharClassFlagsForBracePair( charClasses, "[]");
    setCharClassFlagsForBracePair( charClasses, "<>");

    // ranges
    setCharClassFlags( charClasses, 'a', 'z', umba::tokeniser::CharClass::alpha | umba::tokeniser::CharClass::identifier | umba::tokeniser::CharClass::identifier_first);
    setCharClassFlags( charClasses, 'A', 'Z', umba::tokeniser::CharClass::alpha | umba::tokeniser::CharClass::identifier | umba::tokeniser::CharClass::identifier_first);
    setCharClassFlags( charClasses, '0', '9', umba::tokeniser::CharClass::digit | umba::tokeniser::CharClass::identifier );
    setCharClassFlags( charClasses,   0,  31, umba::tokeniser::CharClass::nonprintable);

    // sets
    setCharClassFlags( charClasses, "!%&*+,-./:;<=>?^|~", umba::tokeniser::CharClass::operator_char);
    setCharClassFlags( charClasses, "\r\n"              , umba::tokeniser::CharClass::linefeed);
    setCharClassFlags( charClasses, "\t"                , umba::tokeniser::CharClass::tab);
    setCharClassFlags( charClasses, "\r\n\t "           , umba::tokeniser::CharClass::space);
    setCharClassFlags( charClasses, ".,!?()\"\'"        , umba::tokeniser::CharClass::punctuation);
    setCharClassFlags( charClasses, "\"\'`"             , umba::tokeniser::CharClass::quot);
    setCharClassFlags( charClasses, "@#$"               , umba::tokeniser::CharClass::semialpha);

    // setCharClassFlags( charClasses, "", umba::tokeniser::CharClass::);
    // setCharClassFlags( charClasses, "", umba::tokeniser::CharClass::);

    // single chars
    setCharClassFlags( charClasses,   0x7F, umba::tokeniser::CharClass::nonprintable); // DEL
    setCharClassFlags( charClasses,   '\\', umba::tokeniser::CharClass::escape);
    setCharClassFlags( charClasses,   '_', umba::tokeniser::CharClass::identifier | umba::tokeniser::CharClass::identifier_first); // underscore


    for(std::size_t idx=0; idx!=0x80; ++idx)
    {
        cout << "  " << (unsigned)idx;
        if (idx>32)
           cout << " '" << (char)idx << "'";
        else
           cout << "    ";

        cout << "  - " << enum_serialize_flags(charClasses[idx]) << "\n";
    
    }
    //enum_serialize_flags(CharClass f)

    return 0;
}

