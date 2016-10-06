#include "lex.hpp"

namespace lex
{
    // Convert a sentence to a vector of tokens based off of a defined language
    Tokens lexWith(const string& sentence, const Language& language)
    {
        // Seperate sentence..
        auto terms  = seperate(sentence, language.seperators);

        print("Seperated terms:");
        for (auto t : terms)
        {
            print(t);
        }
        print("Done"); 

        auto tokens = Tokens(); // Empty vector of tokens

        // Do the actual identification here
        while (terms.size() > 0)
        {
            auto result = language.identify(terms);
            tokens.push_back(get<0>(result));
            terms = get<1>(result);
        }

        return tokens;
    }
}
