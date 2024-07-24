#pragma once

#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"


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
    std::string rtfData;
    rtfData += "{\\*\\shppict ";
    rtfData += "{\\pict \\picscalex100\\picscaley100\\piccropl0\\piccropr0\\piccropt0\\piccropb0";

    rtfData += " ";

    if (fmt==GraphVizTargetFormat::emf)
        rtfData += "\\emfblip";
    else if (fmt==GraphVizTargetFormat::png)
        rtfData += "\\pngblip";
    else if (fmt==GraphVizTargetFormat::jpg)
        rtfData += "\\jpegblip";
    else if (fmt==GraphVizTargetFormat::bmp)
        rtfData += "\\dibitmap";
    else return std::string(); // Ошибка

    rtfData += "\\bliptag-1233012596{\\*\\blipuid b681b88c12fe00e28259b078ae901c0a}";
    //rtfData += " ";
    rtfData += hexData;
    rtfData += "}";
    rtfData += "}";

    return rtfData;

    // png       = 0x0001,
    // bmp       = 0x0002,
    // emf       = 0x0003,
    // jpg       = 0x0004,
    // jpeg      = 0x0004,
    // gif       = 0x0005

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

        std::string rtfForReplace = rtfReadAndGenerateEmbedRtfWithImage(rtfPath, imgFilename);
        if (rtfForReplace.empty())
        {
            findPos = rtfData.find(textToFind, fnameEndPos);
        }
        else
        {
            //rtfData.erase(findPos, fnameEndPos-findPos);
            rtfData.replace(findPos, fnameEndPos-findPos, rtfForReplace);
            findPos = rtfData.find(textToFind, findPos+rtfForReplace.size());
        }

    }

    return umba::filesys::writeFile(rtfFilename, rtfData.data(), rtfData.size(), true /* bOverwrite */);
}

