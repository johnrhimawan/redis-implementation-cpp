#pragma once
#include <unordered_map>
#include <string>
#include <optional>

namespace redis::storage {

class Storage {
public:
    void set(const std::string& key, const std::string& value) {
        _store[key] = value;
    }

    std::optional<std::string> get(const std::string& key) const {
        auto it = _store.find(key);
        if (it != _store.end()) return it->second;
        return std::nullopt;
    }

private:
    std::unordered_map<std::string, std::string> _store;
};

} // namespace redis::storage 