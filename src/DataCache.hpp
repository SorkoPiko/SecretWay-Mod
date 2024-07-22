#pragma once

#ifndef DATACACHE_H
#define DATACACHE_H

using namespace geode::prelude;

#include <unordered_map>
#include <matjson.hpp>

class DataCache {

protected:
    static DataCache* instance;
public:

    std::unordered_map<int, matjson::Value> cache{};

    DataCache();
    void store(const int& id, const matjson::Value& data);
    matjson::Value retrieve(const int& id);

    static long getCurrentUnixTimestamp();

    static DataCache* get(){

        if (!instance) {
            instance = new DataCache();
        };
        return instance;
    }
};


#endif
