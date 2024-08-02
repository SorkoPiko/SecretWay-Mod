#include "Routes.hpp"

using namespace geode::prelude;

std::vector<ccColor4B> colors = {
    {0, 255, 0, 255},
    {255, 0, 0, 255},
    {0, 0, 255, 255},
    {255, 255, 0, 255},
    {255, 0, 255, 255},
    {0, 255, 255, 255},
    {255, 255, 255, 255},
    {0, 0, 0, 255}
};

std::vector<Route> Routes::convertRoutes(const matjson::Array& rts) {
    std::vector<Route> routes = {};

    for (const auto& route : rts) {
        if (route["type"].as_string() != "normal") {
            routes.push_back(Route {
                .desc = route["desc"].as_string(),
                .type = route["type"].as_string()
            });
            continue;
        }
        routes.push_back(Route {
            .start = route["start"].as_int(),
            .end = route["end"].as_int(),
            .desc = route["desc"].as_string(),
            .type = route["type"].as_string()
        });
    }

    return routes;
}

ccColor4B Routes::getColour(const int& i) {
    return colors[i % colors.size()];
}

static bool doRangesIntersect(const Route& r1, const Route& r2) {
    return r1.start <= r2.end && r2.start <= r1.end;
}

static int highestSkippable(const Route& r1, const Route& r2) {
    return std::max(r1.end-r1.start, r2.end-r2.start);
}

static int bothSkippable(const Route& r1, const Route& r2) {
    return r1.end-r1.start + r2.end-r2.start;
}

int Routes::getHighestSkippable(const std::vector<Route>& routes) {
    int highest = 0;

    if (routes.size() == 1) {
        return routes[0].end - routes[0].start;
    }

    for (auto i = 0; i < routes.size(); ++i) {
        for (auto j = i + 1; j < routes.size(); ++j) {
            if (doRangesIntersect(routes[i], routes[j])) {
                highest = std::max(highest, highestSkippable(routes[i], routes[j]));
            } else {
                highest = std::max(highest, bothSkippable(routes[i], routes[j]));
            }
        }
    }

    return highest;
}