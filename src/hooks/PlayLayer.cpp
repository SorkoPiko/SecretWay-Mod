#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/web.hpp>
#include "../managers/DataCache.hpp"
#include "../utils/Routes.hpp"

using namespace geode::prelude;

class $modify(SWPlayLayer, PlayLayer) {

    struct Fields {
        std::vector<ccColor4B> colors = {
            {255, 0, 0, 255},
            {0, 255, 0, 255},
            {0, 0, 255, 255},
            {255, 255, 0, 255},
            {255, 0, 255, 255},
            {0, 255, 255, 255},
            {255, 255, 255, 255},
            {0, 0, 0, 255}
        };
    };

    void startGame() {
        PlayLayer::startGame();

        if (!m_level->isPlatformer()) {
            const auto cache = DataCache::get();
            if (const auto data = cache->retrieve(m_level->m_levelID); data != nullptr) {
                drawRoutes(data);
            }
        }
    }

    void drawRoutes(matjson::Value data) {
        const auto pb = this->getChildByID("progress-bar");
        if (!pb) return;
        const auto pbSize = pb->getScaledContentWidth();
        auto routes = Routes::convertRoutes(data["routes"].as_array());

        float i = 0;
        for (const auto& route : routes) {
            if (route.type != "normal") continue;
            const auto parent = CCNode::create();
            parent->setID(fmt::format("route-{}"_spr, i));
            pb->addChild(parent);
            const auto start = route.start * (pbSize - 2.f) / 100.f;
            const auto end = route.end * (pbSize - 2.f) / 100.f;
            float y;
            if (routes.size() == 1) {
                y = 7.f;
            } else {
                y = 8.f-i*2.f;
            }
            const auto startLine = CCLayerColor::create(m_fields->colors[i], 2.f, 6.f);
            startLine->setPosition(CCPoint(start, 5.f));
            startLine->setVisible(route.start > 1 && route.start < 99);
            startLine->setZOrder(100);
            startLine->setID("start-line"_spr);
            parent->addChild(startLine);
            const auto endLine = CCLayerColor::create(m_fields->colors[i], 2.f, 6.f);
            endLine->setPosition(CCPoint(end, 5.f));
            endLine->setVisible(route.end > 1 && route.end < 99);
            endLine->setZOrder(100);
            endLine->setID("end-line"_spr);
            parent->addChild(endLine);
            const auto middleLine = CCLayerColor::create(m_fields->colors[i], end-start, 2.f);
            middleLine->setPosition(CCPoint(start, y));
            middleLine->setID("middle-line"_spr);
            parent->addChild(middleLine);
            ++i;
        }
    }
};