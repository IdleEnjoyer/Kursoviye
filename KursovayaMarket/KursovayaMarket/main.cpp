#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "Market.h"
#include "Player.h"
#include <iostream>

int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Market");//Создание окна размеров 800х600

    const int frameRate = 60;
    const sf::Time frameTime = sf::seconds(1.0f / frameRate); //Создание таймера, который будет отсчитывать 60 раз в секунду, для того, чтобы программа проводила свои операции с определённой частотой, а не частотой процессора
    sf::Time elapsed = sf::Time::Zero;

    sf::Clock clock;
    sf::Clock Timer2s;
    sf::Time TimerDuration = sf::seconds(2.f);
    sf::Time Timer = TimerDuration;//Создание таймера в 2 секунды, для симуляции рынка, где цена на акции меняется раз в 2 секунды

    sf::Font MainFont;
    if (!MainFont.loadFromFile("MenuFont.ttf")) {
        return 0;
    }//Загрузка шрифта, для отображения текста
    
    bool DifficultySet = false;//Была ли выбрана сложность игры?
    bool MarketReady = false;//Были ли загружены рынки?
    int CurrentMarket = 0;//ID текущего рынка, используется для отображения

    sf::View sectionView(sf::FloatRect(0, 0, window.getSize().x * 0.25f, window.getSize().y));//Создания секции, которая имитирует скроллинг, тк в sfml встроенного скроллинга нет :( здесь определяется размер области
    sectionView.setViewport(sf::FloatRect(0.0f, 0.0f, 0.25f, 1.0f));//Здесь определяется часть экрана, которая отображается в эту секцию
    float ViewTop = sectionView.getCenter().y;//Здесь запоминаем верхнюю позицию секции, чтобы скроллинг не уходил за границы

    Market* MarketList = NULL;
    Player* MarketPlayer = NULL;
    sf::Text* MarketNames = NULL;//Объявление классов Игрока, Рынка, и списка имен рынков

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

    int ListSize = 0;//Здесь храним количество рынков
    sf::VertexArray Graph(sf::LineStrip);//График рынка, заполняется позже

    sf::RectangleShape* Rectangles = NULL;//Прямоугольники списка рынков
    sf::Vector2f RecSize = { window.getSize().x * 0.25f, window.getSize().y * 0.1f };//Вычисляем размер прямоугольников
    
    //Кнопки выбора слоэности
    sf::Text EasyButton("Easy", MainFont, 30);
    EasyButton.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xff));
    sf::Text NormalButton("Normal", MainFont, 30);
    NormalButton.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xff));
    sf::Text HardButton("Hard", MainFont, 30);
    HardButton.setFillColor(sf::Color(0x00, 0x00, 0x00, 0xff));
    //1 часть интерфейса программы - узкий прямоугольник слева, тут будет список рынков
    sf::RectangleShape part1;
    part1.setSize(sf::Vector2f(window.getSize().x * 0.25f, window.getSize().y));
    part1.setPosition(0, 0);
    part1.setFillColor(sf::Color(0xbb, 0xbb, 0xbb, 0xff));

    //2 часть интерфейса - большой прямоугольник посередине, тут будет график рынка
    sf::RectangleShape part2;
    part2.setSize(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.65f));
    part2.setPosition(window.getSize().x * 0.25f, window.getSize().y * 0.1f);
    part2.setFillColor(sf::Color(0xe0, 0xe0, 0xe0, 0xff));

    //3 часть интерфейса - Узкая полоска наверху, тут будет информация о деньгах игрока
    sf::RectangleShape part3;
    part3.setSize(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.1f));
    part3.setPosition(window.getSize().x * 0.25f, 0);
    part3.setFillColor(sf::Color(0xbb, 0xbb, 0xbb, 0xff));

    //4 часть интерфейса - полоска снизу, тут будет взаимодействие игрока с рынком
    sf::RectangleShape part4;
    part4.setSize(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y * 0.25f));
    part4.setPosition(window.getSize().x * 0.25f, window.getSize().y * 0.75f);
    part4.setFillColor(sf::Color(0xbb, 0xbb, 0xbb, 0xff));

    //Здесь заполняет позиции кнопок относительно друг-друга
    EasyButton.setPosition(part1.getPosition().x + (-EasyButton.getGlobalBounds().width + part1.getGlobalBounds().width) / 2,
                           part1.getPosition().y);
    NormalButton.setPosition(part1.getPosition().x + (-NormalButton.getGlobalBounds().width + part1.getGlobalBounds().width)/ 2,
                             EasyButton.getPosition().y + EasyButton.getGlobalBounds().height);
    HardButton.setPosition(part1.getPosition().x + (-HardButton.getGlobalBounds().width + part1.getGlobalBounds().width) / 2,
                           NormalButton.getPosition().y + NormalButton.getGlobalBounds().height);
    //Основной игровой цикл, работающий, пока окно открыто
    while (window.isOpen()) {
        sf::Event event;//Событие и очередь событий, которые обрабатываются, пока очередь не пуста
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {//Если нажата кнопка закрыть - закрываем окно
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed) {//Если нажата кнопка мыши
                if (event.mouseButton.button == sf::Mouse::Left) {//Если она левая
                    if (!DifficultySet) {//Если слоэность не выбрана
                        if (EasyButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {//Определяем в какой кнопке мышка и изменяем цвет кнопки
                            EasyButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                        if (NormalButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            NormalButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                        if (HardButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
                            HardButton.setFillColor(sf::Color(0x80, 0x80, 0x80));
                        }
                    }
                    else {//Если сложность выбрана
                        for (int i = 0; i < ListSize; i++) {//Проходим по всем рынкам
                            sf::Vector2f viewMouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window), sectionView);//Проверяем в какой части секции со скроллингом находится мышь
                            if (Rectangles[i].getGlobalBounds().contains(viewMouseCoords)) {//Меняем цвет соответствующего рынка
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
            if (event.type == sf::Event::MouseButtonReleased) {//Если кнопка мыши отжата
                if (event.mouseButton.button == sf::Mouse::Left) {//Если она левая
                    if (!DifficultySet) {//Если сложность не установлена
                        EasyButton.setFillColor(sf::Color(0x00, 0x00, 0x00));
                        NormalButton.setFillColor(sf::Color(0x00, 0x00, 0x00));
                        HardButton.setFillColor(sf::Color(0x00, 0x00, 0x00));//Приводим все кнопки в изначальный вид
                        if (EasyButton.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {//Определяем в какой кнопке была отжата мышь
                            ListSize = 10;//Устанавливаем соответствующие параметры по сложности
                            MarketPlayer = new Player(20000);
                            DifficultySet = true;//Сложность выбрана
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
                        if (DifficultySet && !MarketReady) {//Если сложность установлена, а рынки нет
                            MarketNames = new sf::Text[ListSize];//Выделяем память под всё, что нужно
                            MarketList = new Market[ListSize];
                            Rectangles = new sf::RectangleShape[ListSize];
                            EasyButton.setString("");//"Убираем" кнопки
                            NormalButton.setString("");
                            HardButton.setString("");
                            PlayerMoney.setString("Current money:" + std::to_string(std::roundf(MarketPlayer->GetMoney())) + "$");//Рисуем остальные элементы интерфейса
                            PlayerMoney.setPosition(window.getSize().x * 0.25f, (window.getSize().y * 0.05f - PlayerMoney.getGlobalBounds().height) / 2.f);
                            InputText.setPosition(window.getSize().x * 0.25f, window.getSize().y * 0.75f + (window.getSize().y * 0.4f - InputText.getGlobalBounds().height) / 2);
                            BuyButton.setPosition(window.getSize().x * 0.275f, window.getSize().y * 0.75f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            SellButton.setPosition(window.getSize().x * 0.35f + BuyButton.getGlobalBounds().width, window.getSize().y * 0.75f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            CurrentCost.setString("Cost: " + std::to_string(MarketList[CurrentMarket].GetCost()));
                            CurrentCost.setPosition(window.getSize().x * 0.5f + SellButton.getGlobalBounds().width, window.getSize().y * 0.75f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            CurrentStocks.setPosition(window.getSize().x * 0.5f + SellButton.getGlobalBounds().width, window.getSize().y * 0.8f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            Profit.setPosition(window.getSize().x * 0.5f + SellButton.getGlobalBounds().width, window.getSize().y * 0.85f + (window.getSize().y * 0.1f - BuyButton.getGlobalBounds().height) / 2);
                            for (int i = 0; i < ListSize; i++) {//Задаём координаты и размеры для прямоугольников рынков
                                Rectangles[i].setPosition(0, i * RecSize.y);
                                Rectangles[i].setSize(RecSize);
                                Rectangles[i].setFillColor(sf::Color(i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40));
                                MarketList[i].GenerateValues();//Заполняем рынок значениями
                                MarketNames[i] = sf::Text(MarketList[i].GetName()->c_str() + sf::String(" - ") + std::to_string(MarketList[i].GetAmount()), MainFont, 30);//Устанавливаем текст рынка
                                MarketNames[i].setPosition(Rectangles[i].getPosition().x,
                                                           Rectangles[i].getPosition().y + MarketNames[i].getGlobalBounds().height / 2.0f);//Устанавливаем координаты для имен рынков
                            }
                            MarketReady = true;//Рынки готовы
                        }
                    }
                    else {//Если сложность выбрана
                        BuyButton.setFillColor(sf::Color::Black);
                        SellButton.setFillColor(sf::Color::Black);
                        for (int i = 0; i < ListSize; i++) {//Проходим по всем рынкам
                            sf::Vector2f viewMouseCoords = window.mapPixelToCoords(sf::Mouse::getPosition(window), sectionView);//Проверяем где в секции со скроллингом находится мышь
                            Rectangles[i].setFillColor(sf::Color(i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40, i % 2 * 0x20 + 0x40));//Восстанавливаем цвет прямоугольникв
                            if (Rectangles[i].getGlobalBounds().contains(viewMouseCoords)) {//проверяем в каком прямоугольнике мышь
                                CurrentMarket = i;//Значит мы находимся в этом рынке, запоминаем это
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
            if (event.type == sf::Event::MouseWheelScrolled) {//Если было прокручено колесико мыши
                float viewSpeed = sectionView.getSize().y / 10.0f; //Насколько сильно скроллим
                if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)//Если колесико вертикальное
                {
                    if (event.mouseWheelScroll.delta > 0 && sectionView.getCenter().y > ViewTop)//Если крутилось вверх, и мы не ушли за границу скроллинга
                    {
                        sectionView.move(0, -viewSpeed);//Двигаем секцию скроллинга
                    }
                    else if (event.mouseWheelScroll.delta < 0 && sectionView.getCenter().y <  - ViewTop + viewSpeed * ListSize)//Крутим вниз и не ушли за границу
                    {
                        sectionView.move(0, viewSpeed);//Двигаем
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

        elapsed += clock.restart();//Считаем сколько времени прошло
        if (DifficultySet) {//Запускаем таймер обновления рынка только после выбора сложности
            Timer -= Timer2s.restart();
        }
        
        
        if (window.hasFocus()) {//Если у окна есть фокус
            
            if (elapsed >= frameTime) {//Прошло больше 1/60 секунды
                window.clear(sf::Color::White);//Очистка окна

                window.draw(part1);//Рисуем части интерфейса
                window.draw(part2); 
                window.draw(part3);
                window.draw(part4);

                if (!DifficultySet) {//Рисуем кнопки, если не выбрали сложность
                    window.draw(EasyButton);
                    window.draw(NormalButton);
                    window.draw(HardButton);
                }

                if (DifficultySet && Rectangles != NULL && MarketNames != NULL) {//Выбрали сложность И прямоугольники И имена объявлены
                    window.setView(sectionView);//Выбираем часть скроллинга
                    for (int i = 0; i < ListSize; i++) {//Проходим по всем рынкам
                        if (i != CurrentMarket) {//Если это не выбранный рынок, то его цвет - белый
                            MarketNames[i].setFillColor(sf::Color::White);
                        }
                        else {// иначе желтый
                            MarketNames[i].setFillColor(sf::Color::Yellow);
                        }
                        window.draw(Rectangles[i]);//Рисуем прямоугольники
                        window.draw(MarketNames[i]);//Потом имена
                    }
                    window.setView(window.getDefaultView());//Выбираем стандартную секцию, уходим от скроллиновой
                    window.draw(Graph);//Рисуем график рынка
                    window.draw(PlayerMoney);//Рисуем деньги игрока
                    window.draw(InputText);//Рисуем вводной текст
                    window.draw(BuyButton);//Рисуем кнопку покупки
                    window.draw(SellButton);//Рисуем кнопку продажи
                    window.draw(CurrentCost);//Рисуем текущую стоимость выбранного рынка
                    window.draw(CurrentStocks);//Рисуем количество акций текущего рынка, которыми владеет игрок
                    window.draw(Profit);//Рисуем доход игрока на данном рынке
                }
                window.display();//отображаем всё, что нарисовали
                elapsed -= frameTime;//Сбрасываем время
            }
            if (Timer <= sf::Time::Zero && MarketList != NULL) {//Таймер в 2 секунды истёк
                Graph.clear();//очищаем график
                for (int i = 0; i < ListSize; i++) {//Симулируем изменения цен на всех рынках
                    MarketList[i].SimulateStep();
                }
                CurrentCost.setString("Cost: " + std::to_string(MarketList[CurrentMarket].GetCost()));//Получаем стоимость рынка
                std::pair<const std::string*, int> Stock = *MarketPlayer->GetStocks()->find(MarketList[CurrentMarket].GetName());//Получаем акции которыми владеет игрок
                if (Stock == *MarketPlayer->GetStocks()->end()) {//Проверяем владеет ли он акциями текущего рынка
                    CurrentStocks.setString("Stocks: " + std::to_string(0));//Если нет, то рисуем 0
                    Profit.setString("Profit: 0$");
                }
                else {
                    CurrentStocks.setString("Stocks: " + std::to_string(Stock.second));//Если да то рисуем количество акций
                    Profit.setString("Profit: " + std::to_string(Stock.second * MarketList[CurrentMarket].GetCost()) + "$");
                }
                
                std::queue<float> QueueToDraw;
                MarketList[CurrentMarket].GetHistory(&QueueToDraw);//Получаем значения, которые ранок имел в прошлом
                float maxValue = 0.f;//Ищем максимальное значение на рынке начиная здесь
                std::queue<float> Temp = QueueToDraw;
                while (!Temp.empty()) {
                    float Val = Temp.front();
                    Temp.pop();
                    if (Val > maxValue) {
                        maxValue = Val;
                    }
                }//Заканчивая здесь
                float xIncr = window.getSize().x * 0.65f / (QueueToDraw.size() - 1);//Делим горизонтально экран на несколько частей согласно тому, сколько у нас данных для рисования
                int i = 0;
                float PrevValue = QueueToDraw.front();//Запоминаем предыдущее значение, чтобы рисовать график зеленым\красным
                QueueToDraw.pop();
                while (!QueueToDraw.empty()) {//Проходим по всему списку
                    sf::Color VertColor = (PrevValue - QueueToDraw.front()) >= 0 ? sf::Color::Red : sf::Color::Green;//Определяем каким цветом рисовать, красным если цена упала, зеленым если выросла
                    PrevValue = QueueToDraw.front();//Запоминаем значение
                    float xPos = xIncr * i + window.getSize().x * 0.3f;//Где рисовать следующее значение по координате Х
                    float yPos = 0.75f * window.getSize().y - (0.65f * QueueToDraw.front() / maxValue) * window.getSize().y;//То же самое по у, но ограничиваем её в области большой части посередине
                    Graph.append(sf::Vertex(sf::Vector2f(xPos, yPos), VertColor));//Добавляем в график соответствующую точку с правильным цветом
                    QueueToDraw.pop();
                    i++;
                }
                Timer = TimerDuration;//Сбрасываем таймер
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
    return 0;//Выходим из программы :)
}