#pragma once

#include "code-options-database.h"

#include <vector>
#include <string>


// umba::md::
namespace umba {
namespace md {



//----------------------------------------------------------------------------
inline
std::string normalizeSignatureLine(const std::string &str)
{
    std::string res; res.reserve();

    for(auto ch : str)
    {
        std::uint8_t uch = (std::uint8_t)ch;
        if (uch>' ' && uch!=0x7Fu)
            res.append(1, ch);
    }

    return res;
}



//----------------------------------------------------------------------------
struct TextSignature
{
    // using options_type  = umba::container::small_vector_options< umba::container::growth_factor<umba::container::growth_factor_50>, umba::container::inplace_alignment<16> >::type;
    // //using signature_lines_vector_type = umba::container::small_vector<std::string, 4, void, umba::container::small_vector_option_inplace_alignment_16_t, umba::container::small_vector_option_growth_50_t >;
    // using signature_lines_vector_type = umba::container::small_vector<std::string, 4, void, options_type >;
    //
    //
    // signature_lines_vector_type    signatureLinesVector; // normalized or original? Чтобы при отладке знать, какой был оригинал, и заодно имеем тут число строк в искомой сигнатуре
    std::string                    normalizedSignature ;

    TextSignature() = default;
    TextSignature(const TextSignature &) = default;
    TextSignature& operator=(const TextSignature &) = default;
    TextSignature(TextSignature &&) = default;
    TextSignature& operator=(TextSignature &&) = default;

    explicit TextSignature(const std::string &signature)
        // : signatureLinesVector()
        // , normalizedSignature()
        : normalizedSignature(normalizeSignatureLine(marty_cpp::cUnescapeString(signature)))
    {
        // auto unescaped      = marty_cpp::cUnescapeString(signature);
        // normalizedSignature = normalizeSignatureLine(unescaped);
        // umba::string_plus::simple_string_split(std::back_inserter(signatureLinesVector), unescaped, std::string("\n") /* , nSplits = -1 */ );
    }

    template<typename IteratorType>
    explicit TextSignature(IteratorType b, IteratorType e)
        // : signatureLinesVector()
        // , normalizedSignature()
        : normalizedSignature(normalizeSignatureLine(marty_cpp::cUnescapeString(std::string(b,e))))
    {
        // auto unescaped      = marty_cpp::cUnescapeString(std::string(b,e));
        // normalizedSignature = normalizeSignatureLine(unescaped);
        // umba::string_plus::simple_string_split(std::back_inserter(signatureLinesVector), unescaped, std::string("\n") /* , nSplits = -1 */ );
    }

    void clear()
    {
        // signatureLinesVector.clear();
        normalizedSignature .clear();
    }

    bool empty() const
    {
        return normalizedSignature.empty();
    }

}; // struct TextSignature







// Iterator parseSnippetTagFirstPart(Iterator b, Iterator e, SnippetTagInfo &parseToSnippetTagInfo)


//----------------------------------------------------------------------------

} // namespace md
} // namespace umba



