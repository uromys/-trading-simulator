#pragma once

#include "trading.h"
#include "indictechnique.h"
//Implementation design pattern strategie

/*! \class StrategieTrading
        \brief Cette classe abstraite permet de définir une première fois les stratégies. Celle ci possèdera par la suite différentes classes dérivées qui définiront les différentes stratégies possibles.
*/
class StrategieTrading {

    virtual QString procedure(unsigned int pos)=0;
public:
    virtual ~StrategieTrading();
    QString executer(unsigned int pos) {return procedure(pos);}

};


/*! \class StrategieRSI
        \brief  Cette classe herite de StrategieTrading et represente la 1ere strategie : indicateur RSI.
        https://www.abcbourse.com/apprendre/11_lecon7_1.html
*/

//1ere strategie : indicateur RSI
class StrategieRSI : public StrategieTrading{   //https://www.abcbourse.com/apprendre/11_lecon7_1.html
    RSI* rsi;
    QString procedure(unsigned int pos) override;
    public:
    StrategieRSI(RSI* r) : rsi(r){}


};


/*! \class StrategieMACD
        \brief Cette classe herite de StrategieTrading et represente la 2eme strategie : indicateur MACD et EMA
         https://www.botraiders.com/apprendre-bourse/analyser-la-bourse/analyse-technique/macd
*/
//2eme strategie : indicateur MACD et EMA
class StrategieMACD : public StrategieTrading{ // https://www.botraiders.com/apprendre-bourse/analyser-la-bourse/analyse-technique/macd
    MACD* macd;
    QString procedure(unsigned int pos) override;
    public:
    StrategieMACD(MACD* m) : macd(m){}


};
/*! \class StrategieBougie
        \brief Cette classe herite de StrategieTrading puisque l'on a besoin de cours pour remplir les bougies.
                Elle représente la 3eme strategie : analyse de la forme
*/
//3eme strategie : analyse de la forme
class StrategieBougie : public StrategieTrading{ //on a besoin de cours pour remplir les bougies
    EvolutionCours* cours;
    QString procedure(unsigned int pos) override;
    public:
    StrategieBougie(EvolutionCours* e) : cours(e){}
};

/*! \class Contexte
        \brief 3eme strategie servant à analyse de la forme
*/

class Contexte final{
    StrategieTrading* strategie;
public:
    Contexte(StrategieTrading* s):strategie(s){}
    QString executer(unsigned int pos){ return strategie->executer(pos);}
    void setStrategie(StrategieTrading* s) {strategie = s;}
};

// Une Transaction
/*! \class Transaction
        \brief Cette classe représente une transaction. Elle possède tous les attributs nécessaires à générer une bonne transaction.
*/

class Transaction{
private:
    QDate date;
    QString MontantBase;
    QString MontantContre;
    double MontantTotal;
    double roi;
    QString type;
public:
    Transaction (double mb, double mc,  QDate d, CoursOHLC cours,QString t, double pourcent, double montant = 100.0); // sans montant précisé, la transaction est de 100 unités
    double getMontantBase() const;
    double getMontantContre() const;
    static double ConversionBaseCotation(double base, double pourcent, CoursOHLC cours,unsigned int mode=0);
    static double ConversionCotationBase(double cotation, double pourcent, CoursOHLC cours,unsigned int mode=0);
    QDate getDate() const {return date;}
    double getRoi() const {return roi;}
    QString getMontantB() const {return MontantBase;}
    QString getMontantC() const {return MontantContre;}
    QString getType() const {return type;}
    void setRoi(double d);
    ~Transaction (){}
};

namespace Tableau{
    void addElement(unsigned int pos, QTableWidget* w, std::vector<Transaction> t);
    void removeElement(unsigned int pos,QTableWidget* w);
    QTableWidget* createTable();
}
