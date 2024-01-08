#pragma once
#include <unordered_map>
#include <string>
#include "Market.h"
/// <summary>
/// Класс игрока
/// Содержит все аттрибуты и методы для работы с рынком
/// </summary>
class Player
{
	float Money = 0;
	std::unordered_map<const std::string*, int> Stocks;
	bool Bankrupt = false;

public:
	Player(float);
	~Player();
	bool BuyStocks(Market*, int);
	bool SellStocks(Market*, int);
	float GetMoney();
	const std::unordered_map<const std::string*, int>* GetStocks();
	bool IsBankrupt();

};

