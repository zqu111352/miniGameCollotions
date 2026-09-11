#pragma once
#include <math.h>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "PageBase.h"
#include "PageMessage.h"
#include "MessageBus.h"

class SnakeGame:public PageBase {
public:
    SnakeGame(){}
    ~SnakeGame(){
        std::cout << " destruct " << typeid(this).name() << std::endl;
    }
    tgui::Group::Ptr Init(sf::Vector2u size, int pageId){
        m_pageId = pageId;
        m_size = size;
        snakeGameGroup = tgui::Group::create();
        snakeGameGroup->setSize(m_size.x, m_size.y);
        snakeGameGroup->setVisible(m_visible);

        auto background = tgui::Picture::create("res/background.jpeg");
        background->setSize(m_size.x, m_size.y);
        snakeGameGroup->add(background);

        auto label = tgui::Label::create();
        label->setText(u8"贪吃蛇");
        label->setTextSize(36);
        label->setPosition("50% - width / 2", 10);
        snakeGameGroup->add(label); // 将控件添加到 Gui

        snakeGameGroup->add(initButton());

        //退出
        auto exitButton = tgui::Button::create();
        exitButton->setText(u8"退出");
        exitButton->setSize(150, 50);
        // 核心定位代码：让按钮右下角对齐窗口右下角，留20像素边距
        exitButton->setPosition("100% - width - 20", "100% - height - 20");
        exitButton->onPress([&]{
            PageChange msg{PAGE_STATE_DESTROY, PAGE_ID_SNAKE}; // 退出游戏
            MessageBus::getInstance().emit(msg);
        });
        snakeGameGroup->add(exitButton);
        return snakeGameGroup;
    }
    void onShow() {
        if(!m_visible){
            m_visible = !m_visible;
            snakeGameGroup->setVisible(m_visible);
        }
    }
    void onHide(){
        if(m_visible){
            m_visible = !m_visible;
            snakeGameGroup->setVisible(m_visible);
        }
    }
    bool HandleEvent(sf::Event event){
        return false;
    }

    void onLogicLoop(){
    }

    int getPageId(){
        return m_pageId;
    }
    tgui::Group::Ptr getCurrentGroup(){
        return snakeGameGroup;
    }
private:
    tgui::VerticalLayout::Ptr initButton(){
        auto autoLayout = tgui::VerticalLayout::create();
        autoLayout->setPosition("50% - width / 2", "30%");
        autoLayout->setSize("50%", "30%");
        autoLayout->getRenderer()->setPadding(20);                         // 整体内边距
        autoLayout->getRenderer()->setSpaceBetweenWidgets(12);             // 控件之间间距
        
        //开始
        auto startButton = tgui::Button::create();
        startButton->setText(u8"开始");
        startButton->setSize("90%", "8%");
        startButton->onPress([&]{
            std::cout<<"start snake game"<<std::endl;
        });
        autoLayout->add(startButton);
        
        //历史分数
        auto historyButton = tgui::Button::create();
        historyButton->setText(u8"历史分数");
        historyButton->setSize("90%", "8%");
        historyButton->onPress([&]{
            std::cout<<"history score"<<std::endl;
        });
        autoLayout->add(historyButton);

        return autoLayout;
    }
private:
    tgui::Group::Ptr snakeGameGroup;
    sf::Vector2u m_size;
    bool m_visible = false;
    int m_pageId;
};