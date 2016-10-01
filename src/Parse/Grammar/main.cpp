#include "construct.hpp"

int main()
{
    using namespace Grammar;

    auto grammar = Grammar::Grammar({"assignment", "expression", "value", "functioncall"}, "python/");

    SymbolicTokens tokens = {SymbolicToken(std::make_shared<Identifier>(Identifier("x")), "identifier", "identifier"),
                             SymbolicToken(std::make_shared<Operator>  (Operator("=")),   "=",          "operator"),
                             SymbolicToken(std::make_shared<Integer>   (Integer(42)),     "int",        "literal"),
                             SymbolicToken(std::make_shared<Operator>  (Operator("+")),   "+",          "operator"),
                             SymbolicToken(std::make_shared<Integer>   (Integer(7)),      "int",        "literal"),
        };

    auto run_result = run(grammar.grammar_map, tokens);
    std::cout << std::get<0>(run_result) << std::endl;

}

