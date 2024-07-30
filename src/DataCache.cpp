#include <matjson.hpp>
#include <unordered_map>
#include <chrono>
#include <any>
#include "DataCache.hpp"

template<>
struct matjson::Serialize<CacheEntry> {
    static CacheEntry from_json(const Value& value) {
        return CacheEntry {
            .data = value["data"].as<Value>(),
            .expiration = value["expiration"].as<long long>()
        };
    }

    static Value to_json(const CacheEntry& value) {
        auto obj = Object();
        obj["data"] = value.data;
        obj["expiration"] = value.expiration;
        return obj;
    }

    static bool is_json(Value const& value) {
        return value.is_object();
    }
};

template<>
struct matjson::Serialize<std::unordered_map<int, CacheEntry>> {
    static std::unordered_map<int, CacheEntry> from_json(const Value& value) {
        std::unordered_map<int, CacheEntry> map;
        for (const auto& [key, val] : value.as<Object>()) {
            map[std::stoi(key)] = val.as<CacheEntry>();
        }
        return map;
    }

    static Value to_json(const std::unordered_map<int, CacheEntry>& value) {
        auto obj = Object();
        for (const auto& [key, val] : value) {
            obj[std::to_string(key)] = Serialize<CacheEntry>::to_json(val);
        }
        return obj;
    }

    static bool is_json(Value const& value) {
        return value.is_object();
    }
};

DataCache* DataCache::instance = nullptr;

DataCache::DataCache() {
    std::unordered_map<int, CacheEntry> cache{};
}

void DataCache::init() {
    cache = Mod::get()->getSavedValue<std::unordered_map<int, CacheEntry>>("cache");
}

void DataCache::store(const int& id, const matjson::Value& data, const long long& expiration) {
    const CacheEntry array = {data, expiration};

    cache[id] = array;
    Mod::get()->setSavedValue("cache", cache);
}

matjson::Value DataCache::retrieve(const int& id) {
    if (const auto it = cache.find(id); it != cache.end()) {
        const auto data = it->second;
        const auto currentTimestamp = getCurrentUnixTimestamp();

        if (currentTimestamp > data.expiration) {
            cache.erase(id);
            Mod::get()->setSavedValue("cache", cache);
            return nullptr;
        }

        return data.data;
    }
    return nullptr;
}

long long DataCache::getCurrentUnixTimestamp() {
    const auto now = std::chrono::system_clock::now();
    const auto duration = now.time_since_epoch();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
    return seconds;
}