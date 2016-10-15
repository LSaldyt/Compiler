#include "grammar.hpp"

namespace grammar
{

// The remaining hardcoded rules for building AST types
const unordered_map<string, StatementConstructor> Grammar::construction_map = {
        {"expression", 
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return make_shared<Expression>(Expression(symbol_groups));
            }
        },
        {"assignment",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return make_shared<Assignment>(Assignment(symbol_groups));
            }
        },
        {"functioncall",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return make_shared<FunctionCall>(FunctionCall(symbol_groups));
            }
        },
        {"value",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                auto symbols = symbol_groups[0];
                if (symbols.size() == 1)
                {
                    return symbols[0];
                }
                else
                {
                    throw named_exception("Token lambda constructor was provided multiple tokens (illegal)");
                }
            }
        },
        {"function",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return make_shared<Function>(Function(symbol_groups));
                //return make_shared<Symbol>(Symbol());
            }
        },
        {"conditional",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return std::make_shared<Conditional>(Conditional(symbol_groups));
            }
        },
        {"boolvalue",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                auto symbols = symbol_groups[0];
                if (symbols.size() == 1)
                {
                    return symbols[0];
                }
                else
                {
                    throw named_exception("Token lambda constructor was provided multiple tokens (illegal)");
                }
            }
        },
        {"boolexpression",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return make_shared<Expression>(Expression(symbol_groups));
            }
        },
        {"statement",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                auto symbols = symbol_groups[0];
                if (symbols.size() == 1)
                {
                    return symbols[0];
                }
                else
                {
                    throw named_exception("Statement lambda constructor was provided multiple tokens (illegal)");
                }
            }
        },
        {"function",
            [](vector<vector<shared_ptr<Symbol>>> symbol_groups)
            {
                return make_shared<Symbol>(Symbol());
            }
        }
   };

Grammar::Grammar(vector<string> filenames, string directory)
{
    for (auto filename : filenames)
    {
        grammar_map[filename] = read(directory + filename);
    }
}

vector<shared_ptr<Symbol>> Grammar::constructFrom(SymbolicTokens& tokens)
{
    vector<shared_ptr<Symbol>> symbols;

    while (tokens.size() > 0)
    {
        auto result = identify(tokens);
        print("Identified tokens as: " + get<0>(result));
        for (auto sub_result : get<1>(result))
        {
            for (auto t : sub_result.consumed)
            {
                print(t.value->representation());
            }
        }
        auto constructed = construct(get<0>(result), get<1>(result)); 
        symbols.push_back(constructed);
    }

    return symbols;
}


SymbolicTokenParsers Grammar::readGrammarPairs(vector<string>& terms)
{
    SymbolicTokenParsers parsers;

    if (terms.size() % 2 != 0)
    {
        throw named_exception("Could not read type pairs");
    }
    for (int i = 0; i < (terms.size() / 2); i++)
    {
        int x = i * 2;
        vector<string> pair(terms.begin() + x, terms.begin() + x + 2);
        parsers.push_back(readGrammarTerms(pair));
    }

    return parsers;
}

SymbolicTokenParser Grammar::readGrammarTerms(vector<string>& terms)
{
    SymbolicTokenParser parser;

    if (terms.size() == 2)
    {
        auto first = terms[0];
        bool keep  = true;
        // If first of pair starts with !, discard its parse result
        if (first[0] == '!')
        {
            first = string(first.begin() + 1, first.end());
            keep = false;
        }
        // Allow linking to other grammar files
        if (first == "link")
        {
            parser = retrieveGrammar(terms[1]);
        }
        // Parse by type only
        else if (terms[1] == "wildcard")
        {
            parser = typeParser(first);
        }
        // Parse by a specific subtype (ex "keyword return")
        else
        {
            if (first == "keyword")
            {
                keywords.push_back(terms[1]);
            }
            parser = dualTypeParser(first, terms[1]);
        }

        // Take care of a "!" if it was found early - make the parser discard its result
        if (not keep)
        {
            parser = discard(parser);
        }
    }
    else if (terms.size() > 2)
    {
        const auto keyword = terms[0];
        terms = vector<string>(terms.begin() + 1, terms.end());

        // Repeatedly parse a parser!
        if (keyword == "many")
        {
            parser = manySeperated(readGrammarTerms(terms)); 
        }
        // Optionally parse a parser
        else if (keyword == "optional")
        {
            parser = optional<SymbolicToken>(readGrammarTerms(terms));
        }
        // Run several parsers in order, failing if any of them fail
        else if (keyword == "inOrder")
        {
            parser = inOrder<SymbolicToken>(readGrammarPairs(terms));
        }
        // Choose from several parsers
        else if (keyword == "anyOf")
        {
            parser = anyOf<SymbolicToken>(readGrammarPairs(terms));
        }
        else
        {
            throw named_exception("Expected keyword");
        }
    }
    else
    {
        throw named_exception("Grammar file incorrectly formatted");
    }

    return parser;
}

tuple<SymbolicTokenParsers, vector<int>> Grammar::read(string filename)
{
    SymbolicTokenParsers parsers;
    auto content = readFile(filename);
    auto construct_line = content.back();
    content = slice(content, 0, -1);
    
    for (auto line : content)
    {
        auto terms = lex::seperate(line, {make_tuple(" ", false)});
        parsers.push_back(readGrammarTerms(terms));
    }

    vector<int> construct_indices;
    auto construct_terms = lex::seperate(construct_line, {make_tuple(" ", false)});
    for (auto t : construct_terms)
    {
        if (t == "sep")
        {
            construct_indices.push_back(-1); // Signal for seperator
        }
        else
        {
            construct_indices.push_back(stoi(t));
        }
    }

    return make_tuple(parsers, construct_indices);
}

SymbolicTokenParser Grammar::retrieveGrammar(string filename)
{
    SymbolicTokenParser grammar_parser = [filename, this](SymbolicTokens tokens)
    {
        std::vector<SymbolicTokenParser> parsers;

        auto search = grammar_map.find(filename);
        if (search != grammar_map.end())
        {
            parsers = std::get<0>(search->second);
        }
        else
        {
            throw named_exception(filename + " is not an element of the grammar map");
        }

        SymbolicTokens tokens_copy(tokens);
        auto result = evaluateGrammar(parsers, tokens_copy);

        if (get<0>(result))
        {
            auto constructed = construct(filename, std::get<1>(result));
            print("Built link to " + filename);
            auto consumed = std::vector<SymbolicToken>(1, SymbolicToken(constructed, filename, filename));
            return Result<SymbolicToken>(true, consumed, tokens_copy); 
        }
        else
        {
            return Result<SymbolicToken>(false, {}, tokens);
        }
    };
    return grammar_parser;
}


tuple<string, vector<Result<SymbolicToken>>> 
Grammar::identify
(SymbolicTokens& tokens)
{
    SymbolicTokens tokens_copy(tokens);

    vector<string> keys;
    keys.reserve(grammar_map.size());
    for (auto kv : grammar_map)
    {
        keys.push_back(kv.first);
    }

    // Sort keys by the lengths of the parsers they refer to
    sortBy(keys, [this] (auto a, auto b) 
                 {
                     auto a_len = get<0>(grammar_map[a]).size();
                     auto b_len = get<0>(grammar_map[b]).size();
                     return a_len > b_len; 
                 });

    for (auto key : keys)
    {
        print("Attempting to identify as: " + key);

        auto value   = grammar_map[key];
        auto parsers = get<0>(value);
        auto result  = evaluateGrammar(parsers, tokens_copy);

        if (get<0>(result))
        {
            tokens = tokens_copy; // Apply our changes once we know the tokens were positively identified
            return make_tuple(key, get<1>(result));
        }
        else
        {
            tokens_copy = tokens;
        }
    }

    throw named_exception("Could not identify tokens");
}

tuple<bool, vector<Result<SymbolicToken>>> 
Grammar::evaluateGrammar
(SymbolicTokenParsers parsers, SymbolicTokens& tokens)
{
    vector<Result<SymbolicToken>> results;

    int i = 0;
    for (auto parser : parsers)
    {
        /*
        print("Parsing parser ", i ," against:");
        for (auto t : tokens)
        {
            print(t.value->representation());
        }
        */
        auto result = parser(tokens);
        if (result.result)
        {
            tokens = result.remaining;
            results.push_back(result);
        }
        else
        {
            /*
            print("Failed on parser ", i ,". Remaining ", tokens.size(), " tokens were: ");
            for (auto t : tokens)
            {
                print(t.value->representation());
            }
            */
            return make_tuple(false, results);
        }
        i++;
    }

    return make_tuple(true, results);
};


vector<shared_ptr<Symbol>> fromTokens(vector<SymbolicToken> tokens)
{
    vector<shared_ptr<Symbol>> symbols;
    symbols.reserve(tokens.size());

    for (auto t : tokens)
    {
        symbols.push_back(t.value);
    }

    return symbols;
}

shared_ptr<Symbol> Grammar::build(string name, vector<vector<shared_ptr<Symbol>>> symbol_groups)
{
    StatementConstructor constructor;
    auto it = Grammar::construction_map.find(name);
    if (it != Grammar::construction_map.end())
    {
        constructor = it->second;
    }
    else
    {
        throw named_exception(name + " is not an element of the construction map");
    }

    auto constructed = constructor(symbol_groups);
    return constructed;
}

shared_ptr<Symbol> Grammar::construct(string name, vector<Result<SymbolicToken>> results)
{
    print("Constructing " + name);
    auto construction_indices = get<1>(grammar_map[name]);

    vector<shared_ptr<Symbol>> result_symbols;

    vector<vector<shared_ptr<Symbol>>> groups;
    groups.push_back(vector<shared_ptr<Symbol>>());

    for (auto i : construction_indices)
    {
        if (i == -1)
        {
            groups.push_back(vector<shared_ptr<Symbol>>());
        }
        else
        {
            auto result = results[i];
            result.consumed = clean(result.consumed); // Discard tokens that have been marked as unneeded

            auto grouped_tokens = reSeperate(result.consumed);
            for (auto group : grouped_tokens)
            {
                for (auto t : group)
                {
                    groups.back().push_back(t.value);
                }
            }
        }
    }

    auto constructed = build(name, groups);
    return constructed; 
}

}
