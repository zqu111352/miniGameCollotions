#include <iostream>
#include <TGUI/TGUI.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>
#include "MyMenu.hpp"

unsigned int width = 1024;
unsigned int height = 768;

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

    // 3. 加载主题 (可选，TGUI 有默认主题，但加载自定义主题更美观)
    // TGUI 自带一些主题文件，通常在 share/TGUI/themes/ 目录下
    // 如果找不到路径，可以先跳过，使用默认样式
    try {
        gui.loadWidgetsFromFile("Black.theme"); 
    } catch (tgui::Exception& e) {
        std::cout << "Failed to load theme: " << e.what() << std::endl;
    }
    tgui::Font::setGlobalFont("font/SourceHanSerifCN-Medium.otf");

    
    MyMenu myMenu;
    myMenu.Init(gui);
    // 6. 主循环
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            gui.handleEvent(*event);
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        window.clear();

        gui.draw();

        window.display();
    }

    return -1;
}
