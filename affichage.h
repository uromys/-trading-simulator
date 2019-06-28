#pragma once

#include "choixparam.h"
#include "strategietrading.h"
#include "edittexte.h"


// Héritage depuis QCandlestickSet : Nous permet de rajouter des fonctionnalités supplémentaires propres à notre programme (notamment le volume ou la forme des bougies)
/*! \class Bougie
            \brief La classe Bougie est héritée depuis QCandlestickSet, ce qui nous permet d'ajouter des fonctionnalités en plus dont notre programme a besoin (ex: sa forme)
            Une bougie représente un coursOHLC de la manière suivante: Sa couleur est rouge quand l'Open est au dessus du Close (la cotation a baissé), et verte inversement (la cotation a augmentée).
            Le haut de sa tige indique la valeur High, et le bas de sa tige la valeur Low. La signification de ces valeurs est reporté dans le rapport pour plus de précision.
    */
class Bougie : public QCandlestickSet {
    Q_OBJECT
    CoursOHLC* cours;
    QString forme;
    int id;  //servant a obtenir toutes les bougies créées afin de pouvoir les deletes

public:
    static int curID;
    static map<int, Bougie*> objects;

    Bougie(qreal open, qreal high, qreal low, qreal close, CoursOHLC* c, qreal timestamp = 0.0, QObject *parent = nullptr):
        QCandlestickSet(open, high, low, close, timestamp, parent), cours(c){
        connect(this,SIGNAL(clicked()),this, SLOT(viewCoursOHLC_B())); // Permet quand on clique sur la bougie d'appeller le slot de CSVViewer permettant d'afficher les données de la bougie dans des QLineEdit
        connect(this,SIGNAL(doubleClicked()),this,SLOT(effectuerTransaction_B()));
        forme = calculForme();
        id = curID++;
        objects[id] = this;
    }
    const CoursOHLC& getCoursOHLC() const { return *cours; }
    QString calculForme() const ;
    const QString getForme() const { return forme; }
    ~Bougie(){ objects.erase(id); }
    static map<int, Bougie*>& GetMapOfObjects() { return objects; }
    static void toutDetruire(){ // Permet de détruire toutes les bougies
        map<int,Bougie*>::iterator Apocalypse;
        for(int i=0; i < curID ;i++){
            Bougie *adelete=objects[i]; // On lie la bougie à celle existante
            delete adelete; // on la delete
        }
        curID=0;
    }

signals:
    void clickBougie(Bougie* c);
    void doubleClickedBougie(Bougie* c);
private slots:
    void viewCoursOHLC_B(){ emit clickBougie(this); }

    void effectuerTransaction_B(){ emit doubleClickedBougie(this); } // Même concept qu'au dessus
};


/*! \class boutonTransaction
            \brief Cette classe hérite de QPushButton.
                Elle a été créée afin de prendre un coursOHLC et le QDoubleSpinBox du montant
                pour pouvoir les transmettre au slot vente() ou achat() quand une transaction est effectuée.
    */
class boutonTransaction : public QPushButton {
    Q_OBJECT

    CoursOHLC *cours; // Le coursOHLC qui sera associé au bouton lors de sa création pour le renvoyer lorsqu'on lui clique dessus
    QDoubleSpinBox *montant; // le montant de la transaction à transmettre

public:
    boutonTransaction(QString text, CoursOHLC *c, QDoubleSpinBox *m, QWidget *parent = nullptr): QPushButton(text,parent), cours(c), montant(m) {
        connect(this,SIGNAL(clicked()),this,SLOT(slotAvecCours()));
    }
    void setCours(CoursOHLC* c){
        cours = c;
    }

signals:
    void clicAvecCours(CoursOHLC* c, QDoubleSpinBox *m);
private slots:
    void slotAvecCours() { emit clicAvecCours(cours,montant); }
};

/*! \class CSVViewer
            \brief la classe CSVViewer contient l'intégralité des graphes et des données qui seront affichés lors des
                    différentes simulation. Elle permet d'effectuer des transactions et a de nombreuses méthodes liées aux modes qui l'utilisent.
                    Elle possède également 2 vue principale : Une vue globale de l'échantillon et une vue en détails (zoomée) de ce dernier afin de le manipuler de manière plus précise.
                    Sur la partie gauche nous avons un graphe en chandelier contenant des bougies, et un graphe contenant les volumes des cours représentés par les bougies.
                    Sur la partie droite nous avons les boutons liés à la manipulation de la fenêtre ou des transactions, ainsi que les informations sur la bougie cliquée (Cours & Indicateurs)
                    Sur la partie basse nous avons un historique des dernières transactions effectuées lors de la simulation.
    */
class CSVViewer : public QWidget{
    Q_OBJECT

    // PARAMETRE FENETRE
    // Si on veut changer la taille de la fenêtre, on peut modifier ces valeurs ci
    int largeur = 1300;
    int hauteur = 700;
    int taille_LineEdit = 100;
    int taille_boutons = 170;

    // GRAPHE
    QCandlestickSeries *series; // un ensemble de bougies
    QBarSeries *barseries; // Un ensemble de barre de volume
    QChart *graphe_haut = nullptr; // un graphique sur un ensemble de bougies
    QChart *graphe_bas = nullptr; // un graphique pour un ensemble de volume
    QChartView *chartView_haut; // un viewer pour le 1er graphique
    QChartView *chartView_bas; // un viewer pour le 2e graphique
    QStringList categorieBougie; // Légende du bas des graphes : 1 bougie par catégorie
    QStringList categorieVolume; // Permet de segmenter les volumes comme les bougies pour un affichage similaire

    // INFOS BOUGIES
    QGroupBox *groupeInfos;
    QLineEdit* open;
    QLineEdit* high;
    QLineEdit* low;
    QLineEdit* close;
    QLineEdit* date;
    QLineEdit* forme;
    QLabel* openl;
    QLabel* highl;
    QLabel* lowl;
    QLabel* closel;
    QLabel* datel;
    QLabel* formel;
        // LAYOUT DE CES WIDGETS
        QHBoxLayout* copen;
        QHBoxLayout* chigh;
        QHBoxLayout* clow;
        QHBoxLayout* cclose;
        QHBoxLayout* cdate;
        QHBoxLayout* cforme;
        QVBoxLayout* layoutInfos; // sera mis dans le QGroupBox

    // INDICATEURS
        // On rajoute également nos nouveaux indicateurs si besoins
        // OBJETS (COMPOSITION)
        IndicTechnique *indic;
        RSI *rsi_obj;
        EMA *ema_obj;
        MACD *macd_obj;
        // VotreIndic *votreindic_obj

        // AFFICHAGE DES INDICS
        QGroupBox *groupeIndic;
        QLineEdit* rsi;
        QLineEdit* ema;
        QLineEdit* macd;
        // QLineEdit* votreindic

        QLabel* rsil;
        QLabel* emal;
        QLabel* macdl;
        // QLabel* votreindicl

        // LAYOUT DE CES WIDGETS
        QHBoxLayout* crsi;
        QHBoxLayout* cema;
        QHBoxLayout* cmacd;
        // QHBoxLayout* cvotreindic;

        QVBoxLayout* layoutIndic; // Contiendra les layout précédents des indicateurs pour se mettre dans le QGroupBox

    // TABLEAU DE TRANSACTIONS
    vector<Transaction> transac;
    QTableWidget* Tableau;
    unsigned int nbTransactions = 0 ;

    // BOUTONS GENERAUX
    QPushButton* terminer;
    QPushButton* details;
    QPushButton* annuler;
    // BOUTONS VUE DETAILS
    QPushButton* gauche;
    QPushButton* droite;

    // BOUTONS PAS A PAS
    QGroupBox *boutonsTransac;
    boutonTransaction *achat_pap;
    boutonTransaction *vente_pap;
    QGroupBox* groupePasaPas;
    QVBoxLayout* layoutPasaPas;

    // LAYOUT PRINCIPAUX
    QHBoxLayout* fleches;
    QVBoxLayout* partieDroite; // Contiendra en vertical les Label/LineEdit/Boutons
    QVBoxLayout* partieDroiteDroite; // Partie tout à droite
    QVBoxLayout* partieGauche; // Contiendra les 2 graphes
    QHBoxLayout* fenetre;
    QVBoxLayout* final; // Layout final

    // DONNEES
    QString chemin; // Chemin du fichier importé pour pouvoir enregistrer le csv à la fin de la simulation
    QString mode_affichage; // Mode d'affichage selectionné (Mode Manuel / Mode Pas à Pas)
    ChoixParam *param; // On récupère le paramétrage défini précédement
    EvolutionCours *eCSV;
    EvolutionCours *sous_eCSV = nullptr; // Sera une sous partie de eCSV pour la vue en détails. Seulement 30 coursOHLC y seront contenu par défaut, valeur pouvant être changé ci dessous.
    unsigned int nbBougiesDetails = 30; // Permet de changer le nombre de coursOHLC affichés dans la vue en détails.
    unsigned int part = 0; // Facteur multiplicatif utilisé lors de la modification des graphes pour savoir quelle partie des données affichées

    // AMITIE
    friend class Bougie;
    friend class EvolutionCours;
    friend class Transaction;

public:
    explicit CSVViewer(EvolutionCours *e, ChoixParam* p, QString c, QWidget *parent, QString mode = "manuel");
    CSVViewer(CSVViewer *c);
    void remplissageBougiesManuel();
    void remplissageBougiesPasAPas();
    void remplissageVolume();
private slots:
    void viewCoursOHLC(Bougie* b); // Permet d'envoyer les infos de la bougie cliquée
    void afficherTableau();
    // VUES
    void vueGlobale();
        // EN DETAILS
        void vueDetails();
        void suivant();
        void precedent();


    // TRANSACTION
    void effectuerTransaction(Bougie* b);
    void annuler_transaction();
    void vente(CoursOHLC *c, QDoubleSpinBox *m);
    void achat(CoursOHLC *c, QDoubleSpinBox *m);
    void inactif(); // rendre la fenêtre d'affichage inactive
    void actif(); // rendre la fenêtre d'affichage de nouveau active
    void action(){
        param->setDateSuivante(); // La bougie où l'on vient de faire la transaction sera maintenant non plate


        if(details->text() == "Affichage global"){
            vueDetails();
        }
        else
            vueGlobale();
    }

};
