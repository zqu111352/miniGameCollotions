#pragma once
#include <math.h>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "PageBase.h"
#include "PageMessage.h"
#include "MessageBus.h"
#include <random>
#include <cstdlib>
#include <ctime>

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

        initGameCanvas();

        //标题
        auto label = tgui::Label::create();
        label->setText(u8"贪吃蛇");
        label->setTextSize(36);
        label->setPosition("50% - width / 2", 10);
        snakeGameGroup->add(label); // 将控件添加到 Gui

        m_menuButton = initButton();
        snakeGameGroup->add(m_menuButton);

        //退出
        auto exitButton = tgui::Button::create();
        exitButton->setText(u8"退出");
        exitButton->setSize(150, 50);
        // 核心定位代码：让按钮右下角对齐窗口右下角，留20像素边距
        exitButton->setPosition("100% - width - 20", "100% - height - 20");
        exitButton->onPress([&]{
            PageChange msg{PAGE_STATE_DESTROY, PAGE_ID_SNAKE}; // 退出游戏
            MessageBus::getInstance().emit(msg);
            stopGame();
        });
        snakeGameGroup->add(exitButton);
        return snakeGameGroup;
    }
    void onShow() {
        if(!m_visible){
            m_visible = !m_visible;
            snakeGameGroup->setVisible(m_visible);
        }
        if(m_bGameStart){
            m_Canvas->draw(*m_gameBackGroud);
            // 绘制食物
            m_foodShape->setPosition({m_food.x * GRID_SIZE + 1, m_food.y * GRID_SIZE + 1});
            m_Canvas->draw(*m_foodShape);

            for(auto i = 0; i < m_snake.size(); i++){
                std::shared_ptr<sf::RectangleShape> snakeUnit = nullptr;
                if(0 == i){
                    snakeUnit = m_snakeHead;
                }else{
                    snakeUnit = m_snakeBody;
                }
                snakeUnit->setPosition({m_snake[i].x * GRID_SIZE + 1, m_snake[i].y * GRID_SIZE + 1});
                m_Canvas->draw(*snakeUnit);
            }

            m_Canvas->display();
        }
    }
    void onHide(){
        if(m_visible){
            m_visible = !m_visible;
            snakeGameGroup->setVisible(m_visible);
        }
    }
    bool HandleEvent(sf::Event event){
        if (m_gameState == GameState::Playing && event.is<sf::Event::KeyPressed>())
        {
            switch (event.getIf<sf::Event::KeyPressed>()->code)
            {
            case sf::Keyboard::Key::Up:
                //std::cout<<"Up:"<<(int)m_dir<<std::endl;
                if (m_dir != Direction::Down) m_nextDir = Direction::Up;
                break;
            case sf::Keyboard::Key::Down:
                //std::cout<<"Down:"<<(int)m_dir<<std::endl;
                if (m_dir != Direction::Up) m_nextDir = Direction::Down;
                break;
            case sf::Keyboard::Key::Left:
                //std::cout<<"Left:"<<(int)m_dir<<std::endl;
                if (m_dir != Direction::Right) m_nextDir = Direction::Left;
                break;
            case sf::Keyboard::Key::Right:
                //std::cout<<"Right:"<<(int)m_dir<<std::endl;
                if (m_dir != Direction::Left) m_nextDir = Direction::Right;
                break;
            }
        }
        return false;
    }

    void onLogicLoop(){
        gameLogic();
    }

    int getPageId(){
        return m_pageId;
    }
    tgui::Group::Ptr getCurrentGroup(){
        return snakeGameGroup;
    }
private:
    void initGameCanvas(){
        m_gameGroup = tgui::Group::create();
        m_gameGroup->setSize(m_size.x, m_size.y);
        m_gameGroup->setVisible(m_bGameStart);
        //初始化游戏画布
        m_gameSize = {m_size.x/2/GRID_SIZE, m_size.y/2/GRID_SIZE};
        tgui::CanvasSFML::Ptr canvas = tgui::CanvasSFML::create({m_gameSize.x*GRID_SIZE, m_gameSize.y*GRID_SIZE});
        canvas->setPosition("50% - width / 2", "50% - height / 2");
        m_Canvas = canvas;
        m_gameGroup->add(canvas);
        m_maxSnakeSize = m_gameSize.x*m_gameSize.y;

        //初始化游戏界面的元素
        m_gameBackGroud = std::make_shared<sf::RectangleShape>(canvas->getSize());
        m_gameBackGroud->setFillColor(sf::Color::Black);
        m_gameBackGroud->setOutlineThickness(-1);
        m_gameBackGroud->setOutlineColor(sf::Color(255,75,75));

        m_foodShape = std::make_shared<sf::RectangleShape>(sf::Vector2f(GRID_SIZE - 2, GRID_SIZE - 2));
        m_foodShape->setFillColor(sf::Color::Green);
        m_snakeHead = std::make_shared<sf::RectangleShape>(sf::Vector2f(GRID_SIZE - 2, GRID_SIZE - 2));
        m_snakeHead->setFillColor(sf::Color::Blue);
        m_snakeBody = std::make_shared<sf::RectangleShape>(sf::Vector2f(GRID_SIZE - 2, GRID_SIZE - 2));
        m_snakeBody->setFillColor(sf::Color(70, 70, 155));
        // 分数标签
        m_scoreLabel = tgui::Label::create("分数: 0");
        m_scoreLabel->setPosition(m_Canvas->getPosition().x + 20, m_Canvas->getPosition().y - 35);
        m_scoreLabel->setTextSize(22);
        m_scoreLabel->setFocusable(false); // 禁止聚焦，避免抢方向键
        m_gameGroup->add(m_scoreLabel);

        // 状态提示标签（居中）
        m_statusLabel = tgui::Label::create(u8"点击开始按钮开始游戏");
        m_statusLabel->setPosition("50% - width / 2", "50% - height / 2");
        m_statusLabel->setTextSize(24);
        m_statusLabel->getRenderer()->setTextColor(sf::Color::Yellow);
        m_statusLabel->setFocusable(false);
        m_gameGroup->add(m_statusLabel);
        // 开始/重新开始按钮
        m_restartBtn = tgui::Button::create(u8"开始游戏");
        m_restartBtn->setPosition("50% - width / 2", "50% - height / 2 + 40");
        m_restartBtn->setSize(150, 40);
        m_restartBtn->setTextSize(20);
        m_restartBtn->setFocusable(false); // 禁止聚焦，避免抢方向键
        m_restartBtn->onPress([&]()
        {
            resetGame();
            m_gameState = GameState::Playing;
            m_statusLabel->setVisible(false);
            m_restartBtn->setText(u8"重新开始");
            m_restartBtn->setVisible(false);
        });
        m_gameGroup->add(m_restartBtn);

        snakeGameGroup->add(m_gameGroup);
    }
    void startGame(){
        m_bGameStart = true;
        m_gameGroup->setVisible(m_bGameStart);
        m_menuButton->setVisible(false);
        m_menuButton->setFocusable(false); // 禁止聚焦，避免抢方向键
        // 初始化游戏数据
        resetGame();
        m_gameState = GameState::Ready;
        m_statusLabel->setVisible(true);
        m_statusLabel->setText(u8"点击开始按钮开始游戏");
        m_restartBtn->setVisible(true);
    }
    void stopGame(){
        m_bGameStart = false;
        m_gameGroup->setVisible(m_bGameStart);
        m_menuButton->setVisible(true);
        m_menuButton->setFocusable(true);
    }
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
            if(m_menuButton){
                m_menuButton->setVisible(false);
            }
            startGame();
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
    void spawnFood(){
        if(m_maxSnakeSize <= m_snake.size())
            return;
        while (true)
        {
            m_food.x = rand() % m_gameSize.x;
            m_food.y = rand() % m_gameSize.y;
            bool onSnake = false;
            for (auto& segment : m_snake)
            {
                if (segment == m_food)
                {
                    onSnake = true;
                    break;
                }
            }
            if (!onSnake) break;
        }
    }
    // 重置游戏
    void resetGame()
    {
        m_snake.clear();
        m_snake.emplace_back(sf::Vector2i{2,0});
        m_snake.emplace_back(sf::Vector2i{1,0});
        m_snake.emplace_back(sf::Vector2i{0,0});
        m_dir = Direction::Right;
        m_nextDir = Direction::Right;
        m_score = 0;
        m_moveInterval = INIT_MOVE_INTERVAL;
        m_timeSinceLastMove = 0;
        m_scoreLabel->setText("分数: 0");
        spawnFood();
    }
    void gameLogic(){
        // 游戏逻辑更新
        if (m_gameState == GameState::Playing)
        {
            float deltaTime = m_clock.restart().asSeconds();
            m_timeSinceLastMove += deltaTime;
            if (m_timeSinceLastMove >= m_moveInterval)
            {
                m_timeSinceLastMove = 0;
                //std::cout<<"cur:"<<(int)m_dir<<"next:"<<(int)m_nextDir<<std::endl;
                m_dir = m_nextDir;

                // 计算新蛇头位置
                sf::Vector2i head = m_snake.front();
                switch (m_dir)
                {
                case Direction::Up: head.y--; break;
                case Direction::Down: head.y++; break;
                case Direction::Left: head.x--; break;
                case Direction::Right: head.x++; break;
                }

                // 撞墙检测
                bool gameOver = false;
                if (head.x < 0 || head.x >= m_gameSize.x || head.y < 0 || head.y >= m_gameSize.y)
                    gameOver = true;

                // 撞自身检测
                if (!gameOver)
                {
                    for (auto& segment : m_snake)
                    {
                        if (segment == head)
                        {
                            gameOver = true;
                            break;
                        }
                    }
                }

                if (gameOver)
                {
                    m_gameState = GameState::GameOver;
                    m_statusLabel->setVisible(true);
                    m_statusLabel->setText(u8"游戏结束！最终分数: " + std::to_string(m_score) + u8"，点击重新开始");
                    // 重新居中状态标签
                    m_statusLabel->setPosition("50% - width / 2", "50% - height / 2");
                    m_restartBtn->setVisible(true);
                }
                else
                {
                    m_snake.insert(m_snake.begin(), head);
                    // 吃食物检测
                    if (head == m_food)
                    {
                        m_score += 1;
                        m_scoreLabel->setText(u8"分数: " + std::to_string(m_score));
                        m_moveInterval = std::max(MIN_MOVE_INTERVAL, m_moveInterval - SPEED_UP_RATE);
                        spawnFood();
                    }
                    else
                    {
                        m_snake.pop_back();
                    }
                }
            }
        }
    }
private:
    tgui::Group::Ptr snakeGameGroup;
    sf::Vector2u m_size;
    bool m_visible = false;
    int m_pageId;
    tgui::VerticalLayout::Ptr m_menuButton = nullptr;
    tgui::Group::Ptr m_gameGroup = nullptr;
    //游戏相关
    // 移动方向
    enum class Direction { Up, Down, Left, Right };
    // 游戏状态
    enum class GameState { Ready, Playing, GameOver };
    const float INIT_MOVE_INTERVAL = 0.2f; // 初始移动间隔（秒）
    const float SPEED_UP_RATE = 0.02f;      // 每次吃食物加速值
    const float MIN_MOVE_INTERVAL = 0.05f;  // 最小移动间隔（最快速度）
    int m_maxSnakeSize = 0;
    bool m_bGameStart = false;
    sf::Clock m_clock;
    tgui::CanvasSFML::Ptr m_Canvas = nullptr;
    std::shared_ptr<sf::RectangleShape> m_gameBackGroud = nullptr;
    const int GRID_SIZE = 10;
    sf::Vector2u m_gameSize;
    Direction m_dir = Direction::Right;
    Direction m_nextDir = Direction::Right;
    sf::Vector2i m_food;
    std::shared_ptr<sf::RectangleShape> m_foodShape = nullptr;
    std::shared_ptr<sf::RectangleShape> m_snakeHead = nullptr;
    std::shared_ptr<sf::RectangleShape> m_snakeBody = nullptr;
    std::vector<sf::Vector2i> m_snake = {{2,0},{1,0},{0,0}};
    int m_score = 0;
    float m_moveInterval = INIT_MOVE_INTERVAL;
    float m_timeSinceLastMove = 0;
    GameState m_gameState = GameState::Ready;
    tgui::Label::Ptr m_scoreLabel = nullptr;
    tgui::Label::Ptr m_statusLabel = nullptr;
    tgui::Button::Ptr m_restartBtn = nullptr;
};