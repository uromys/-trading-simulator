#pragma once

#include "trading.h"

/*! \class ChoixParam
            \brief La classe ChoixParam hérite de QWidget et permet la selection de paramètres pour effectuer des simulations.
            Ces paramètres (montant de base, montant de contrepartie, pourcentage, date de début) sont choisis par l'utilisateur.
    */

class ChoixParam : public QWidget{

    Q_OBJECT

private:

    // ATTRIBUTS

    QDateEdit *date; // date de début de la transaction

    QDoubleSpinBox *montantBase;
    QDoubleSpinBox *montantContrepartie;
    QDoubleSpinBox *pourcentage;
    QGroupBox *editsGroup;
    QGroupBox *doubleSpinBoxesGroup;
    QGroupBox *intermediaireBoxes;
    EvolutionCours *eChoix;
    QVBoxLayout *layout;

    // METHODES

    void createDoubleSpinBoxes();   // méthode pour créer les DoubleSpinBoxes
    void createDateEdits();         // méthode pour créer les DatesEdits
    void intermediaire();           // méthode pour implémenter l'intermédiaire

    //AMITIE

    friend class EvolutionCours;

public:

    //FENETRE
    explicit ChoixParam(EvolutionCours *e , QWidget *parent);
    ChoixParam* clone() {return this;}
    QDate getDate() const{return date->date();}
    void setDateSuivante() {
        if(date->date().addDays(1) != date->maximumDate())
            date->setDate(date->date().addDays(1));
        else {
            QMessageBox::critical(this,"Erreur","Impossible d'incrémenter la date. \nLa date limite de l'échantillon de données a été atteinte.");
        }
    } // Incrémente de 1 la date saisie au départ dans le choix Paramètre si possible
    void setDatePrecedente() {
        if(date->date().addDays(-1) != date->maximumDate())
            date->setDate(date->date().addDays(-1));
        else {
            QMessageBox::critical(this,"Erreur","Impossible de décrémenter la date. \nLa date limite de l'échantillon de données a été atteinte.");
        }
    } // Décrémente de 1 la date saisie au départ dans le choix Paramètre si possible
    double getBase() const{return montantBase->value();}
    double getContrepartie() const{return montantContrepartie->value();}
    double getPourcentage() const{return pourcentage->value();}

signals:
public slots:
    void setFormatString(const QString &formatString); // slot pour modifier le format de la date à l'affichage
};
