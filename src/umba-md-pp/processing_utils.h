#pragma once


#include <iterator>
#include <string>
#include <iostream>

//----------------------------------------------------------------------------


namespace umba_md {

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
template< typename OutputIterator
        , typename InputIterator
        , typename SymbolHandler
        > inline
OutputIterator transformMarkdownText(OutputIterator out, InputIterator itBegin, InputIterator itEnd, SymbolHandler handler)
{
    enum State
    {
        stNormal                     = 0,
        stReadBacktickEnclosedStart     ,
        stReadBacktickEnclosed          ,
        stReadDblBacktickEnclosed       ,
        stReadDblBacktickEnclosedWaitEnd,
    };

    State st = stNormal;

    //bool atStart

    for(; itBegin!=itEnd; ++itBegin)
    {
        auto ch = *itBegin;
        switch(st)
        {
            case stNormal:
            {
                if (ch=='`')
                {
                    st = stReadBacktickEnclosedStart;
                    out = handler(out, itBegin, itEnd, true /*in a backticks*/); // , false /* no a line start */) // а надо вообще про начало строки? Или там само пусть разбирается, а нам тут только энклозинг бэктиками обработать надо?
                }
                else
                {
                    out = handler(out, itBegin, itEnd, false /*not in a backticks*/);
                }
            }
            break;

            case stReadBacktickEnclosedStart:
            {
                if (ch=='`')
                    st = stReadDblBacktickEnclosed;
                else
                    st = stReadBacktickEnclosed;

                out = handler(out, itBegin, itEnd, true /*in a backticks*/);
            }
            break;

            case stReadBacktickEnclosed:
            {
                if (ch=='`')
                    st = stNormal;
                out = handler(out, itBegin, itEnd, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
            }
            break;

            case stReadDblBacktickEnclosed:
            {
                if (ch=='`')
                    st = stReadDblBacktickEnclosedWaitEnd;
                out = handler(out, itBegin, itEnd, true /*in a backticks*/);
            }
            break;

            case stReadDblBacktickEnclosedWaitEnd:
            {
                if (ch=='`')
                    st = stNormal;
                else
                    st = stReadDblBacktickEnclosed; // Был одиночный бэктик - откатываемся в нормальное чтение двухтиковой строки
                out = handler(out, itBegin, itEnd, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
            }
            break;

        }
    }


    // Финализируем обработку - в обработчике не забываем каждый раз проверять на конец последовательности
    return handler(out, itBegin, itEnd, st!=stNormal);

}

//----------------------------------------------------------------------------
inline
void testTransformMarkdownText(const std::string &input)
{
    std::string res; res.reserve(input.size());

    auto handler = [](auto out, auto b, auto e, bool inBackticks)
    {
        if (b!=e)
        {
            if (inBackticks)
            {
                *out++ = *b;
            }
            else
            {
                *out++ = '_';
            }
        }

        return out;
    };


    transformMarkdownText(std::back_inserter(res), input.begin(), input.end(), handler);
    
    //std::cout << "testTransformMarkdownText:";
    std::cout << "In : " << input << "\n";
    std::cout << "Out: " << res << "\n";
    std::cout << "\n";

}

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace umba_md

