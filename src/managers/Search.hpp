#ifndef SEARCH_HPP
#define SEARCH_HPP

using namespace geode::prelude;

class Search {
protected:
    static bool on;
    static std::unordered_map<std::string, bool> lastUsed;
public:
    [[nodiscard]] static bool getOn();
    static void setOn(bool value);

    static bool checkLastUsed(const char* key);
};

#endif
