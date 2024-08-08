#include <Geode/modify/LevelSearchLayer.hpp>
#include "../managers/Search.hpp"

using namespace geode::prelude;

class $modify(SWLevelSearchLayer, LevelSearchLayer) {
    bool init(const int searchType) {
        if (!LevelSearchLayer::init(searchType)) return false;

        const auto onSpr = CCSprite::createWithSpriteFrameName("swToggleOn.png"_spr);
        const auto offSpr = CCSprite::createWithSpriteFrameName("swToggleOff.png"_spr);
        onSpr->setScale(0.8f);
        offSpr->setScale(0.8f);

        const auto btn = CCMenuItemToggler::create(
            onSpr,
            offSpr,
            this,
            menu_selector(SWLevelSearchLayer::toggleSearch)
        );
        const auto menu = getChildByID("other-filter-menu");
        btn->setID("swToggle"_spr);
        if (!Search::getOn()) {
            btn->toggle(true);
        }
        menu->addChild(btn);
        menu->updateLayout();
        return true;
    }

    void toggleSearch(CCObject* sender) {
        const auto toggle = typeinfo_cast<CCMenuItemToggler*>(sender);
        Search::setOn(toggle->isOn());
        GameLevelManager::sharedState()->m_timerDict->removeAllObjects();
    }
};