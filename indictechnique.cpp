#include "indictechnique.h"

// INDICTECHNIQUE

IndicTechnique::IndicTechnique(EvolutionCours *e){

    // Remplissage du vecteur de cours avec le tableau de cours de EvolutionCours (qui n'est pas un vecteur)
    unsigned int i;
    for(i = 0 ; i < e->getNbCours() ; i++){
        cours.push_back(e->getCours(i));
    }
}

double IndicTechnique::calculFirstEMA(unsigned int nb,unsigned int pos)const{ // calcul des premiers EMA
    if (pos<nb) return NULL;
    double SMA =0;
    const double multiplicateur = 2/(nb+1);

    // CALCUL DES SMA
    for(unsigned int i=pos-nb; i<pos; ++i){
       SMA += cours[i].getClose();
    }
    SMA /=nb;

    // CALCUL DU EMA A PARTIR DU SMA
    double EMA= (cours[pos].getClose()- SMA)*multiplicateur + SMA;
    return EMA;

}

// RSI

void RSI::CalculRSI(IndicTechnique *indic, unsigned int nb){
    for (unsigned int i=0; i<indic->cours.size();i++){ // calcul complet de tous les RSI d'un cours
        if(nb>i) evoRSI.push_back("NA"); // si on a pas assez de cours pour notre periode(nb), on ne peut pas faire le calcul
        else{
            unsigned int nbHausse=0; // viendra prendre le nombre de tendance a la hausse
            unsigned int nbBaisse=0; // meme chose mais pour tendance haussiere

            for (unsigned int j=i-nb; j<i;++j){ // parcourt des nb derniers jours
                double tendance = indic->cours[j].getClose()- indic->cours[j].getOpen();
                if (tendance<0) nbBaisse++; // on est à la baisse
                else nbHausse++; // sinon à la hausse;

            }
        double Rsi = 100 * nbHausse /(nbHausse+nbBaisse); // formule RSI

        evoRSI.push_back(QString::number(Rsi));
        }
    }
}

// EMA

void EMA::CalculEMA(IndicTechnique *indic, unsigned int nb){
    for (unsigned int i=0; i<indic->cours.size();i++){ // calcul de toutes les valeurs EMA d'un cours
        if (firstEMA==1){                              // si premier EMA
            double E= indic->calculFirstEMA(nb,i);
            if(E == NULL) evoEMA.push_back("NA");      // si incalculable on met NA
            else{                                      // sinon calcul du premier EMA fait et on va maintenant calculer les EMA a partir de celui ci
                notFirstEMA();
                setLastEMA(E);
                evoEMA.push_back(QString::number(E));  // on le met dans le tableau des EMA
            }
        }
        else{
            double E = (indic->cours[i].getClose()-getLastEMA())*2/(nb+1)+getLastEMA();  // formule EMA
            setLastEMA(E);
            evoEMA.push_back(QString::number(E));
        }
    }
}

// MACD

void MACD::CalculMACD(IndicTechnique *indic, unsigned int  high, unsigned int low){
    double EMAHIGH,EMALOW;

    for (unsigned int i=0; i<indic->cours.size();i++){

        if (firstMACD==1){                                  // si premier MACD
            EMAHIGH = indic->calculFirstEMA(high,i);
            if(EMAHIGH == NULL) {
                evoMACD.push_back("NA");                    //calcul impossible si on n'a pas de valeur sur high jours
                signal.push_back("NA");
            }
            else{
                EMALOW = indic->calculFirstEMA(low,i);      // pas besoin de vérifier ici car low<high
                setLastMMEs(EMALOW,EMAHIGH);                // on récupère ces EMA qui vont nous servir sur les calculs prochains
                notFirstMACD();
                evoMACD.push_back(QString::number(EMAHIGH-EMALOW)); // le premier macd se fait sur la différence entre les deux EMA
                signal.push_back("NA");
            }
        }
        else {
            EMAHIGH = (0.91*indic->calculFirstEMA(high,i-1));  // calcul des EMA par rapport aux anciennes valeurs
            EMALOW = (0.91*indic->calculFirstEMA(low,i-1));


            double m = EMAHIGH-EMALOW;
            setLastMMEs(EMALOW,EMAHIGH);
            evoMACD.push_back(QString::number(m));
            if(i>high+9){
                double sig = 0;
                for (unsigned int j=i-9; j<i;j++) sig += evoMACD[j].toDouble();
                sig/=9;
                signal.push_back(QString::number(sig));

            }
            else signal.push_back("NA");
        }
    }
}
