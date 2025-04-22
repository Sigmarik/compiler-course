#pragma once

#include <map>

enum class DataType {
    Int = 0,
};

using SymbolId = uint64_t;

template <class AssignedData>
using SymbolTable = std::map<SymbolId, AssignedData>;
