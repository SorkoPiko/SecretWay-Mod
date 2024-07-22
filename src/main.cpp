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

        downloadManager();
    }

    void downloadManager() {
        m_fields->m_downloadListener.bind([this] (web::WebTask::Event* e) {
            if (auto res = e->getValue()) {
                m_fields->m_text->setString("yes");
                m_fields->m_text->setPosition({200, 50});
                addChild(m_fields->m_text);
            } else if (e->isCancelled()) {
                log::warn("Web request cancelled");
            }
        });
    }
};