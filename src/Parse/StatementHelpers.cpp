#include "StatementHelpers.hpp"

namespace Parse
{
    // Parse a symbolic token, but discard it (throwing an exeption if it doesn't parse)
    void advance(SymbolicTokenParser function, SymbolicTokens& tokens)
    {
        auto result = function(tokens);
        if (!result.result)
        {
            throw bad_bind("Failed to bind to advance parser"); 
        }
        else
        {
            tokens = SymbolicTokens(tokens.begin() + result.parsed.size(), tokens.end());
        }
    }

    // Helper function for catching exceptions during Statement Building
    // Will convert exception handled interface into tuple of the form (success, remaining_tokens, statement)
    StatementParser statementBuilder(std::function<std::shared_ptr<Statement>(SymbolicTokens&)> builder)
    {
       return [builder](SymbolicTokens& tokens)
       {
           try{
               auto statement = builder(tokens);
               return StatementResult(true, tokens, statement);
           }
           catch (const bad_bind& e){
               std::cout << e.s << std::endl;
               return StatementResult(false, tokens, std::make_shared<Statement>(Statement())); //Empty statement
           }
       };
    }
}