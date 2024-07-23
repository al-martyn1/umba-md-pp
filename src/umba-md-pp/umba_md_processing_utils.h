#pragma once


#include <iterator>
#include <string>
#include <iostream>
#include <type_traits>
#include <vector>
#include <stack>

//
// #include <boost/container/static_vector.hpp>

#include "umba/container.h"

//
#include "enums.h"

//
#include "umba/filename.h"

//----------------------------------------------------------------------------

// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename StringType>
bool isUrlAbsolute(const StringType &strUrl)
{
    using CharType = typename StringType::value_type;

    // looking for scheme: "protocol://..."
    // "://..." - is invalid, missing protocol

    auto colonPos = strUrl.find((CharType)':');

    if (colonPos==strUrl.npos)
        return false;

    if (colonPos==0)
        return false;

    if ((colonPos+1u)>=strUrl.size() || (colonPos+2u)>=strUrl.size())
        return false;

    if (strUrl[colonPos+1u]==(CharType)'/' && strUrl[colonPos+2u]==(CharType)'/')
        return true;

    return false;
}

//----------------------------------------------------------------------------
//! Абсолютный путь на текущем хосте без протокола и адреса хоста
template<typename StringType>
bool isUrlAbsoluteHostPath(StringType urlPath)
{
    using CharType = typename StringType::value_type;

    if (urlPath.size()>0 && (urlPath[0]==(CharType)'\\' || urlPath[0]==(CharType)'/'))
        return true;
    return false;
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType makeUrlPathCanonical(StringType urlPath)
{
    using CharType = typename StringType::value_type;
    typename StringType::size_type pos = 0;

    for(typename StringType::size_type colonPos = 0; colonPos!=urlPath.size(); ++colonPos)
    {
        if (urlPath[colonPos]==(CharType)':')
        {
            if ((colonPos+1u)<urlPath.size() || (colonPos+2u)<urlPath.size() && urlPath[colonPos+1u]==(CharType)'/' && urlPath[colonPos+2u]==(CharType)'/')
            {
                pos = colonPos+3; // символ, следующий за "://", пропускаем протокольную часть
                break;
            }
        }
        else if (urlPath[colonPos]==(CharType)'#' || urlPath[colonPos]==(CharType)'?') // Ищем протокольную часть, но нашли разделитель параметров/якоря, просто выскакиваем, протокольной части нет
        {
            break;
        }
    }

    for(; pos!=urlPath.size(); ++pos)
    {
        if (urlPath[pos]==(CharType)'\\')
            urlPath[pos] = (CharType)'/';
        else if (urlPath[pos]==(CharType)'#' || urlPath[pos]==(CharType)'?')
            break;
    }

    return urlPath;
}

//----------------------------------------------------------------------------
template<typename StringType>
bool splitUrlToPathAndTag(const StringType &url, StringType &urlPath, StringType &urlTag)
{
    using CharType = typename StringType::value_type;
    typename StringType::size_type pos = url.find((CharType)'#', 0u);

    if (pos==url.npos)
    {
        urlPath = url;
        urlTag.clear();
        return false;
    }

    urlPath = StringType(url, 0, pos);
    urlTag  = StringType(url, pos+1, url.npos);

    return true;
}

//----------------------------------------------------------------------------
template<typename StringType>
StringType mergeUrlFromPathAndTag(StringType urlPath, const StringType &urlTag)
{
    using CharType = typename StringType::value_type;

    if (!urlTag.empty())
    {
        urlPath.append(1, (CharType)'#');
        urlPath.append(urlTag);
    }

    return urlPath;
}

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
                    //out =
                    handler(out, itBegin, itEnd, st, true /*in a backticks*/); // , false /* no a line start */) // а надо вообще про начало строки? Или там само пусть разбирается, а нам тут только энклозинг бэктиками обработать надо?
                }
                else
                {
                    //out =
                    handler(out, itBegin, itEnd, st, false /*not in a backticks*/);
                }
            }
            break;

            case BacktickProcessingState::readBacktickEnclosedStart:
            {
                //out =
                handler(out, itBegin, itEnd, st, true /*in a backticks*/);

                if (ch==(value_type)'`')
                    st = BacktickProcessingState::readDblBacktickEnclosed;
                else
                    st = BacktickProcessingState::readBacktickEnclosed;
            }
            break;

            case BacktickProcessingState::readBacktickEnclosed:
            {
                //out =
                handler(out, itBegin, itEnd, st, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
                if (ch==(value_type)'`')
                    st = BacktickProcessingState::normal;
            }
            break;

            case BacktickProcessingState::readDblBacktickEnclosed:
            {
                //out =
                handler(out, itBegin, itEnd, st, true /*in a backticks*/);
                if (ch==(value_type)'`')
                    st = BacktickProcessingState::readDblBacktickEnclosedWaitEnd;
            }
            break;

            case BacktickProcessingState::readDblBacktickEnclosedWaitEnd:
            {
                //out =
                handler(out, itBegin, itEnd, st, true /*in a backticks*/); // Завершающий бэктик всё равно входит в энклозинг
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
// https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#links
// https://docs.github.com/ru/get-started/writing-on-github/getting-started-with-writing-and-formatting-on-github/basic-writing-and-formatting-syntax#images


// LinkHandler handler(bool bImageLink, OutputIterator out, InputIterator itLinkBegin, InputIterator itLinkEnd)

template< typename OutputIterator
        , typename InputIterator
        , typename LinkHandler
        > inline
OutputIterator transformMarkdownLinksUrlIterator(OutputIterator out, InputIterator itBegin, InputIterator itEnd, LinkHandler handler)
{
    using value_type = typename std::decay<decltype(*itBegin)>::type;

    //#define UMBA_MD_PROCESSING_UTILS_TRANSFORM_MARKDOWN_LINKS_URL_ITERATOR_USE_STACK

    auto callHandler = [&handler](OutputIterator out, InputIterator b, InputIterator e, bool bImageLink)
    {
        // в ссылке вида [Some link text]( link_url bla-bla)
        // могут быть пробелы до линка (хз, насколько это допустимо)
        // и после линка - где-то видел, как это используется, но не помню.
        for(; b!=e; ++b)
        {
            if (*b==(value_type)' ' || *b==(value_type)'\n' || *b==(value_type)'\r' || *b==(value_type)'\t')
            {
                *out++ = *b;
                continue;
            }
            break;
        }

        InputIterator it = b;
        for(; it!=e; ++it)
        {
            if (*it==(value_type)' ' || *it==(value_type)'\n' || *it==(value_type)'\r' || *it==(value_type)'\t')
                break;
        }

        handler(out, b, it, bImageLink);

        for(; it!=e;  /* ++it */ )
        {
            *out++ = *it++;
        }

        return out;
    };


    enum State
    {
        stNormal        = 0,
        stImageWaitText    ,
        stReadText         , // href text
        stWaitLink         ,
        stWaitLinkLink     ,
#if !defined(UMBA_MD_PROCESSING_UTILS_TRANSFORM_MARKDOWN_LINKS_URL_ITERATOR_USE_STACK)
        stReadLink
#else
        stReadImageText         ,
        stWaitImageLink         ,
        stWaitImageLinkLink     ,
        stReadImageLink
#endif
    };


    State st = stNormal; // Он всегда нужен, если стек - там предыдущее значение состояния
    //InputIterator itUrlEnd   = itEnd;

    typedef boost::container::small_vector_options< boost::container::growth_factor<boost::container::growth_factor_50> >::type growth_50_option_t;
    umba::container::small_vector<int, 5, void, void> testSmallVector;

#if !defined(UMBA_MD_PROCESSING_UTILS_TRANSFORM_MARKDOWN_LINKS_URL_ITERATOR_USE_STACK)

    unsigned bracketCount = 0;
    bool bImageLink = false;
    InputIterator itUrlBegin = itEnd;

#else

    struct ParsingState
    {
        State         st         = stNormal;
        bool          bImageLink = false;
        InputIterator itUrlBegin;
    };

    std::stack< ParsingState, std::vector<ParsingState> >  stateStack;
    // stateStack.

/*
    Было бы удобно сразу хранить на стеке состояние.
    Но тут проблема - даже если рекурсия не нужна, а это 99.99% случаев, мы будем лазать в кучу.

    std::inplace_vector - это C++26 и то не факт - дата пропозала - 2023-06-16 -  https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2023/p0843r8.html
    fixed_capacity_vector - дата пропозала - 2017-10-15 - https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0843r0.html
    boost::container::static_vector - упомянули тут - https://stackoverflow.com/questions/53932651/c-vector-with-fixed-capacity-after-initialization

    Если состояние не хранить на стеке, его надо хранить в переменной. И вот тут можно легко запутаться,
    когда использоывать состояние на стеке, когда - переменную, когда переменную переустанавливать
    значением со стека и тп.






*/


#endif


#if !defined(UMBA_MD_PROCESSING_UTILS_TRANSFORM_MARKDOWN_LINKS_URL_ITERATOR_USE_STACK)
    auto linkDetector = [&st, &bImageLink, &bracketCount, &itUrlBegin, &callHandler](OutputIterator out, InputIterator b, InputIterator e, BacktickProcessingState bktpsSt, bool inBackticks)
#else
    auto linkDetector = [&callHandler](OutputIterator out, InputIterator b, InputIterator e, BacktickProcessingState bktpsSt, bool inBackticks)
#endif
    {
        auto doNormal = [&]()
        {
            auto ch = *b;

            if (inBackticks)
            {
                *out++ = *b;
            }
            else
            {
#if !defined(UMBA_MD_PROCESSING_UTILS_TRANSFORM_MARKDOWN_LINKS_URL_ITERATOR_USE_STACK)
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
#else
                // if (ch==(value_type)'[')
                // {
                //     stateStack
                //     bracketCount = 1;
                //     st = stReadText;
                // }
                // else if (ch==(value_type)'!')
                // {
                //     st = stImageWaitText;
                // }

#endif
            }
        };

        auto doReadText = [&](bool readImageLink)
        {
            auto ch = *b;

            if (!inBackticks)
            {
#if !defined(UMBA_MD_PROCESSING_UTILS_TRANSFORM_MARKDOWN_LINKS_URL_ITERATOR_USE_STACK)
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
#else
                if (ch==(value_type)'[')
                {

                    stateStack.push({})
                }
#endif

            }

            *out++ = *b;
        };

        auto doWaitLink = [&](bool readImageLink)
        {
            auto ch = *b;

            if (ch==(value_type)'(')
            {
                bracketCount = 1;
                st = stReadLink;
            }

            *out++ = *b;
        };

        auto doReadLink = [&](bool readImageLink)
        {
            auto ch = *b;
            if (itUrlBegin==e) // itEnd
            {
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
                    //out =
                    callHandler(out, itUrlBegin, b, readImageLink /* bImageLink */ );
                    itUrlBegin = e; // itEnd
                    st = stNormal;
                    *out++ = *b;
                }
            }
            // Иначе - ничего не делаем
        };




        if (b==e)
        {
            // Flush buffered
            for(; itUrlBegin!=e /* itEnd */ ; ++itUrlBegin)
            {
                *out++ = *itUrlBegin;
            }
        }
        else // main job
        {
            auto ch = *b;

            switch(st)
            {
                case stNormal:
                {
                    doNormal();
                    // if (inBackticks)
                    // {
                    //     *out++ = *b;
                    // }
                    // else
                    // {
                    //     if (ch==(value_type)'[')
                    //     {
                    //         bracketCount = 1;
                    //         st = stReadText;
                    //     }
                    //     else if (ch==(value_type)'!')
                    //     {
                    //         st = stImageWaitText;
                    //     }
                    //
                    //     *out++ = *b;
                    // }
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
                    doReadText(bImageLink);
                }
                break;

                case stWaitLink:
                {
                    doWaitLink(bImageLink);
                }
                break;

                case stReadLink:
                {
                    doReadLink(bImageLink);
                }
                break;

            }
        }

        return out;
    };

    // InputIterator itUrlBegin = itEnd;
    // InputIterator itUrlEnd   = itEnd;

    return transformMarkdownText(out, itBegin, itEnd, linkDetector);

}

//----------------------------------------------------------------------------
template<typename LinkStringHandler>
class LinksUrlIteratorHandler
{
    LinkStringHandler  m_handler;

public:

    LinksUrlIteratorHandler() = delete;
    LinksUrlIteratorHandler(LinkStringHandler handler) : m_handler(handler) {}
    LinksUrlIteratorHandler(const LinksUrlIteratorHandler &) = delete;
    LinksUrlIteratorHandler& operator=(const LinksUrlIteratorHandler &) = delete;
    LinksUrlIteratorHandler(LinksUrlIteratorHandler &&) = default;
    LinksUrlIteratorHandler& operator=(LinksUrlIteratorHandler &&) = default;

    template< typename OutputIterator
            , typename InputIterator
            >
    OutputIterator operator()( OutputIterator out, InputIterator itUrlBegin, InputIterator itUrlEnd, bool bImageLink)
    {
        using value_type = typename std::decay<decltype(*itUrlBegin)>::type;

        auto handlerRes = m_handler(std::basic_string<value_type>(itUrlBegin, itUrlEnd), bImageLink); // Получаем обработанный URL, также строка
        for(auto resIt=handlerRes.begin(); resIt!=handlerRes.end(); ++resIt)
        {
            *out++ = *resIt;
        }

        return out;
    }
};

//----------------------------------------------------------------------------
template <typename LinkStringHandler> inline
LinksUrlIteratorHandler<LinkStringHandler> makeLinksUrlIteratorHandler(LinkStringHandler linkStringHandler)
{
    return LinksUrlIteratorHandler<LinkStringHandler>(linkStringHandler);
}

//----------------------------------------------------------------------------
template< typename OutputIterator
        , typename InputIterator
        , typename LinkStrHandler
        > inline
OutputIterator transformMarkdownLinksUrlString(OutputIterator out, InputIterator itBegin, InputIterator itEnd, LinkStrHandler strHandler)
{
    return transformMarkdownLinksUrlIterator(out, itBegin, itEnd, makeLinksUrlIteratorHandler(strHandler));
}

//----------------------------------------------------------------------------
inline
void testTransformMarkdownLinksUrlString(const std::string &input)
{
    std::string res; res.reserve(input.size());

    auto handler = [](std::string url, bool bImage)
    {
        std::cout << "URL: " << url << "\n";
        if (isUrlAbsolute(url))
        {
        }
        else if (isUrlAbsoluteHostPath(url))
        {
        }
        else // path reletive to current document
        {
            url = "../../" + url; // Для теста добавляем два уровня вверх
        }

        return makeUrlPathCanonical(url);
    };


    transformMarkdownLinksUrlString(std::back_inserter(res), input.begin(), input.end(), handler);

    //std::cout << "testTransformMarkdownText:";
    std::cout << "In : " << input << "\n";
    std::cout << "Out: " << res << "\n";
    std::cout << "\n";
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



