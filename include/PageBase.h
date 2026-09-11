#pragma once
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>


class PageBase {
public:
    virtual ~PageBase() = default;
    virtual tgui::Group::Ptr Init(sf::Vector2u size, int pageId) = 0;
    virtual void onShow()  = 0;
    virtual void onHide() = 0;
    virtual bool HandleEvent(sf::Event event) = 0;
    virtual void onLogicLoop() = 0;
    virtual int getPageId() = 0;
    virtual tgui::Group::Ptr getCurrentGroup() = 0;
};