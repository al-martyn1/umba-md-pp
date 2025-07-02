#pragma once

#include "enums.h"
#include "umba/string_plus.h"
//
#include "umba/macros.h"
#include "umba/macro_helpers.h"
//
#include "utils.h"
#include "processing_utils.h"
//
#include <exception>
#include <stdexcept>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <vector>
#include <utility>
#include <iterator>
#include <deque>
#include <queue>

//
#include "signature.h"
//
#include "umba/container.h"
//
#include "marty_cpp/marty_cpp.h"
//
#include "code-options-database.h"
//

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------

// umba::md::
namespace umba {
namespace md {



//----------------------------------------------------------------------------

/*

 1) Запретить в метках/тэгах выкусывания фрагментов кода символ '-' - он только для разделения частей тэга
 2) Конечного тэга нет - ориентируемся по скобкам: "#start_tag-{}" - при этом скобки реально используются те, которые в конфиге для языка заданы, а не именно что фигурные
 3) Конечного номера строки нет - ориентируемся по скобкам: "#NNN-{}" - при этом скобки реально используются те, которые в конфиге для языка заданы, а не именно что фигурные
 4) У нас нет тэга, мы хотим выцепить по сигнатуре текста, возможно, многострочного: "#`inline\nvoid\ndoSomething()`-{}".
    Сигнатуру можно искать только после определённого номера строки: "#NNN`inline\nvoid\ndoSomething()`-{}".
 5) Когда ищем скобки - строковые литералы не сканируем - слишком геморно пока, да и для разных языков оно разное, таким образом, может получится косяк распознования блока.
 6) Хотим закончить по generic stop маркеру: "#start_tag-(-)"/"#NNN-(-)"
 7) Хотим закончить по N пустых строк: "#start_tag-(N)"/"#NNN-(N)"

 Алгоритм работы текстовой сигнатуры.
 1) Парсим с учётом escape-последовательности
 2) Разбиваем на строки по переводу строки
 3) Каждую строку "нормализуем" - удаляем все пробелы
 4) Строки текстовой сигнатуры не обязательно будут встречаться на разных строках - их могут переформатировать туда-обратно. Надо просекать такой вариант, и на каждом шаге склеивать нужное количество строк перед проверкой

*/

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool testFlagSnippetOption(const std::unordered_set<SnippetOptions> &flagOptions, SnippetOptions opt)
{
    auto baseOpt = (SnippetOptions)(((std::uint32_t)opt)|0x0001u);

    if (flagOptions.find(baseOpt)!=flagOptions.end())
        return true;

    return false;
}

//----------------------------------------------------------------------------
template<typename SomeIntegralOrEnumType>
bool testIntSnippetOption(const std::unordered_map<SnippetOptions, int> &intOptions, SnippetOptions opt, SomeIntegralOrEnumType testVal)
{
    auto baseOpt = (SnippetOptions)(((std::uint32_t)opt)|0x0001u);

    auto it = intOptions.find(baseOpt);
    if (it==intOptions.end())
        return false;

    return it->second==(int)testVal;
}

//----------------------------------------------------------------------------
template<typename SomeIntegralOrEnumType>
SomeIntegralOrEnumType getIntSnippetOption(const std::unordered_map<SnippetOptions, int> &intOptions, SnippetOptions opt, SomeIntegralOrEnumType defVal)
{
    auto baseOpt = (SnippetOptions)(((std::uint32_t)opt)|0x0001u);

    auto it = intOptions.find(baseOpt);
    if (it==intOptions.end())
        return defVal;

    return SomeIntegralOrEnumType(it->second);
}

// //----------------------------------------------------------------------------
// template<typename SomeIntegralOrEnumType>
// bool testIntSnippetOptionNotEqual(const std::unordered_map<SnippetOptions, int> &intOptions, SnippetOptions opt, SomeIntegralOrEnumType testVal)
// {
//     auto baseOpt = (SnippetOptions)(((std::uint32_t)opt)|0x0001u);
//  
//     auto it = intOptions.find(baseOpt);
//     if (it==flagOptions.end())
//         return false;
//  
//     return it->second!=(int)testVal;
// }
//  
//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
//! Возвращает номер строки, в которой найден закрывающий блок символ, или (std::size_t)-1
inline
std::size_t findBlockInLines(const std::vector<std::string> &lines, char blockCharOpen, char blockCharClose, const std::string &stSep, std::size_t startLine=(std::size_t)-1)
{
    if (startLine==(std::size_t)-1)
        startLine = 0;

    if (startLine>=lines.size())
        return (std::size_t)-1;

    std::size_t openCount = 0;
    std::size_t curLineIdx = startLine;

    // Если блочные символы разные - второй или нулевой, или закрывающий, то второй - нулевой - маркерит остановится на первом
    // Если оба одинаковые - они скорее всего нулевые, и это не то
    bool stopOnOpenBlock = blockCharOpen!=blockCharClose && blockCharClose==0;

    if (stSep=="\n") // Если у нас символ разделитель - перевод строки, то у нас только одна строка влезает всегда, и блок символы не используются
        return curLineIdx;

    for(; curLineIdx!=lines.size(); ++curLineIdx)
    {
        if (!stSep.empty() && openCount==0)
        {
            auto sepPos = lines[curLineIdx].find(stSep);
            if (sepPos!=lines[curLineIdx].npos)
               return curLineIdx;
        }

        for(auto ch : lines[curLineIdx])
        {
            if (ch==blockCharOpen)
            {
                if (stopOnOpenBlock)
                    return curLineIdx;
                ++openCount;
            }
            else if (ch==blockCharClose)
            {
                --openCount;
                if (openCount==0)
                {
                    return curLineIdx;
                }
            }
        }
    }

    // Закрытия блока не нашли
    // Тогда считаем, что закрытие блока произошло по окончании данных, и возвращаем номер последней строки
    return lines.size()-1; // Строк у нас ненулевое количество
}

//----------------------------------------------------------------------------
//! Возвращает номер строки, после которой найден разделитель/стоп строка
inline
std::size_t findStopPrefixInLines(const std::vector<std::string> &lines, const std::unordered_set<std::string> &stopPrefixes, std::size_t startLine=(std::size_t)-1)
{
    if (startLine==(std::size_t)-1)
        startLine = 0;

    if (startLine>=lines.size())
        return (std::size_t)-1;

    std::size_t curLineIdx = startLine;
    for(; curLineIdx!=lines.size(); ++curLineIdx)
    {
        std::string trimmedLine = umba::string_plus::trim_copy(lines[curLineIdx]);
        for(const auto &p : stopPrefixes)
        {
            if (umba::string_plus::starts_with(trimmedLine, p))
            {
                if (curLineIdx==0)
                    return 0;
                return curLineIdx-1;
            }
        }
    }

    // Стоп-строку не нашли
    // Тогда считаем, что искомый блок заканчивается последней строкой файла
    return lines.size()-1; // Строк у нас ненулевое количество
}

//----------------------------------------------------------------------------
//! Возвращает номер строки, после которой найден разделитель из пустых строк
inline
std::size_t findEmptyLinesStopInLines(const std::vector<std::string> &lines, std::size_t numEmptyLines, std::size_t startLine=(std::size_t)-1)
{
    if (startLine==(std::size_t)-1)
        startLine = 0;

    if (startLine>=lines.size())
        return (std::size_t)-1;

    if (numEmptyLines==(std::size_t)-1)
        numEmptyLines = 1;

    if (numEmptyLines<1u)
        numEmptyLines = 1;


    std::size_t emptyLineCount = 0;
    std::size_t curLineIdx = startLine;
    for(; curLineIdx!=lines.size(); ++curLineIdx)
    {
        if (umba::string_plus::trim_copy(lines[curLineIdx]).empty())
        {
            // Нашли пустую строку
            ++emptyLineCount;
            if (emptyLineCount>=numEmptyLines)
            {
                if (curLineIdx<numEmptyLines)
                    return 0;
                return curLineIdx - numEmptyLines;
            }
        }
        else
        {
            emptyLineCount = 0; // при непустой строке счётчик пустых сбрасываем
        }
    }

    // Стоп-строки не нашли
    // Тогда считаем, что искомый блок заканчивается последней строкой файла
    return lines.size()-1; // Строк у нас ненулевое количество
}

//----------------------------------------------------------------------------
inline
bool hasIdentifierChars(const std::string str)
{
    for(char ch: str)
    {
        if (ch>='a' && ch<='z')
            return true;
        if (ch>='A' && ch<='Z')
            return true;
        if (ch>='0' && ch<='9')
            return true;
        if (ch=='_')
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
//! Тэг остаётся в line
inline
bool extractCodeTagFromLine(std::string &line, const std::string &tagPrefix)
{
    auto lineCopy = line;
    umba::string_plus::trim(lineCopy);

    if (!umba::string_plus::starts_with_and_strip(lineCopy, tagPrefix))
        return false;

    // Префикс нашли и отрезали

    // Тэг не обязан быть вплотную к префиксу, а в некоторых языках вообще может быть невозможным лепить вплотную
    umba::string_plus::trim(lineCopy);

    // Тэг не должен содержать пробелов
    // Но у нас может быть ситуация, когда в языке не поддерживаются однострочные коментарии,
    // и тогда мы в строке после тэга дописываем после пробела завершающую последовательность
    // По пробелу мы её находим и удаляем
    auto spacePos = lineCopy.find(' ');
    if (spacePos!=lineCopy.npos)
    {
        lineCopy.erase(spacePos, lineCopy.npos);
    }

    line = lineCopy;

    if (!hasIdentifierChars(line)) // Мы проверили, что оставшееся соджержит хоть какие-то символы идентификатора, а не полный мусор. Если мусор - удаляем его
        line.clear();

    return true;
}

//----------------------------------------------------------------------------
//! Тэг помещается в extractedTag
inline
bool extractCodeTagFromLine(std::string line, const std::string &tagPrefix, std::string &extractedTag)
{
    if (!extractCodeTagFromLine(line, tagPrefix))
        return false;

    extractedTag = line;

    return true;
}

//----------------------------------------------------------------------------
inline
std::vector<std::string> extractCodeFragmentBySnippetTag( const umba::md::CodeOptions             &langOpts
                                                        , const std::string                       &lang
                                                        , std::vector<std::string>                lines
                                                        , std::size_t                             &firstFoundLineIdx
                                                        , const std::string                       &targetFragmentTag
                                                        , ListingNestedTagsMode                   listingNestedTagsMode
                                                        , std::size_t                             startLine // = (std::size_t)-1
                                                        , std::size_t                             tabSize // =4u
                                                        //, bool                        trimLeadingSpaces_a = true
                                                        )
{
    UMBA_USED(lang);

    marty_cpp::expandTabsToSpaces(lines, tabSize);

    if (targetFragmentTag.empty() || lines.empty())
    {
        firstFoundLineIdx = 0;
        return std::vector<std::string>();
    }

    if (startLine==(std::size_t)-1)
        startLine = 0;

    if (startLine>=lines.size())
        return std::vector<std::string>();

    //const auto &langOpts = codeOptionsDatabase.getCodeOptions(lang);
    //return langOpts.isCodeTagLine(line, pTagPrefix);


    std::vector<std::string> fragmentLines; fragmentLines.reserve(lines.size());


    auto addTagLine = [&](const std::string &line)
    {
        switch(listingNestedTagsMode)
        {
            case ListingNestedTagsMode::keep  :
                 fragmentLines.emplace_back(line);
                 break;

            case ListingNestedTagsMode::remove:
                 break;

            case ListingNestedTagsMode::empty :
                 fragmentLines.emplace_back(std::string());
                 break;

            case ListingNestedTagsMode::invalid :
                 break;
        }
    };

    //std::stack<std::string>  openedTags;
    std::vector<std::string> openedTags;

    auto isClosingTag = [&](std::string tagName)
    {
        if (openedTags.empty())
            return false;

        if (tagName.empty())
            return true; // разрешаем закрывающим тэгам не иметь имени

        if (openedTags.back()==tagName)
            return true; // текущий тэг закрывает того, что на стеке - имя одно

        return false;
    };

    auto openCodeFragment = [&](std::string tagName)
    {
        openedTags.emplace_back(tagName);
    };

    auto closeCurTag = [&]()
    {
        if (openedTags.empty())
            return;
        openedTags.pop_back();
    };

    auto isTargetFragmentTagOpened = [&]()
    {
        for(const auto &openedTag: openedTags)
        {
            if (openedTag==targetFragmentTag)
                return true;
        }

        return false;
    };

    auto isTargetFragmentTagOnTop = [&]()
    {
        if (openedTags.empty())
            return false;

        if (openedTags.back()==targetFragmentTag)
            return true; // искомый тэг на вершине

        return false;
    };

    std::size_t lineIdx = startLine;
    std::string foundTagPrefix;

    for(; lineIdx!=lines.size(); ++lineIdx)
    {
        auto l = lines[lineIdx];

        //bool isCodeTagLine(std::string line, std::string *pFoundTagPrefix=0) const
        if (!langOpts.isCodeTagLine(l, &foundTagPrefix))
        {
            // Строка обычная, не тэговая
            if (isTargetFragmentTagOpened())
            {
                fragmentLines.emplace_back(l);
            }

            continue;
        }

        // Строка - тэговая
        std::string curTag;
        extractCodeTagFromLine(l, foundTagPrefix, curTag);

        if (isClosingTag(curTag))
        {
            if (isTargetFragmentTagOnTop())
            {
                // Закрываем целевой тэг
                closeCurTag(); // на самом деле - насрать, всё равно из цикла выходим
                break;
            }
            else
            {
                // закрываем какой-то левый тэг
                closeCurTag();
                addTagLine(l);
                continue;
            }
        }

        // Если тэг не закрывающий, то он - открывающий
        // Или он может быть пустым при пустом стеке
        if (curTag.empty())
        {
            continue; // Игнорим закрывающие тэги, когда не было открывающих
        }

        if (curTag==targetFragmentTag)
        {
            firstFoundLineIdx = lineIdx+1; // начали со строки, которая следует за открывающим тэгом
        }

        // Если наш целевой тэг открыт, то надо добавить строчку, а тут идёт обработка вложенных тэгов
        if (isTargetFragmentTagOpened())
        {
            //fragmentLines.emplace_back(l);
            addTagLine(l);
        }

        openCodeFragment(curTag);
    }

    return fragmentLines;
}

//----------------------------------------------------------------------------





//----------------------------------------------------------------------------
struct SnippetTagInfo
{
    // using options_type  = umba::container::small_vector_options< umba::container::growth_factor<umba::container::growth_factor_50>, umba::container::inplace_alignment<16> >::type;
    // using text_signature_vector = umba::container::small_vector<TextSignature, 4, void, options_type >;
    using text_signature_vector = std::vector<TextSignature>;

    SnippetTagType             startType               = SnippetTagType::invalid;
    std::size_t                startNumber             = 0; // line number
    text_signature_vector      startTagOrSignaturePath ; // start tag or text signatures path. For start tag only one element of the  vector used
    std::size_t                extractCount            = 0;

    SnippetTagType             endType                 = SnippetTagType::invalid;
    std::size_t                endNumber               = 0; // end line number or number of empty lines to stop
    TextSignature              endSignature            ;    // paths not supported here

    bool isTagValid() const
    {
        if (startTagOrSignaturePath.empty())
            return false;
        if (startTagOrSignaturePath[0].normalizedSignature.empty())
            return false;

        return true;
    }

    std::string getTag() const
    {
        return isTagValid() ? startTagOrSignaturePath[0].normalizedSignature : std::string();
    }

    // std::size_t getLastStartSignatureLinesNumber() const
    // {
    //     if (startTagOrSignaturePath.empty())
    //         return 0;
    //
    //     return startTagOrSignaturePath[startTagOrSignaturePath.size()-1].signatureLinesVector.size();
    // }

    bool isStartSignatureValid() const
    {
        if (startTagOrSignaturePath.empty())
            return false;

        for(const auto &s : startTagOrSignaturePath)
        {
            if (s.normalizedSignature.empty())
                return false;
        }

        return true;
    }

    bool isEndSignatureValid() const
    {
        return !endSignature.normalizedSignature.empty();
    }

    bool isValid() const
    {
        if (startType==SnippetTagType::invalid)
            return false;

        else if (startType==SnippetTagType::lineNumber)
        {
            if (startNumber==(std::size_t)-1)
                return false;
        }
        else if (startType==SnippetTagType::normalTag)
        {
            if (!isTagValid())
                return false;
        }
        else if (startType==SnippetTagType::textSignature)
        {
            if (!isStartSignatureValid())
                return false;
        }
        else
        {
            return false; // Стартовый тэг не может быть других типов
        }

        // Теперь проверим конечный тэг - к нему требования попроще

        if (endType==SnippetTagType::lineNumber)
        {
            if (endNumber==(std::size_t)-1)
                return false;
        }
        else if (endType==SnippetTagType::normalTag)
        {
            return false; // Конечный тэг не задаётся
        }
        else if (endType==SnippetTagType::textSignature)
        {
            if (!isEndSignatureValid())
                return false;
        }

        return true;
    }


}; // struct SnippetTagInfo

//----------------------------------------------------------------------------




//----------------------------------------------------------------------------
inline
std::vector<std::string> extractCodeFragmentBySnippetTagInfo( const umba::md::CodeOptions               &langOpts
                                                            , const std::string                         &lang
                                                            , const std::unordered_set<SnippetOptions>  &snippetFlagsOptions
                                                            , std::vector<std::string>                  lines
                                                            , SnippetTagInfo                            tagInfo
                                                            , std::size_t                               &firstFoundLineIdx
                                                            //, const std::string                         &targetFragmentTag
                                                            , ListingNestedTagsMode                     listingNestedTagsMode
                                                            , std::size_t                               tabSize // =4u
                                                            )
{
    // UMBA_ARG_USED(bPrototype);
    // tagInfo на валидность не проверяем, считаем, что он валидный

    // Человечий номер (с 1) превращаем в машинный (с 0)
    std::size_t startLineIdx = tagInfo.startNumber;
    if (startLineIdx==(std::size_t)-1 || startLineIdx<1)
    {
        startLineIdx = 0;
    }
    else
    {
        --startLineIdx;
    }

    std::size_t endLineIdx = tagInfo.endNumber;
    if (endLineIdx==(std::size_t)-1 || endLineIdx<1)
    {
        endLineIdx = 0;
    }
    else
    {
        --endLineIdx;
    }

    std::string blockCharacters = langOpts.getBlockCharacters();
    char chBlockOpen  = 0;
    char chBlockClose = 0;
    if (blockCharacters.size()==2)
    {
        chBlockOpen  = blockCharacters[0];
        chBlockClose = blockCharacters[1];
    }


    std::size_t nextLookupStartIdx = 0;
    firstFoundLineIdx = (std::size_t)-1;

    if (tagInfo.startType==SnippetTagType::normalTag)
    {
        if (tagInfo.startTagOrSignaturePath.empty())
            return std::vector<std::string>();

        std::string targetFragmentTag = tagInfo.startTagOrSignaturePath[0].normalizedSignature;
        return extractCodeFragmentBySnippetTag( langOpts, lang, lines, firstFoundLineIdx, targetFragmentTag, listingNestedTagsMode, startLineIdx, tabSize); // Поиск по тэгу - ищем начало и конец по тэгу и тут же выходим
    }

    // Тут поиск по сигнатуре - ищем начало, не выходим
    else if (tagInfo.startType==SnippetTagType::textSignature)
    {
        #if defined(UMBA_MD_FIND_TEXT_SIGNATURE_IN_LINES_OLD_VERSION)
        firstFoundLineIdx  = findTextSignatureInLines(lines, tagInfo.startTagOrSignaturePath, startLineIdx);
        if (firstFoundLineIdx!=(std::size_t)-1)
            nextLookupStartIdx = firstFoundLineIdx + tagInfo.getLastStartSignatureLinesNumber();
        #else
        std::size_t foundSignatureNumLines = 0;
        firstFoundLineIdx  = findTextSignaturePathInLines(langOpts, lines, tagInfo.startTagOrSignaturePath, foundSignatureNumLines, startLineIdx);
        if (firstFoundLineIdx!=(std::size_t)-1)
        {
            nextLookupStartIdx = firstFoundLineIdx + foundSignatureNumLines;
            // Блок начинаем искать в последней строке сигнатуры.
            // В плюсиках может быть ситуация, когда всё записано в одну строчку - и искомая сигнатура, и её блок кода
            // Ну, или, как минимум, открывающая скобка при использовании K&R стиля
            // !!! Тут бы надо предусмотреть, что в первой строке поиска надо пропустить окончание сигнатуры, но пока сойдёт и так
            if ( ( tagInfo.endType==SnippetTagType::block 
                || tagInfo.endType==SnippetTagType::blockOrSeparator
                || tagInfo.endType==SnippetTagType::statementSeparator
                 )
              && chBlockOpen!=0
               )
            {
                if (nextLookupStartIdx>0)
                    nextLookupStartIdx -= 1;
            }
        }
        #endif
    }

    // Тут поиск по номеру строки - ищем начало, не выходим
    else if (tagInfo.startType==SnippetTagType::lineNumber)
    {
        firstFoundLineIdx  = startLineIdx;
        nextLookupStartIdx = firstFoundLineIdx+1;
    }

    // Нашли начало фрагмента в пределах строк?
    if (firstFoundLineIdx==(std::size_t)-1 || firstFoundLineIdx>=lines.size())
    {
        firstFoundLineIdx = (std::size_t)-1;
        return std::vector<std::string>();
    }

    // Если у нас задан блок, но для языка блоки не заданы - ищем окончание блока как пустую строку
    if ( ( (tagInfo.endType==SnippetTagType::block || tagInfo.endType==SnippetTagType::blockOrSeparator) && chBlockOpen==0
         )
      || tagInfo.endType==SnippetTagType::invalid
       )
    {
        tagInfo.endType   = SnippetTagType::stopOnEmptyLines;
        tagInfo.endNumber = 1;
    }

    bool stripOpenBlockFromLastLine = false;

    std::size_t foundLastFragmentLineIdx = (std::size_t)-1;

    bool bPrototype  = umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::prototype);
    bool bProtoClass = umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::class_   );
    //bool bProtodoc   = umba::md::testFlagSnippetOption(snippetFlagsOptions, SnippetOptions::protodoc );


    auto pCodeExtractor = (mdpp::code::simpleCodeLinesProcessingFnPtr)0;
    if (bPrototype)
    {
        pCodeExtractor = bProtoClass ? langOpts.m_funcPrototypeExtractor : langOpts.m_classPrototypeExtractor;
    }
    // snippetFlagsOptions


    if (tagInfo.endType==SnippetTagType::textSignature)
    {
        // Окончание ищем по сигнатуре следующего фрагмента, и номер строки тут игнорируем
        #if defined(UMBA_MD_FIND_TEXT_SIGNATURE_IN_LINES_OLD_VERSION)
        foundLastFragmentLineIdx = findTextSignatureInLines(lines, tagInfo.endSignature, nextLookupStartIdx);
        #else
        std::size_t dummyFoundSignatureLen = 0;
        foundLastFragmentLineIdx = findTextSignatureInLines(langOpts, lines, tagInfo.endSignature, dummyFoundSignatureLen, nextLookupStartIdx);
        #endif
        if (foundLastFragmentLineIdx!=(std::size_t)-1)
        {
            --foundLastFragmentLineIdx; // У нас начало следующего фрагмента, сдвигаем, чтобы указывало на конец предыдущего
            // !!! - или тут надо уменьшить на размер конечной сигнатуры в строках?
        }
    }

    // langOpts
    // mdpp::code::simpleCodeLinesProcessingFnPtr                         m_funcPrototypeExtractor  = 0;
    // mdpp::code::simpleCodeLinesProcessingFnPtr                         m_classPrototypeExtractor = 0;
    // mdpp::code::simpleCodeLinesProcessingFnPtr                         m_funcPrototypeFormatter  = 0;
    // mdpp::code::simpleCodeLinesProcessingFnPtr                         m_classPrototypeFormatter = 0;

    else if (tagInfo.endType==SnippetTagType::lineNumber)
    {
        foundLastFragmentLineIdx = endLineIdx;
    }
    else if (tagInfo.endType==SnippetTagType::block)
    {
        foundLastFragmentLineIdx = findBlockInLines(lines, chBlockOpen, chBlockClose, std::string(), nextLookupStartIdx);
    }
    else if (tagInfo.endType==SnippetTagType::blockOrSeparator)
    {
        // if (!pCodeExtractor)
        //     foundLastFragmentLineIdx = findBlockInLines(lines, chBlockOpen, chBlockClose, std::string()                   , nextLookupStartIdx);
        // else
            foundLastFragmentLineIdx = findBlockInLines(lines, chBlockOpen, chBlockClose, langOpts.getStatementSeparator(), nextLookupStartIdx);
    }
    else if (tagInfo.endType==SnippetTagType::statementSeparator)
    {
        // if (!pCodeExtractor)
        // {
            // Мы хотим остановится на сепараторе, если это прототип, 
            // или, для извлечения прототипа мы хотим извлечь всё до начала блока
            // И нам надо удалить символ начала блока
            stripOpenBlockFromLastLine = true;
            foundLastFragmentLineIdx = findBlockInLines(lines, chBlockOpen, 0 /* chBlockClose */, langOpts.getStatementSeparator(), nextLookupStartIdx);
        // }
        // else
        // {
        //     foundLastFragmentLineIdx = findBlockInLines(lines, chBlockOpen, chBlockClose        , std::string()                   , nextLookupStartIdx);
        // }
    }
    else if (tagInfo.endType==SnippetTagType::genericStopMarker)
    {
        foundLastFragmentLineIdx = findStopPrefixInLines(lines, langOpts.getGenericCutStopPrefixes(), nextLookupStartIdx);
    }
    else // Остался вариант, когда считываем N строк
    {
        std::size_t numLines = tagInfo.endNumber;
        if (numLines==(std::size_t)-1 || numLines<1)
            numLines = 1;

        foundLastFragmentLineIdx = findEmptyLinesStopInLines(lines, numLines, nextLookupStartIdx);
    }

    if (foundLastFragmentLineIdx==(std::size_t)-1)
    {
        foundLastFragmentLineIdx = lines.size()-1;
    }

    if (firstFoundLineIdx>foundLastFragmentLineIdx)
        return std::vector<std::string>();

    for(; firstFoundLineIdx!=foundLastFragmentLineIdx; ++firstFoundLineIdx)
    {
        if (umba::string_plus::trim_copy(lines[firstFoundLineIdx]).empty())
            continue;
        break;
    }

    // stripOpenBlockFromLastLine = true;

    std::vector<std::string> resLines; resLines.reserve(foundLastFragmentLineIdx+1u - firstFoundLineIdx);
    for(auto idx=firstFoundLineIdx; idx<=foundLastFragmentLineIdx; ++idx)
    {
        if (idx==foundLastFragmentLineIdx && stripOpenBlockFromLastLine && chBlockOpen!=0)
        {
            auto openBlockPos = lines[idx].find(chBlockOpen);
            if (openBlockPos==lines[idx].npos)
            {
                resLines.push_back(lines[idx]);
            }
            else
            {
                auto line = std::string(lines[idx], 0, openBlockPos);
                // Если в результате удаления символа открытия блока 
                // строка стала пустой, то не добавляем её
                if (!umba::string_plus::trim_copy(line).empty()) 
                    resLines.push_back(line);
            }
        }
        else
        {
            resLines.push_back(lines[idx]);
        }
    }


    // else if (tagInfo.endType==SnippetTagType::statementSeparator)
    // {
    //     // Мы хотим остановится на сепараторе, если это прототип, 
    //     // или, для извлечения прототипа мы хотим извлечь всё до начала блока
    //     // И нам надо удалить символ начала блока
    //     stripOpenBlockFromLastLine = true;
    //     foundLastFragmentLineIdx = findBlockInLines(lines, chBlockOpen, 0 /* chBlockClose */, langOpts.getStatementSeparator(), nextLookupStartIdx);

    if (tagInfo.endType==SnippetTagType::statementSeparator && !resLines.empty())
    {
        // Мы остановились по терминатору выражений или по открытию блока
        // Блочный символ уже отрезан
        // Надо проверить, есть ли терминатор, и добавить его, если его нет
        // результирующий набор строк не пуст, так что проверяем последнюю строку

        auto &lastLine = resLines.back();
        umba::string_plus::rtrim(lastLine);
        umba::string_plus::ends_with_and_strip(lastLine, langOpts.getStatementSeparator());
        umba::string_plus::rtrim(lastLine);
        lastLine.append(langOpts.getStatementSeparator());
    }

    return resLines;

    // return std::vector<std::string>(lines.begin()+ std::ptrdiff_t(firstFoundLineIdx), lines.begin()+ std::ptrdiff_t(foundLastFragmentLineIdx+1u));
}


// std::size_t findTextSignatureInLines(const std::vector<std::string> &lines, const TextSignature &ts, std::size_t startLine=(std::size_t)-1)
// std::size_t findTextSignatureInLines(const std::vector<std::string> &lines, const VectorType &signaturesVec, std::size_t startLine=(std::size_t)-1)
// std::size_t findBlockInLines(const std::vector<std::string> &lines, char blockCharOpen, char blockCharClose, std::size_t startLine=(std::size_t)-1)
// std::size_t findStopPrefixInLines(const std::vector<std::string> &lines, const std::unordered_set<std::string> &stopPrefixes, std::size_t startLine=(std::size_t)-1)
// std::size_t findEmptyLinesStopInLines(const std::vector<std::string> &lines, std::size_t numEmptyLines, std::size_t startLine=(std::size_t)-1)

// std::vector<std::string> extractCodeFragmentBySnippetTag( const umba::md::CodeOptions         &langOpts
//                                                         , const std::string                       &lang
//                                                         , std::vector<std::string>                lines
//                                                         , std::size_t                             &firstFoundLineIdx
//                                                         , const std::string                       &targetFragmentTag
//                                                         , ListingNestedTagsMode                   listingNestedTagsMode
//                                                         , std::size_t                             startLine // = (std::size_t)-1
//                                                         , std::size_t                             tabSize // =4u
//                                                         //, bool                        trimLeadingSpaces_a = true
//                                                         )

    // SnippetTagType             startType               = SnippetTagType::invalid;
    // std::size_t                startNumber             = 0; // line number
    // text_signature_vector      startTagOrSignaturePath ; // start tag or text signatures path. For start tag only one element of the  vector used
    //
    // SnippetTagType             endType                 = SnippetTagType::invalid;
    // std::size_t                endNumber               = 0; // end line number or number of empty lines to stop
    // TextSignature              endSignature            ;    // paths not supported here



// enum class SnippetTagType : std::uint32_t
// {
//     invalid             = (std::uint32_t)(-1),
//     unknown             = (std::uint32_t)(-1),
//     normalTag           = 0x0000 /*!< Allowed for start/end */,
//     lineNumber          = 0x0001 /*!< Allowed for start/end */,
//     textSignature       = 0x0002 /*!< Allowed for start/end - end signature not included to code snippet */,
//     block               = 0x0003 /*!< Allowed for end only - signals that we need to cat code block in block symbols */,
//     genericStopMarker   = 0x0004 /*!< Allowed for end only */,
//     stopOnEmptyLines    = 0x0005 /*!< Allowed for end only */
//
// }; // enum class SnippetTagType : std::uint32_t



//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
template<typename Iterator> inline
Iterator parseSnippetTagFirstPart(Iterator b, Iterator e, SnippetTagInfo &parseToSnippetTagInfo)
{
    enum State
    {
        stParseStart    ,
        stParseNumber   ,
        stParseTag      ,
        stParseSignature,
        stParseSignatureWaitNext
    };

    State st = stParseStart;

    parseToSnippetTagInfo.startType    = SnippetTagType::invalid;
    parseToSnippetTagInfo.startNumber  = (std::size_t)-1 ;
    parseToSnippetTagInfo.startTagOrSignaturePath.clear();
    parseToSnippetTagInfo.extractCount = 0;

    Iterator textStartIt = e;

    auto finalizeSignaturePath = [&]()
    {
        if (parseToSnippetTagInfo.startTagOrSignaturePath.empty())
        {
            if (parseToSnippetTagInfo.startNumber==(std::size_t)-1)
            {
                parseToSnippetTagInfo.startType = SnippetTagType::invalid;
            }
            else
            {
                parseToSnippetTagInfo.startType = SnippetTagType::lineNumber;
            }
        }

    };

    // NNN`inline\nvoid\ndoSomething`/`inline\nvoid\ndoSomeOther`-
    // NNNtag-
    // NNN-

    for(; b!=e; ++b)
    {
        switch(st)
        {
            case stParseStart    :
            {
                if (*b>='0' && *b<='9')
                {
                    parseToSnippetTagInfo.startNumber = (std::size_t)((*b)-'0');
                    st = stParseNumber;
                    parseToSnippetTagInfo.startType = SnippetTagType::lineNumber;
                }
                else if (*b=='-' || *b==' ' || *b=='\t')
                {
                    return b;
                }
                else if (*b=='`')
                {
                    st = stParseSignature;
                    textStartIt = b;
                    ++textStartIt;
                }
                else
                {
                    parseToSnippetTagInfo.startType = SnippetTagType::normalTag;
                    st = stParseTag;
                    textStartIt = b;
                }
            }
            break;

            case stParseNumber   :
            {
                if (*b>='0' && *b<='9')
                {
                    parseToSnippetTagInfo.startNumber *= (std::size_t)10u;
                    parseToSnippetTagInfo.startNumber += (std::size_t)((*b)-'0');
                }
                else if (*b=='-' || *b==' ' || *b=='\t')
                {
                    return b;
                }
                else if (*b=='`')
                {
                    st = stParseSignature;
                    textStartIt = b;
                    ++textStartIt;
                }
                else
                {
                    parseToSnippetTagInfo.startType = SnippetTagType::normalTag;
                    st = stParseTag;
                    textStartIt = b;
                }
            }
            break;

            case stParseTag      :
            {
                if (*b=='-' || *b==' ' || *b=='\t')
                {
                    if (textStartIt!=e && textStartIt!=b)
                    {
                        parseToSnippetTagInfo.startTagOrSignaturePath.emplace_back(TextSignature(textStartIt, b));
                    }
                    textStartIt = e;
                    return b;
                }
                else if (*b=='`')
                {
                    parseToSnippetTagInfo.startType = SnippetTagType::invalid;
                    return b;
                }
                else
                {
                    // simple continue
                }
            }
            break;

            case stParseSignature:
            {
                parseToSnippetTagInfo.startType = SnippetTagType::textSignature;

                if (*b=='`')
                {
                    if (textStartIt!=e && textStartIt!=b)
                    {
                        parseToSnippetTagInfo.startTagOrSignaturePath.emplace_back(textStartIt, b);
                    }
                    textStartIt = e;

                    st = stParseSignatureWaitNext;
                }
            }
            break;

            case stParseSignatureWaitNext:
            {
                if (*b=='-' || *b==' ' || *b=='\t')
                {
                    if (textStartIt!=e && textStartIt!=b)
                    {
                        parseToSnippetTagInfo.startTagOrSignaturePath.emplace_back(textStartIt, b);
                    }
                    textStartIt = e;

                    finalizeSignaturePath();
                    return b;
                }
                else if (*b>='0' && *b<='9')
                {
                    if (parseToSnippetTagInfo.extractCount!=std::size_t(-1))
                    {
                        // У нас не установлено бесконечное повторение, значит, считываем конкретное значение
                        parseToSnippetTagInfo.extractCount *= (std::size_t)10u;
                        parseToSnippetTagInfo.extractCount += (std::size_t)((*b)-'0');
                    }
                    // Не меняем состояние
                }
                else if (*b=='*')
                {
                    parseToSnippetTagInfo.extractCount = std::size_t(-1);
                }
                else if (*b=='/')
                {
                    // simple continue
                }
                else if (*b=='`')
                {
                    st = stParseSignature;
                    textStartIt = b;
                    ++textStartIt;
                }
                else
                {
                    parseToSnippetTagInfo.startType = SnippetTagType::invalid;
                    return b;
                }
            }
            break;

        } // switch(st)

    } // for(; b!=e; ++b)


    switch(st)
    {
        case stParseStart    :  break;
        case stParseNumber   :  break;
        case stParseTag      :  parseToSnippetTagInfo.startTagOrSignaturePath.emplace_back(TextSignature(textStartIt, b)); break;

        case stParseSignature:
            if (textStartIt!=e && textStartIt!=b)
            {
                parseToSnippetTagInfo.startTagOrSignaturePath.emplace_back(textStartIt, b);
            }
            finalizeSignaturePath();
        break;

        case stParseSignatureWaitNext:
        finalizeSignaturePath();
        break;

    } // switch(st)

    return b;
}

//----------------------------------------------------------------------------
template<typename Iterator> inline
Iterator parseSnippetTagSecondPart(Iterator b, Iterator e, SnippetTagInfo &parseToSnippetTagInfo)
{
    enum State
    {
        stParseStart          ,
        stParseNumber         , // NNN
        stParseBrace          , // {}
        stParseBrace2         , // {}
        //stParseBrace3         , // {};
        stParseStopWait       , // (-/N)
        // stParseStopMarker     , // (----)
        stParseStopLinesNumber, // (NNN)
        stParseSignature
    };

    State st = stParseStart;

    parseToSnippetTagInfo.endType      = SnippetTagType::invalid;
    parseToSnippetTagInfo.endNumber    = (std::size_t)-1 ;
    parseToSnippetTagInfo.endSignature.clear();

    Iterator textStartIt = e;

    // NNN
    // {}
    // (-)
    // (N)
    // `inline\nvoid\ndoAnotherSomething`

    for(; b!=e; ++b)
    {
        switch(st)
        {
            case stParseStart          :
            {
                if (*b>='0' && *b<='9')
                {
                    parseToSnippetTagInfo.endNumber = (std::size_t)((*b)-'0');
                    st = stParseNumber;
                    parseToSnippetTagInfo.endType = SnippetTagType::lineNumber;
                }
                // else if (*b=='-' || *b==' ' || *b=='\t')
                // {
                //     return b;
                // }
                else if (*b==';')
                {
                    parseToSnippetTagInfo.endType = SnippetTagType::statementSeparator;
                    return b;
                }
                else if (*b=='{')
                {
                    parseToSnippetTagInfo.endType = SnippetTagType::block;
                    st = stParseBrace;
                }
                else if (*b=='(')
                {
                    st = stParseStopWait;
                }
                else if (*b=='`')
                {
                    st = stParseSignature;
                    textStartIt = b;
                    ++textStartIt;
                }
                else
                {
                    return b;
                }
            }
            break;


            case stParseNumber         :
            {
                if (*b>='0' && *b<='9')
                {
                    parseToSnippetTagInfo.endNumber *= (std::size_t)10u;
                    parseToSnippetTagInfo.endNumber += (std::size_t)((*b)-'0');
                }
                else
                {
                    return b;
                }
            }
            break;


            case stParseBrace          :
            {
                if (*b=='}')
                    st = stParseBrace2; // SnippetTagType::block; уже установлен
                else
                    return b;
            }
            break;


            case stParseBrace2          :
            {
                if (*b==';') // SnippetTagType::block; уже установлен, меняем на blockOrSeparator, если обнаружен semicolon
                    parseToSnippetTagInfo.endType = SnippetTagType::blockOrSeparator;
                return b;
            }
            break;


            case stParseStopWait       :
            {
                if (*b>='0' && *b<='9')
                {
                    parseToSnippetTagInfo.endNumber = (std::size_t)((*b)-'0');
                    st = stParseStopLinesNumber;
                    parseToSnippetTagInfo.endType = SnippetTagType::stopOnEmptyLines;
                }
                else
                {
                    parseToSnippetTagInfo.endType = SnippetTagType::genericStopMarker;
                    return b;
                }
            }
            break;

            // case stParseStopMarker     :
            // {
            // }
            // break;

            case stParseStopLinesNumber:
            {
                if (*b>='0' && *b<='9')
                {
                    parseToSnippetTagInfo.endNumber *= (std::size_t)10u;
                    parseToSnippetTagInfo.endNumber += (std::size_t)((*b)-'0');
                }
                else
                {
                    return b;
                }
            }
            break;


            case stParseSignature      :
            {
                parseToSnippetTagInfo.endType = SnippetTagType::textSignature;
                if (*b=='`')
                {
                    if (textStartIt!=e && textStartIt!=b)
                    {
                        parseToSnippetTagInfo.endSignature = TextSignature(std::string(textStartIt, b));
                    }

                    return b;
                }
            }
            break;

        } // switch(st)

    } // for(; b!=e; ++b)

    switch(st)
    {
        case stParseStart          :        break;
        case stParseNumber         :        break;
        case stParseBrace          :        break;
        case stParseBrace2         :        break;

        case stParseStopWait       :
        {
            parseToSnippetTagInfo.endType = SnippetTagType::genericStopMarker;
        }
        break;

        case stParseStopLinesNumber:        break;

        case stParseSignature      :
        {
            if (textStartIt!=e && textStartIt!=b)
            {
                parseToSnippetTagInfo.endSignature = TextSignature(std::string(textStartIt, b));
            }
        }
        break;

    } // switch(st)

    return b;

}

//----------------------------------------------------------------------------
inline
SnippetTagInfo parseSnippetTag(const std::string &tagStr)
{
    SnippetTagInfo snippetTagInfo;

    auto b = tagStr.begin();
    auto e = tagStr.end  ();

    if (b==e)
        return snippetTagInfo;

    auto b2 = parseSnippetTagFirstPart(b, e, snippetTagInfo);
    if (b2==e)
        return snippetTagInfo;

    if (*b2!='-')
        return snippetTagInfo;

    ++b2;
    parseSnippetTagSecondPart(b2, e, snippetTagInfo);

    return snippetTagInfo;
}

//----------------------------------------------------------------------------
template<typename StreamType> inline
void testParseSnippetTag(StreamType &s, const std::string &tag)
{
    s << "\n";
    s << "Input tag string: [" << tag << "]\n";

    SnippetTagInfo snippetTagInfo = parseSnippetTag(tag);

    s << "\n";

    s << "Start type: " << enum_serialize(snippetTagInfo.startType) << "\n";
    if (snippetTagInfo.startNumber!=(std::size_t)-1)
    {
        s << "Start line: " << snippetTagInfo.startNumber << "\n";
    }

    if (snippetTagInfo.startType==SnippetTagType::normalTag)
    {
        if (snippetTagInfo.startTagOrSignaturePath.empty() || snippetTagInfo.startTagOrSignaturePath[0].normalizedSignature.empty())
        {
            s << "Start tag: <MISSING>\n";
        }
        else
        {
            s << "Start tag: " << snippetTagInfo.startTagOrSignaturePath[0].normalizedSignature << "\n";
        }
    }
    else if (snippetTagInfo.startType==SnippetTagType::textSignature)
    {
        if (snippetTagInfo.startTagOrSignaturePath.empty() || snippetTagInfo.startTagOrSignaturePath[0].normalizedSignature.empty())
        {
            s << "Start signature: <MISSING>\n";
        }
        else
        {
            //s << "Start signature: `" << << "`\n";
            s << "Start signature:\n";
            for(const auto &sigLines : snippetTagInfo.startTagOrSignaturePath)
            {
                s << "    " << sigLines.normalizedSignature << "\n";
                // s << "    ---\n";
                // for(const auto &sigLine : sigLines.signatureLinesVector)
                // {
                //     s << "    " << sigLine << "\n";
                // }
            }
            //s << "\n";
        }
    }

    // signature_lines_vector_type    signatureLinesVector; // normalized or original?
    // std::string                    normalizedSignature ;

    s << "\n";
    s << "End type  : " << enum_serialize(snippetTagInfo.endType) << "\n";
    if (snippetTagInfo.endType==SnippetTagType::lineNumber)
    {
        if (snippetTagInfo.endNumber!=(std::size_t)-1)
        {
            s << "End line: " << snippetTagInfo.endNumber << "\n";
        }
    }
    else if (snippetTagInfo.endType==SnippetTagType::stopOnEmptyLines)
    {
        if (snippetTagInfo.endNumber!=(std::size_t)-1)
        {
            s << "Stop on " << snippetTagInfo.endNumber << " empty lines\n";
        }
        else
        {
            s << "Stop on <INVALID_NUMBER> empty lines\n";
        }
    }

    if (snippetTagInfo.endType==SnippetTagType::textSignature)
    {
        if (snippetTagInfo.endSignature.normalizedSignature.empty())
        {
            s << "End signature: <MISSING>\n";
        }
        else
        {
            s << "End signature: `" << snippetTagInfo.endSignature.normalizedSignature << "`\n";
            // s << "End signature:\n";
            // for(const auto &sigLine : snippetTagInfo.endSignature.signatureLinesVector)
            // {
            //     s << "    " << sigLine << "\n";
            // }
            // s << "\n";
        }
    }


    s << "----------\n";

}

//----------------------------------------------------------------------------

// struct SnippetTagInfo
// {
//     using options_type  = umba::container::small_vector_options< umba::container::growth_factor<umba::container::growth_factor_50>, umba::container::inplace_alignment<16> >::type;
//     using text_signature_vector = umba::container::small_vector<TextSignature, 4, void, options_type >;
//
//     SnippetTagType             startType               = SnippetTagType::invalid;
//     std::size_t                startNumber             = 0; // line number
//     text_signature_vector      startTagOrSignaturePath ; // start tag or text signatures path. For start tag only one element of the  vector used
//
//     SnippetTagType             endType                 = SnippetTagType::invalid;
//     std::size_t                endNumber               = 0; // end line number or number of empty lines to stop
//     TextSignature              endSignature            ;    // paths not supported here
//
// }; // struct SnippetTagInfo




// enum class SnippetTagType : std::uint32_t
// {
//     invalid             = (std::uint32_t)(-1),
//     unknown             = (std::uint32_t)(-1),
//     normalTag           = 0x0000 /*!< Allowed for start/end */,
//     lineNumber          = 0x0001 /*!< Allowed for start/end */,
//     textSignature       = 0x0002 /*!< Allowed for start/end - end signature not included to code snippet */,
//     block               = 0x0003 /*!< Allowed for end only - signals that we need to cat code block in block symbols */,
//     genericStopMarker   = 0x0004 /*!< Allowed for end only */,
//     stopOnEmptyLines    = 0x0005 /*!< Allowed for end only */
//
// }; // enum class SnippetTagType : std::uint32_t












// Опция "делать-что-то" - включает
// Опция "не-делать-что-то" - выключает, если было включено

// По умолчанию никакие опции не установлены
// Где-то в конфигах по умолчанию делаем --set-snippet-options=bla-bla
// Потом можем ещё раз сделать --set-snippet-options=bla-bla, и дополнительно включить или выключить какие-либо опции
// Потом при обработке снипета ещё раз парсим опции, устанавливаем/сбрасываем.

// ifdef:VAL
// ifndef:VAL
// if:VAL==
// if:VAL!=
// if:VAL>=
// if:VAL<=
// if:VAL>
// if:VAL<


//----------------------------------------------------------------------------
inline
std::vector< std::pair<ConditionOperators, std::string> > makeConditionOperatorsInfoVec() //! Создаёт вектор условных операторов
{
    return std::vector< std::pair<ConditionOperators, std::string> >{ { ConditionOperators::equal       , "==" }
                                                                    , { ConditionOperators::notEqual    , "!=" }
                                                                    , { ConditionOperators::greaterEqual, ">=" }
                                                                    , { ConditionOperators::lessEqual   , "<=" }
                                                                    , { ConditionOperators::greater     , ">" }
                                                                    , { ConditionOperators::less        , "<" }
                                                                    };
}

//----------------------------------------------------------------------------
inline
const std::vector< std::pair<ConditionOperators, std::string>>& getConditionOperatorsInfoVec() //! Возвращает вектор условных операторов
{
    static auto vec = makeConditionOperatorsInfoVec();
    return vec;
}

//----------------------------------------------------------------------------
inline
ConditionOperators splitCondition(std::string str, std::string &left, std::string &right)
{
    const auto &condInfoVec = getConditionOperatorsInfoVec();
    for(const auto &condInfo : condInfoVec)
    {
        auto pos = str.find(condInfo.second);
        if (pos!=str.npos)
        {
            left .assign(str, 0, pos);
            right.assign(str, pos+condInfo.second.size(), str.npos);
            umba::string_plus::trim(left);
            umba::string_plus::trim(right);
            return condInfo.first;
        }
    }

    return ConditionOperators::unknown;
}

//----------------------------------------------------------------------------
inline
bool isConditionVar(std::string &condVal)
{
    if (condVal.empty())
        return false;

    if (condVal.front()!='$')
        return false;

    condVal.erase(0u, 1u); // Удаляем бакс
    if (condVal.empty())
        return false;

    if (condVal.front()=='$') // Это литерал бакс, был экранирован баксом
        return false;

    return true; // Начиналось с бакса, оказалось непустым именем переменной, и не экраинированным баксом

}

//----------------------------------------------------------------------------
inline
bool isConditionTrue(const umba::macros::StringStringMap<std::string> &condVars, std::string condStr)
{
    umba::string_plus::trim(condStr);

    std::string left ;
    std::string right;

    ConditionOperators condOp = splitCondition(condStr, left, right);
    if (condOp==ConditionOperators::unknown)
    {
        // нет оператора отношения
        if (!isConditionVar(condStr))
        {
            // и это - не переменная, так нельзя
            throw std::runtime_error("Invalid condition");
        }
        else
        {
            // если в if у нас указана переменная, то это аналогично ifdef, то есть, её надо просто поискать в condVars
            return condVars.find(condStr)!=condVars.end();
        }
    }
    else
    {
        // у нас есть выражения справа и слева
        // если участвует неизвестная переменная слева или справа, то результатом будет всегда false

        if (isConditionVar(left))
        {
            umba::macros::StringStringMap<std::string>::const_iterator it = condVars.find(left);
            if (it==condVars.end())
                return false;
        }

        if (isConditionVar(right))
        {
            umba::macros::StringStringMap<std::string>::const_iterator it = condVars.find(right);
            if (it==condVars.end())
                return false;
        }

        // Теперь слева и справа у нас значения, которые можно сравнивать
        switch(condOp)
        {
            case ConditionOperators::equal       : return left == right;
            case ConditionOperators::notEqual    : return left != right;
            case ConditionOperators::greaterEqual: return left >= right;
            case ConditionOperators::lessEqual   : return left <= right;
            case ConditionOperators::greater     : return left >  right;
            case ConditionOperators::less        : return left <  right;
            case ConditionOperators::unknown     : throw std::runtime_error("Unexpected unknown condition operator");
            default                              : throw std::runtime_error("Unknown condition operator");
        }
    }
}

//----------------------------------------------------------------------------
inline
bool splitToPair(const std::string &str, std::string &first, std::string &second, char ch)
{
    if (umba::string_plus::split_to_pair(str, first, second, ch))
    {
        umba::string_plus::trim(first);
        umba::string_plus::trim(second);
        return true;
    }

    return false;
}

//----------------------------------------------------------------------------
inline
// std::string serializeSnippetOptions(const std::unordered_set<SnippetOptions> &flagOptions, const std::unordered_map<SnippetOptions, int> &intOptions)
std::string serializeSnippetOptions(std::unordered_set<SnippetOptions> flagOptions, const std::unordered_map<SnippetOptions, int> &intOptions)
{

    flagOptions.erase(SnippetOptions::snippetOptions);

    std::set<std::string> flagOptionStrings;

    for(std::unordered_set<SnippetOptions>::const_iterator fit=flagOptions.begin(); fit!=flagOptions.end(); ++fit)
    {
        flagOptionStrings.insert(enum_serialize(*fit));
        //optList.emplace_back(enum_serialize(*fit));
    }

    std::map<std::string,std::string> intOptionStrings;
    for(std::unordered_map<SnippetOptions, int>::const_iterator fit=intOptions.begin(); fit!=intOptions.end(); ++fit)
    {
        std::string optName = enum_serialize(fit->first);
        std::string optVal  = std::to_string(fit->second);
        //optList.emplace_back(optName+"="+optVal);
        intOptionStrings[optName] = optVal;
    }

    std::vector<std::string> optList;
    for(auto opt: flagOptionStrings)
        optList.emplace_back(opt);

    for(auto it=intOptionStrings.begin(); it!=intOptionStrings.end(); ++it)
    {
        optList.emplace_back(it->first+"="+it->second);
    }

    return umba::string_plus::merge< std::string, std::vector<std::string>::const_iterator >( optList.begin(), optList.end(), std::string(",") );
}

//----------------------------------------------------------------------------
inline
SnippetOptionsParsingResult deserializeSnippetOptions( const std::string &optListStr
                                                     , std::unordered_set<SnippetOptions> *pFlagOptions
                                                     , std::unordered_map<SnippetOptions, int> *pIntOptions=0
                                                     , const umba::macros::StringStringMap<std::string> *pCondVars=0
                                                     )
{
    std::vector<std::string> optList = splitAndTrimAndSkipEmpty(optListStr, ',');

    //const std::unordered_map<std::string, std::string> *pCondVars

    for(auto opt : optList)
    {
        umba::string_plus::trim(opt);

        std::string optName, optVal;

        // ifdef:VAL
        // ifndef:VAL
        // if:$VAL==
        if (splitToPair(opt, optName, optVal, ':') && (optName=="ifdef" || optName=="ifndef" || optName=="if"))
        {
            if (pCondVars)
            {
                const umba::macros::StringStringMap<std::string> &condVars = *pCondVars;
                if (optName=="ifdef")
                {
                    return condVars.find(optVal)==condVars.end() ? SnippetOptionsParsingResult::okButCondition : SnippetOptionsParsingResult::ok;
                }
                else if (optName=="ifndef")
                {
                    return condVars.find(optVal)!=condVars.end() ? SnippetOptionsParsingResult::okButCondition : SnippetOptionsParsingResult::ok;
                }
                else // if (optName=="if")
                {
                    try
                    {
                        return isConditionTrue(condVars, optVal) ? SnippetOptionsParsingResult::ok : SnippetOptionsParsingResult::okButCondition;
                    }
                    catch(...)
                    {
                        //bool isConditionTrue(const std::unordered_map<std::string, std::string> &condVars, std::string condStr)
                        return SnippetOptionsParsingResult::fail;
                    }
                }
            }
        }
        else if (splitToPair(opt, optName, optVal, '='))
        {
            if (pIntOptions)
            {
                umba::string_plus::trim(optName);
                umba::string_plus::trim(optVal);

                auto optId = enum_deserialize(optName, SnippetOptions::invalid);
                if (optId==SnippetOptions::invalid)
                {
                    return SnippetOptionsParsingResult::fail;
                }

                if ((((std::uint32_t)optId)&0xF000u)!=0x2000u)
                {
                    // Not a numeric option
                    return SnippetOptionsParsingResult::fail;
                }

                int iVal = 0;
                try
                {
                    iVal = std::stoi(optVal);
                }
                catch(...)
                {
                    if (optId==SnippetOptions::prototypeFormat)
                    {
                        auto enumVal = enum_deserialize(optVal, umba::mdpp::code::PrototypeFormatStyle::invalid);
                        if (!(enumVal==umba::mdpp::code::PrototypeFormatStyle::none
                           || enumVal==umba::mdpp::code::PrototypeFormatStyle::msdn
                           || enumVal==umba::mdpp::code::PrototypeFormatStyle::man
                           || enumVal==umba::mdpp::code::PrototypeFormatStyle::tabMan
                           || enumVal==umba::mdpp::code::PrototypeFormatStyle::mdr)
                           )
                            return SnippetOptionsParsingResult::fail;

                        iVal = (int)enumVal;
                    }
                    else
                    {
                        return SnippetOptionsParsingResult::fail;
                    }
                }

                std::unordered_map<SnippetOptions, int> &intOptions = *pIntOptions;
                intOptions[optId] = iVal;

            }
        }
        else // flag option
        {
            if (pFlagOptions)
            {
                std::unordered_set<SnippetOptions> &flagOptions = *pFlagOptions;

                auto optId = enum_deserialize(opt, SnippetOptions::invalid);
                if (optId==SnippetOptions::invalid)
                {
                    return SnippetOptionsParsingResult::fail;
                }

                if ((((std::uint32_t)optId)&0xF000u)!=0x1000u)
                {
                    // Not a flag option
                    return SnippetOptionsParsingResult::fail;
                }

                bool isOff = false;
                if ((((std::uint32_t)optId)&0x0001u)==0x0000u)
                {
                    isOff = true;
                }

                auto baseOpt = (SnippetOptions)(((std::uint32_t)optId)|0x0001u);

                if (isOff)
                {
                    flagOptions.erase(baseOpt);
                }
                else
                {
                    flagOptions.insert(baseOpt);
                }
            }
        }
    }

    return SnippetOptionsParsingResult::ok;
}

//----------------------------------------------------------------------------
inline
SnippetOptionsParsingResult deserializeProcessingOptions(const std::string &optListStr, std::unordered_set<ProcessingOptions> &flagOptions)
{
    std::vector<std::string> optList = splitAndTrimAndSkipEmpty(optListStr, ',');

    for(auto opt : optList)
    {
        umba::string_plus::trim(opt);

        if (opt.empty())
            continue;

        if (opt[0]=='-')
        {
            opt = "no" + opt;
        }

        {
            std::string tmp; tmp.reserve(opt.size());
            for(auto ch: opt)
            {
                if (ch!='-')
                    tmp.append(1, ch);
            }

            opt.swap(tmp);
        }

        auto optId = enum_deserialize(opt, ProcessingOptions::invalid);
        if (optId==ProcessingOptions::invalid)
        {
            return SnippetOptionsParsingResult::fail;
        }

        if ((((std::uint32_t)optId)&0xF000u)!=0x1000u)
        {
            // Not a flag option
            return SnippetOptionsParsingResult::fail;
        }

        bool isOff = false;
        if ((((std::uint32_t)optId)&0x0001u)==0x0000u)
        {
            isOff = true;
        }

        auto baseOpt = (ProcessingOptions)(((std::uint32_t)optId)|0x0001u);

        if (isOff)
        {
            flagOptions.erase(baseOpt);
        }
        else
        {
            flagOptions.insert(baseOpt);
        }


    }

    return SnippetOptionsParsingResult::ok;
}

//----------------------------------------------------------------------------
inline
SnippetOptionsParsingResult parseSnippetInsertionCommandLine( std::unordered_set<SnippetOptions>               &snippetFlagsOptions
                                                            , std::unordered_map<SnippetOptions, int>          &snippetIntOptions
                                                            , const umba::macros::StringStringMap<std::string> &condVars
                                                            , std::string line
                                                            , std::string &snippetFile, std::string &snippetTag
                                                            )
{
    umba::string_plus::trim(line);

    std::size_t directiveNumCharsStrip = 0;
    auto pd = testLineForPreprocessorDirectiveImplHelper(line, &directiveNumCharsStrip);
    if (!umba::TheValue(pd).oneOf( PreprocessorDirective::insert 
                                 , PreprocessorDirective::snippet
                                 , PreprocessorDirective::prototype
                                 , PreprocessorDirective::doc    
                                 , PreprocessorDirective::subsection
                                 , PreprocessorDirective::quote  
                                 , PreprocessorDirective::pre    
                                 )
       )
    {
        return SnippetOptionsParsingResult::fail;
    }

    auto updateFlagsByDirective = [&]()
    {
        #include "umba/warnings/push_disable_enumerator_not_handled.h"
        switch(pd)
        {
            case PreprocessorDirective::snippet: // Снипет - вставляет снипет, если заданы уточняющие/модифицирующие опций - удаляем их, если они там были
            {
                snippetFlagsOptions.erase(SnippetOptions::doc);
                snippetFlagsOptions.erase(SnippetOptions::subsection);
                snippetFlagsOptions.erase(SnippetOptions::quote);
                snippetFlagsOptions.erase(SnippetOptions::pre);
                break;
            }

            case PreprocessorDirective::prototype: // Снипет - вставляет прототип функции из снипета, если заданы уточняющие/модифицирующие опций - удаляем их, если они там были
            {
                snippetFlagsOptions.erase(SnippetOptions::doc);
                snippetFlagsOptions.erase(SnippetOptions::subsection);
                snippetFlagsOptions.erase(SnippetOptions::quote);
                snippetFlagsOptions.erase(SnippetOptions::pre);

                snippetFlagsOptions.insert(SnippetOptions::prototype);

                break;
            }

            case PreprocessorDirective::doc:
            {
                snippetFlagsOptions.erase(SnippetOptions::quote);
                snippetFlagsOptions.erase(SnippetOptions::pre);

                snippetFlagsOptions.insert(SnippetOptions::doc);

                break;
            }

            case PreprocessorDirective::subsection:
            {
                snippetFlagsOptions.erase(SnippetOptions::quote);
                snippetFlagsOptions.erase(SnippetOptions::pre);

                snippetFlagsOptions.insert(SnippetOptions::doc);
                snippetFlagsOptions.insert(SnippetOptions::subsection);

                break;
            }

            case PreprocessorDirective::quote:
            {
                snippetFlagsOptions.erase(SnippetOptions::doc);
                snippetFlagsOptions.erase(SnippetOptions::subsection);

                snippetFlagsOptions.insert(SnippetOptions::quote);

                break;
            }

            case PreprocessorDirective::pre:
            {
                snippetFlagsOptions.erase(SnippetOptions::doc);
                snippetFlagsOptions.erase(SnippetOptions::subsection);

                snippetFlagsOptions.insert(SnippetOptions::pre);

                break;
            }

            // case PreprocessorDirective::insert: // делает всё, генерализованая версия
            // {
            //     break;
            // }
            //  
        }
        #include "umba/warnings/pop.h"

    }; // updateFlagsByDirective


    // Если задана опция subsection, автоматом добавляем опцию doc
    std::unordered_set<SnippetOptions>::const_iterator soIt = snippetFlagsOptions.find(SnippetOptions::subsection);
    if (soIt!=snippetFlagsOptions.end())
    {
        snippetFlagsOptions.insert(SnippetOptions::doc);
    }

    // if (!umba::string_plus::starts_with_and_strip(line, ("#!insert")) && !umba::string_plus::starts_with_and_strip(line, ("#$insert")))
    //     return SnippetOptionsParsingResult::fail;

    line.erase(0, directiveNumCharsStrip);

    umba::string_plus::trim(line);

    if (line.empty())
        return SnippetOptionsParsingResult::fail;

    if (line[0]=='{')
    {
        std::string::size_type idx = 1u;
        for(; idx!=line.size() && line[idx]!='}'; ++idx) { }
        if (idx==line.size())
            return SnippetOptionsParsingResult::fail; // не нашли завершающую фигурную скобку - что-то пошло не так

        auto optionsString = std::string(line, (std::string::size_type)1u, idx-1u);
        line.erase(0u, idx+1);
        umba::string_plus::trim(line);

        // обновляем переданные нам дефолтные опции
        auto res = deserializeSnippetOptions(optionsString, &snippetFlagsOptions, &snippetIntOptions, &condVars);
        if (res!=SnippetOptionsParsingResult::ok)
            return res; // что-то пошло не так
    }

    auto hashPos = line.find('#');

    if (hashPos==line.npos)
    {
        // Нет тэга - включаем весь файл
        snippetTag.clear();
        snippetFile = line;
    }
    else
    {
        snippetTag .assign(line, hashPos+1u, line.npos);
        snippetFile.assign(line, 0u, hashPos);
    }

    updateFlagsByDirective();

    return SnippetOptionsParsingResult::ok;
}


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba




