#pragma once
#include <QtGui>
#include <QtXml>
#include <QWidget>
#include <QFileDialog>
#include<QMessageBox>
#include <QTableWidget>


using namespace std;

/*! \class Sauvegarde_XML
            \brief Classe servant à la sauvegarde des trois fichiers demandés en xml.
            Les methodes static permettent d'être appelées sans instancier d'objet.
            Les attributs static quant à eux sont communs à tous les objets de la classe.
            */


// La plupart des méthodes et attributs seront statiques afin d'y accèder dans tout notre programme
// On peut aussi utiliser la méthode faite pour le .csv et la sauvegarde en .txt, qui nécéssite le passage de ces informations par paramètres de chaque constructeur jusqu'à l'Editeur de Texte
class Sauvegarde_XML:public QObject{
       Q_OBJECT

private:

        QDomDocument doc;
        QDomElement mesures;
        QFile file;
        QTextStream out;
        static QString chemin;
        static QString notes;
        static QTableWidget* tab;
        static QString pathSave;

public:

        static bool CreationDuFichier_XML(QString note);
        static bool LectureDuFichier_XML();
        static void setChemin(QString c){chemin=c;}
        static void setTableau(QTableWidget* t){tab=t;}
        static QString getChemin() { return chemin; }
        static QString getNotes() { return notes; }
        static QTableWidget* getTab() { return tab; }
        static QString getPathSave() { return pathSave; }
        Sauvegarde_XML(){}
        ~Sauvegarde_XML(){}

};


