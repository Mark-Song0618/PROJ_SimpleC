#include "constEntry.hh"


namespace IR
{

int ConstEntry::_cnt = 0;

void
ConstTable::addLiteral(ConstEntry* entry)
{
    if (_entries.find(entry->getLiteral()) == _entries.end())
        _entries.insert({entry->getLiteral(), entry});
}

ConstEntry*
ConstTable::getLiteral(const std::string& literal)
{
    if (_entries.find(literal) != _entries.end()) {
        return _entries[literal];
    }
    return nullptr;
}
    
std::vector<ConstEntry*>    
ConstTable::getLiterals()
{
    std::vector<ConstEntry*> rt;    
    for (auto&[_, entry] : _entries) {
        rt.push_back(entry);
    }
    return std::move(rt);
}

}
