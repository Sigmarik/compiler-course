#pragma once

#include <memory>
#include <string>
#include <unordered_map>

#include "ast.hpp"

using SymbolTable = std::
    unordered_map<std::string, std::unique_ptr<ExprValueType>>;
