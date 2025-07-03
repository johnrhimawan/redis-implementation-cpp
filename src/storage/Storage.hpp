#pragma once
#include <unordered_map>
#include <string>
#include <optional>
#include <chrono>

namespace redis::storage {

class Storage {
public:
    void set(const std::string& key, const std::string& value, std::optional<int64_t> px_expiry_ms = std::nullopt) {
        _store[key] = value;
        if (px_expiry_ms) {
            auto now = std::chrono::steady_clock::now();
            _expiries[key] = now + std::chrono::milliseconds(*px_expiry_ms);
        } else {
            _expiries.erase(key);
        }
    }

    std::optional<std::string> get(const std::string& key) {
        auto it = _store.find(key);
        if (it == _store.end()) return std::nullopt;
        // Check expiry
        auto exp_it = _expiries.find(key);
        if (exp_it != _expiries.end()) {
            auto now = std::chrono::steady_clock::now();
            if (now >= exp_it->second) {
                // Expired
                _store.erase(it);
                _expiries.erase(exp_it);
                return std::nullopt;
            }
        }
        return it->second;
    }

private:
    std::unordered_map<std::string, std::string> _store;
    std::unordered_map<std::string, std::chrono::steady_clock::time_point> _expiries;
};

} // namespace redis::storage 