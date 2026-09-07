#pragma once
#include <math.h>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

class MyMenu {
public:
    MyMenu(){}
    ~MyMenu(){
    }
    void Init(tgui::Gui& gui){
        m_size = gui.getWindow()->getSize();
        myMenuGroup = tgui::Group::create();
        myMenuGroup->setSize(m_size.x, m_size.y);
        myMenuGroup->setVisible(true);

        auto background = tgui::Picture::create("res/background.jpeg");
        background->setSize(m_size.x, m_size.y);
        myMenuGroup->add(background);

        auto label = tgui::Label::create();
        label->setText(u8"小游戏合集");
        label->setTextSize(36);
        label->setPosition("50% - width / 2", 10);
        myMenuGroup->add(label); // 将控件添加到 Gui

        /*auto grid = tgui::Grid::create();
        grid->setPosition(50, 50);
        grid->setSize((m_size.x - 100) / 2, 600);
        // 贪吃蛇
        auto snakeButton = tgui::Button::create();
        snakeButton->setText(u8"贪吃蛇");
        snakeButton->setSize(150, 50);
        snakeButton->onPress([]{
            //进入贪吃蛇游戏界面
        });
        grid->addWidget(snakeButton, 0, 0);
        myMenuGroup->add(grid);*/
        myMenuGroup->add(initGameList());

        //退出
        auto exitButton = tgui::Button::create();
        exitButton->setText(u8"退出");
        exitButton->setSize(150, 50);
        // 核心定位代码：让按钮右下角对齐窗口右下角，留20像素边距
        exitButton->setPosition("100% - width - 20", "100% - height - 20");
        exitButton->onPress([&]{
        //    PageChange msg{PAGE_STATE_DESTROY, 0}; // 退出游戏
        //    MessageBus::getInstance().emit(msg);
            gui.getWindow()->close();
        });
        myMenuGroup->add(exitButton);

        gui.add(myMenuGroup);
    }
    void onShow() {
    }
    void onHide(){
    }
    void DealWindowEvent(const std::optional<sf::Event> &event){
    }

    void onLogicLoop(){
    }
private:
    tgui::Grid::Ptr initGameList(){
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
};