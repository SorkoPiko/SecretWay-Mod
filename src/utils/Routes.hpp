#ifndef UTILS_H
#define UTILS_H

using namespace geode::prelude;

struct Route {
    int start;
    int end;
    std::string desc;
    std::string type;
};

class Routes {
public:
    static std::vector<Route> convertRoutes(const matjson::Array& rts);
    static ccColor4B getColor(const int& i);
    static int getHighestSkippable(const std::vector<Route>& routes);
};

#endif
