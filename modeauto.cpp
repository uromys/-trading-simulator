#include "modeauto.h"

ModeAuto::ModeAuto(ChoixParam* p, QString c, QWidget *parent) : QWidget (parent), chemin(c)
{
    param = p->clone(); // On clone le choix paramètre pour qu'ils soient affecté à ce mode uniquement
}

void ModeAuto::lancer_simulation(QString strategie, EvolutionCours* e){
    Contexte* c;
    QTableWidget* Tableau = Tableau::createTable();

    // ACTION SELON STRATEGIE CHOISIE

    if(strategie == "RSI"){

        c = new Contexte(new StrategieRSI(new RSI(new IndicTechnique(e))));
    }
    else if(strategie == "MACD/SMA"){
        c = new Contexte(new StrategieMACD(new MACD(new IndicTechnique(e))));

    }
    else if(strategie == "Analyse de la forme"){
        c = new Contexte(new StrategieBougie(e));
    }



    else{
        QMessageBox::critical(this,"Erreur","Une erreur est survenue \nLa date fournie dans le choix des paramètres est invalide \navec l'échantillon de données actuel.");
        QApplication::quit(); // on quitte l'application si la variable etat rencontre une valeur inconnue
    }

        unsigned int j=0;
        while (e->getCours(j).getDate().toString("dd.MM.yyyy")!=param->getDate().toString("dd.MM.yyyy")){
            j++;
        }
        // On va au jour qui nous intéresse
        for (unsigned int i=j; i<e->getNbCours(); i++){             //on boucle du début de nos cours jusqu'à la fin
            QString decision = c->executer(i);
            if (decision=="achat"){

                if (nbTransactions==0){

                    if (param->getBase()>25){
                        tab.push_back(Transaction(param->getBase(),param->getContrepartie(),e->getCours(i).getDate(),e->getCours(i),"achat",param->getPourcentage(),25.0));
                        tab[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),e->getCours(i)));
                        Tableau::addElement(nbTransactions,Tableau,tab);
                        nbTransactions++;
                    }
                    else continue;
                }
                else {

                    if (tab[nbTransactions-1].getMontantBase()>25){

                        tab.push_back(Transaction(tab[nbTransactions-1].getMontantBase(),tab[nbTransactions-1].getMontantContre(),e->getCours(i).getDate(),e->getCours(i),"achat",param->getPourcentage(),25.0));
                        tab[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),e->getCours(i)));
                        Tableau::addElement(nbTransactions,Tableau,tab);
                        nbTransactions++;
                    }
                    else continue;
                }

            }

            else if (decision=="vente"){
                if (nbTransactions==0){
                    if (param->getContrepartie()>100){
                        tab.push_back(Transaction(param->getBase(),param->getContrepartie(),e->getCours(i).getDate(),e->getCours(i),"vente",param->getPourcentage()));
                        tab[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),e->getCours(i)));
                        Tableau::addElement(nbTransactions,Tableau,tab);
                        nbTransactions++;
                    }
                    else continue;
                }
                else {
                    if (tab[nbTransactions-1].getMontantContre()>100){
                        tab.push_back(Transaction(tab[nbTransactions-1].getMontantBase(),tab[nbTransactions-1].getMontantContre(),e->getCours(i).getDate(),e->getCours(i),"vente",param->getPourcentage()));
                        tab[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),e->getCours(i)));
                        Tableau::addElement(nbTransactions,Tableau,tab);
                        nbTransactions++;
                    }
                    else continue;
                }

            }
            else continue;
        }
    Tableau->setFixedSize(largeur,hauteur);
    Tableau->setWindowTitle("Historique des transactions effectuées");
    Tableau->show();
    EditTexte* edit= new EditTexte(Tableau,chemin);
    edit->show();

}
