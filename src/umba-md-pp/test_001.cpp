/*! \file
    \brief Тестируем корректность генерации индексов для таблицы CharClass по символу
 */

#define UMBA_TOKENISER_DISABLE_UMBA_TOKENISER_GET_CHAR_CLASS_FUNCTION

#include "umba/umba.h"
#include "umba/tokeniser.h"


#include <iostream>

using std::cout;
using std::cerr;

int main(int argc, char* argv[])
{
    cout << "Character indices in CharClass table:\n";

    for(std::uint8_t i=0; i!=(std::uint8_t)-1; ++i)
    {
        cout << "  " << (unsigned)i;
        if (i>32)
           cout << " '" << (char)i << "'";
        else
           cout << "    ";

        cout << "  - " << umba::tokeniser::charToCharClassTableIndex((char)i) << "\n";
    }

    return 0;
}

