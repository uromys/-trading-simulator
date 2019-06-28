/*!
 * \file XXXr.h
 * \brief Projet de Trading Simulator
 * \author {Théo CHEVALIER, Elise CREMILLIEU, Anaël LACOUR, Estelle ROSE, Robin ROSSEEUW}
 * \version 2.0
 */

#pragma once
#include <iostream>
#include <QString>
#include <QDate>
#include <QWidget>
#include <QtCharts>
#include <QGridLayout>
#include <QList>
#include <QColor>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QHeaderView>
#include <QtGui>
#include <cmath>
#include <stdexcept>

using namespace std;

/*! \class TradingException
            \brief Classe permettant de gérer les exceptions des classes
    */
class TradingException {
public:
    TradingException(const QString& message) :info(message) {
        QMessageBox::critical(NULL,"Erreur",info);
    }
    QString getInfo() const { return info; }
private:
    QString info;

};
/*! \class Devise
            \brief Classe permettant de créer une monnaie, instancier une devise
    */
class Devise {
    QString code;
    QString monnaie;
    QString zone;
    Devise(const QString& c, const QString& m, const QString& z = "");
    ~Devise() = default;
    friend class DevisesManager;
public:
    QString getCode() const { return code; }
    const QString& getMonnaie() const { return monnaie; }
    const QString& getZoneGeographique() const { return zone; }
};

/*! \class ProduitFinancier
            \brief Classe virtuelle permettant de définir un produit financier qui peut être un achat ou une vente
    */

class ProduitFinancier{ //class abstraite
public :
    virtual QString displayName() = 0; // methode virtuel pure
    virtual QString displayTitre() = 0;
    virtual ~ProduitFinancier();
};
inline ProduitFinancier :: ~ProduitFinancier(){}
/*! \class Action
            \brief Classe permettant de definir l'action réalisé par l'utilisateur (vente ou achat)
    */
class Action : public virtual ProduitFinancier {
    QString code;
    QString entreprise;
    QString zone;
    Action( const QString& e, const QString& c, const QString& z = "");
    ~Action() = default;
    friend class DevisesManager;
public:
    QString getCode() const { return entreprise; }
    const QString& getMonnaie() const { return entreprise; }
    const QString& getZoneGeographique() const { return zone; }
    QString displayName(){
        return getCode();
    }
    QString displayTitre() {

        return (code + "-" +entreprise);
    }

};

/*! \class PaireDevises
            \brief Classe  permettant de savoir quelles devises sont impliquées lors d'une transaction.
                   Relation entre les devises de base et de contrepartie.
                   Cette relation est décrite par la concaténation des codes de la devise de base et de celle de contrepartie, en les séparant par une barre oblique

    */

class PaireDevises : public virtual ProduitFinancier {
    Devise* base;
    Devise* contrepartie;
    QString surnom;
    PaireDevises(Devise& b,  Devise& c, const QString& s = "");
    ~PaireDevises() = default;
    friend class DevisesManager;
public:
    const Devise& getBase() const { return *base; }
    const Devise& getContrepartie() const { return *contrepartie; }
    const QString& getSurnom() const { return surnom; }
    QString displayName(){
        return surnom;
    }
    QString displayTitre(){
        return (getBase().getCode() + " - " + getContrepartie().getCode());
    }
};
/*! \class CoursOHLC
            \brief Classe  permettant de definir les paramètres relatif au cours du marché  sur une journée avec
                    les prix d'ouverture, de fermeture du marché, le prix le plus haut et le plus bas ainsi que le volume
    */
class CoursOHLC {
    double open = 0, high = 0, low = 0, close = 0, volume = 0;
    QDate date;
    int indice; // Correspond au numéro de ligne du coursOHLC dans le fichier de base
public:
    CoursOHLC() {}
    CoursOHLC(double o, double h, double l, double c, double vol, QDate d, int i);
    double getOpen() const { return open; }
    double getHigh() const { return high; }
    double getLow() const { return low; }
    double getClose() const { return close; }
    double getVolume() const { return volume; }
    QDate getDate() const { return date; }
    int getIndice() const { return indice; }
    void setCours(double o, double h, double l, double c, double vol, QDate d, int i);
};
/*! \class EvolutionCours
            \brief Classe  permettant de voir les variations des cours du marché
    */
class  EvolutionCours {
    ProduitFinancier* financialProduct;
    CoursOHLC* cours = nullptr;
    unsigned int nbCours = 0;
    unsigned int nbMaxCours = 0;
public:
    EvolutionCours(ProduitFinancier& p){
        this->financialProduct = &p;
    }
    // EvolutionCours(const Action &a ) :stockoption(&a) {} //surchage pour pouvoir créé avec autres choses que des actions

    EvolutionCours(const EvolutionCours& e, const unsigned int nbCoursTotal, const unsigned int part); // Constructeur par recopie spécial utilisé pour obtenir sous_eCSV dans Affichage en mode manuel
    EvolutionCours(const EvolutionCours& e, const unsigned int nbCoursTotal, const QDate date);  // Constructeur par recopie spécial utilisé pour obtenir sous_eCSV dans Affichage en mode pas à pas
    EvolutionCours(const EvolutionCours& e, const unsigned int pos); // Constructeur par recopie spécial utilisé pour remplir l'EvolutionCours a partir d'un certain coursOHLC (utilisé dans mode Manuel)
    void addCours(double o, double h, double l, double c, double vol, QDate d, int indice);
    ~EvolutionCours();
    EvolutionCours(EvolutionCours& e);
    unsigned int getNbCours() const { return nbCours; }
    unsigned int getNbMaxCours() const { return nbCours; }
    CoursOHLC getCours(unsigned int i){ return cours[i]; }
    ProduitFinancier& getobjetsducours()  { return *financialProduct; }
    using iterator = CoursOHLC*;
    iterator begin() { return iterator(cours); }
    iterator end() { return iterator(cours + nbCours); }
    using const_iterator = const CoursOHLC*;
    const_iterator begin() const { return cours; }
    const_iterator end() const { return cours + nbCours; }
    const_iterator cbegin() const { return cours; }
    const_iterator cend() const { return cours + nbCours; }
};
/*! \class DeviseManager
            \brief Permet de gérer nos devises, actions importées. Design Pattern Singleton.
    */
class DevisesManager {
    Action ** actions =nullptr;
    Devise** devises = nullptr; // tableau de pointeurs sur objets Devise
    unsigned int nbDevises = 0;
    unsigned int nbMaxDevises = 0;
    mutable PaireDevises** paires = nullptr; // tableau de pointeurs sur objets PaireDevises
    mutable unsigned int nbPaires = 0;
    mutable unsigned int nbMaxPaires = 0;
    // empecher la duplication par recopie ou affectation
    DevisesManager(const DevisesManager& m) = delete;
    DevisesManager& operator=(const DevisesManager& m) = delete;
    // pour le singleton
    DevisesManager() {}
    ~DevisesManager();
    struct Handler {
        DevisesManager* instance = nullptr;
        ~Handler() { delete instance; }
    };

    static Handler handler;
public:
    static DevisesManager& getManager() {
        if (handler.instance == nullptr)
            handler.instance = new DevisesManager;
        return *handler.instance;
    }
    static void libererManager() {
        delete handler.instance;  handler.instance = nullptr;
    }
    const Devise& creationDevise(const QString& c, const QString& m="",
        const QString& z="");
    Devise &getDevise(const QString& c);
    PaireDevises &getPaireDevises(QString &c1,
        QString &c2);
    const Action &creationAction(const QString& c, const QString& m="",
                                 const QString& z="");
     Action& getAction(const QString& c)const;
};

// PROJET
/*! \class InterfaceFinale
            \brief La séquence d'objet est la suivante:
                    Le fonctionnement global est le suivant:
                        - Une fenêtre principale ayant un layout à 2 widgets (= 2 sous fenetre)
                            - Un Widget fenetreCentre qui acceuille le layout "layout_objets" dynamique
                            - Un Widget fenetreBas qui acceuille le layout "boutons" pour les boutons de bas de fenêtre
                        Pourquoi 2 Widgets et pas 2 layout ? car on pose la taille fixe afin d'avoir un design propre !
                        - On va mettre à jour la fenêtre avec l'objet voulu à un instant t, pour cela on a un compteur "état" qui permet de nous repérer dans la séquence tel que:
                            - le bouton Suivant incrémente etat de 1
                            - le bouton Précédent décrémente etat de 1
                        - On gère quel Widget affiché dans "layout_objets" avec un SWITCH (CASE) sur etat:
                            - on hide() tous les autres widgets créés
                    - on show() seulement le widget qui nous intéresse
    */
class InterfaceFinale : public QWidget{

    // On décide de mettre la plupart des objets / widgets ici en pointeur (dynamique) afin garder leur existence et valeur si l'on souhaite revenir en arrière dans la séquence du programme.

    Q_OBJECT

    // PARAMETRES FENETRE (Pour la rendre Modulable facilement comme voulu dans le sujet)
    // Tailles à modifier si besoin, en pixels. La définition de base est standard
    int hauteur = 600;
    int largeur = 690;
    // Marges des widgets suivant la dimension de la fenêtre pour avoir un affichage correct (centré)
    int margeX = 40;
    int margeY = 300;
    int taille_boutons = 80;
    // On les fixe ici pour adapter les layout et sous fenêtre en conséquence pour garder l'affichage voulu

    // BOUTONS
    QPushButton *suivant; // Afficher l'objet suivant
    QPushButton *precedent; // Afficher l'objet precedent
    QPushButton *quitter; // Afficher l'objet quitter

    // DONNEES
    int etat = 0; // Compteur signalant l'état actuel du programme, utilisé par les boutons précédent / suivant pour savoir quel objet affiché ensuite lors de l'appui.
    // on mettra ce compteur dans un SWITCH afin de savoir quel Layout afficher
    EvolutionCours* e = nullptr; // Contiendra les données du .csv importé

    // FENETRE
    QWidget *fenetreCentre; // Sous-fenêtre centrale qui accueillera les différents objets au fur et à mesures
    QWidget *fenetreBas;
    QLabel *bienvenue; // Message de Bienvenue

    // LAYOUT
    // On aura un et un seul Layout pour tous les objet, les ajoutant / enlevant à notre convenance pour afficher seulement le widget qui nous intéresse
        QVBoxLayout *layout_objets; // Ne prendra qu'un widget à chaque fois, celui qu'on veut afficher
        QHBoxLayout *boutons; // Pour regrouper les boutons en bas de la fenêtre
        QVBoxLayout *layout; // layout principal de la fenêtre

    // OBJETS

        // 1 - IMPORT
        QWidget *import = nullptr;
        QLabel *message_import;
        QLineEdit *affiche_chemin; // Affichera la string du chemin récupéré avec import
        QPushButton *ouvrir;
        QLabel *message_XML;
        QPushButton *charger; //Bouton de chargement d'une sauvegarde
        QHBoxLayout *boutons_import;
        QVBoxLayout *layout_import;

        // 2 - CHOIXPARAM
        // Composition ChoixParam -> Interface
        QWidget *choix = nullptr; // On le met en QWidget ici car le type ChoixParam n'est pas reconnu dans le .h
        // Le QLabel est directement implémenté dans le ChoixParam

        // 3 - CHOIXMODE
        QWidget *mode = nullptr;
        QLabel *message_choixmode;
        QLabel *desc_auto;
        QLabel *desc_manuel;
        QLabel *desc_pasapas;
        QGroupBox *mode_boutons;
        QVBoxLayout *boutons_layout;
        QVBoxLayout *mode_layout;
        QRadioButton *automa;
        QRadioButton *manuel;
        QRadioButton *pasapas;
            // SPECIFIQUE A MODE AUTO
            QGroupBox *strategieBoxes;
            QComboBox *choixStrategie;
            QVBoxLayout *layout_choixStrat;

        //contenu de l editeur de texte
        QString notes;
        //variable pour le fichier .csv
        QString chemin;
        //chargement du tableau d'une sauvegarde
        QTableWidget* tab;

public:
    explicit InterfaceFinale();
    void LayoutUpdate();
    void viderLayout(QLayout* layout); // Permet de vider un Layout de tous ses widgets

    // Methodes créant les divers widgets, étant nommée "creer" suivi du nom du widget à créer
    void creerImport();
    void creerChoixParam();
    void creerChoixMode();
public slots:
    void precedent_itf();   // Slot du bouton précédent
    void suivant_itf();     // Slot du bouton suivant
    void quitter_itf();     // Slot du bouton quitter
    void charger_itf();     // Slot du bouton charger
    void import_itf();      // Slot appellant la classe import pour importer le fichier .csv pour remplire EvolutionCours
    void lancerSimulation();
    void afficherDescriptionMode();
};

// AFFICHAGE

QList<QStringList> readCsvFile();
void Print(const QList<QStringList> &data);
