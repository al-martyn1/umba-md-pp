#pragma once

#include <string>
#include <map>

//
#include "log.h"


//----------------------------------------------------------------------------
class ImageFileForCopyInfo
{
protected:

    std::string    sourceFilename;
    std::string    sourceFilenameForCompare;
    std::string    targetFilename;
    std::string    targetFilenameForCompare;

    std::string    imageLink        ; // Оригинальный линк
    std::string    imageLinkTarget  ; // Модифицированный (возможно) линк
    std::string    documentFilename ; // Файл документа, в котором  используется линк


public:

    ImageFileForCopyInfo() = default;
    ImageFileForCopyInfo(const ImageFileForCopyInfo&) = default;
    ImageFileForCopyInfo& operator=(const ImageFileForCopyInfo&) = default;
    ImageFileForCopyInfo(ImageFileForCopyInfo&&) = default;
    ImageFileForCopyInfo& operator=(ImageFileForCopyInfo&&) = default;

    void setSourceFilename(const std::string &f)
    {
        sourceFilename           = umba::filename::makeCanonical(f);
        sourceFilenameForCompare = umba::filename::makeCanonicalForCompare(f);
    }

    std::string getSourceFilename() const
    {
        return sourceFilename;
    }

    std::string getSourceFilenameForCompare() const
    {
        return sourceFilenameForCompare;
    }

    void setTargetFilename(const std::string &f)
    {
        targetFilename           = umba::filename::makeCanonical(f);
        targetFilenameForCompare = umba::filename::makeCanonicalForCompare(f);
    }

    std::string getTargetFilename() const
    {
        return targetFilename;
    }

    std::string getTargetFilenameForCompare() const
    {
        return targetFilenameForCompare;
    }

    void setImageLink(const std::string &f)
    {
        imageLink = f;
    }

    std::string getImageLink() const
    {
        return imageLink;
    }

    void setImageLinkTarget(const std::string &f)
    {
        imageLinkTarget = f;
    }

    std::string getImageLinkTarget() const
    {
        return imageLinkTarget;
    }

    void setDocumentFilename(const std::string &f)
    {
        documentFilename = f;
    }

    std::string getDocumentFilename() const
    {
        return documentFilename;
    }

    bool isSourceFilenameTheSame(const std::string &f) const
    {
        return sourceFilenameForCompare==umba::filename::makeCanonicalForCompare(f);
    }

    bool isTargetFilenameTheSame(const std::string &f) const
    {
        return targetFilenameForCompare==umba::filename::makeCanonicalForCompare(f);
    }


}; // class ImageFileForCopyInfo

//----------------------------------------------------------------------------



//----------------------------------------------------------------------------
inline
bool addImageFilesForCopying( std::map<std::string, ImageFileForCopyInfo>       &imagesToCopy
                           , const std::string                                  &sourceDocumentFilename
                           , const std::string                                  &targetDocumentFilename
                           , const std::unordered_map<std::string, std::string> &imagLinks // src -> dst
                           )
{
    std::string inputPath  = umba::filename::getPath(sourceDocumentFilename);
    std::string outputPath = umba::filename::getPath(targetDocumentFilename);

    bool res = true;

    for(const auto &imgFilePair : imagLinks)
    {
        const auto &imgSrcLink = imgFilePair.first ;
        const auto &imgTgtLink = imgFilePair.second;

        ImageFileForCopyInfo imgInfo;
        imgInfo.setSourceFilename(umba::filename::makeCanonical(umba::filename::appendPath(inputPath , imgSrcLink)));
        imgInfo.setTargetFilename(umba::filename::makeCanonical(umba::filename::appendPath(outputPath, imgTgtLink)));
        imgInfo.setImageLink(imgSrcLink);
        imgInfo.setImageLinkTarget(imgTgtLink);
        imgInfo.setDocumentFilename(sourceDocumentFilename);


        std::map<std::string, ImageFileForCopyInfo>::const_iterator imgIt = imagesToCopy.find(imgInfo.getTargetFilenameForCompare());
        if (imgIt==imagesToCopy.end())
        {
            imagesToCopy[imgInfo.getTargetFilenameForCompare()] = imgInfo;
        }
        else
        {
            if (!imgIt->second.isSourceFilenameTheSame(imgInfo.getSourceFilenameForCompare()))
            {
                // Вообще, такое не должно происходить
                LOG_WARN("img-copy-target-exist") << "target file already added for copying, but source files are different (target file: '" << imgIt->second.getTargetFilename() << "')\n";
                LOG_WARN("img-copy-target-exist") << "previosly added source file is: '" << imgIt->second.getSourceFilename() << "'\n";
                LOG_WARN("img-copy-target-exist") << "added from '" << imgIt->second.getDocumentFilename() << "' ('" << imgIt->second.getImageLink() << "')\n";
                LOG_WARN("img-copy-target-exist") << "current source file is: '" << imgInfo.getSourceFilename() << "'\n";
                LOG_WARN("img-copy-target-exist") << "added from '" << imgInfo.getDocumentFilename() << "' ('" << imgInfo.getImageLink() << "')\n";
                res = false;
            }
        }
    }

    return res;
}

//----------------------------------------------------------------------------
template<typename LogStreamType>
inline
bool copyDocumentImageFiles(LogStreamType & logStream, const std::map<std::string, ImageFileForCopyInfo> &imagesToCopy, bool bOverwrite, std::vector<std::string> *pGitAddFiles=0)
{
    if (!imagesToCopy.empty())
    {
        logStream << "Copying image files\n";
    }
    else
    {
        logStream << "Copying image files: nothing to copy\n";
    }

    bool res = true;
    std::map<std::string, ImageFileForCopyInfo>::const_iterator it = imagesToCopy.begin();
    for(; it!=imagesToCopy.end(); ++it)
    {
        const ImageFileForCopyInfo &imgInfo = it->second;
        const auto &srcFile = imgInfo.getSourceFilename();
        const auto &tgtFile = imgInfo.getTargetFilename();

        umba::filesys::createDirectoryEx( umba::filename::getPath(tgtFile), true /* forceCreatePath */ );

        logStream << "    Copying image file '" << srcFile << "' to '" << tgtFile << "'\n";

        if (!CopyFileA(srcFile.c_str(), tgtFile.c_str(), bOverwrite ? FALSE : TRUE)) // ошибка, если существует и не режим overwrite
        {
            DWORD err = GetLastError();
            if (err==ERROR_FILE_NOT_FOUND)
            {
                LOG_WARN("img-copy") << "failed to copy image file: source file '" << srcFile << "' not exist\n";
            }
            else if (err==ERROR_FILE_EXISTS)
            {
                LOG_WARN("img-copy") << "failed to copy image file: target file '" << tgtFile << "' already exist\n";
            }
            else
            {
                LOG_WARN("img-copy") << "failed to copy image file: error " << err << "\n";
            }

            res = false;
        }
        else
        {
            if (pGitAddFiles)
                pGitAddFiles->emplace_back(tgtFile);
        }

    }

    return res;
}
