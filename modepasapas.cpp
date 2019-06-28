#include "modepasapas.h"

Modepasapas::Modepasapas(QWidget *parent,ChoixParam *p, QString c, EvolutionCours *e): QWidget(parent), chemin(c)
{
    param = p->clone();

    ePasapas = new EvolutionCours(*e);

}

void Modepasapas::lancer_simulation(){

    // CREATION FENETRE FILLE
    QWidget *fen = new QWidget;
    fen->setWindowTitle("Simulation de Trading - Pas a pas");

    // CREATION CSVVIEWER (Dans la fenêtre fille)
    CSV_Pasapas = new CSVViewer(ePasapas,param,chemin,fen,"pasapas");

    // AFFICHAGE
    fen->show();
}
