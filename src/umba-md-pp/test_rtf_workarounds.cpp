/*! \file
    \brief Тесты воркараундов/фиксов RTF, который сгенерил Doxygen
 */

// Должна быть первой
#include "umba/umba.h"
#include "umba/filename.h"
#include "umba/filesys.h"
//---

//#-sort
#include "umba/simple_formatter.h"
#include "umba/char_writers.h"
//#+sort

#include "umba/debug_helpers.h"
#include "umba/time_service.h"

#if defined(_WIN32) || defined(WIN32)
    #include "umba/winconhelpers.h"
#endif

#include "rtf_workarounds.h"

//
#include <iostream>
#include <string>


int safe_main(int argc, char* argv[]);

int main(int argc, char* argv[])
{
    try
    {
        return safe_main(argc, argv);
    }
    catch(const std::exception &e)
    {
        //LOG_ERR_OPT 
        std::cerr << "Exception found: " << e.what() << "\n";
    }
    catch(...)
    {
        //LOG_ERR_OPT
        std::cerr << "Exception found: " << "unknown exception" << "\n";
    }

    return 66;
}

int safe_main(int argc, char* argv[])
{
    // umba::time_service::init();
    //  
    // marty_tr::tr_set_err_handler(&trErrHandler);
    // marty_tr::tr_init_all_translations(tr_get_translations_json());
    //  

    //std::cout << "Doxygen: " << umba::toUtf8(findDoxygenExecutableName()) << "\n";

    //using namespace umba::omanip;

    std::string rtfFilename;

    if (umba::isDebuggerPresent())
    {

        #if (defined(WIN32) || defined(_WIN32))

            if (winhelpers::isProcessHasParentOneOf({"devenv"}))
            {
            }
            else if (winhelpers::isProcessHasParentOneOf({"code"}))
            {
                 rtfFilename = "C:\\work\\github\\umba-tools\\umba-md-pp\\tests\\README.rtf";
            }
            else
            {
                //rootPath = umba::filename::makeCanonical(umba::filename::appendPath<std::string>(cwd, "..\\..\\..\\"));
            }

            //#endif

            // rootPath = umba::filename::appendPathSepCopy(rootPath);

        #endif

    }
    else
    {
        if (argc>1)
        {
            rtfFilename = argv[1];
        }
    }

    if (rtfFilename.empty())
    {
        std::cerr << "Filename not taken\n";
        return 1;
    }

    std::string pathName     = umba::filename::getPathFile(rtfFilename);
    std::string ext          = umba::filename::getExt(rtfFilename);
    std::string testFilename = umba::filename::appendExt(pathName+"_2", ext);

    if (!CopyFileA( rtfFilename.c_str(), testFilename.c_str(), FALSE))
    {
        std::cerr << "Failed to copy test file\n";
        return 2;
    }

    if (!rtfEmbedImagesWorkaround(testFilename))
    {
        std::cerr << "Failed to embed images\n";
        return 3;
    }

    std::cout << "Done\n";

    return 0;
}
