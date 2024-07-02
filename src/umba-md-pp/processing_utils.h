#pragma once


#include <iterator>
#include <string>
#include <iostream>

// 
#include "enums.h"

//----------------------------------------------------------------------------


namespace umba_md {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using BacktickProcessingState = ::BacktickProcessingState;



//----------------------------------------------------------------------------
template< typename OutputIterator
        , typename InputIterator
        , typename SymbolHandler
        > inline
OutputIterator transformMarkdownText(OutputIterator out, InputIterator itBegin, InputIterator itEnd, SymbolHandler handler)
{

    BacktickProcessingState st = BacktickProcessingState::normal;

    //bool atStart

    for(; itBegin!=itEnd; ++itBegin)
    {
        auto ch = *itBegin;
        switch(st)
        {
            case BacktickProcessingState::normal:
            {
                if (ch=='`')
                {
                    st = BacktickProcessingState::readBacktickEnclosedStart;
                    out = handler(out, itBegin, itEnd, st, true /*in a backticks*/); // , false /* no a line start */) // а надо вообще про начало строки? Или там само пусть разбирается, а нам тут только энклозинг бэктиками обработать надо?
                }
                else
                {
                    out = handler(out, itBegin, itEnd, st, false /*not in a backticks*/);
                }
            }
            break;

            case BacktickProcessingState::readBacktickEnclosedStart:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/);

                if (ch=='`')
                    st = BacktickProcessingState::readDblBacktickEnclosed;
                else
                    st = BacktickProcessingState::readBacktickEnclosed;
            }
            break;

            case BacktickProcessingState::readBacktickEnclosed:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
                if (ch=='`')
                    st = BacktickProcessingState::normal;
            }
            break;

            case BacktickProcessingState::readDblBacktickEnclosed:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/);
                if (ch=='`')
                    st = BacktickProcessingState::readDblBacktickEnclosedWaitEnd;
            }
            break;

            case BacktickProcessingState::readDblBacktickEnclosedWaitEnd:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
                if (ch=='`')
                    st = BacktickProcessingState::normal;
                else
                    st = BacktickProcessingState::readDblBacktickEnclosed; // Был одиночный бэктик - откатываемся в нормальное чтение двухтиковой строки
            }
            break;

        }
    }


    // Финализируем обработку - в обработчике не забываем каждый раз проверять на конец последовательности
    return handler(out, itBegin, itEnd, st, st!=BacktickProcessingState::normal);

}

//----------------------------------------------------------------------------
inline
void testTransformMarkdownText(const std::string &input)
{
    std::string res; res.reserve(input.size());

    auto handler = [](auto out, auto b, auto e, BacktickProcessingState st, bool inBackticks)
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

