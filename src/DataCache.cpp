#include <matjson.hpp>
#include <unordered_map>
#include <chrono>
#include "DataCache.hpp"

DataCache::DataCache() {
    std::unordered_map<int, matjson::Value> cache{};
}

void DataCache::store(const int& id, const matjson::Value& data) {
    cache[id] = data;
}

matjson::Value DataCache::retrieve(const int& id) {
    if (const auto it = cache.find(id); it != cache.end()) {
        const auto data = it->second;
        if (const)
        return it->second;
    }
    return nullptr;
}

long DataCache::getCurrentUnixTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return seconds;
}