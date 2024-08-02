#ifndef SWLAYER_HPP
#define SWLAYER_HPP

using namespace geode::prelude;

class SWLayer : public Popup<matjson::Value const&>, public FLAlertLayerProtocol {
protected:
    bool setup(matjson::Value const& data) override;
public:
    static SWLayer* create(matjson::Value const& data);
    void onYouTube(CCObject* sender);
    void onSource(CCObject* sender);

    matjson::Value m_data;
    CCScale9Sprite* m_bg;
    ScrollLayer* m_scrollLayer;
    Scrollbar* m_scrollBar;
    std::vector<CCNode*> m_routes;
    std::string m_ytLink;
    std::string m_srcLink;
};

#endif
