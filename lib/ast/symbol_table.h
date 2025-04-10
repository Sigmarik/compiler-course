#pragma once

#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

using STEntryId = uint64_t;
using STContext = std::vector<std::string>;

template <class AssociatedData>
class SymbolTable {
   public:
    STEntryId getOrRegister(const STContext& context, const std::string& name,
                            AssociatedData&& data);

    STEntryId findHereOrLower(const STContext& context,
                              const std::string& name);
    STEntryId find(const STContext& context, const std::string& name);
    AssociatedData* get(const STContext& context, const std::string& name);
    AssociatedData* get(STEntryId id);

   private:
    struct Cell {
        AssociatedData data;
        STEntryId entry;
    };

    Cell* findCell(const STContext& context, const std::string& name);
    Cell* findCell(STEntryId id);

    static std::string translateContext(const STContext& context,
                                        const std::string& name);

    // F it, we are LEAKING
    std::vector<Cell*> m_entries{};
    std::map<STEntryId, Cell*> m_entry_map{};
    std::unordered_map<std::string, Cell*> m_name_map{};
};

template <class AssociatedData>
inline STEntryId SymbolTable<AssociatedData>::getOrRegister(
    const STContext& context, const std::string& name, AssociatedData&& data) {
    if (STEntryId id = find(context, name)) {
        return id;
    }

    static STEntryId s_id = 0;
    STEntryId id = ++s_id;

    std::string identifier = translateContext(context, name);

    Cell* cell = new Cell{};
    cell->data = std::move(data);
    cell->entry = id;
    m_entries.push_back(cell);
    m_entry_map[id] = m_entries.back();
    m_name_map[identifier] = m_entries.back();

    return id;
}

template <class AssociatedData>
inline STEntryId SymbolTable<AssociatedData>::findHereOrLower(
    const STContext& context, const std::string& name) {
    if (context.empty()) return find(context, name);

    STContext shrinkingContext = context;

    while (!shrinkingContext.empty()) {
        STEntryId id = find(shrinkingContext, name);
        if (id) return id;
        shrinkingContext.pop_back();
    }

    return find(shrinkingContext, name);
}

template <class AssociatedData>
inline STEntryId SymbolTable<AssociatedData>::find(const STContext& context,
                                                   const std::string& name) {
    Cell* cell = findCell(context, name);

    if (!cell) return 0;

    return cell->entry;
}

template <class AssociatedData>
inline AssociatedData* SymbolTable<AssociatedData>::get(
    const STContext& context, const std::string& name) {
    Cell* cell = findCell(context, name);

    if (!cell) return nullptr;

    return &cell->data;
}

template <class AssociatedData>
inline AssociatedData* SymbolTable<AssociatedData>::get(STEntryId id) {
    Cell* cell = findCell(id);

    if (!cell) return nullptr;

    return &cell->data;
}

template <class AssociatedData>
inline typename SymbolTable<AssociatedData>::Cell*
SymbolTable<AssociatedData>::findCell(const STContext& context,
                                      const std::string& name) {
    std::string identifier = translateContext(context, name);
    auto found = m_name_map.find(identifier);
    if (found == m_name_map.end()) {
        return nullptr;
    }

    return found->second;
}

template <class AssociatedData>
inline typename SymbolTable<AssociatedData>::Cell*
SymbolTable<AssociatedData>::findCell(STEntryId id) {
    auto found = m_entry_map.find(id);
    if (found == m_entry_map.end()) {
        return nullptr;
    }

    return found->second;
}

template <class AssociatedData>
inline std::string SymbolTable<AssociatedData>::translateContext(
    const STContext& context, const std::string& name) {
    std::string identifier = "";

    for (const std::string& part : context) {
        identifier += part;
        identifier += "::";
    }

    identifier += name;

    return identifier;
}
