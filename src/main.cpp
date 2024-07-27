// ReSharper disable CppEnforceOverridingFunctionStyle
// ReSharper disable CppHidingFunction
#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelCell.hpp>
#include <Geode/utils/web.hpp>
#include "DataCache.hpp"

class $modify(MyLevelCell, LevelCell) {

    struct Fields {
        Ref<CCLabelBMFont> m_text;
        EventListener<web::WebTask> m_downloadListener;
    };

    void loadCustomLevelCell() {
        LevelCell::loadCustomLevelCell();

        m_fields->m_text = CCLabelBMFont::create("a","bigFont.fnt");
        m_fields->m_text->setPosition({200, 50});
        m_fields->m_text->setAnchorPoint({1, 0});

        retain();

        downloadManager();
    }

    void downloadManager() {

        const auto cache = DataCache::get();
        if (const auto data = cache->retrieve(m_level->m_levelID); data != nullptr) {
            m_fields->m_text->setString("Found");
            postDownload(data);
            return;
        }

        const std::string URL = fmt::format("http://secretway.sorkopiko.com/secretway/{}",static_cast<int>(m_level->m_levelID));

        m_fields->m_downloadListener.bind([this, cache] (web::WebTask::Event* e) {
            if (const auto res = e->getValue()) {
                if (auto data = res->json().value(); data["found"].as_bool() == true) {
                    const auto timestamp = DataCache::getCurrentUnixTimestamp();
                    const auto expirationHeader = res->header("cache-control").value_or("max-age=0");
                    const auto expiration = expirationHeader.substr(expirationHeader.find("=") + 1);
                    Loader::get()->queueInMainThread([this, cache, data, expiration, timestamp] {
                        cache->store(m_level->m_levelID, data, std::stol(expiration)+timestamp);
                        postDownload(data);
                    });
                } else {
                    m_fields->m_text->removeMeAndCleanup();
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
        const auto badge = CCSprite::createWithSpriteFrameName("highObjectIcon_001.png");
        float spriteSize;
        CCPoint a;
        const auto levelName = dynamic_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("level-name"));
        const auto creator = dynamic_cast<CCLabelBMFont*>(this->m_mainLayer->getChildByID("main-menu")->getChildByID("creator-name")->getChildren()->objectAtIndex(0));
        const float text = creator->getContentWidth() / 2 * creator->getScale();
        if ( this->m_compactView ) {
            spriteSize = 0.80000001;
            a = CCPoint(levelName->getContentWidth() * levelName->getScale() + text + 5.0, 0.0)+levelName->getPosition();
        } else {
            spriteSize = 1.0;
            a = CCPoint(text + 2.0, -22.0)+levelName->getPosition();
        }
        const CCPoint offset(15.0, 0.0);
        auto pos = CCPoint(a.x + text + spriteSize * 6, a.y - 1.0);
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
    }

    void postDownload(matjson::Value data) {
        createBadge();
    }
};