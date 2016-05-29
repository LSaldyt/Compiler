#pragma once
#include "../Syntax/Statements.hpp"
#include "../Parse/Parse.hpp"

namespace Gen
{
    using namespace Syntax;
    using SymbolicTokenParser      = std::function<TokenResult<SymbolicToken>(SymbolicTokens)>;
    using SymbolicTokenParsers     = std::vector<SymbolicTokenParser>;
    using SymbolicStatementParser  = std::tuple<SymbolicTokenParsers, StatementGenerator>;
    using SymbolicStatementParsers = std::vector<SymbolicStatementParser>;
}
