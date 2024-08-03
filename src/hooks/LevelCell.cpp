#include <Geode/modify/LevelCell.hpp>
#include <Geode/utils/web.hpp>
#include "../managers/DataCache.hpp"

using namespace geode::prelude;

class $modify(SWLevelCell, LevelCell) {

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
            createBadge();
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
                        cache->store(m_level->m_levelID, data["data"], std::stol(expiration)+timestamp);
                        createBadge();
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
        const auto levelName = typeinfo_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("level-name"));
        const auto creator = typeinfo_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->getChildren()->objectAtIndex(0));
        if (!levelName || !creator) return;
        const auto badge = CCSprite::createWithSpriteFrameName("badge.png"_spr);
        badge->setID("secret-way-badge"_spr);
        badge->retain();
        float spriteSize;
        CCPoint a;

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
};