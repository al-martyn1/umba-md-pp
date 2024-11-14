#if !defined(UMBA_MD_PP_VIEW)
    std::string appFullName   = "Umba Markdown Preprocessor";
#else
    std::string appFullName   = "Umba MDPP Markdown Viewer";
#endif

std::string appVersion    = "1.0";
std::string appCommitHash;  //UNDONE
std::string appBuildDate  = __DATE__;
std::string appBuildTime  = __TIME__;

const char *appHomeUrl    = "https://github.com/al-martyn1/umba-md-pp";
const char *appistrPath   = "";


#if !defined(UMBA_MD_PP_VIEW)
    const char *appSubPath    = "bin/umba-md-pp" UMBA_FILESYS_EXE_EXT;
#else
    const char *appSubPath    = "bin/umba-md-pp-view" UMBA_FILESYS_EXE_EXT;
#endif
