#pragma once

#include "trading.h"
#include "affichage.h"
#include "choixparam.h"

/*! \class ModeManuel
        \brief La classe hérite de QWidget et se base sur les choix de paramètre que l'utilisateur
                a choisi (la date et les devises). Le mode manuel permet à l'utilisateur de choisir lui-même
                ce qu'il désire faire lors de la simulation (acheter,vendre, ne rien faire ou revenir en arrière), sur n'importe quel bougie en double cliquant dessus lors de l'affichage
                Chaque choix réalisés sera sauvegardé dans le tableau dynamique.
*/

class ModeManuel : public QWidget{

    Q_OBJECT

    // EVOLUTIONCOURS
    EvolutionCours *eManuel; // EvolutionCours prenant les cours entre la date de début de ChoixParam et le dernier cours seulement.

    // OBJETS
    QString chemin; // Chemin du fichier importé pour pouvoir enregistrer le csv à la fin de la simulation
    CSVViewer *CSV_manuel; //CSV pour afficher une bougie à la fois
    ChoixParam* param; // Va récupérer les valeurs entrées lors du choix des paramètres précédents

    // COMPTEUR
    unsigned int i =0; // compteur du cours à traiter actuellement

    // AMITIE
    friend class ChoixParam;

public:
    explicit ModeManuel(EvolutionCours* e,ChoixParam *p, QString c, QWidget *parent);
public slots:
    void lancer_simulation();
};
