#include "Market.h"
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <random>

Market::Market() {
	StockCost = 0;
	StockAmount = 0;
	MarketVolatility = 0;
	MarketName = "";
}

Market::~Market() {
	delete MarketHistory;
	delete this;
}

void Market::GenerateValues() {
	std::random_device rd;//генератор случайных чисел
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(0.1, 0.7);
	std::uniform_int_distribution<int> int_dist(5000,15000);
	std::uniform_int_distribution<int> int_Amount(1000, 20000);
	Market::MarketVolatility = dist(gen);//Случайная генерация значений для рынка
	Market::StockCost = static_cast<float>(int_dist(gen)) / 100;
	Market::StockAmount = int_Amount(gen);
	Market::MarketHistory = new std::queue<float>;
	Market::MarketHistory->push(Market::StockCost);
	for (int Letter = 0; Letter < 4; Letter++) {
		Market::MarketName += 'A' + int_dist(gen) % ('Z' - 'A');
	}
}

const std::string* Market::GetName() {//Получить имя рынка
	return &MarketName;
}

float Market::GetCost() {//Получить цену акции
	return StockCost;
}

int Market::GetAmount() {//Получить кол-во акций
	return StockAmount;
}

void Market::ChangeAmountBy(int Amount) {//Изменить кол-во акций на ...
	StockAmount + Amount < 0 ? StockAmount = 0 : StockAmount += Amount;
}

void Market::GetHistory(std::queue<float>* InputQueue) {//Получить историю рынка
	*InputQueue = *MarketHistory;
}

void Market::SimulateStep() {//симуляция одного цикла рынка
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(1 - MarketVolatility / 2, 1 + MarketVolatility / 2);

	StockCost *= dist(gen);
	if (MarketHistory->size() == MaxSize) {
		MarketHistory->pop();		
	}
	MarketHistory->push(StockCost);
}