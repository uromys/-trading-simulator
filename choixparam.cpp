#include "choixparam.h"

using namespace std;

ChoixParam :: ChoixParam(EvolutionCours *e, QWidget *parent): QWidget(parent){

    // SAUVEGARDE EVOLUTION COURS
    eChoix = new EvolutionCours(*e);

    // CREATION DES ATTRIBUTS (à travers les méthodes respectives
    createDateEdits();
    createDoubleSpinBoxes();
    intermediaire();

    // MESSAGE INTERFACE
    QLabel *message = new QLabel("Veuillez choisir les paramètres de la simulation :");

    // LAYOUT
    layout =new QVBoxLayout;
    layout->addWidget(message);
    layout->addWidget(editsGroup);
    layout->addWidget(doubleSpinBoxesGroup);
    layout->addWidget(intermediaireBoxes);
    this->setLayout(layout);

    // TITRE FENETRE (si détachée)
    setWindowTitle(tr("Choix paramètres"));
}

void ChoixParam :: createDateEdits(){

    editsGroup = new QGroupBox(tr("Date de la transaction"));// Titre du box date

    QLabel *dateLabel =new QLabel("Date de début des données chargées");
    date = new QDateEdit();

    // VALEUR PAR DEFAUT : Date du 1er cours
    // VALEUR MINIMALE : Date du 1er cours
    // VALEUR MAXIMALE : Date du dernier cours

    QDate dateDebut = eChoix->getCours(0).getDate();
    QDate dateLimite = eChoix->getCours(eChoix->getNbCours() - 1).getDate();

    date->setDate(dateDebut);
    date->setMinimumDate(dateDebut);
    date->setMaximumDate(dateLimite);

    // Creation d'un comboBox pour les différents formats de la date

    QLabel *formatLabel = new QLabel(tr("Format pour la date "));

    QComboBox *formatComboBox = new QComboBox;
    formatComboBox->addItem("yyyy-MM-dd"); //format 1
    formatComboBox->addItem("MM/dd/yyyy"); //format 2
    formatComboBox->addItem("dd/MM/yyyy"); //format 3

    // Connection entre le slot pour changer le format de la date et l'affichage de celle ci
    connect(formatComboBox,SIGNAL(activated(QString)),this,SLOT(setFormatString(QString)));
    setFormatString(formatComboBox->currentText());

    // Layout pour insérer tous les paramètres pour la date
    QVBoxLayout *editsLayout = new QVBoxLayout;
    editsLayout->addWidget(dateLabel);
    editsLayout->addWidget(date);
    editsLayout->addWidget(formatLabel);
    editsLayout->addWidget(formatComboBox);
    editsGroup->setLayout(editsLayout);

}

// Slot pour changer le format de la date avec le format rentrée
void ChoixParam:: setFormatString(const QString &formatString){
    date->setDisplayFormat(formatString);
}

void ChoixParam :: createDoubleSpinBoxes(){

    doubleSpinBoxesGroup = new QGroupBox(tr(" Choix des devises ainsi que du pourcentage ")); // Titre pour le box contenant les montants ainsi que le pourcentage


    QLabel *doublelabel =new QLabel(tr(" Montant de base "));// titre
    montantBase = new QDoubleSpinBox;
    montantBase->setRange(0.00,1000000.00);     // intervalle des valeurs possibles
    montantBase->setSingleStep(0.01);           // pas de 0.01 grâce aux flêches
    montantBase->setValue(0.0);                 // valeur par défaut

    QLabel *doublelabel2 = new QLabel(tr(" Montant de contrepartie."));//titre
    montantContrepartie = new QDoubleSpinBox;
    montantContrepartie->setRange(0.00,1000000.00);
    montantContrepartie->setSingleStep(0.01);
    montantContrepartie->setValue(1000000.00);

    QLabel *pourcentagelabel =new QLabel(tr("Pourcentage à prélever sur les transactions")); //titre
    pourcentage = new QDoubleSpinBox;
    pourcentage->setRange(0.00,100.00);
    pourcentage->setSingleStep(0.01);
    pourcentage->setSuffix("%");
    pourcentage->setValue(0.10);

    QVBoxLayout *spinBoxLayout = new QVBoxLayout;
    spinBoxLayout->addWidget(doublelabel);
    spinBoxLayout->addWidget(montantBase);
    spinBoxLayout->addWidget(doublelabel2);
    spinBoxLayout->addWidget(montantContrepartie);
    spinBoxLayout->addWidget(pourcentagelabel);
    spinBoxLayout->addWidget(pourcentage);
    doubleSpinBoxesGroup->setLayout(spinBoxLayout);


}

void ChoixParam :: intermediaire(){

    intermediaireBoxes = new QGroupBox(tr("Intermediaire"));

    QLabel *intermediaireLabel = new QLabel(tr("Intermediaire : plateforme / broker"));

    QComboBox *intermediaireComboBox = new QComboBox;
    intermediaireComboBox->addItem("plateforme");
    intermediaireComboBox->addItem("broker");

    QVBoxLayout *intermediaireLayout = new QVBoxLayout;
    intermediaireLayout->addWidget(intermediaireLabel);
    intermediaireLayout->addWidget(intermediaireComboBox);
    intermediaireBoxes->setLayout(intermediaireLayout);

}
