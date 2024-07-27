#pragma once

#ifndef DATACACHE_H
#define DATACACHE_H

using namespace geode::prelude;

#include <unordered_map>
#include <matjson.hpp>
#include <any>
#include <vector>

class DataCache {

protected:
    static DataCache* instance;
public:

    std::unordered_map<int, std::vector<std::any>> cache{};

    DataCache();
    void store(const int& id, const matjson::Value& data, const long long& expiration);
    matjson::Value retrieve(const int& id);

    static long long getCurrentUnixTimestamp();

    static DataCache* get(){

        if (!instance) {
            instance = new DataCache();
        };
        return instance;
    }
};


#endif
