#include "DBUtils.h"
#include <Wt/Dbo/Impl>

//DBO_INSTANTIATE_TEMPLATES(Catalog);
namespace db
{



DBUtils::DBUtils()
{
    //ctor
}

DBUtils::~DBUtils()
{
    //dtor
}

Catalog::Catalog():price(0),quantity(0)
{

}
/*
Folders_::Folders_()
{

}
*/
GoodsSimple::GoodsSimple(){};
GoodsHistory::GoodsHistory(){};
Price::Price(){};
PersonalPrice::PersonalPrice(){};

OrdersHeader::OrdersHeader():state(OrdersHeader::modifed),orderId(0),orderDate(WDateTime::currentDateTime()),orderSumm(0.0){};
OrderRecords::OrderRecords():state(OrderRecords::newed),lineNumber(0),quantity(0),price(0),total(0){};
Comments::Comments(){};
Message::Message(){};
CalcOptions::CalcOptions(){};

bool DBUtils::query(const int &user_id,const int &action_key,const std::string &query)
{
    //Connect con();
    return true;
};

Catalog::Catalog(const Catalog& cat)
{
    //ref              = cat.ref;
    parentRef          = cat.parentRef;
    owner           = cat.owner;
//    page            = cat.page;
    division        = cat.division;
    //groupType       = cat.groupType;
    name            = cat.name;
    code            = cat.code;
    articul         = cat.articul;
    upakovka        = cat.upakovka;
    notVisible      = cat.notVisible;
    level           = cat.level;
    isGroup         = cat.isGroup;
    fileName        = cat.fileName;
    shortDescription= cat.shortDescription;
    description     = cat.description;
    price           = cat.price;
    quantity        = cat.quantity;

}
/*
OrdersHeader::OrdersHeader(const OrdersHeader& ord)
{
    orderId     = ord.orderId;
    author      = ord.author;
    orderDate   = ord.orderDate;
    state       = ord.state;
    sysNumber   = ord.sysNumber;
    sessionId   = ord.sessionId;
    orderSumm   = ord.orderSumm;

}
*/
}
int str_36_2_ten(std::string str36)
{
	std::string symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	//СтСтр = ВРЕГ(СокрЛП(Стр36));
	int number = 0;
	for (size_t i=0;i<str36.length();i++)
	{
		number *= 36;
		number += symbols.find(str36[i]) - 1;
	}


	return number;

}

int getChar(std::string str)
{
    std::string symbols = "АБВГДЕЁЖЗИКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзиклмнопрстуфчцчшщъыьэюя";
    int res = 0;
	for (size_t i=0;i<str.length();i++)
    {
        res = symbols.find(str[i]);
        if(res)
            break;
    }
    return res;
}

int getNumber(std::string str)
{
    std::string symbols = "0123456789";
    int res = 0;
	for (size_t i=0;i<str.length();i++)
    {
        res = symbols.find(str[i]);
        if(res)
            break;
    }
    return res;
}

//*******************************************
std::string str_ten_to_36(int number)
{
	std::string symbols = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int beginNum = number;
	int oldNumber = 0;
	std::string str36 = "";
	while (beginNum)
	{
		oldNumber = beginNum - beginNum / 36 * 36;

		str36 =  symbols[oldNumber + 1] + str36;

		beginNum /=  36;

	};

	return str36;
}

