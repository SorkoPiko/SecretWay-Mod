// ReSharper disable CppEnforceOverridingFunctionStyle
// ReSharper disable CppHidingFunction
#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/utils/web.hpp>
#include "DataCache.hpp"

struct Route {
    int start;
    int end;
    std::string desc;
    std::string type;
};

class $modify(MyLevelCell, LevelCell) {

    struct Fields {
        EventListener<web::WebTask> m_downloadListener;
    };

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();

        retain();

        downloadManager();
    }

    void downloadManager() {

        const auto cache = DataCache::get();
        if (const auto data = cache->retrieve(m_level->m_levelID); data != nullptr) {
            postDownload();
            return;
        }

        const std::string URL = fmt::format("http://secretway.sorkopiko.com/secretway/{}",static_cast<int>(m_level->m_levelID));

        m_fields->m_downloadListener.bind([this, cache] (web::WebTask::Event* e) {
            if (const auto res = e->getValue()) {
                if (!res->ok()) {
                    log::warn("Request Failed");
                    release();
                    return;
                }
                auto data = res->json().value();
                if (!data.contains("found")) {
                    log::warn("Invalid Request");
                    release();
                    return;
                }
                if (data["found"].as_bool() == true) {
                    const auto timestamp = DataCache::getCurrentUnixTimestamp();
                    const auto expirationHeader = res->header("cache-control").value_or("max-age=0");
                    const auto expiration = expirationHeader.substr(expirationHeader.find("=") + 1);
                    Loader::get()->queueInMainThread([this, cache, data, expiration, timestamp] {
                        cache->store(m_level->m_levelID, data, std::stol(expiration)+timestamp);
                        postDownload();
                    });
                } else {
                    release();
                }
            } else if (e->isCancelled()) {
                log::warn("Request Cancelled");
                release();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_downloadListener.setFilter(req.get(URL));
    }

    void createBadge() const {
        const auto badge = CCSprite::create("badge.png"_spr);
        badge->setID("secret-way-badge"_spr);
        badge->retain();
        float spriteSize;
        CCPoint a;
        const auto levelName = typeinfo_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("level-name"));
        const auto creator = typeinfo_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->getChildren()->objectAtIndex(0));
        const float text = creator->getScaledContentWidth() / 2;
        if ( this->m_compactView ) {
            spriteSize = 0.8;
            a = CCPoint(levelName->getScaledContentWidth() + text + 5.0, 0.0)+levelName->getPosition();
        } else {
            spriteSize = 1.0;
            a = CCPoint(text + 2.0, -22.0)+levelName->getPosition();
        }
        const CCPoint offset(15.0, 0.0);
        auto pos = CCPoint(a.x + text + spriteSize * 6.0, a.y - 1.0);
        if (this->m_level->m_originalLevel.value() != 0) {
            pos += offset;
        }
        if (this->m_level->m_objectCount.value() > 40000) {
            pos += offset;
        }
        badge->setAnchorPoint(CCPoint(0.0, 0.5));
        this->m_mainLayer->addChild(badge);
        badge->setPosition(pos);
        badge->setScale(spriteSize);
        badge->release();
    }

    void postDownload() const {
        createBadge();
    }
};

class $modify(MyLevelInfoLayer, LevelInfoLayer) {

    struct Fields {
        EventListener<web::WebTask> m_downloadListener;
    };

    bool init(GJGameLevel *level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) {
            return false;
        }

        downloadManager();
        return true;
    }

    void downloadManager() {

        const auto cache = DataCache::get();
        if (const auto data = cache->retrieve(m_level->m_levelID); data != nullptr) {
            postDownload(data);
            return;
        }

        const std::string URL = fmt::format("http://secretway.sorkopiko.com/secretway/{}",static_cast<int>(m_level->m_levelID));

        m_fields->m_downloadListener.bind([this, cache] (web::WebTask::Event* e) {
            if (const auto res = e->getValue()) {
                if (!res->ok()) {
                    log::warn("Request Failed");
                    release();
                    return;
                }
                auto data = res->json().value();
                if (!data.contains("found")) {
                    log::warn("Invalid Request");
                    release();
                    return;
                }
                if (data["found"].as_bool() == true) {
                    const auto timestamp = DataCache::getCurrentUnixTimestamp();
                    const auto expirationHeader = res->header("cache-control").value_or("max-age=0");
                    const auto expiration = expirationHeader.substr(expirationHeader.find("=") + 1);
                    Loader::get()->queueInMainThread([this, cache, data, expiration, timestamp] {
                        cache->store(m_level->m_levelID, data, std::stol(expiration)+timestamp);
                        postDownload(data);
                    });
                } else {
                    release();
                }
            } else if (e->isCancelled()) {
                log::warn("Request Cancelled");
                release();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_downloadListener.setFilter(req.get(URL));
    }

    void createBadge() {
        const auto badge = CCSprite::create("badge.png"_spr);
        badge->setID("secret-way-badge"_spr);
        badge->retain();

        CCPoint a;
        const auto levelName = dynamic_cast<CCLabelBMFont*>(this->getChildByID("title-label"));
        const auto creator = dynamic_cast<CCLabelBMFont*>(this->getChildByID("creator-info-menu")->getChildByID("creator-name")->getChildren()->objectAtIndex(0));
        const float text = creator->getScaledContentWidth() / 2;
        a = CCPoint(levelName->getPositionX(), levelName->getPositionY()-26.0);
        const CCPoint offset(18.0, 0.0);
        auto pos = CCPoint(a.x + text + 6.0, a.y - 1.0);
        if (this->m_level->m_originalLevel.value() != 0) {
            pos += offset;
        }
        if (this->m_level->m_objectCount.value() > 40000) {
            pos += offset;
        }
        badge->setAnchorPoint(CCPoint(0.0, 0.5));
        this->addChild(badge);
        badge->setPosition(pos);
        badge->setScale(1);
        badge->release();
    }

    void createButton(matjson::Value data) {
        const auto button = nullptr;
    }

    void postDownload(matjson::Value data) {
        createBadge();
    }

};

class $modify(MyPlayLayer, PlayLayer) {

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

    void setupHasCompleted() {
        PlayLayer::setupHasCompleted();

        const auto cache = DataCache::get();
        if (const auto data = cache->retrieve(m_level->m_levelID); data != nullptr) {
            drawRoutes(data);
        }
    }

    void drawRoutes(matjson::Value data) {
        const auto pb = this->getChildByID("progress-bar");
        if (!pb) return;
        const auto pbSize = pb->getScaledContentSize();
        std::vector<Route> routes = {};
        const std::string dump = data.dump(0);
        Mod::get()->getConfigDir();
        const auto rts = data["routes"].as_array();

        for (const auto& route : rts) {
            routes.push_back(Route {
                .start = route["start"].as_int(),
                .end = route["end"].as_int(),
                .desc = route["desc"].as_string(),
                .type = route["type"].as_string()
            });
        }

        for (auto i = 0; ++i;) {
            const auto route = routes[i];
            const auto start = route.start * (pbSize.width - 2.f) / 100.f;
            const auto end = route.end * (pbSize.width - 2.f) / 100.f;
            auto startLine = CCLayerColor::create(m_fields->colors[i], 2.f, 6.f);
            startLine->setPosition(CCPoint(start, 5.f));
            startLine->setVisible(route.start > 1 && route.start < 99);
            pb->addChild(startLine);
            auto endLine = CCLayerColor::create(m_fields->colors[i], 2.f, 6.f);
            endLine->setPosition(CCPoint(end, 5.f));
            startLine->setVisible(route.end > 1 && route.end < 99);
            pb->addChild(endLine);
        }
    }
};