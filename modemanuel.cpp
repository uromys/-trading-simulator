#include "modemanuel.h"

using namespace std;

ModeManuel::ModeManuel(EvolutionCours* e,ChoixParam *p, QString c, QWidget *parent):QWidget(parent), chemin(c)
{

    // RECUPERATION PARAMETRES

    param = p->clone();

    // REMPLISSAGE EVOLUTION COURS

    eManuel = new EvolutionCours(*e);

}

void ModeManuel::lancer_simulation()
{
    // CREATION FENETRE FILLE
    QWidget *fen = new QWidget;
    fen->setWindowTitle("Simulation de Trading - Mode Manuel");

    // CREATION CSVVIEWER (Dans la fenêtre fille)
    CSV_manuel = new CSVViewer(eManuel,param,chemin,fen,"manuel");

    // AFFICHAGE
    fen->show();
}





