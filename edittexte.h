#pragma once

#include "trading.h"
#include "sauvegarde_simulation_xml.h"


/*! \class EditTexte
            \brief La classe EditTexte hérite de QMainWindow et permet la saisie et l'enregistrement des notes prises lors d'une simulation
            et la sauvegarde dans un fichier texte ou XML du fichier .csv importé au début, du tableau des transactions réalisées, ainsi que les notes prises.
    */
class EditTexte : public QMainWindow
{
    Q_OBJECT

    // FENETRE
    // Si on veut changer la taille de la fenêtre, on peut modifier ces valeurs ci
    int largeur = 600;
    int hauteur = 550;

    // ONGLET

    QMenu *menuFichier;
        QAction *actionNew;
        QAction *actionOpen;
        QAction *actionSave;
        QAction *actionSaveXML;
        QAction *actionExit;
    // l'onglet menu aura 5 actions

    QMenu *menuEdition;
        QAction *actionUndo;
        QAction *actionRedo;
        QAction *actionCut;
        QAction *actionCopy;
        QAction *actionPaste;
        QAction *actionSelectAll;
    // l'onglet edition aura 6 actions

    QToolBar *barre;

    // ZONE TEXTE
    QTextEdit *zoneTexte;
    QString text;


    // DONNEES
    QString chemin;
    QTableWidget *transac;

public:
    EditTexte(QTableWidget* tab, QString c);
public slots:
    void quitter(){ QApplication::quit(); }
    void saveFile();
    void saveFileXML(){
        if (Sauvegarde_XML::CreationDuFichier_XML(this->zoneTexte->toPlainText()))
            QMessageBox::information(this,"Enregistrement","Données sauvegardées dans le dossier suivant: \n" + Sauvegarde_XML::getPathSave());
        else
            QMessageBox::warning(this,"Erreur","Une erreur s'est produite lors de l'enregistrement des fichiers. \nEssayez d'exporter en fichier .txt");
    }
    void openFile();
};
