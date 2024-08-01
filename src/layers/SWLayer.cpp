#include "../utils/Routes.hpp"
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

    m_scrollLayer = ScrollLayer::create({m_size.width / 1.6f - 20.f, m_size.height / 1.25f - 20.f});
    m_scrollLayer->setPosition(CCSize{winSize.width / 2 + 70.f, winSize.height / 2 - 20.f} - m_bg->getContentSize() / 2);
    m_scrollLayer->setZOrder(11);
    m_mainLayer->addChild(m_scrollLayer);

    m_scrollBar = Scrollbar::create(m_scrollLayer);
    m_scrollBar->setScale(1);
    m_scrollBar->setZOrder(11);
    m_scrollBar->setPosition({499, 130});
    m_mainLayer->addChild(m_scrollBar);

    const auto routes = Routes::convertRoutes(data["routes"].as_array());

    auto yPos = m_scrollLayer->getScaledContentHeight();
    auto yPos2 = 0.f;
    auto i = data["routes"].as_array().size();

    for (const auto& route : routes) {
        auto yPosDecrease = 0.f;
        const auto parent = CCNode::create();
        parent->setAnchorPoint({0, 0});
        parent->setContentWidth(width);
        const auto desc = SimpleTextArea::create(
            //route.desc,
            "Lorem ipsum odor amet, consectetuer adipiscing elit. Tellus tincidunt est sociosqu nec donec magnis. Arcu amet finibus dolor semper aenean. Efficitur dapibus leo metus nisi luctus litora bibendum. Adipiscing netus semper ut et, neque condimentum placerat convallis. Platea montes laoreet congue varius justo sit elit. Rhoncus iaculis primis class netus ridiculus in. Ipsum ad dolor amet nulla efficitur sagittis accumsan commodo. Aliquet netus suspendisse consequat senectus tempus suscipit. Mattis odio per vitae posuere metus nulla. Viverra class adipiscing odio aliquam, morbi pharetra viverra. Nunc netus mus ullamcorper interdum interdum eros augue dictum. Dolor hendrerit augue lectus turpis scelerisque convallis bibendum maximus. Sit metus vitae velit felis rhoncus lectus fames mollis. Penatibus dis nec cubilia donec libero taciti tortor scelerisque. Etiam gravida mauris purus pellentesque viverra. Ante nostra duis euismod luctus vestibulum.",
            "bigFont.fnt",
            0.6f
        );
        desc->setWrappingMode(WORD_WRAP);
        desc->setWidth(width);
        desc->setAnchorPoint({0, 0});
        yPosDecrease += desc->getScaledContentHeight() + 2.f;
        parent->addChild(desc);

        std::string infoText;
        if (route.type == "normal") {
            infoText = fmt::format("Route: {}%-{}% (Total: {}%)", route.start, route.end, route.end-route.start);
        } else {
            infoText = fmt::format("Route: {}", route.type);
        }
        const auto info = CCLabelBMFont::create(infoText.c_str(), "goldFont.fnt");
        info->limitLabelWidth(width, 0.5f, 0.5f);
        info->setAnchorPoint({0, 0});
        info->setPositionY(yPosDecrease);
        yPosDecrease += info->getScaledContentHeight() + 4.f;
        parent->addChild(info);

        const auto title = CCLabelBMFont::create(fmt::format("Route {}", i).c_str(), "bigFont.fnt");
        title->limitLabelWidth(width, 1.f, 1.f);
        title->setAnchorPoint({0, 0});
        title->setPositionY(yPosDecrease);
        yPosDecrease += title->getScaledContentHeight();
        parent->addChild(title);
        parent->setContentHeight(yPosDecrease);
        yPos2 += yPosDecrease;
        yPos -= yPosDecrease;
        parent->setPosition(5.f, yPos);
        m_scrollLayer->m_contentLayer->addChild(parent);
        m_routes.push_back(parent);
        --i;
    }

    m_scrollLayer->m_contentLayer->setAnchorPoint({0, 1});
    m_scrollLayer->m_contentLayer->setContentSize({width, std::max(m_scrollLayer->getScaledContentHeight(), yPos2)});
    m_scrollLayer->m_contentLayer->setPositionY(m_scrollLayer->getScaledContentHeight()-m_scrollLayer->m_contentLayer->getScaledContentHeight());
    m_scrollLayer->m_contentLayer->setVisible(true);

    if (m_scrollLayer->m_contentLayer->getScaledContentHeight() == m_scrollLayer->getScaledContentHeight()) {
        for (const auto route : m_routes) {
            route->setPositionY(m_scrollLayer->getScaledContentHeight()-route->getScaledContentHeight());
        }
    } else {
        for (const auto route : m_routes) {
            route->setPositionY(route->getPositionY() - yPos);
        }
    }

    return true;
}