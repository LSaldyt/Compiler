#pragma once
#include <tuple>
#include <vector>
#include <string>
#include <unordered_map>
#include "run.hpp"

namespace Grammar
{


class Grammar
{
public:
    Grammar(std::vector<std::string> grammar_files, std::string directory);
    GrammarMap grammar_map; 

private:
    std::tuple<SymbolicTokenParsers, std::vector<int>> read(std::string filename);
    SymbolicTokenParsers readGrammarPairs(std::vector<std::string>& terms);
    SymbolicTokenParser  readGrammarTerms(std::vector<std::string>& terms);
    SymbolicTokenParser  retrieveGrammar(std::string filename); 
};

}
