#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Market.h"
#include "Player.h"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Market");//�������� ���� �������� 800�600

    const int frameRate = 60;
    const sf::Time frameTime = sf::seconds(1.0f / frameRate); //�������� �������, ������� ����� ����������� 60 ��� � �������, ��� ����, ����� ��������� ��������� ���� �������� � ����������� ��������, � �� �������� ����������
    sf::Time elapsed = sf::Time::Zero;

    sf::Clock clock;
    sf::Clock Timer2s;
    sf::Time TimerDuration = sf::seconds(2.f);
    sf::Time Timer = TimerDuration;//�������� ������� � 2 �������, ��� ��������� �����, ��� ���� �� ����� �������� ��� � 2 �������

    sf::Font MainFont;
    if (!MainFont.loadFromFile("MenuFont.ttf")) {
        return 0;
    }//�������� ������, ��� ����������� ������
    
    bool DifficultySet = false;//���� �� ������� ��������� ����?
    bool MarketReady = false;//���� �� ��������� �����?
    int CurrentMarket = 0;//ID �������� �����, ������������ ��� �����������

    sf::View sectionView(sf::FloatRect(0, 0, window.getSize().x * 0.25f, window.getSize().y));//�������� ������, ������� ��������� ���������, �� � sfml ����������� ���������� ��� :( ����� ������������ ������ �������
    sectionView.setViewport(sf::FloatRect(0.0f, 0.0f, 0.25f, 1.0f));//����� ������������ ����� ������, ������� ������������ � ��� ������
    float ViewTop = sectionView.getCenter().y;//����� ���������� ������� ������� ������, ����� ��������� �� ������ �� �������

    Market* MarketList = NULL;
    Player* MarketPlayer = NULL;
    sf::Text* MarketNames = NULL;//���������� ������� ������, �����, � ������ ���� ������

    sf::Text PlayerMoney("", MainFont, 30);
    PlayerMoney.setFillColor(sf::Color::Black);

    sf::Text InputText("Amount to buy: <Press a key>", MainFont, 30);
    InputText.setFillColor(sf::Color::Black);
    std::string InputString;

    sf::Text BuyButton("Buy", MainFont, 30);
    BuyButton.setFillColor(sf::Color::Black);

    sf::Text SellButton("Sell", MainFont, 30);
    SellButton.setFillColor(sf::Color::Black);

    sf::Text CurrentCost("Cost: ", MainFont, 30);
    CurrentCost.setFillColor(sf::Color::Black);

    sf::Text CurrentStocks("Stocks: ", MainFont, 30);
    CurrentStocks.setFillColor(sf::Color::Black);

    sf::Text Profit("Profit: ", MainFont, 30);
    Profit.setFillColor(sf::Color::Black);

    int ListSize = 0;//����� ������ ���������� ������
    sf::VertexArray Graph(sf::LineStrip);//������ �����, ����������� �����

    sf::RectangleShape* Rectangles = NULL;//�������������� ������ ������
    sf::Vector2f RecSize = { window.getSize().x * 0.25f, window.getSize().y * 0.1f };//��������� ������ ���������������
    
    //������ ������ ���������
    sf::Text EasyButton("Easy", MainFont, 30);
    EasyButton.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xff));
    sf::Text NormalButton("Normal", MainFont, 30);
    NormalButton.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xff));
    sf::Text HardButton("Hard", MainFont, 30);
    HardButton.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xff));
    //1 ����� ���������� ��������� - ����� ������������� �����, ��� ����� ������ ������
    sf::RectangleShape part1;
    part1.setSize(sf::Vector2f(window.getSize().x * 0.25f, window.getSize().y));
    part1.setPosition(0, 0);
    part1.setFillColor(sf::Color(0xbb, 0xbb, 0xbb, 0xff));

    //2 ����� ���������� - ������� ������������� ����������, ��� ����� ������ �����
    sf::RectangleShape part2;
    part2.setSize(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.65f));
    part2.setPosition(window.getSize().x * 0.25f, window.getSize().y * 0.1f);
    part2.setFillColor(sf::Color(0xe0, 0xe0, 0xe0, 0xff));

    //3 ����� ���������� - ����� ������� �������, ��� ����� ���������� � ������� ������
    sf::RectangleShape part3;
    part3.setSize(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.1f));
    part3.setPosition(window.getSize().x * 0.25f, 0);
    part3.setFillColor(sf::Color(0xbb, 0xbb, 0xbb, 0xff));

    //4 ����� ���������� - ������� �����, ��� ����� �������������� ������ � ������
    sf::RectangleShape part4;
    part4.setSize(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.25f));
    part4.setPosition(window.getSize().x * 0.25f, window.getSize().y * 0.75f);
    part4.setFillColor(sf::Color(0xbb, 0xbb, 0xbb, 0xff));

    //����� ��������� ������� ������ ������������ ����-�����
    EasyButton.setPosition(part1.getPosition().x + (-EasyButton.getGlobalBounds().width + part1.getGlobalBounds().width) / 2,
                           part1.getPosition().y);
    NormalButton.setPosition(part1.getPosition().x + (-NormalButton.getGlobalBounds().width + part1.getGlobalBounds().width)/ 2,
                             EasyButton.getPosition().y + EasyButton.getGlobalBounds().height);
    HardButton.setPosition(part1.getPosition().x + (-HardButton.getGlobalBounds().width + part1.getGlobalBounds().width) / 2,
                           NormalButton.getPosition().y + NormalButton.getGlobalBounds().height);
    //�������� ������� ����, ����������, ���� ���� �������
    while (window.isOpen()) {
        sf::Event event;//������� � ������� �������, ������� ��������������, ���� ������� �� �����
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {//���� ������ ������ ������� - ��������� ����
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {//���� ������ ������ ����
                if (event.mouseButton.button == sf::Mouse::Left) {//���� ��� �����
                    if (!DifficultySet) {//���� ��������� �� �������
                        if (EasyButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {//���������� � ����� ������ ����� � �������� ���� ������
                            EasyButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                        if (NormalButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            NormalButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                        if (HardButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            HardButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                    }
                    else {//���� ��������� �������
                        for (int i = 0; i < ListSize; i++) {//�������� �� ���� ������
                            sf::Vector2f viewMouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window), sectionView);//��������� � ����� ����� ������ �� ����������� ��������� ����
                            if (Rectangles[i].getGlobalBounds().contains(viewMouseCoords)) {//������ ���� ���������������� �����
                                Rectangles[i].setFillColor(sf::Color(i % 2 * 0x20 + 0x70, i % 2 * 0x20 + 0x70, i % 2 * 0x20 + 0x70));
                            }
                        }
                        if (BuyButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            BuyButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                        if (SellButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            BuyButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseButtonReleased) {//���� ������ ���� ������
                if (event.mouseButton.button == sf::Mouse::Left) {//���� ��� �����
                    if (!DifficultySet) {//���� ��������� �� �����������
                        EasyButton.setFillColor(sf::Color(0x00, 0x00, 0x00));
                        NormalButton.setFillColor(sf::Color(0x00, 0x00, 0x00));
                        HardButton.setFillColor(sf::Color(0x00, 0x00, 0x00));//�������� ��� ������ � ����������� ���
                        if (EasyButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {//���������� � ����� ������ ���� ������ ����
                            ListSize = 10;//������������� ��������������� ��������� �� ���������
                            MarketPlayer = new Player(20000);
                            DifficultySet = true;//��������� �������
                        }
                        if (NormalButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            ListSize = 15;
                            MarketPlayer = new Player(15000);
                            DifficultySet = true;
                        }
                        if (HardButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            ListSize = 20;
                            MarketPlayer = new Player(10000);
                            DifficultySet = true;
                        }
                        if (DifficultySet && !MarketReady) {//���� ��������� �����������, � ����� ���
                            MarketNames = new sf::Text[ListSize];//�������� ������ ��� ��, ��� �����
                            MarketList = new Market[ListSize];
                            Rectangles = new sf::RectangleShape[ListSize];
                            EasyButton.setString("");//"�������" ������
                            NormalButton.setString("");
                            HardButton.setString("");
                            PlayerMoney.setString("Current money:" + std::to_string(std::roundf(MarketPlayer->GetMoney())) + "$");//������ ��������� �������� ����������
                            PlayerMoney.setPosition(window.getSize().x * 0.25f, (window.getSize().y * 0.05f - PlayerMoney.getGlobalBounds().height) / 2.f);
                            InputText.setPosition(window.getSize().x * 0.25f, window.getSize().y * 0.75f + (window.getSize().y * 0.4f - InputText.getGlobalBounds().height) / 2);
                            BuyButton.setPosition(window.getSize().x * 0.275f, window.getSize().y * 0.75f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            SellButton.setPosition(window.getSize().x * 0.35f + BuyButton.getGlobalBounds().width, window.getSize().y * 0.75f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            CurrentCost.setString("Cost: " + std::to_string(MarketList[CurrentMarket].GetCost()));
                            CurrentCost.setPosition(window.getSize().x * 0.5f + SellButton.getGlobalBounds().width, window.getSize().y * 0.75f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            CurrentStocks.setPosition(window.getSize().x * 0.5f + SellButton.getGlobalBounds().width, window.getSize().y * 0.8f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            Profit.setPosition(window.getSize().x * 0.5f + SellButton.getGlobalBounds().width, window.getSize().y * 0.85f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            for (int i = 0; i < ListSize; i++) {//����� ���������� � ������� ��� ��������������� ������
                                Rectangles[i].setPosition(0, i * RecSize.y);
                                Rectangles[i].setSize(RecSize);
                                Rectangles[i].setFillColor(sf::Color(i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40));
                                MarketList[i].GenerateValues();//��������� ����� ����������
                                MarketNames[i] = sf::Text(MarketList[i].GetName()->c_str() + sf::String(" - ") + std::to_string(MarketList[i].GetAmount()), MainFont, 30);//������������� ����� �����
                                MarketNames[i].setPosition(Rectangles[i].getPosition().x,
                                                           Rectangles[i].getPosition().y + MarketNames[i].getGlobalBounds().height / 2.0f);//������������� ���������� ��� ���� ������
                            }
                            MarketReady = true;//����� ������
                        }
                    }
                    else {//���� ��������� �������
                        BuyButton.setFillColor(sf::Color::Black);
                        SellButton.setFillColor(sf::Color::Black);
                        for (int i = 0; i < ListSize; i++) {//�������� �� ���� ������
                            sf::Vector2f viewMouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window), sectionView);//��������� ��� � ������ �� ����������� ��������� ����
                            Rectangles[i].setFillColor(sf::Color(i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40));//��������������� ���� ��������������
                            if (Rectangles[i].getGlobalBounds().contains(viewMouseCoords)) {//��������� � ����� �������������� ����
                                CurrentMarket = i;//������ �� ��������� � ���� �����, ���������� ���
                            }
                        }
                        if (BuyButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            int AmountToBuy = atoi(InputString.c_str());
                            if (AmountToBuy <= MarketList[CurrentMarket].GetAmount()) {
                                if (MarketPlayer->BuyStocks(&MarketList[CurrentMarket], AmountToBuy)) {
                                    PlayerMoney.setString("Current money: " + std::to_string(MarketPlayer->GetMoney())+"$");
                                    MarketNames[CurrentMarket].setString(MarketList[CurrentMarket].GetName()->c_str() + sf::String(" - ") + std::to_string(MarketList[CurrentMarket].GetAmount()));
                                }
                            }
                        }
                        if (SellButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            int AmountToSell = atoi(InputString.c_str());
                            if (MarketPlayer->SellStocks(&MarketList[CurrentMarket], AmountToSell)) {
                                PlayerMoney.setString("Current money: " + std::to_string(MarketPlayer->GetMoney())+"$");
                                MarketNames[CurrentMarket].setString(MarketList[CurrentMarket].GetName()->c_str() + sf::String(" - ") + std::to_string(MarketList[CurrentMarket].GetAmount()));
                            }
                        }
                    }
                }
            }
            if (event.type == sf::Event::MouseWheelScrolled) {//���� ���� ���������� �������� ����
                float viewSpeed = sectionView.getSize().y / 10.0f; //��������� ������ ��������
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)//���� �������� ������������
                {
                    if (event.mouseWheelScroll.delta > 0 && sectionView.getCenter().y > ViewTop)//���� ��������� �����, � �� �� ���� �� ������� ����������
                    {
                        sectionView.move(0, -viewSpeed);//������� ������ ����������
                    }
                    else if (event.mouseWheelScroll.delta < 0 && sectionView.getCenter().y <  - ViewTop + viewSpeed * ListSize)//������ ���� � �� ���� �� �������
                    {
                        sectionView.move(0, viewSpeed);//�������
                    }
                }
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode >= '0' && event.text.unicode <= '9') {
                    InputString += char(event.text.unicode);
                    InputText.setString("Amount to buy: " + InputString);
                }
                else if (event.text.unicode == '\b' && !InputString.empty()) {
                    InputString.pop_back();
                    InputText.setString("Amount to buy: " + InputString);
                }
            }
        }

        elapsed += clock.restart();//������� ������� ������� ������
        if (DifficultySet) {//��������� ������ ���������� ����� ������ ����� ������ ���������
            Timer -= Timer2s.restart();
        }
        
        
        if (window.hasFocus()) {//���� � ���� ���� �����
            
            if (elapsed >= frameTime) {//������ ������ 1/60 �������
                window.clear(sf::Color::White);//������� ����

                window.draw(part1);//������ ����� ����������
                window.draw(part2); 
                window.draw(part3);
                window.draw(part4);

                if (!DifficultySet) {//������ ������, ���� �� ������� ���������
                    window.draw(EasyButton);
                    window.draw(NormalButton);
                    window.draw(HardButton);
                }

                if (DifficultySet && Rectangles != NULL && MarketNames != NULL) {//������� ��������� � �������������� � ����� ���������
                    window.setView(sectionView);//�������� ����� ����������
                    for (int i = 0; i < ListSize; i++) {//�������� �� ���� ������
                        if (i != CurrentMarket) {//���� ��� �� ��������� �����, �� ��� ���� - �����
                            MarketNames[i].setFillColor(sf::Color::White);
                        }
                        else {// ����� ������
                            MarketNames[i].setFillColor(sf::Color::Yellow);
                        }
                        window.draw(Rectangles[i]);//������ ��������������
                        window.draw(MarketNames[i]);//����� �����
                    }
                    window.setView(window.getDefaultView());//�������� ����������� ������, ������ �� ������������
                    window.draw(Graph);//������ ������ �����
                    window.draw(PlayerMoney);//������ ������ ������
                    window.draw(InputText);//������ ������� �����
                    window.draw(BuyButton);//������ ������ �������
                    window.draw(SellButton);//������ ������ �������
                    window.draw(CurrentCost);//������ ������� ��������� ���������� �����
                    window.draw(CurrentStocks);//������ ���������� ����� �������� �����, �������� ������� �����
                    window.draw(Profit);//������ ����� ������ �� ������ �����
                }
                window.display();//���������� ��, ��� ����������
                elapsed -= frameTime;//���������� �����
            }
            if (Timer <= sf::Time::Zero && MarketList != NULL) {//������ � 2 ������� ����
                Graph.clear();//������� ������
                for (int i = 0; i < ListSize; i++) {//���������� ��������� ��� �� ���� ������
                    MarketList[i].SimulateStep();
                }
                CurrentCost.setString("Cost: " + std::to_string(MarketList[CurrentMarket].GetCost()));//�������� ��������� �����
                std::pair<const std::string*, int> Stock = *MarketPlayer->GetStocks()->find(MarketList[CurrentMarket].GetName());//�������� ����� �������� ������� �����
                if (Stock == *MarketPlayer->GetStocks()->end()) {//��������� ������� �� �� ������� �������� �����
                    CurrentStocks.setString("Stocks: " + std::to_string(0));//���� ���, �� ������ 0
                    Profit.setString("Profit: 0$");
                }
                else {
                    CurrentStocks.setString("Stocks: " + std::to_string(Stock.second));//���� �� �� ������ ���������� �����
                    Profit.setString("Profit: " + std::to_string(Stock.second * MarketList[CurrentMarket].GetCost()) + "$");
                }
                
                std::queue<float> QueueToDraw;
                MarketList[CurrentMarket].GetHistory(&QueueToDraw);//�������� ��������, ������� ����� ���� � �������
                float maxValue = 0.f;//���� ������������ �������� �� ����� ������� �����
                std::queue<float> Temp = QueueToDraw;
                while (!Temp.empty()) {
                    float Val = Temp.front();
                    Temp.pop();
                    if (Val > maxValue) {
                        maxValue = Val;
                    }
                }//���������� �����
                float xIncr = window.getSize().x * 0.65f / (QueueToDraw.size() - 1);//����� ������������� ����� �� ��������� ������ �������� ����, ������� � ��� ������ ��� ���������
                int i = 0;
                float PrevValue = QueueToDraw.front();//���������� ���������� ��������, ����� �������� ������ �������\�������
                QueueToDraw.pop();
                while (!QueueToDraw.empty()) {//�������� �� ����� ������
                    sf::Color VertColor = (PrevValue - QueueToDraw.front()) >= 0 ? sf::Color::Red : sf::Color::Green;//���������� ����� ������ ��������, ������� ���� ���� �����, ������� ���� �������
                    PrevValue = QueueToDraw.front();//���������� ��������
                    float xPos = xIncr * i + window.getSize().x * 0.3f;//��� �������� ��������� �������� �� ���������� �
                    float yPos = 0.75f * window.getSize().y - (0.65f * QueueToDraw.front() / maxValue) * window.getSize().y;//�� �� ����� �� �, �� ������������ � � ������� ������� ����� ����������
                    Graph.append(sf::Vertex(sf::Vector2f(xPos, yPos), VertColor));//��������� � ������ ��������������� ����� � ���������� ������
                    QueueToDraw.pop();
                    i++;
                }
                Timer = TimerDuration;//���������� ������
            }
        }
    }
    if (MarketList != nullptr) {
        delete[] MarketList;
    }
    if (MarketNames != nullptr) {
        delete[] MarketNames;
    }
    if (Rectangles != nullptr) {
        delete[] Rectangles;
    }
    return 0;//������� �� ��������� :)
}