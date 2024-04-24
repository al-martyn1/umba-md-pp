#pragma once                                                // 1
                                                            // 2
// https://stackoverflow.com/questions/11697820/how-to-use-date-and-time-predefined-macros-in-as-two-integers-then-stri
                                                            // 4
//#!print-ver-all
//#!printOnlyVersion
void printOnlyVersion()                                     // 7
{                                                           // 8
    std::cout<<appVersion<<"\n";                            // 9
}                                                           // 10
//#!
                                                            // 12
//#!printNameVersion
void printNameVersion( const std::string &indent = "" )     // 14
{                                                           // 15
    std::cout<<indent << appFullName << " version ";        // 16
    printOnlyVersion();                                     // 17
    //<<rdlcVersion<<"\n";                                  // 18
}                                                           // 19
//#!printNameVersion
                                                            // 21
//#!printCommitHash
void printCommitHash( const std::string &indent = "" )      // 23
{                                                           // 24
    if (appCommitHash.empty())                              // 25
        return;                                             // 26
    std::cout<<indent<<"#"<<appCommitHash<<"\n";            // 27
}                                                           // 28
//#!
                                                            // 30
//#!printBuildDateTime
void printBuildDateTime( const std::string &indent = "" )   // 32
{                                                           // 33
    std::cout<<indent<<"Built at "<<appBuildDate<<" "<<appBuildTime<<"\n";
}                                                           // 35
//#!
                                                            // 37

