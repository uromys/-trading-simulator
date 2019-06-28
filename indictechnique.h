#pragma once

#include "trading.h"
#include <iterator>

/*! \class IndicTechnique
        \brief  On décide de faire une classe IndicTechnique générale qui contiendra le tableau de coursOHLC
                dont auront besoin les différents indicateurs
                On met pour cela cette classe en "friend" dans chaque indicateur pour qu'ils aient accès
                au vecteur de cours
                On ne fait pas d'héritage car cela recopierait le tableau dans chaque indicateur,
                ce qui n'est pas optimal car de la place serait prise inutilement étant donné que c'est le même
                tableau pour tous les indicateurs. Une référence ou pointeur est donc largement suffisante.

                Pour ajouter un indicateur simplement, il faut:
                - Créer une nouvelle classe avec IndicTechnique comme classe amie (friend)
                - Creer la méthode "Update" de votre indicateur avec les informations à afficher
*/

class IndicTechnique{

public:
    vector<CoursOHLC> cours;
    IndicTechnique(EvolutionCours *e);
    double calculFirstEMA(unsigned int nb,unsigned int pos) const;
};

// On décide de faire une classe IndicTechnique générale qui contiendra le tableau de coursOHLC dont auront besoin les différents indicateurs
// On met pour cela cette classe en "friend" dans chaque indicateur pour qu'ils aient accès au vecteur de cours
// On ne fait pas d'héritage car cela recopierai le tableau dans chaque indicateur, ce qui n'est pas optimal car prend de la place inutilement étant donné que c'est le même tableau pour tous les indicateurs. Une référence ou pointeur est donc largement suffisante.

// EVOLUTION : Pour ajouter un indicateur simplement, il faut:
// - Créer une nouvelle classe avec IndicTechnique comme classe amie (friend)
// - Creer la méthode "Update" de votre indicateur avec les informations à afficher
// - Ajouter l'indicateur dans la classe CSVViewer aux endroits prévus à cet effet (voir commentaires)

// Indicateur n°1 : RSI
/*! \class RSI
        \brief  Indicateur n°1 : Relative Strength Index (RSI)
*/

class RSI {
    friend IndicTechnique;
    vector<QString> evoRSI;
public:
    RSI(IndicTechnique* indic,unsigned int nb=14) {CalculRSI(indic, nb);}
    void CalculRSI(IndicTechnique *indic, unsigned int nb);
    QString getRSI(unsigned int i){return evoRSI[i];}
};



// Indicateur n°2 : EMA
/*! \class EMA
        \brief  Indicateur n°2 : Exponential Moving Average (EMA)
*/

class EMA {

    friend IndicTechnique;
    vector<QString> evoEMA;
    double lastEMA; // dernier EMA
    int firstEMA = 1; //
    void notFirstEMA(){ firstEMA = 0 ;}
    void setLastEMA(const double& l){lastEMA = l;}


public:
    EMA(IndicTechnique* indic,unsigned int nb=20) {CalculEMA(indic, nb);}
    void CalculEMA(IndicTechnique *indic, unsigned int nb);
    QString getEMA(unsigned int i) const {return evoEMA[i];}
    double getLastEMA(){return lastEMA;}
};

// Indicateur n°3 : MACD
/*! \class MACD
        \brief  Indicateur n°3 : Moving Average Convrgence Divergence (MACD)
*/
class MACD {
    vector<QString> evoMACD;
    vector<QString> signal;
    friend IndicTechnique;
    int firstMACD = 1; //
    void notFirstMACD(){firstMACD = 0;}
    double lastLowerMME; // dernière MME de la période la plus courte
    double lastHigherMME; // dernière MME de la période la plus longue
    void setLastMMEs(const double& l,const double& h){lastLowerMME=l; lastHigherMME = h;} // permet de changer la valeur des derniers MME (l=lower, h=higher)
    double getLastLowerMME() const { return lastLowerMME; }
    double getLastHigherMME() const { return lastHigherMME; }
public:
    MACD(IndicTechnique* indic,unsigned int high=26, unsigned int low=12) {CalculMACD(indic, high,low);}
    void CalculMACD(IndicTechnique *indic, unsigned int high, unsigned int low);
    QString getMACD(unsigned int i){return evoMACD[i];}
    QString getSignal(unsigned int i){return signal[i];}

// Insérez vos nouveaux indicateurs ici

};
