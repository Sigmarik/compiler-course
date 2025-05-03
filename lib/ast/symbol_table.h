#pragma once

#include <map>

using ConstValueType = std::variant<int, bool>;

enum class DataType {
    Int = 0,
    Bool,
};

using SymbolId = uint64_t;

template <class AssignedData>
using SymbolTable = std::map<SymbolId, AssignedData>;
