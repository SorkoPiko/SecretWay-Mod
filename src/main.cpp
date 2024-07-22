#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/LevelCell.hpp>
#include <Geode/utils/web.hpp>

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
        const std::string URL = fmt::format("http://secretway.sorkopiko.com/secretway/{}",static_cast<int>(m_level->m_levelID));

        m_fields->m_downloadListener.bind([this] (web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
            auto data = res->json().value();
                if (data["found"].as_bool() == true) {
                    m_fields->m_text->setString("Found");
                } else {
                    m_fields->m_text->setString("Not Found");
                }
                addChild(m_fields->m_text);
                release();
            } else if (e->isCancelled()) {
                log::warn("Request Cancelled");
                release();
            }
        });

        auto req = web::WebRequest();
        m_fields->m_downloadListener.setFilter(req.get(URL));
    }
};