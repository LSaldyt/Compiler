#pragma once
#include <vector>
#include <string>
#include <functional>

namespace Parse
{
    using Comparator = std::function<bool(const std::string&)>;
    using Terms      = std::vector<std::string>;
    using Term       = std::string;
}