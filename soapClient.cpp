#include "soapClient.h"
//#include "ConvertStrings.h"

#include "gSoap/soapInfoBaseSoapBindingProxy.h"
#include "gSoap/InfoBaseSoapBinding.nsmap"


#include <Wt/WApplication>
#include <Wt/WDate>
#include <Wt/WStandardItemModel>
#include <Wt/WStandardItem>
//#include <Wt/WLocalizedStrings>

SoapClient::ConnectionMap SoapClient::m_soapConnectionMap = {
    {"spb","http://192.168.11.17/FIT-SPB/ws/InfoBase"},
    {"kzt","http://192.168.10.26/FIT-KZ/ws/InfoBase"},
    {"ukr","http://192.168.10.26/FIT-UA/ws/InfoBase"},
    {"nsk","http://192.168.10.26/FIT-NSK/ws/InfoBase"},
    {"krd","http://192.168.10.26/FIT-KRD/ws/InfoBase"},
    {"kzn","http://192.168.10.26/FIT-KZN/ws/InfoBase"},
    {"ekt","http://192.168.10.26/FIT-EKB/ws/InfoBase"},
    {"spb_1","http://192.168.11.17/ONLINE-SPB/ws/InfoBase"}
};

SoapClient::SoapClient(const std::string &endPoint)
{
    m_client = new InfoBaseSoapBindingProxy();
    WApplication::readConfigurationProperty("soapUser",soapUserName);
    WApplication::readConfigurationProperty("soapPassword",soapUserPasswd);
    std::string value = m_soapConnectionMap[endPoint];
#ifdef DEBUG
    //WApplication::readConfigurationProperty("soapEndPoint",value);
#endif
    log("SOAP")<<"Регион "<<endPoint<<" End point "<<value;
    log("SOAP")<<"User "<<soapUserName;
    log("SOAP")<<"passwd "<<soapUserPasswd;
    m_client->soap_endpoint = value.c_str();//m_soapConnectionMap[endPoint].c_str();
    m_client->recv_timeout = 10;


}

SoapClient::~SoapClient()
{
    delete m_client;
}



std::vector<WString> SoapClient::getUserDept(const std::string userCode) const
{
    std::vector<WString> result;

    std::wstring wquery = INLINE_WQUERY(

            ВЫБРАТЬ
                ВзаиморасчетыСКонтрагентамиОстатки.ВалютаДокумента.Наименование,
                СУММА(ВзаиморасчетыСКонтрагентамиОстатки.СуммаВзаиморасчетовОстаток) КАК СуммаВзаиморасчетовОстаток,
                СУММА(ВЫБОР
                        КОГДА РАЗНОСТЬДАТ(РеализацияТоваровУслугСхемаОплаты.Дата, ДАТАВРЕМЯ({1},{2},{3},0,0,0), ДЕНЬ) > 0
                            ТОГДА ВзаиморасчетыСКонтрагентамиОстатки.СуммаВзаиморасчетовОстаток
                        ИНАЧЕ 0
                    КОНЕЦ) КАК СуммаПросроченныхОплат,
                МАКСИМУМ(РАЗНОСТЬДАТ(РеализацияТоваровУслугСхемаОплаты.Дата, ДАТАВРЕМЯ({1},{2},{3},0,0,0), ДЕНЬ)) КАК ПросроченоДней
            ИЗ
                РегистрНакопления.ВзаиморасчетыСКонтрагентами.Остатки(, Контрагент  В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{4}")) КАК ВзаиморасчетыСКонтрагентамиОстатки
                    ЛЕВОЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг.СхемаОплаты КАК РеализацияТоваровУслугСхемаОплаты
                    ПО ВзаиморасчетыСКонтрагентамиОстатки.Сделка = РеализацияТоваровУслугСхемаОплаты.Ссылка

            СГРУППИРОВАТЬ ПО
                ВзаиморасчетыСКонтрагентамиОстатки.ВалютаДокумента

		    );//.arg(2014).arg(6).arg(27).arg(WString::fromUTF8("Л0005604"));

    WDate date = WDate::currentDate();
    WString squery = WString(wquery).arg(date.year()).arg(date.month()).arg(date.day()).arg(WString::fromUTF8(userCode));
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();
    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
    {
        for (auto values = respone.return_->row.begin(); values != respone.return_->row.end(); ++values)
        //auto values = respone.return_->row.begin();
        //for (auto k = 0;i<(*i)->Value.size();k++)
        for (auto i = (*values)->Value.begin(); i != (*values)->Value.end(); ++i)
        {
            result.push_back(WString(*i));
        }

    }
    return result;
}

std::vector<std::vector<WString> >SoapClient::getDeptStucture(std::string userCode)const
{
    std::vector<std::vector<WString> > result;
    std::wstring wquery = INLINE_WQUERY(
            ВЫБРАТЬ
                ВзаиморасчетыОстатки.Сделка,
                ВзаиморасчетыОстатки.СуммаВзаиморасчетовОстаток КАК Сумма,
                ВзаиморасчетыОстатки.ВалютаДокумента
            ПОМЕСТИТЬ Остатки
            ИЗ
                РегистрНакопления.ВзаиморасчетыСКонтрагентами.Остатки(
                        ДАТАВРЕМЯ({2},{3},{4},0,0,0),
                        Контрагент В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{1}")) КАК ВзаиморасчетыОстатки
            ;

            ////////////////////////////////////////////////////////////////////////////////
            ВЫБРАТЬ
                Остатки.Сделка,
                Остатки.Сумма,
                Остатки.ВалютаДокумента,
                РеализацияТоваровУслугСхемаОплаты.Дата КАК ДатаОплаты,
                РАЗНОСТЬДАТ(РеализацияТоваровУслугСхемаОплаты.Дата, ДАТАВРЕМЯ({2},{3},{4},0,0,0), ДЕНЬ) КАК ДнейПросрочено
            ПОМЕСТИТЬ Взаиморасчёты
            ИЗ
                Остатки КАК Остатки
                    ЛЕВОЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг.СхемаОплаты КАК РеализацияТоваровУслугСхемаОплаты
                    ПО Остатки.Сделка = РеализацияТоваровУслугСхемаОплаты.Ссылка
                        И (РеализацияТоваровУслугСхемаОплаты.Дата В
                            (ВЫБРАТЬ
                                МАКСИМУМ(СхемаОплаты.Дата)
                            ИЗ
                                Документ.РеализацияТоваровУслуг.СхемаОплаты КАК СхемаОплаты
                            ГДЕ
                                РеализацияТоваровУслугСхемаОплаты.Ссылка = СхемаОплаты.Ссылка))
            ;

            ////////////////////////////////////////////////////////////////////////////////
            ВЫБРАТЬ
                РеализацияТоваровУслуг.ИД_Документа,
                Взаиморасчёты.ДатаОплаты,
                ПРЕДСТАВЛЕНИЕ(Взаиморасчёты.Сделка) КАК Сделка,
                ВЫБОР
                    КОГДА ЕСТЬNULL(Взаиморасчёты.ДнейПросрочено, 0) > 0
                        ТОГДА Взаиморасчёты.ДнейПросрочено
                    ИНАЧЕ NULL
                КОНЕЦ КАК ПросроченоДней,
                ВЫБОР
                    КОГДА ЕСТЬNULL(Взаиморасчёты.ДнейПросрочено, 0) > 0
                        ТОГДА Взаиморасчёты.Сумма
                    ИНАЧЕ NULL
                КОНЕЦ КАК ПросроченоСумма,
                ВЫБОР
                    КОГДА Взаиморасчёты.ДнейПросрочено ЕСТЬ NULL
                        ТОГДА NULL
                    КОГДА Взаиморасчёты.ДнейПросрочено <= 0
                        ТОГДА -Взаиморасчёты.ДнейПросрочено
                    ИНАЧЕ NULL
                КОНЕЦ КАК ОжидаетсяДней,
                ВЫБОР
                    КОГДА Взаиморасчёты.ДнейПросрочено ЕСТЬ NULL
                        ТОГДА NULL
                    КОГДА Взаиморасчёты.ДнейПросрочено <= 0
                        ТОГДА Взаиморасчёты.Сумма
                    ИНАЧЕ NULL
                КОНЕЦ КАК ОжидаетсяСумма,
                Взаиморасчёты.Сумма КАК ВсегоСумма
            ИЗ
                Взаиморасчёты КАК Взаиморасчёты
                    ЛЕВОЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
                    ПО Взаиморасчёты.Сделка = РеализацияТоваровУслуг.Ссылка

            УПОРЯДОЧИТЬ ПО
                Взаиморасчёты.ДнейПросрочено УБЫВ
		            );

    WDate date = WDate::currentDate();
    //wquery = tr("soap.get.dept.structure");
    WString squery = WString(wquery).arg(WString::fromUTF8(userCode)).arg(date.year()).arg(date.month()).arg(date.day());
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            auto values = *i;
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = values->Value.begin(); k != values->Value.end(); ++k)
            {
                columns.push_back(WString(*k));
            }

            result.push_back(columns);
        }

    return result;
}

std::vector<std::vector<WString> >SoapClient::getOrdersShipping(const WDate &beginDate,const WDate &endDate,const std::string userCode) const
{
    std::vector<std::vector<WString> > result;
    std::wstring wquery = INLINE_WQUERY(
        ВЫБРАТЬ
            ЗаказПокупателя.ИД_Документа КАК ИД_Заявки,
            ВЫБОР
                КОГДА ЗаказПокупателя.Проведен
                    ТОГДА 1
                ИНАЧЕ 0
            КОНЕЦ КАК СтатусЗаказа,
            ЗаказПокупателя.Ссылка КАК Сделка,
            ЗаказПокупателя.Номер КАК НомерЗаказа,
            ЗаказПокупателя.СуммаДокумента КАК СуммаНеотгр,
            ЗаказПокупателя.Комментарий,
            ЗаказПокупателя.Дата
        ПОМЕСТИТЬ Заявки
        ИЗ
            Документ.ЗаказПокупателя КАК ЗаказПокупателя
        ГДЕ
            ЗаказПокупателя.Контрагент В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{1}")
            И ЗаказПокупателя.Дата МЕЖДУ ДАТАВРЕМЯ({2},{3},{4},0,0,0) И ДАТАВРЕМЯ({5},{6},{7},23,59,59)
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            Заявки.Дата КАК ДатаЗаявки,
            Заявки.ИД_Заявки,
            Заявки.СтатусЗаказа,
            Заявки.НомерЗаказа,
            Заявки.СуммаНеотгр,
            Заявки.Комментарий,
            Заявки.Дата,
            "" КАК ИД_Реализации,
            "" КАК Реализация,
            0 КАК СуммаРеализации
        ПОМЕСТИТЬ Заявки1
        ИЗ
            Заявки КАК Заявки

        ОБЪЕДИНИТЬ ВСЕ

        ВЫБРАТЬ
            Заявки.Дата,
            Заявки.ИД_Заявки,
            "",
            "",
            0,
            "",
            РеализацияТоваровУслуг.Дата,
            РеализацияТоваровУслуг.ИД_Документа,
            РеализацияТоваровУслуг.Номер,
            РеализацияТоваровУслуг.СуммаДокумента
        ИЗ
            Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
                ВНУТРЕННЕЕ СОЕДИНЕНИЕ Заявки КАК Заявки
                ПО РеализацияТоваровУслуг.ДокументОснование = Заявки.Сделка
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            Заявки1.ИД_Заявки КАК ИД_Заявки,
            Заявки1.СтатусЗаказа,
            Заявки1.НомерЗаказа,
            Заявки1.СуммаНеотгр,
            Заявки1.Комментарий,
            Представление(Заявки1.Дата) Дата,
            Заявки1.ИД_Реализации КАК ИД_Реализации,
            Заявки1.Реализация,
            Заявки1.СуммаРеализации
        ИЗ
            Заявки1 КАК Заявки1

        УПОРЯДОЧИТЬ ПО
            Заявки1.ДатаЗаявки,
            ИД_Заявки,
            ИД_Реализации
     );
    WString squery = WString(wquery).arg(WString::fromUTF8(userCode)).arg(beginDate.year()).arg(beginDate.month()).arg(beginDate.day()).arg(endDate.year()).arg(endDate.month()).arg(endDate.day());
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            auto values = *i;
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = values->Value.begin(); k != values->Value.end(); ++k)
            {
                columns.push_back(WString(*k));
            }

            result.push_back(columns);
        }

    return result;
}

std::vector<std::vector<WString> >SoapClient::getGoodsSales(const WDate &beginDate,const WDate &endDate,const std::string userCode) const
{
    std::vector<std::vector<WString> > result;
    return result;
}


std::vector<std::vector<WString> >SoapClient::getMutualItog(const WDate &beginDate,const WDate &endDate,const std::string userCode) const
{
    std::vector<std::vector<WString> > result;
    std::wstring wquery = INLINE_WQUERY(
        ВЫБРАТЬ
            1 КАК Уровень,
            1 Как НомерСтроки,
            ДАТАВРЕМЯ({2},{3},{4},0,0,0) КАК ПериодСекунда,
            "" КАК ИД_Документа,
            ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.Контрагент.НаименованиеПолное,
            "" КАК Регистратор,
            ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовНачальныйОстаток КАК НачальныйОстаток,
            ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовПриход КАК Приход,
            ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовРасход КАК Расход,
            ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовКонечныйОстаток КАК КонечныйОстаток,
            0 КАК ВсегоЗаписей,
            "" КАК ВалютаДокумента
        ИЗ
            РегистрНакопления.ВзаиморасчетыСКонтрагентами.ОстаткиИОбороты(ДАТАВРЕМЯ({2},{3},{4},0,0,0), ДАТАВРЕМЯ({5},{6},{7},23,59,59), Авто, ДвиженияИГраницыПериода, Контрагент В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{1}")) КАК ВзаиморасчетыСКонтрагентамиОстаткиИОбороты


     );
    WString squery = WString(wquery).arg(WString::fromUTF8(userCode)).arg(beginDate.year()).arg(beginDate.month()).arg(beginDate.day()).arg(endDate.year()).arg(endDate.month()).arg(endDate.day());
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            auto values = *i;//respone.return_->row.begin();
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = (values)->Value.begin(); k != (values)->Value.end(); ++k)
                columns.push_back(WString(*k));
            result.push_back(columns);
        }
    return result;

}

std::vector<std::vector<WString> >SoapClient::getDocTablel(const std::string docRef) const
{
    std::vector<std::vector<WString> > result;
    std::wstring wquery = INLINE_WQUERY(
            ВЫБРАТЬ
                РеализацияТоваровУслугТовары.НомерСтроки,
                РеализацияТоваровУслугТовары.Номенклатура.Код,
                РеализацияТоваровУслугТовары.Номенклатура.Наименование,
                РеализацияТоваровУслугТовары.Количество,
                РеализацияТоваровУслугТовары.Цена,
                РеализацияТоваровУслугТовары.Сумма,
                ПРЕДСТАВЛЕНИЕ(РеализацияТоваровУслугТовары.СтавкаНДС) КАК СтавкаНДС,
                РеализацияТоваровУслугТовары.СуммаНДС
            ИЗ
                Документ.РеализацияТоваровУслуг.Товары КАК РеализацияТоваровУслугТовары
            ГДЕ
                РеализацияТоваровУслугТовары.Ссылка.ИД_Документа = "{1}"

            ОБЪЕДИНИТЬ ВСЕ

            ВЫБРАТЬ
                ЗаказПокупателяТовары.НомерСтроки,
                ЗаказПокупателяТовары.Номенклатура.Код,
                ЗаказПокупателяТовары.Номенклатура.Наименование,
                ЗаказПокупателяТовары.Количество,
                ЗаказПокупателяТовары.Цена,
                ЗаказПокупателяТовары.Сумма,
                ПРЕДСТАВЛЕНИЕ(ЗаказПокупателяТовары.СтавкаНДС),
                ЗаказПокупателяТовары.СуммаНДС
            ИЗ
                Документ.ЗаказПокупателя.Товары КАК ЗаказПокупателяТовары
            ГДЕ
                ЗаказПокупателяТовары.Ссылка.ИД_Документа = "{1}"
    );
    WString squery = WString(wquery).arg(WString::fromUTF8(docRef));
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            //auto values = respone.return_->row.begin();
            auto values = *i;
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = values->Value.begin(); k != values->Value.end(); ++k)
                columns.push_back(WString(*k));
            result.push_back(columns);
        }
    return result;

}
std::vector<std::vector<WString> >SoapClient::getMutual(const WDate &beginDate,const WDate &endDate,const std::string userCode) const
{
    ///РегистрНакопления.ВзаиморасчетыСКонтрагентами.ОстаткиИОбороты(ДАТАВРЕМЯ({2},{3},{4},0,0,0), ДАТАВРЕМЯ({5},{6},{7},23,59,59), Авто, ДвиженияИГраницыПериода, Контрагент.Код = "{1}") КАК ВзаиморасчетыСКонтрагентамиОстаткиИОбороты
    std::vector<std::vector<WString> > result;
    std::wstring wquery = INLINE_WQUERY(
            ВЫБРАТЬ
                0 КАК Уровень,
                0 КАК НомерСтроки,
                НЕОПРЕДЕЛЕНО КАК ДатаВремя,
                "" КАК ИД_Документа,
                "" КАК Документ,
                "" КАК Регистратор,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовНачальныйОстаток КАК НачальныйОстаток,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовПриход КАК Приход,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовРасход КАК Расход,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовКонечныйОстаток КАК КонечныйОстаток,
                "" КАК ВалютаДокумента
            ИЗ
                РегистрНакопления.ВзаиморасчетыСКонтрагентами.ОстаткиИОбороты(
                        ДАТАВРЕМЯ({2},{3},{4},0,0,0),
                        ДАТАВРЕМЯ({5},{6},{7},23,59,59),
                        Авто,
                        Движения,
                        Контрагент В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{1}")) КАК ВзаиморасчетыСКонтрагентамиОстаткиИОбороты

            ОБЪЕДИНИТЬ ВСЕ

            ВЫБРАТЬ
                1,
                0,
                РеализацияТоваровУслуг.Дата,
                РеализацияТоваровУслуг.ИД_Документа,
                ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.Сделка),
                "",
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовНачальныйОстаток,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовПриход,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовРасход,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовКонечныйОстаток,
                ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.ВалютаДокумента)
            ИЗ
                РегистрНакопления.ВзаиморасчетыСКонтрагентами.ОстаткиИОбороты(
                        ДАТАВРЕМЯ({2},{3},{4},0,0,0),
                        ДАТАВРЕМЯ({5},{6},{7},23,59,59),
                        Авто,
                        Движения,
                        Контрагент В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{1}")) КАК ВзаиморасчетыСКонтрагентамиОстаткиИОбороты
                    ЛЕВОЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
                    ПО ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.Сделка = РеализацияТоваровУслуг.Ссылка

            ОБЪЕДИНИТЬ ВСЕ

            ВЫБРАТЬ
                2,
                0,
                РеализацияТоваровУслуг.Дата,
                РеализацияТоваровУслуг.ИД_Документа,
                "",
                ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.Регистратор),
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовНачальныйОстаток,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовПриход,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовРасход,
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.СуммаВзаиморасчетовКонечныйОстаток,
                ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.ВалютаДокумента)
            ИЗ
                РегистрНакопления.ВзаиморасчетыСКонтрагентами.ОстаткиИОбороты(
                        ДАТАВРЕМЯ({2},{3},{4},0,0,0),
                        ДАТАВРЕМЯ({5},{6},{7},23,59,59),
                        Авто,
                        Движения,
                        Контрагент В
                            (ВЫБРАТЬ
                                Контрагенты.Ссылка
                            ИЗ
                                Справочник.Контрагенты КАК Контрагенты
                            ГДЕ
                                Контрагенты.Код = "{1}")) КАК ВзаиморасчетыСКонтрагентамиОстаткиИОбороты
                    ЛЕВОЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
                    ПО ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.Сделка = РеализацияТоваровУслуг.Ссылка
            ГДЕ
                ВзаиморасчетыСКонтрагентамиОстаткиИОбороты.Регистратор <> НЕОПРЕДЕЛЕНО

            УПОРЯДОЧИТЬ ПО
                ДатаВремя,
                Уровень
	);
    WString squery = WString(wquery).arg(WString::fromUTF8(userCode)).arg(beginDate.year()).arg(beginDate.month()).arg(beginDate.day()).arg(endDate.year()).arg(endDate.month()).arg(endDate.day());
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            //auto values = respone.return_->row.begin();
            auto values = *i;
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = values->Value.begin(); k != values->Value.end(); ++k)
                columns.push_back(WString(*k));
            result.push_back(columns);
        }
    return result;

}

std::vector<std::vector<WString> >SoapClient::getLastShipment(std::string userCode) const
{
    std::vector<std::vector<WString> > result;

    std::wstring wquery = INLINE_WQUERY(
        ВЫБРАТЬ ПЕРВЫЕ 1
            РеализацияТоваровУслуг.УникальныйИдентификатор КАК УникальныйИдентификатор,
            РеализацияТоваровУслуг.Представление КАК Реализация,
            РеализацияТоваровУслуг.СуммаДокумента
        ИЗ
            Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
        ГДЕ
            РеализацияТоваровУслуг.Контрагент.Код = "{1}"

        УПОРЯДОЧИТЬ ПО
            Дата УБЫВ
    );

    WString squery = WString(wquery).arg(WString::fromUTF8(userCode));
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            auto values = respone.return_->row.begin();
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = (*values)->Value.begin(); k != (*values)->Value.end(); ++k)
            {
                columns.push_back(WString(*k));
            }
            result.push_back(columns);
        }
    return result;

}
std::vector<std::vector<WString> >SoapClient::getLastOrder(std::string userCode) const
{
    std::vector<std::vector<WString> > result;

    std::wstring wquery = INLINE_WQUERY(

        ВЫБРАТЬ
            ЗаказПокупателя.Дата КАК ДатаЗаказа,
            ЗаказПокупателя.Ссылка
        ПОМЕСТИТЬ ТаблВсеЗаявки
        ИЗ
            Документ.ЗаказПокупателя КАК ЗаказПокупателя
        ГДЕ
            ЗаказПокупателя.Проведен
            И ЗаказПокупателя.Контрагент.Код = "{1}"
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            МАКСИМУМ(ТаблВсеЗаявки.ДатаЗаказа) КАК ДатаЗаказа
        ПОМЕСТИТЬ ТаблМаксДата
        ИЗ
            ТаблВсеЗаявки КАК ТаблВсеЗаявки
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            ТаблМаксДата.ДатаЗаказа,
            ТаблВсеЗаявки.Ссылка
        ПОМЕСТИТЬ ПоследниеЗаявки
        ИЗ
            ТаблВсеЗаявки КАК ТаблВсеЗаявки
                ВНУТРЕННЕЕ СОЕДИНЕНИЕ ТаблМаксДата КАК ТаблМаксДата
                ПО ТаблВсеЗаявки.ДатаЗаказа = ТаблМаксДата.ДатаЗаказа
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            ПоследниеЗаявки.ДатаЗаказа,
            ПоследниеЗаявки.Ссылка КАК Заявка,
            ПоследниеЗаявки.Ссылка КАК Документ
        ПОМЕСТИТЬ ТаблЗаявкаЗаявка
        ИЗ
            ПоследниеЗаявки КАК ПоследниеЗаявки
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            ПоследниеЗаявки.ДатаЗаказа,
            ПоследниеЗаявки.Ссылка КАК Заявка,
            РеализацияТоваровУслуг.Ссылка КАК Реализация,
            РеализацияТоваровУслуг.СуммаДокумента КАК СуммаРеализации,
            РеализацияТоваровУслуг.ВалютаДокумента КАК ВалютаРеализации
        ПОМЕСТИТЬ ТаблРеализации
        ИЗ
            ПоследниеЗаявки КАК ПоследниеЗаявки
                ВНУТРЕННЕЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
                ПО ПоследниеЗаявки.Ссылка = РеализацияТоваровУслуг.ДокументОснование
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            ТаблЗаявкаЗаявка.ДатаЗаказа,
            ТаблЗаявкаЗаявка.Заявка,
            ТаблЗаявкаЗаявка.Документ,
            ТаблЗаявкаЗаявка.Документ.ВалютаДокумента КАК ВалютаДокумента,
            ТаблЗаявкаЗаявка.Документ.СуммаДокумента КАК СуммаДокумента
        ПОМЕСТИТЬ ТаблЗаявкаИРеализ
        ИЗ
            ТаблЗаявкаЗаявка КАК ТаблЗаявкаЗаявка

        ОБЪЕДИНИТЬ ВСЕ

        ВЫБРАТЬ
            ТаблРеализации.ДатаЗаказа,
            ТаблРеализации.Заявка,
            ТаблРеализации.Реализация,
            ТаблРеализации.ВалютаРеализации,
            ТаблРеализации.СуммаРеализации
        ИЗ
            ТаблРеализации КАК ТаблРеализации
        ;

        ////////////////////////////////////////////////////////////////////////////////
        ВЫБРАТЬ
            ТаблЗаявкаИРеализ.Заявка.УникальныйИдентификатор,
            ПРЕДСТАВЛЕНИЕ(ТаблЗаявкаИРеализ.Заявка) КАК Заявка,
            СУММА(ТаблЗаявкаИРеализ.СуммаДокумента) КАК СуммаДокумента,
            ТаблЗаявкаИРеализ.ВалютаДокумента.Наименование КАК ВалютаДокумента
        ИЗ
            ТаблЗаявкаИРеализ КАК ТаблЗаявкаИРеализ

        СГРУППИРОВАТЬ ПО
            ТаблЗаявкаИРеализ.ДатаЗаказа,
            ТаблЗаявкаИРеализ.ВалютаДокумента,
            ТаблЗаявкаИРеализ.Заявка

    );
    WString squery = WString(wquery).arg(WString::fromUTF8(userCode));
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            auto values = respone.return_->row.begin();
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = (*values)->Value.begin(); k != (*values)->Value.end(); ++k)
                columns.push_back(WString(*k));
            result.push_back(columns);
        }
    return result;

}

std::vector<std::vector<WString> >SoapClient::getUnbackedDocs(std::string userCode) const
{
    std::vector<std::vector<WString> > result;

    std::wstring wquery = INLINE_WQUERY(
        ВЫБРАТЬ
            ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОбороты.Контрагент) Как Контрагент,
            ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОбороты.Регистратор ) КАК Реализация,
            ПРЕДСТАВЛЕНИЕ(ВзаиморасчетыСКонтрагентамиОбороты.Организация) КАК Организация,
            РеализацияТоваровУслуг.СуммаДокумента,
            РеализацияТоваровУслуг.ВалютаДокумента.Наименование КАК Валюта,
            РеализацияТоваровУслуг.Статус.Ключ КАК Статус
        ИЗ
            РегистрНакопления.ВзаиморасчетыСКонтрагентами.Обороты(, , Регистратор, Контрагент.Код = "{1}") КАК ВзаиморасчетыСКонтрагентамиОбороты
                ВНУТРЕННЕЕ СОЕДИНЕНИЕ Документ.РеализацияТоваровУслуг КАК РеализацияТоваровУслуг
                ПО ВзаиморасчетыСКонтрагентамиОбороты.Регистратор = РеализацияТоваровУслуг.Ссылка
        ГДЕ
            РеализацияТоваровУслуг.Статус.Ключ < 8
    );

    WString squery = WString(wquery).arg(WString::fromUTF8(userCode));
    std::wstring query = squery.value();
    _ns1__QueryUniversalResponse respone;
    _ns1__QueryUniversal request;
    request.SystemExpression = &query;
    m_client->userid=soapUserName.c_str();
    m_client->passwd=soapUserPasswd.c_str();

    m_client->QueryUniversal(&request,&respone);
    if(respone.return_)
        for (auto i = respone.return_->row.begin(); i != respone.return_->row.end(); ++i)
        {
            auto values = respone.return_->row.begin();
            //for (auto k = 0;i<(*i)->Value.size();k++)
            std::vector<WString> columns;
            for (auto k = (*values)->Value.begin(); k != (*values)->Value.end(); ++k)
                columns.push_back(WString(*k));
            result.push_back(columns);
        }
    return result;
}

WStandardItemModel * SoapClient::vectorToModel(std::vector<std::vector<WString> > params,WStandardItemModel *model)
{
    int dataRow = model->rowCount();

    for(auto i = params.begin();i!=params.end();++i)
    {
        int columns = i->size();
        int col = 0;
        if (columns >= model->columnCount())
            model->insertColumns(model->columnCount(), columns + 1 - model->columnCount());
        model->insertRows(model->rowCount(),1);
        for(auto k = i->begin();k!=i->end();++k)
        {
            WStandardItem *item = new WStandardItem();
            //std::cout<<dataRow<<" "<<col<<" "<< *k<<std::endl;
            item->setText(*k);
            model->setItem(dataRow, col++ ,item);
        }
        dataRow++;
    }
    return model;
}
