#include "Search.hpp"

bool Search::on = Mod::get()->getSavedValue<bool>("searchOn", false);
std::unordered_map<std::string, bool> Search::lastUsed{};

bool Search::getOn() {
    return on;
}

void Search::setOn(const bool value) {
    on = value;
    Mod::get()->setSavedValue("searchOn", on);
}

bool Search::checkLastUsed(const char* key) {
    auto newKey = std::string(key);

    if (!lastUsed.contains(newKey)) {
        lastUsed[newKey] = on;
        return false;
    }

    if (const auto obj =  lastUsed[newKey]; obj == on) {
        return false;
    }

    lastUsed[newKey] = on;
    return true;
}