#include <Geode/modify/CCHttpClient.hpp>

using namespace geode::prelude;

std::string httpRequestTypeToString(CCHttpRequest::HttpRequestType type) {
    switch (type) {
        case CCHttpRequest::kHttpGet:
            return "GET";
        case CCHttpRequest::kHttpPost:
            return "POST";
        case CCHttpRequest::kHttpPut:
            return "PUT";
        case CCHttpRequest::kHttpDelete:
            return "DELETE";
        default:
            return "UNKNOWN";
    }
}

class $modify(SWCCHttpClient, CCHttpClient) {
    static void onModify(auto& self) {
        if (!self.setHookPriority("cocos2d::extension::CCHttpClient::send", INT_MIN)) {
            log::error("Failed to set hook priority for CCHttpClient::send");
        }
    }

    void send(CCHttpRequest* request) {
        if (std::string reqUrl = "https://www.boomlings.com/database/getGJLevels21.php"; static_cast<std::string>(request->getUrl()) == reqUrl) {
            request->setUrl("http://localhost:8000/robtop");
        } else {
            Mod::get()->setSavedValue("reqUrl", static_cast<std::string>(request->getUrl()));
            log::info("{}", request->getUrl());
        }
        CCHttpClient::send(request);
    }
};