#pragma once
#include <math.h>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "PageBase.h"
#include "PageMessage.h"
#include "MessageBus.h"

class MyMenu:public PageBase {
public:
    MyMenu(){}
    ~MyMenu(){
        std::cout << " destruct " << typeid(this).name() << std::endl;
    }
    tgui::Group::Ptr Init(sf::Vector2u size, int pageId){
        m_pageId = pageId;
        m_size = size;
        myMenuGroup = tgui::Group::create();
        myMenuGroup->setSize(m_size.x, m_size.y);
        myMenuGroup->setVisible(m_visible);

        auto background = tgui::Picture::create("res/background.jpeg");
        background->setSize(m_size.x, m_size.y);
        myMenuGroup->add(background);

        auto label = tgui::Label::create();
        label->setText(u8"小游戏合集");
        label->setTextSize(36);
        label->setPosition("50% - width / 2", 10);
        myMenuGroup->add(label); // 将控件添加到 Gui

        myMenuGroup->add(initButton());

        //退出
        auto exitButton = tgui::Button::create();
        exitButton->setText(u8"退出");
        exitButton->setSize(150, 50);
        // 核心定位代码：让按钮右下角对齐窗口右下角，留20像素边距
        exitButton->setPosition("100% - width - 20", "100% - height - 20");
        exitButton->onPress([&]{
            PageChange msg{PAGE_STATE_DESTROY, PAGE_ID_MENU}; // 退出游戏
            MessageBus::getInstance().emit(msg);
        });
        myMenuGroup->add(exitButton);
        return myMenuGroup;
    }
    void onShow() {
        if(!m_visible){
            m_visible = !m_visible;
            myMenuGroup->setVisible(m_visible);
        }
    }
    void onHide(){
        if(m_visible){
            m_visible = !m_visible;
            myMenuGroup->setVisible(m_visible);
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
        return myMenuGroup;
    }
private:
    tgui::Grid::Ptr initButton(){
        auto grid = tgui::Grid::create();
        grid->setPosition("50% - width / 2", "30%");
        grid->setAutoSize(true);
        
        std::array<std::pair<std::string, std::function<void()>>, 8> configs{
            std::make_pair(u8"贪吃蛇", std::bind(&MyMenu::doSnakeGame, this)),
            std::make_pair(u8"吃豆人", std::bind(&MyMenu::doPacManGame, this)),
            std::make_pair(u8"炸弹人", std::bind(&MyMenu::doBombermanGame, this)),
            std::make_pair(u8"是男人就下一百层", std::bind(&MyMenu::doHundredFloorsGame, this)),
            std::make_pair(u8"俄罗斯方块", std::bind(&MyMenu::doTetrisGame, this)),
            std::make_pair(u8"消消乐", std::bind(&MyMenu::doCandyCrushGame, this)),
            std::make_pair(u8"数独", std::bind(&MyMenu::doSudokuGame, this)),
            std::make_pair(u8"2048", std::bind(&MyMenu::do2048Game, this))
        };
        int index = 0;
        int column = 4;
        for (auto& [name, cb] : configs) {
            auto btn = tgui::Button::create(name);
            btn->onClick.connect(cb);
            btn->setSize(150, 50);
            grid->addWidget(btn, index/column, index%column, tgui::Grid::Alignment::Center, tgui::Padding{20});
            index++;
        }
        return grid;
    }
    void doSnakeGame(){
        std::cout<<__func__<<std::endl;
        PageChange msg{PAGE_STATE_CREATE, PAGE_ID_SNAKE};
        MessageBus::getInstance().emit(msg);
    }
    void doPacManGame(){
        std::cout<<__func__<<std::endl;
    }
    void doBombermanGame(){
        std::cout<<__func__<<std::endl;
    }
    void doHundredFloorsGame(){
        std::cout<<__func__<<std::endl;
    }
    void doTetrisGame(){
        std::cout<<__func__<<std::endl;
    }
    void doCandyCrushGame(){
        std::cout<<__func__<<std::endl;
    }
    void doSudokuGame(){
        std::cout<<__func__<<std::endl;
    }
    void do2048Game(){
        std::cout<<__func__<<std::endl;
    }
private:
    tgui::Group::Ptr myMenuGroup;
    sf::Vector2u m_size;
    bool m_visible = false;
    int m_pageId;
};