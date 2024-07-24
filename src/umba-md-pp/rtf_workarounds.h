#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"

//
#include "enums.h"


//----------------------------------------------------------------------------
inline
char rtfHalfByteToHex(std::uint8_t hb)
{
    hb &= 0x0Fu;
    if (hb<10)
        return '0'+hb;
    //return 'A'+hb;
    return 'a'+hb-10;
}

//----------------------------------------------------------------------------
inline
bool rtfReadBinaryFileToHexString(const std::string &imgFilename, std::string &hexData)
{
    std::vector<std::uint8_t> fileData;
    if (!umba::filesys::readFile(imgFilename, fileData))
        return false;

    hexData.clear();
    hexData.reserve(fileData.size());

    for(auto b: fileData)
    {
        hexData.append(1, rtfHalfByteToHex(b>>4));
        hexData.append(1, rtfHalfByteToHex(b));

    }

    return true;
}

//----------------------------------------------------------------------------
inline
std::string rtfMakePictRtf(const std::string &hexData, GraphVizTargetFormat fmt)
{
    std::string pictRtfData;
    pictRtfData += "{\\pict ";
    if (fmt==GraphVizTargetFormat::emf)
        pictRtfData += "\\emfblip";
    else if (fmt==GraphVizTargetFormat::png)
        pictRtfData += "\\pngblip";
    else if (fmt==GraphVizTargetFormat::jpg)
        pictRtfData += "\\jpegblip";
    else if (fmt==GraphVizTargetFormat::bmp)
        pictRtfData += "\\dibitmap";
    else return std::string(); // Ошибка

    pictRtfData += " ";
    pictRtfData += hexData;
    pictRtfData += "}"; // close \pict


    // {\*\shppict {\pict \emfblip ..... }}{\nonshppict {\pict ....}}
    std::string rtfData;
    rtfData += "{\\*\\shppict " + pictRtfData + "}";
    rtfData += "{\\nonshppict " + pictRtfData + "}";

    return rtfData;

}

//----------------------------------------------------------------------------
inline
std::string rtfReadAndGenerateEmbedRtfWithImage(const std::string &rtfPath, const std::string &imgFilename)
{
    std::string ext = umba::filename::getExt(imgFilename);

    GraphVizTargetFormat fmt = enum_deserialize(ext, GraphVizTargetFormat::invalid);
    if (fmt!=GraphVizTargetFormat::emf && fmt!=GraphVizTargetFormat::png && fmt!=GraphVizTargetFormat::jpg && fmt!=GraphVizTargetFormat::bmp)
        return std::string();

    std::string hexData;
    if (!rtfReadBinaryFileToHexString(umba::filename::appendPath(rtfPath, imgFilename), hexData))
        return std::string();

    return rtfMakePictRtf(hexData, fmt);
}

//----------------------------------------------------------------------------
inline
bool rtfEmbedImagesWorkaround(const std::string &rtfFilename)
{
    std::string rtfPath = umba::filename::getPath(rtfFilename);

    std::string rtfData;
    if (!umba::filesys::readFile(rtfFilename, rtfData))
    {
        return false;
    }

    // {\*\fldinst  INCLUDEPICTURE "doc_icons_todo-list-50.png" \\d \\*MERGEFORMAT}
    static const std::string textToFind = "{\\*\\fldinst  INCLUDEPICTURE ";
    std::size_t startPos = 0;

    std::size_t findPos = rtfData.find(textToFind);

    while(findPos!=rtfData.npos)
    {
        
        std::size_t fieldPos = findPos;
        #ifndef NDEBUG
        const char *rtfDataFieldPtr = &rtfData[fieldPos];
        #endif

        if (fieldPos>0)
        {
            --fieldPos;
            #ifndef NDEBUG
            --rtfDataFieldPtr;
            #endif
        }

        while(fieldPos!=0 && rtfData[fieldPos]!='{')
        {
            --fieldPos;
            #ifndef NDEBUG
            --rtfDataFieldPtr;
            #endif
        }


        std::size_t fnamePos = findPos+textToFind.size();
        char charToStop = ' ';

        if (rtfData[fnamePos]=='\"')
        {
            ++fnamePos;
            charToStop = '\"';
        }

        std::size_t fnameEndPos = fnamePos;
        while(fnameEndPos!=rtfData.size() && rtfData[fnameEndPos]!=charToStop) ++fnameEndPos;

        auto imgFilename = std::string(rtfData, fnamePos, fnameEndPos-fnamePos);


        while(fnameEndPos!=rtfData.size() && rtfData[fnameEndPos]!='}') ++fnameEndPos;
        if(fnameEndPos!=rtfData.size())
           ++fnameEndPos; // skip closing '}'


        std::size_t fieldEndPos = fnameEndPos;
        #ifndef NDEBUG
        const char *rtfDataFieldEndPtr = &rtfData[fieldEndPos];
        #endif

        while(fieldEndPos!=rtfData.size() && rtfData[fieldEndPos]!='}')
        {
            ++fieldEndPos;
            #ifndef NDEBUG
            ++rtfDataFieldEndPtr;
            #endif
        }

        if(fieldEndPos!=rtfData.size() && rtfData[fieldEndPos]=='}')
        {
            ++fieldEndPos;
            #ifndef NDEBUG
            ++rtfDataFieldEndPtr;
            #endif
        }

        if(fieldEndPos!=rtfData.size())
        {
            ++fieldEndPos;
            #ifndef NDEBUG
            ++rtfDataFieldEndPtr;
            #endif
        }


        std::string rtfForReplace = rtfReadAndGenerateEmbedRtfWithImage(rtfPath, imgFilename);
        if (rtfForReplace.empty())
        {
            findPos = rtfData.find(textToFind, fieldEndPos);
        }
        else
        {
            //rtfData.erase(findPos, fnameEndPos-findPos);
            rtfData.replace(fieldPos, fieldEndPos-fieldPos, rtfForReplace);
            findPos = rtfData.find(textToFind, findPos+rtfForReplace.size());
        }

    }

    return umba::filesys::writeFile(rtfFilename, rtfData.data(), rtfData.size(), true /* bOverwrite */);
}

