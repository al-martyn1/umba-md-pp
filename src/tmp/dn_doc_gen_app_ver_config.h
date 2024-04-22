#ifndef UMBA_2RCFS
std::string appFullName   = "DotNutDocGen";
#else
std::string appFullName   = "DotNutDocGen";
#endif

std::string appVersion    = "1.01";
std::string appCommitHash;  //UNDONE
std::string appBuildDate  = __DATE__;
std::string appBuildTime  = __TIME__;

const char *appHomeUrl    = "https://github.com/al-martyn1/";
const char *appistrPath   = "";

#if defined(WIN32) || defined(_WIN32)
    const char *appSubPath    = "bin/dn_doc_gen.exe";
#else
    const char *appSubPath    = "bin/dn_doc_gen";
#endif
