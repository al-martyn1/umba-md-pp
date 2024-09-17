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
    #if defined(WIN32) || defined(_WIN32)
        const char *appSubPath    = "bin/umba-md-pp.exe";
    #else
        const char *appSubPath    = "bin/umba-md-pp";
    #endif
#else
    #if defined(WIN32) || defined(_WIN32)
        const char *appSubPath    = "bin/umba-md-pp-view.exe";
    #else
        const char *appSubPath    = "bin/umba-md-pp-view";
    #endif
#endif
