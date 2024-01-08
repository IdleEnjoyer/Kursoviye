#include "Player.h"

Player::Player(float InputMoney) {
	Player::Money = InputMoney;
}

Player::~Player() {
	delete this;
}

bool Player::BuyStocks(Market* TargetMarket, int AmountToBuy) {//Покупка акций
	if (AmountToBuy * TargetMarket->GetCost() > Money) {
		return false;
	}
	else {
		Money -= AmountToBuy * TargetMarket->GetCost();
		if (Stocks.find(TargetMarket->GetName()) == Stocks.end()) {
			Stocks.insert({ TargetMarket->GetName(), AmountToBuy });
		}
		else {
			Stocks.at(TargetMarket->GetName()) += AmountToBuy;
		}
		
		TargetMarket->ChangeAmountBy(-AmountToBuy);
		return true;
	}
}

bool Player::SellStocks(Market* TargetMarket, int AmountToSell) {//Продажа акций
	if (AmountToSell > Stocks[TargetMarket->GetName()]) {
		return false;
	}
	else {
		Money += AmountToSell * TargetMarket->GetCost();
		TargetMarket->ChangeAmountBy(AmountToSell);
		Stocks.erase(TargetMarket->GetName());
		return true;
	}
}

float Player::GetMoney() {//Получить кол-во денег у игрока
	return Money;
}

bool Player::IsBankrupt() {//Является ли игрок банкротом
	return Bankrupt;
}

const std::unordered_map<const std::string*, int>* Player::GetStocks() {//получить словарь с именами рынков и кол-вом акций на рынке
	return &Stocks;
}