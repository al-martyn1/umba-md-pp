template<>        // for the specialized A
template<>        // for the specialized B
template<class T> // for the unspecialized mf1
void A<int>::B<double>::mf1(T t) {}

//----------------------------------------------------------------------------

template< typename CharType = char, // Тип символа
typename StringType = std::string<CharType, std::char_traits<CharType>, std::allocator<CharType> > //!< Тип строки
>
inline
std::vector< std::pair<ConditionOperators, StringType> > 
makeConditionOperatorsInfoVec( void (__stdcall *)(const std::string<char> &) handler
, StringType str, const std::string<char> &str2 = std::string("aaa") /* AAA */ 
) //! Создаёт вектор условных операторов
const = 0
{
    const auto &condInfoVec = getConditionOperatorsInfoVec();
    for(const auto &condInfo : condInfoVec)
    {
        auto pos = str.find(condInfo.second);
        if (pos!=str.npos)
        {
            left .assign(str, 0, pos);
            return condInfo.first;
        }
    }

    return ConditionOperators::unknown;
}

// linux'овый прототип функции
// есть ключевые слова после символа указателя - не всё так просто, надо фиксить разбор аргумента
inline
int select(int nfds, fd_set *_Nullable restrict readfds,
                  fd_set *_Nullable restrict writefds,
                  fd_set *_Nullable restrict exceptfds,
                  struct timeval *_Nullable restrict timeout)
{
    return nfds;
}

HANDLE CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                   LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                   DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
                   HANDLE hTemplateFile
                 );

BOOL CloseHandle(
  HANDLE hObject
);

DWORD GetLastError();

