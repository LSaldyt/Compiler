#include "function.hpp"

namespace syntax
{
    Function::Function() 
    {
    }
    Function::Function(vector<vector<shared_ptr<Symbol>>> symbol_groups)
    {
        print(symbol_groups.size());
        identifier = symbol_groups[0][0]->name();
        args = symbol_groups[1];
        body = symbol_groups[2];
    }
    string Function::name()
    {
        return "*" + identifier;
    }
    string Function::source(unordered_set<string>& names, string n_space)
    {
        string arglist = commaSep(args, names, n_space, "auto ");

        string body_source = "";
        for (auto statement : generate(body))
        {
            body_source += statement;
        }

        string function_source = "";
        if (identifier == "main")
        {
            function_source += "int ";
        }
        else
        {
            function_source += "auto ";
        }
        function_source += (n_space + identifier + "(" + arglist + ")");
        function_source += "\n{\n" + body_source + "\n}";
        return function_source;
    }

    string Function::header(unordered_set<string>& names, string n_space)
    {
        if (identifier == "main")
        {
            return "// No main declaration required";
        }
        string arglist = commaSepH(args, names, n_space, "auto ");

        string function_source = "";
        function_source += "auto ";
        function_source += (n_space + identifier + "(" + arglist + ")");
        return function_source;
    }
}
