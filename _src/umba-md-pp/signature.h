#pragma once

#include "signature_types.h"


// umba::md::
namespace umba {
namespace md {

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline 
bool testLineIsBlockBackwardStopLine(const std::string &line, const std::unordered_set<std::string> &allStopPrefixes, const std::string &sepStr, const std::string &blockEndStr)
{
    if (line.empty())
        return true;

    if (line.find(sepStr)!=line.npos)
        return true;

    if (line.find(blockEndStr)!=line.npos)
        return true;

    for(const auto &prefix : allStopPrefixes)
    {
        if (umba::string_plus::starts_with(line, prefix))
            return true;
    }

    return false;
}

//----------------------------------------------------------------------------
//! Возвращает номер первой строки сигнатуры в тексте, или (std::size_t)-1
/*! NOTE: !!! Не работает для много строчных сигнатур, если в файле они
    по другому разбиты на строки.
    Нужен новый алгоритм.

    Сигнатура, которую мы ищем, всегда задаётся в одну строку.

    Итак.
    1) Очередь пуста. Тупо кладём элемент
    2) Сигнатурная строка, сформированная из очереди, короче искомой - значит, она не может начинаться с искомой - тупо добавляем туда очередную строку (п.1 является частным случаем п.2)
    3) Сигнатурная строка (СС), сформированная из очереди, равна по длине, или длиннее
       Пока текущая СС больше искомой:
         проверяем, начинается ли она с искомой
           Если начинается, то количество элементов в очереди надо вернуть вместе с результатом.
           Если не начинается, то удаляем элемент с начала очереди


 */
// #define UMBA_MD_FIND_TEXT_SIGNATURE_IN_LINES_OLD_VERSION



#if !defined(UMBA_MD_FIND_TEXT_SIGNATURE_IN_LINES_OLD_VERSION)

//----------------------------------------------------------------------------
inline
std::size_t findTextSignatureInLines(const umba::md::CodeOptions &langOpts, const std::vector<std::string> &lines, const TextSignature &ts, std::size_t &foundSignatureNumLines, std::size_t startLine /* =(std::size_t)-1 */ )
{
    if (startLine==(std::size_t)-1)
        startLine = 0;

    if (startLine>=lines.size())
        return (std::size_t)-1;

    if (ts.normalizedSignature.empty()) // Пустые сигнатуры не ищем
        return (std::size_t)-1;

    std::size_t curLineIdx = startLine;

    std::deque<std::string>  curTestLinesQue;
    auto makeSingleTestLineFromDeque = [&]()
    {
        std::string res;
        for(const auto &l: curTestLinesQue)
            res.append(l);
        return res;
    };

    auto findSignatureStart = [&](std::size_t foundStartIdx) -> std::size_t
    {
        // std::size_t foundStartIdx = curLineIdx - curTestLinesQue.size();
        if (foundStartIdx==0)
            return foundStartIdx;

        auto allStopPrefixes = langOpts.getAllStopPrefixes();
        auto sepStr          = langOpts.getSatementSeparator();
        auto blockEndStr     = langOpts.getBlockEndString();

        std::size_t testIdx = foundStartIdx - 1u;
        for(; testIdx!=0; --testIdx, ++foundSignatureNumLines)
        {
            const auto &curLine = lines[testIdx];
            if (testLineIsBlockBackwardStopLine(curLine, allStopPrefixes, sepStr, blockEndStr))
                return testIdx+1u;
        }

        return 0u;
    };


    // curLineIdx указывает на строку, следующую за нашей последней сигнатурной
    for(; curLineIdx!=lines.size(); ++curLineIdx)
    {
        std::string curSignature = makeSingleTestLineFromDeque();
        // if (curSignature.size()<ts.normalizedSignature.size())
        // {
        //     curTestLinesQue.emplace_back(normalizeSignatureLine(lines[curLineIdx]));
        //     continue;
        // }

        while(curSignature.size()>=ts.normalizedSignature.size())
        {
            if (umba::string_plus::starts_with(curSignature, ts.normalizedSignature))
            {
                foundSignatureNumLines = curTestLinesQue.size();
                return findSignatureStart(curLineIdx - curTestLinesQue.size());
            }

            curTestLinesQue.pop_front();
            while(!curTestLinesQue.empty() && curTestLinesQue.front().empty())
                curTestLinesQue.pop_front();

            curSignature = makeSingleTestLineFromDeque();
        }

        curTestLinesQue.emplace_back(normalizeSignatureLine(lines[curLineIdx]));
    }

    std::string curSignature = makeSingleTestLineFromDeque();
    if (umba::string_plus::starts_with(curSignature, ts.normalizedSignature))
    {
        foundSignatureNumLines = curTestLinesQue.size();
        return findSignatureStart(curLineIdx - curTestLinesQue.size());

        // auto allStopPrefixes = langOpts.getAllStopPrefixes();
        // auto sepStr          = langOpts.getSatementSeparator();
        // auto blockEndStr     = langOpts.getBlockEndString();
        //  
        // std::size_t foundStartIdx = curLineIdx - curTestLinesQue.size();
        // if (foundStartIdx==0)
        //     return foundStartIdx;
        //  
        // std::size_t testIdx = foundStartIdx - 1u;
        // for(; testIdx!=0; --testIdx, ++foundSignatureNumLines)
        // {
        //     if (testLineIsBlockBackwardStopLine(lines[testIdx], allStopPrefixes, sepStr, blockEndStr))
        //         return testIdx+1u;
        // }
        //  
        // return 0u;
    }

    return (std::size_t)-1;
}

//----------------------------------------------------------------------------
//! Возвращает номер первой строки последней сигнатуры в тексте, или (std::size_t)-1
template<typename VectorType> inline
std::size_t findTextSignaturePathInLines(const umba::md::CodeOptions& langOpts, const std::vector<std::string> &lines, const VectorType &signaturesVec, std::size_t &foundSignatureNumLines, std::size_t startLine /* =(std::size_t)-1 */ )
{
    if (signaturesVec.empty()) // По пустому списку сигнатур не ищем
        return (std::size_t)-1;

    //std::size_t lastSignatueLinesNumber = 0;
    for(const auto &signature : signaturesVec)
    {
        startLine = findTextSignatureInLines(langOpts, lines, signature, foundSignatureNumLines, startLine);
        if (startLine==(std::size_t)-1)
        {
            foundSignatureNumLines = 0;
            return startLine;
        }
        //lastSignatueLinesNumber = signature.signatureLinesVector.size();
        startLine += foundSignatureNumLines; // Пропускаем найденную сигнатуру целиком
    }

    return startLine - foundSignatureNumLines; // Делаем откат на размер последней сигнатуры в строках
}

//----------------------------------------------------------------------------

#else

//----------------------------------------------------------------------------
inline
std::size_t findTextSignatureInLines(const std::vector<std::string> &lines, const TextSignature &ts, std::size_t startLine=(std::size_t)-1)
{
    if (startLine==(std::size_t)-1)
        startLine = 0;

    if (startLine>=lines.size())
        return (std::size_t)-1;

    const std::size_t numSignatureLines = ts.signatureLinesVector.size();

    if (numSignatureLines==0 || ts.normalizedSignature.empty())
        return (std::size_t)-1;


    std::size_t curLineIdx = startLine;

    std::deque<std::string>  curTestLines;
    auto makeSingleTestLineFromDeque = [&]()
    {
        std::string res;
        for(const auto l: curTestLines)
        {
            res.append(l);
        }

        return res;
    };

    // initial fill deque
    for(std::size_t i=0; i!=numSignatureLines && curLineIdx!=lines.size(); ++i, ++curLineIdx )
    {
        curTestLines.emplace_back(normalizeSignatureLine(lines[curLineIdx]));
    }

    if (curTestLines.size()!=numSignatureLines)
        return (std::size_t)-1;

    // curLineIdx указывает на строку, следующую за нашей последней сигнатурной
    for(; curLineIdx!=lines.size(); ++curLineIdx)
    {
        if (umba::string_plus::starts_with(makeSingleTestLineFromDeque(), ts.normalizedSignature))
        {
            return curLineIdx-numSignatureLines;
        }

        curTestLines.pop_front();
        curTestLines.emplace_back(normalizeSignatureLine(lines[curLineIdx]));
    }

    // На последней итерации запихали последнюю строку в сигнатуную очередь, но в тело цикла уже не попали и не проверили, что там получилось
    if (umba::string_plus::starts_with(makeSingleTestLineFromDeque(), ts.normalizedSignature))
    {
        return curLineIdx-numSignatureLines;
    }

    return (std::size_t)-1;
}

//----------------------------------------------------------------------------
//! Возвращает номер первой строки последней сигнатуры в тексте, или (std::size_t)-1
template<typename VectorType> inline
std::size_t findTextSignatureInLines(const std::vector<std::string> &lines, const VectorType &signaturesVec, std::size_t startLine=(std::size_t)-1)
{
    std::size_t lastSignatueLinesNumber = 0;
    for(const auto &signature : signaturesVec)
    {
        startLine = findTextSignatureInLines(lines, signature, startLine);
        if (startLine==(std::size_t)-1)
            return startLine;

        lastSignatueLinesNumber = signature.signatureLinesVector.size();
        startLine += lastSignatueLinesNumber; // Пропускаем найденную сигнатуру целиком
    }

    return startLine-lastSignatueLinesNumber; // Делаем откат на размер последней сигнатуры в строках
}

#endif
//----------------------------------------------------------------------------





//----------------------------------------------------------------------------

} // namespace md
} // namespace umba



