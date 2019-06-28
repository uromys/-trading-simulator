#pragma once
#include "trading.h"
#include "strategietrading.h"
#include "choixparam.h"
#include "edittexte.h"

/*! \class ModeAuto
        \brief La classe hérite de QWidget et se base sur les choix de paramètre que l'utilisateur
                a choisi (la date et le montant des devises qu'il souhaite impliquer dans sa simulation)
                L'utilisateur va également choisir la stratégie qu'il veut utiliser pour sa simulation.
                Les transactions seront ensuite stockées dans un vector tab mais également dans un QTableWidget afin de les afficher.
*/

class ModeAuto : public QWidget{
    Q_OBJECT

    // PARAMETRE FENETRE
    int largeur = 800;
    int hauteur = 400;

    // DONNEES
    QString chemin;             // Chemin du fichier importé pour pouvoir enregistrer le csv à la fin de la simulation
    ChoixParam* param;          // Va récupérer les valeurs entrées lors du choix des paramètres précédents
    vector<Transaction> tab;    // Vecteur de transaction importé depuis le fichier csv.
public:
    unsigned int nbTransactions=0;
    ModeAuto(ChoixParam* p, QString c, QWidget *parent);
    void lancer_simulation(QString choix, EvolutionCours* e);
};
