#include <iostream>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "MyMenu.hpp"
#include <stack>
#include "PageMessage.h"
#include "MessageBus.h"
#include "SnakeGmae.hpp"

unsigned int width = 1024;
unsigned int height = 768;
std::vector<std::shared_ptr<PageBase>> pageStack;
tgui::Gui* g_pGui = nullptr;

void onPageChange(const PageChange& msg);

std::shared_ptr<PageBase> getTopPage(){
    if(pageStack.empty())
        return nullptr;
    return *pageStack.rbegin();
}

int main(void){

    // 1. 创建 SFML 窗口
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    sf::RenderWindow window(sf::VideoMode({width, height}, desktop.bitsPerPixel), "TGUI Demo");
    window.setFramerateLimit(60);
    if (!window.isOpen()) {
        std::cerr << "Failed to create window" << std::endl;
        return -1;
    }
    // 2. 创建 TGUI Gui 对象，并绑定到 SFML 窗口
    tgui::Gui gui(window);
    g_pGui = &gui;

    // 3. 加载主题 (可选，TGUI 有默认主题，但加载自定义主题更美观)
    // TGUI 自带一些主题文件，通常在 share/TGUI/themes/ 目录下
    // 如果找不到路径，可以先跳过，使用默认样式
    try {
        gui.loadWidgetsFromFile("Black.theme"); 
    } catch (tgui::Exception& e) {
        std::cout << "Failed to load theme: " << e.what() << std::endl;
    }
    tgui::Font::setGlobalFont("font/SourceHanSerifCN-Medium.otf");

    
    MessageBus::getInstance().subscribe<PageChange>([](const PageChange& msg) {
        onPageChange(msg);
    });
    
    //初始化主界面
    if(window.isOpen()){
        auto myMenu = std::make_shared<MyMenu>();
        gui.add(myMenu->Init(window.getSize(), PAGE_ID_MENU));
        pageStack.push_back(std::move(myMenu));
    }
    // 6. 主循环
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        if(pageStack.empty())
            break;
        auto topPage = getTopPage();
        topPage->onLogicLoop();
        topPage->onShow();

        window.clear(sf::Color::Cyan);

        gui.draw();

        window.display();
    }

    return -1;
}

void Exit(){
    pageStack.clear();//销毁所有界面
}
std::shared_ptr<PageBase> findPageInStack(PAGE_ID_ENUM pageId){
    for(auto &item:pageStack){
        if(pageId == item->getPageId())
            return item;
    }
    return nullptr;
}
void removePageInStack(PAGE_ID_ENUM pageId){
    for(auto it = pageStack.begin(); it != pageStack.end(); it++){
        if(pageId == (*it)->getPageId()){
            pageStack.erase(it);
            g_pGui->remove((*it)->getCurrentGroup());
            return;
        }
    }
}
void onPageChange(const PageChange& msg){
    std::cout << "[pageId] " << msg.pageId << " state: " << msg.state << std::endl;
    switch(msg.state){
        case PAGE_STATE_NONE:{
        }
        break;
        case PAGE_STATE_CREATE:{
            if(!g_pGui)
                break;
            auto window = g_pGui->getWindow();
            if(!window)
                break;
            if(!window->isOpen())
                break;
            auto page = findPageInStack((PAGE_ID_ENUM)(msg.pageId));
            auto topPage = getTopPage();
            if(topPage){
                if(topPage->getPageId() == msg.pageId)
                    break;
                topPage->onHide();
            }
            if(page){
                if(page){
                    page->onShow();
                    removePageInStack((PAGE_ID_ENUM)(msg.pageId));
                    pageStack.push_back(std::move(page));
                }
                break;
            }
            switch (msg.pageId)
            {
                case PAGE_ID_MENU:{
                    auto myMenu = std::make_shared<MyMenu>();
                    g_pGui->add(myMenu->Init(window->getSize(), PAGE_ID_MENU));
                    pageStack.push_back(std::move(myMenu));
                    break;
                }
                case PAGE_ID_SNAKE:{
                    auto myMenu = std::make_shared<SnakeGame>();
                    g_pGui->add(myMenu->Init(window->getSize(), PAGE_ID_SNAKE));
                    pageStack.push_back(std::move(myMenu));
                    break;
                }
                
                default:
                    break;
            }
        }
        break;
        case PAGE_STATE_SHOW:{
            auto topPage = getTopPage();
            if(topPage){
                if(topPage->getPageId() == msg.pageId)
                    break;
                topPage->onHide();
            }
            auto page = findPageInStack((PAGE_ID_ENUM)(msg.pageId));
            if(page){
                page->onShow();
                removePageInStack((PAGE_ID_ENUM)(msg.pageId));
                pageStack.push_back(std::move(page));
            }
            
        }
        break;
        case PAGE_STATE_DESTROY:
        case PAGE_STATE_HIDE:{
            auto page = findPageInStack((PAGE_ID_ENUM)(msg.pageId));
            if(page){
                page->onHide();
                removePageInStack((PAGE_ID_ENUM)(msg.pageId));
            }
            auto topPage = getTopPage();
            if(topPage)
                topPage->onShow();
        }
        break;
    }
}