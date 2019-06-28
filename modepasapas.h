#pragma once

#include "trading.h"
#include "affichage.h"
#include "choixparam.h"

/*! \class ModePasAPas
        \brief Cette classe héritant de QWidget permet d'afficher petit à petit les bougies selon les achats.
                Cette classe utilise également les paramètres choisis par l'utilisateur afin de réaliser la simulation.
                La différence de ce mode par rapport à mode manuel est le fait qu'il demande à l'utilisateur d'effectuer les transactions (ou ne rien faire) jours après jours
                à partir de la date de début saisie, et ce jusqu'à la fin de l'echantillon sans connaitre les variations futures.
                Lors de l'affichage les bougies s'afficheront donc petit à petit lorsque la transaction qui lui ait effectué a été réalisée, les autres restant plates en attendant que l'utilisateur se décide.
*/

class Modepasapas : public QWidget
{
    Q_OBJECT
private:

    // DONNEES
    EvolutionCours *ePasapas;
    QString chemin; // Chemin du fichier importé pour pouvoir enregistrer le csv à la fin de la simulation
    ChoixParam* param;
    CSVViewer *CSV_Pasapas;

public:
    Modepasapas( QWidget *parent,ChoixParam *p,QString c,EvolutionCours *e=nullptr);

public slots:
    void lancer_simulation();

};

