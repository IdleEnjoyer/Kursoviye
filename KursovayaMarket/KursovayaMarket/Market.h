#pragma once
#include <queue>
#include <string>
/// <summary>
///  ласс рынка
/// —одержит все аттрибуты и методы нужные дл€ симул€ции и взаимодействи€ с игроком
/// </summary>
class Market
{
private:
	float StockCost;
	int StockAmount;
	const int MaxSize = 100;
	float MarketVolatility;
	std::queue<float>* MarketHistory = NULL;
	std::string MarketName;

public:
	Market();
	~Market();
	void SimulateStep();
	void GenerateValues();
	float GetCost();
	int GetAmount();
	void ChangeAmountBy(int);
	void GetHistory(std::queue<float>*);
	const std::string* GetName();

};

