#ifndef DATACACHE_H
#define DATACACHE_H

using namespace geode::prelude;

#include <unordered_map>
#include <matjson.hpp>
#include <any>
#include <unordered_map>

struct CacheEntry {
    matjson::Value data;
    long long expiration;
};

class DataCache {

protected:
    static DataCache* instance;
public:

    std::unordered_map<int, CacheEntry> cache{};

    DataCache();
    void init();
    void store(const int& id, const matjson::Value& data, const long long& expiration);
    matjson::Value retrieve(const int& id);

    static long long getCurrentUnixTimestamp();

    static DataCache* get(){

        if (!instance) {
            instance = new DataCache();
            instance->init();
        };
        return instance;
    }
};


#endif
