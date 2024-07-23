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
                    const auto expirationHeader = res->header("cache-control").value_or("max-age=3600");
                    const auto expiration = expirationHeader.substr(expirationHeader.find("=") + 1);
                    cache->store(m_level->m_levelID, data, timestamp, std::stol(expiration));
                    postDownload(data);
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

    void postDownload(const matjson::Value& data) {

    }
};