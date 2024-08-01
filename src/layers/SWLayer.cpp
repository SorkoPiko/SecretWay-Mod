#include "SWLayer.hpp"

using namespace geode::prelude;

SWLayer* SWLayer::create(matjson::Value const& data) {
    const auto ret = new SWLayer();
    if (ret->initAnchored(520, 280, data, "square01_001.png", {0.f, 0.f, 94.f, 94.f})) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}


bool SWLayer::setup(matjson::Value const& data) {
    const auto winSize = CCDirector::sharedDirector()->getWinSize();
    const auto width = 300.f;

    this->setZOrder(100);

    this->setTitle("Secret Ways", "goldFont.fnt", 1.5f);

    m_bg = CCScale9Sprite::create("GJ_square05.png", {0,0, 80, 80});
    m_bg->setContentSize({m_size.width / 1.6f, m_size.height / 1.25f});
    m_bg->setPosition({winSize.width / 2 + 60.f, winSize.height / 2 - 30.f});
    m_bg->setOpacity(100);
    m_bg->setZOrder(10);
    m_mainLayer->addChild(m_bg);

    m_scrollLayer = ScrollLayer::create({m_size.width / 1.6f, m_size.height / 1.25f});
    m_scrollLayer->setPosition(CCSize{winSize.width / 2 + 60.f, winSize.height / 2 - 30.f} - m_bg->getContentSize() / 2);
    m_scrollLayer->setZOrder(11);
    m_mainLayer->addChild(m_scrollLayer);

    m_scrollBar = Scrollbar::create(m_scrollLayer);
    m_scrollBar->setScale(0.95f);
    m_scrollBar->setZOrder(11);
    m_scrollBar->setPosition({499, 130});
    m_mainLayer->addChild(m_scrollBar);

    auto yPos = m_scrollLayer->getContentSize().height;
    auto i = 0;

    for (const auto& route : data["routes"].as_array()) {
        auto yPosDecrease = 0.f;
        const auto parent = CCNode::create();
        parent->setAnchorPoint({0, 1});
        parent->setContentWidth(width);
        parent->setPosition(10.f, yPos);
        const auto desc = SimpleTextArea::create(
            route["desc"].as_string(),
            "goldFont.fnt",
            0.6f
        );
        desc->setWrappingMode(WORD_WRAP);
        desc->setWidth(width);
        desc->setAnchorPoint({0, 0});
        yPosDecrease += desc->getScaledContentHeight() + 5.f;
        parent->addChild(desc);
        const auto title = CCLabelBMFont::create(fmt::format("Route {}", i+1).c_str(), "bigFont.fnt");
        title->limitLabelWidth(width, 1.f, 1.f);
        title->setAnchorPoint({0, 0});
        title->setPositionY(yPosDecrease);
        yPosDecrease += title->getScaledContentHeight();
        parent->addChild(title);
        parent->setContentHeight(yPosDecrease);
        m_scrollLayer->m_contentLayer->addChild(parent);
        m_routes.push_back(parent);
        ++i;
    }

    return true;
}

void SWLayer::update(float delta) {
    for (const auto& node : m_routes) {
        node->setVisible(true);
    }
}