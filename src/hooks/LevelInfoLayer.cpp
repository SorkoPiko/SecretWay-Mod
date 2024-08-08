#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/utils/web.hpp>
#include "../managers/DataCache.hpp"
#include "../layers/SWLayer.hpp"

using namespace geode::prelude;

class $modify(SWLevelInfoLayer, LevelInfoLayer) {

    struct Fields {
        EventListener<web::WebTask> m_downloadListener;
        matjson::Value m_data;
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
                    return;
                }
                auto data = res->json().value();
                if (!data.contains("found")) {
                    log::warn("Invalid Request");
                    return;
                }
                if (data["found"].as_bool() == true) {
                    const auto timestamp = DataCache::getCurrentUnixTimestamp();
                    const auto expirationHeader = res->header("cache-control").value_or("max-age=0");
                    const auto expiration = expirationHeader.substr(expirationHeader.find("=") + 1);
                    Loader::get()->queueInMainThread([this, cache, data, expiration, timestamp] {
                        cache->store(m_level->m_levelID, data["data"], std::stol(expiration)+timestamp);
                        postDownload(data["data"]);
                    });
                } else {
                }
            } else if (e->isCancelled()) {
                log::warn("Request Cancelled");
            }
        });

        auto req = web::WebRequest();
        m_fields->m_downloadListener.setFilter(req.get(URL));
    }

    void createBadge() {
        const auto levelName = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("title-label"));
        const auto creator = typeinfo_cast<CCLabelBMFont*>(this->getChildByID("creator-info-menu")->getChildByID("creator-name")->getChildren()->objectAtIndex(0));
        if (!levelName || !creator) return;

        const auto badge = CCSprite::createWithSpriteFrameName("badge.png"_spr);
        badge->setID("secret-way-badge"_spr);

        const float text = creator->getScaledContentWidth() / 2;
        CCPoint a = CCPoint(levelName->getPositionX(), levelName->getPositionY() - 26.0);
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
    }

    void createButton(const matjson::Value &data) {
        const auto menu = getChildByID("other-menu");
        if (!menu) return;

        m_fields->m_data = data;
        const auto s = CCSprite::createWithSpriteFrameName("badgeBig.png"_spr);
        s->setScale(0.75f);
        const auto button = CCMenuItemSpriteExtra::create(
            s,
            this,
            menu_selector(SWLevelInfoLayer::openSWLayer)
        );

        menu->addChild(button);
        if (menu->getChildByID("favorite-button")->isVisible())
            button->setPosition({menu->getChildByID("favorite-button")->getPositionX()+menu->getChildByID("favorite-button")->getPositionX()-menu->getChildByID("info-button")->getPositionX(), button->getPositionY()});
        else
            button->setPosition({menu->getChildByID("favorite-button")->getPosition()});

        menu->updateLayout();
    }

    void postDownload(const matjson::Value &data) {
        createBadge();
        createButton(data);
    }

    void openSWLayer(CCObject* sender) {
        const auto swLayer = SWLayer::create(m_fields->m_data);
        this->addChild(swLayer);
    }

};