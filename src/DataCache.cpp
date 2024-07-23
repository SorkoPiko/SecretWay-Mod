#include <matjson.hpp>
#include <unordered_map>
#include <chrono>
#include <any>
#include "DataCache.hpp"

DataCache::DataCache() {
    std::unordered_map<int, std::vector<std::any>> cache{};
}

void DataCache::store(const int& id, const matjson::Value& data, const long long& timestamp, const long& expiration) {
    const std::vector<std::any> array = {data, timestamp, expiration};

    cache[id] = array;
}

matjson::Value DataCache::retrieve(const int& id) {
    if (const auto it = cache.find(id); it != cache.end()) {
        const auto data = it->second;
        const auto timestamp = std::any_cast<long>(data[1]);
        const auto expiration = std::any_cast<long>(data[2]);
        const auto currentTimestamp = getCurrentUnixTimestamp();

        if (currentTimestamp > timestamp + expiration) {
            cache.erase(id);
            return nullptr;
        }

        return std::any_cast<matjson::Value>(data[0]);
    }
    return nullptr;
}

long long DataCache::getCurrentUnixTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return seconds;
}