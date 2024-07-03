#pragma once


#include <iterator>
#include <string>
#include <iostream>
#include <type_traits>

// 
#include "enums.h"

// 
#include "umba/filename.h"

//----------------------------------------------------------------------------


namespace umba {
namespace md {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
using BacktickProcessingState = ::BacktickProcessingState;



//----------------------------------------------------------------------------
// https://www.markdownguide.org/basic-syntax/#escaping-backticks
template< typename OutputIterator
        , typename InputIterator
        , typename SymbolHandler
        > inline
OutputIterator transformMarkdownText(OutputIterator out, InputIterator itBegin, InputIterator itEnd, SymbolHandler handler)
{
    using value_type = typename std::decay<decltype(*itBegin)>::type;

    BacktickProcessingState st = BacktickProcessingState::normal;

    //bool atStart

    for(; itBegin!=itEnd; ++itBegin)
    {
        auto ch = *itBegin;
        switch(st)
        {
            case BacktickProcessingState::normal:
            {
                if (ch==(value_type)'`')
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

                if (ch==(value_type)'`')
                    st = BacktickProcessingState::readDblBacktickEnclosed;
                else
                    st = BacktickProcessingState::readBacktickEnclosed;
            }
            break;

            case BacktickProcessingState::readBacktickEnclosed:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
                if (ch==(value_type)'`')
                    st = BacktickProcessingState::normal;
            }
            break;

            case BacktickProcessingState::readDblBacktickEnclosed:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/);
                if (ch==(value_type)'`')
                    st = BacktickProcessingState::readDblBacktickEnclosedWaitEnd;
            }
            break;

            case BacktickProcessingState::readDblBacktickEnclosedWaitEnd:
            {
                out = handler(out, itBegin, itEnd, st, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
                if (ch==(value_type)'`')
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
// https://www.markdownguide.org/basic-syntax/#images
// ![Tux, the Linux mascot](/assets/images/tux.png)
// https://www.markdownguide.org/basic-syntax/#links
// https://www.markdownguide.org/basic-syntax/#images-1
// https://www.markdownguide.org/hacks/#image-captions

// LinkHandler handler(OutputIterator out, InputIterator itLinkBegin, InputIterator itLinkEnd)

template< typename OutputIterator
        , typename InputIterator
        , typename LinkHandler
        > inline
OutputIterator transformMarkdownLinksUrlIter(OutputIterator out, InputIterator itBegin, InputIterator itEnd, LinkHandler handler)
{
    using value_type = typename std::decay<decltype(*itBegin)>::type;

    enum State
    {
        stNormal        = 0,
        stImageWaitText    ,
        stReadText         , // href text
        stWaitLink         ,
        stWaitLinkLink     ,
        stReadLink
    };

    State st = stNormal;
    bool bImageLink = false;
    unsigned bracketCount = 0;

    InputIterator itUrlBegin = itEnd;
    //InputIterator itUrlEnd   = itEnd;

    auto linkDetector = [&](OutputIterator out, InputIterator b, InputIterator e, BacktickProcessingState st, bool inBackticks)
    {
        if (b==e)
        {
            // Flush buffered
            for(; itUrlBegin!=itEnd; ++itUrlBegin)
            {
                *out++ = *itUrlBegin;
            }
        }
        else // main job
        {
            auto ch = *itBegin;

            switch(st)
            {
                case stNormal:
                {
                    if (inBackticks)
                    {
                        *out++ = *b;
                    }
                    else
                    {
                        if (ch==(value_type)'[')
                        {
                            bracketCount = 1;
                            st = stReadText;
                        }
                        else if (ch==(value_type)'!')
                        {
                            st = stImageWaitText;
                        }

                        *out++ = *b;
                    }
                }
                break;
            
                case stImageWaitText:
                {
                    if (ch==(value_type)'[')
                    {
                        bImageLink = true;
                        bracketCount = 1;
                        st = stReadText;
                    }
                    else
                    {
                        st = stNormal;
                    }

                    *out++ = *b;
                }
                break;
            
                case stReadText:
                {
                    if (!inBackticks)
                    {
                        if (ch==(value_type)'[')
                        {
                            ++bracketCount;
                        }
                        else if (ch==(value_type)']')
                        {
                            --bracketCount;
                            if (bracketCount==0)
                            {
                                st = stWaitLink;
                            }
                        }

                    }

                    *out++ = *b;
                }
                break;
            
                case stWaitLink:
                {
                    if (ch==(value_type)'(')
                    {
                        bracketCount = 1;
                        st = stReadLink;
                    }

                    *out++ = *b;
                }
                break;
            
                case stReadLink:
                {
                    if (itUrlBegin==itEnd)
                    {
                        //itUrlEnd = itEnd;
                        //st = stReadLinkTail;

                        // сохраняем итератор начала 
                        itUrlBegin = b;
                    }

                    if (ch==(value_type)'(')
                    {
                        ++bracketCount;
                    }
                    else if (ch==(value_type)')')
                    {
                        --bracketCount;
                        if (bracketCount==0)
                        {
                            // call handler here
                            handler(out, itUrlBegin, b);
                            *out++ = *b;
                            itUrlBegin = itEnd;
                        }
                    }

                    // Иначе - ничего не делаем

                }
                break;
            
            }
        }

    };

    // InputIterator itUrlBegin = itEnd;
    // InputIterator itUrlEnd   = itEnd;

    return transformMarkdownText(out, itBegin, itEnd, linkDetector)

}


#if 0
template<typename FilenameStringType> inline
std::string inDocRefsRenameMdppExtentionsAndStoreImagePaths(const AppConfig<FilenameStringType> &appCfg, Document &doc, const std::string &line, std::vector<std::string> &imagePaths)
{
    std::string resLine; resLine.reserve(line.size());

    std::string::size_type linkStartPos = std::string::npos;
    std::string::size_type linkEndPos   = std::string::npos;

    State st = stNormal;
    unsigned bracketCount = 0; // also used as readingLink flag 
    bool readingLinkUrl = false;
    bool imageLink      = false;


    std::string::size_type pos = 0;
    for(; pos!=line.size(); ++pos)
    {
        char ch = line[pos];

        switch(st)
        {
            case stNormal:
            {
                if (ch=='`')
                {
                    st = stReadBacktickEnclosedStart;
                    resLine.append(1, ch);
                }
                else if (ch=='!')
                {
                    st = stImageMarkFound;
                    resLine.append(1, ch);
                }
                else if (ch=='[' || ch==']' || ch=='(' || ch==')')
                {
                    if (bracketCount==0) // Не производится чтение какой-либо части ссылки
                    {
                        if (ch=='[')
                        {
                            bracketCount = 1;
                            readingLinkUrl = false;
                        }
                        else if (ch==']')
                        {
                            --bracketCount;
                            if (bracketCount==0) // Закрывающая текст ссылки скобка 
                            {
                                st = stWaitLinkUrl;
                            }
                        }
                    }
                    else // Читаем либо текст ссылки, либо URL-часть
                    {
                        if (!readingLinkUrl) // Читаем текст ссылки
                        {
                            if (ch=='[')
                            {
                                bracketCount = 1;
                                readingLinkUrl = false;
                            }
                            else if (ch==']')
                            {
                                --bracketCount;
                                if (bracketCount==0) // Закрывающая текст ссылки скобка 
                                {
                                    st = stWaitLinkUrl;
                                }
                            }
                        }
                        else // Читаем URL ссылки
                        {
                        
                        }
                    }
                    //st = stReadingLinkText;
                    resLine.append(1, ch);
                    //bracketCount = 1;
                }
                else
                {
                    resLine.append(1, ch);
                }
            }
            break;

            case stReadBacktickEnclosedStart:
            {
                if (ch=='`')
                    st = stReadDblBacktickEnclosed;
                else
                    st = stReadBacktickEnclosed;

                resLine.append(1, ch);
            }
            break;

            case stReadBacktickEnclosed:
            {
                if (ch=='`')
                    st = stNormal;
                resLine.append(1, ch);
            }
            break;

            case stReadDblBacktickEnclosed:
            {
                if (ch=='`')
                    st = stReadDblBacktickEnclosedWaitEnd;
                resLine.append(1, ch);
            }
            break;

            case stReadDblBacktickEnclosedWaitEnd:
            {
                if (ch=='`')
                    st = stNormal;
                else
                    st = stReadDblBacktickEnclosed; // Был одиночный бэктик - откатываемся в нормальное чтение двухтиковой строки

                resLine.append(1, ch);
            
            }
            break;

            case stImageMarkFound :
            {
            
            }
            break;

            case stReadingLinkText:
            {
            
            }
            break;

            case stWaitLinkText   :
            {
            
            }
            break;

            case stReadingLinkUrl :
            {
            
            }
            break;

        }


    }

}
#endif


//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

} // namespace md
} // namespace umba



