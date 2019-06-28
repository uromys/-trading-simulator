#include "affichage.h"
/*
    OPTIMISATION A REALISER:
        - IMPORTANT : Réfléchir à tous les endroits où c'est nécéssaire de faire des delete ou ~destructeur (relire le mail envoyé par le prof, je l'ai mis vendredi matin dans la conv)
        - IMPORTANT : tester plusieurs jeux de données pour être sûr que notre programme marche bien ! Vérifier si les valeurs sont bonnes (cours, transactions...etc)
        - Essayer de centraliser les divergences mode pas à pas / manuel pour que ce soit plus claire, quite à faire le design pattern Adapter ou deux constructeurs différents selon le mode
        - Faire tourner le programme et tester plein de trucs pour être sûr qu'il n'a aucune faille
        - Réfléchir à ce qu'il faut montrer dans la vidéo (faire un mini script)
        - Petit problème dans Pas à Pas quand on va trop loin dans la vue en Détails -> Me laisser le régler demain matin
        - Essayer de bien placer "Montant" dans Mode pas à pas au dessus du QDoubleSpinBox, il est espacé je sais pas pourquoi... c'est bizarre


*/

using namespace std;

// BOUGIE

// Attributs statiques :
int Bougie::curID = 0;
map<int, Bougie*> Bougie::objects;

QString Bougie::calculForme() const {

    // CARACTERISTIQUES
    enum type_corps {plat, hausse, baisse, corps_defaut};
    enum type_meche {plate, haute, basse, meche_defaut};
    enum type_bougie {toupie, bougie_defaut};
    type_corps corps;
    type_meche meche;
    type_bougie boug;

    // TESTS
    corps = corps_defaut; // Initialisation par défaut (par sécurité)
    if(cours->getOpen() == cours->getClose())
        corps = plat;
    if(cours->getOpen() < cours->getClose())
        corps = hausse; // correspond à verte
    if(cours->getOpen() > cours->getClose())
        corps = baisse; // correspond à rouge

    meche = meche_defaut; // Initialisation par défaut (par sécurité)
    if(abs(cours->getClose() - cours->getLow()) == abs(cours->getHigh() - cours->getOpen()))
        meche = plate;
    if(abs(cours->getClose() - cours->getLow()) < abs(cours->getHigh() - cours->getOpen()))
        meche = haute;
    if(abs(cours->getClose() - cours->getLow()) > abs(cours->getHigh() - cours->getOpen()))
        meche = basse;

    boug = bougie_defaut; // Initialisation par défaut (par sécurité)
    if(abs(cours->getOpen() - cours->getClose()) < abs(cours->getHigh() - cours->getLow()))
        boug = toupie;

    // DEDUCTION DE LA FORME

        // CAS TOUPIE
        if(corps == plat && boug == toupie)
                return "Doji"; // la bougie doit être écrasée et une toupie
        if(corps == hausse && meche == basse)
                return "Marteau"; // la bougie doit être verte & avoir une mèche basse > mèche haute
        if (corps == baisse && meche == haute)
                return "Etoile Filante"; // la bougie doit être rouge & avoir une mèche haute > mèche basse
        if (corps == baisse && meche == basse)
                return "Pendu"; // la bougie doit être rouge & avoir une mèche basse < mèche haute

        return "Toupie"; // Si on a aucun des cas particuliers d'une toupie, alors c'est une toupie si le 1er if est vérifié

        // CAS AJOUTES
        if(cours->getLow() == cours->getOpen() && cours->getHigh() == cours->getClose())
            return "Marubozu";

        if(cours->getLow() == cours->getClose() && cours->getHigh() == cours->getOpen())
            return "Morubozu";

        // * INSERER AUTRES CAS ICI *

    return "Inconnu"; // Return par défaut si aucune des conditions ci-dessus n'ait rempli.
}

// CSVVIEWER

CSVViewer::CSVViewer(EvolutionCours *e, ChoixParam* p, QString c, QWidget *parent, QString mode): QWidget(parent), chemin(c), mode_affichage(mode){

    // COPIE DES PARAMETRES

    param = p->clone();

    // SAUVEGARDE DANS eCSV

    auto it = e->begin();
    // Placement de l'itérateur sur le cours correspondant à la date rentrée en paramètre
    while(it != e->end() && (*it).getDate() != param->getDate())
        it++;

    if(it == e->end()){
        QMessageBox::critical(this,"Erreur","La date parametrée est invalide et dépasse l'échantillon fourni. \nVeuillez revoir votre choix de paramètres. \nLe programme va maintenant s'arrêter.");
        QApplication::quit(); // on quitte l'application
    }
    else{
        int pos = distance(e->begin(),it);
        if(mode_affichage == "manuel"){
            // MODE MANUEL : On ajoute seulement les cours à partir de la date donnée dans ChoixParam
            eCSV = new EvolutionCours(*e,static_cast<unsigned int>(pos));
        }
        else{
            // MODE PAS A PAS : On copie l'EvolutionCours initial pour avoir toutes les données.
            eCSV=new EvolutionCours(*e);

            // CREATION BOUTONS EXCLUSIFS PAS A PAS
            // On le fait en amont pour mettre à jour directement les boutons achat_pap et vente pap dans le remplissage pas à pas

            QLabel *messMontant = new QLabel("Montant :");

            QDoubleSpinBox *montant = new QDoubleSpinBox;
            montant->setRange(0.00,1000000.00);
            montant->setValue(35.00);

            // BOUTONS
            // it pointe sur le bon cours
            achat_pap = new boutonTransaction("Achat",it,montant);
            vente_pap = new boutonTransaction("Vente",it,montant);
            // On passe un pointeur vers le QDoubleSpinBox, car impossible de passer un pointeur vers la valeur (double) du QDoubleSpinBox seulement
            QPushButton *rienFaire = new QPushButton("Ne rien faire");
            // Appuyer sur "Ne rien faire" va enclencher la bougie suivante

            // CONNEXION DES BOUTONS
            connect(achat_pap,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(achat(CoursOHLC*,QDoubleSpinBox*)));
            connect(vente_pap,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(vente(CoursOHLC*,QDoubleSpinBox*)));
            // ACTION : Permet de modifier la date
            connect(achat_pap,SIGNAL(clicked()),this,SLOT(action()));
            connect(vente_pap,SIGNAL(clicked()),this,SLOT(action()));
            connect(rienFaire,SIGNAL(clicked()),this,SLOT(action()));

            layoutPasaPas= new QVBoxLayout();
            layoutPasaPas->addWidget(messMontant);
            layoutPasaPas->addWidget(montant);
            layoutPasaPas->addWidget(achat_pap);
            layoutPasaPas->addWidget(vente_pap);
            layoutPasaPas->addWidget(rienFaire);

            boutonsTransac = new QGroupBox("Transaction");
            boutonsTransac->setLayout(layoutPasaPas);
            // On met ce layout dans celui de la fenêtre à la fin du constructeur

        }
    }

    // GRAPHE EN CHANDELIER

        // CANDLESTICK

        series = new QCandlestickSeries();
        series->setName(eCSV->getobjetsducours().displayName()); // Nom de la série de bougies, importé depuis le nom du fichier .csv
        series->setIncreasingColor(QColor(Qt::green));
        series->setDecreasingColor(QColor(Qt::red));

        // CREATION DES CATEGORIES & BOUGIES

        if(mode_affichage == "pasapas")
            remplissageBougiesPasAPas();
        else
            remplissageBougiesManuel();

        // Les deux modes ayant leur affichage ne divergeant que légèrement,
        // On décide de garder un seul constructeur et de diverger le remplissage des graphes ainsi

        // CREATION DU GRAPHE

        QString titre = eCSV->getobjetsducours().displayTitre(); // Permet de récupérer le code des Devises du fichier .csv contenu dans EvolutionCours pour les afficher en tant que titre du graphe
        // MODE PAS A PAS : On affiche la date de la transaction actuelle dans le titre pour plus de claireté
        if(mode_affichage == "pasapas"){
            QString dateTransactionActuelle = param->getDate().toString("dd.MM.yyyy");
            titre += " - Transaction pour le " + dateTransactionActuelle; // On ajoute cette partie au titre actuel
        }

        graphe_haut = new QChart();
        graphe_haut->addSeries(series);
        graphe_haut->setTitle(titre);
        graphe_haut->setAnimationOptions(QChart::SeriesAnimations); // Animation fluide pour l'affichage

        // CREATION DES AXES DU GRAPHE

        graphe_haut->createDefaultAxes();

        QBarCategoryAxis *axisXBougie = new QBarCategoryAxis();
        axisXBougie->append(categorieBougie);
        graphe_haut->setAxisX(axisXBougie,series);

        graphe_haut->legend()->setVisible(false); // On n'affiche pas la légende, tout est dans le titre du graphe ici
        graphe_haut->legend()->setAlignment(Qt::AlignBottom);


        // VUE DU CHART (GRAPHE)

        QChartView *chartView_haut = new QChartView(graphe_haut,this);
        chartView_haut->setRenderHint(QPainter::Antialiasing);
        chartView_haut ->setFixedHeight(static_cast<int>(hauteur*0.4));
        chartView_haut ->setFixedWidth(static_cast<int>(largeur*0.72));

    // GRAPHE EN BARRE VOLUME

        // CREATION DU SET

        barseries = new QBarSeries();

        remplissageVolume(); // le remplissage est le même pour mode manuel ou pas à pas
        // On l'a déporté dans une fonction afin de pouvoir facilement éditer son fonctionnement et pouvoir le rappeller n'importe quand

        // CREATION DU GRAPHE

        graphe_bas = new QChart();
        graphe_bas->addSeries(barseries);
        graphe_bas->setTitle("Volume des coursOHLC");
        graphe_bas->setAnimationOptions(QChart::SeriesAnimations);

        // CREATION DES AXES DU GRAPHE

        graphe_bas->createDefaultAxes();
        QBarCategoryAxis *axisXVolume = new QBarCategoryAxis();
        axisXVolume->append(categorieVolume);
        graphe_bas->setAxisX(axisXVolume,barseries);

        graphe_bas->legend()->setVisible(false); // On n'affiche pas la légende, tout est dans le titre du graphe ici
        graphe_bas->legend()->setAlignment(Qt::AlignBottom);

        // VUE DU CHART (GRAPHE)

        QChartView *chartView_bas = new QChartView(graphe_bas,this);
        chartView_bas->setRenderHint(QPainter::Antialiasing);
        chartView_bas ->setFixedHeight(static_cast<int>(hauteur*0.4));
        chartView_bas ->setFixedWidth(static_cast<int>(largeur*0.72));

    // PARTIE DROITE (Boutons, Valeurs, Indicateurs...)

    // LABELS

        openl = new QLabel("Open");
        highl = new QLabel("High");
        lowl = new QLabel("Low");
        closel = new QLabel("Close");
        datel = new QLabel("Date");
        formel = new QLabel("Forme");

    // LINEEDIT

        open = new QLineEdit;
        open->setReadOnly(true); // Permet de rendre les QLineEdit non modifiable
        high = new QLineEdit;
        high->setReadOnly(true);
        low = new QLineEdit;
        low->setReadOnly(true);
        close = new QLineEdit;
        close->setReadOnly(true);
        date = new QLineEdit;
        date->setReadOnly(true);
        forme = new QLineEdit;
        forme->setReadOnly(true);
        // On a laissé des QLineEdit au lieu de mettre des Label pour l'esthetique et pour laisser la possibilité de modifier les bougies dans une possible future évolution du programme.

        // Taille standard pour les champs
        open->setFixedWidth(taille_LineEdit);
        high->setFixedWidth(taille_LineEdit);
        low->setFixedWidth(taille_LineEdit);
        close->setFixedWidth(taille_LineEdit);
        date->setFixedWidth(taille_LineEdit);
        forme->setFixedWidth(taille_LineEdit);

    // BUTTONS

        details = new QPushButton("Voir en détails");
        details->setFixedWidth(taille_boutons);
        connect(details,SIGNAL(clicked()),this,SLOT(vueDetails()));

        QDir dir("../"); // Permet d'accéder aux ressources depuis un autre dossier que celui de l'executable

        gauche = new QPushButton();
        gauche->setVisible(false); // Permet de rendre invisible le bouton dans un premier temps
        gauche->setIcon(QIcon(dir.relativeFilePath("../icones/backward.png")));
        gauche->setFixedWidth(32);
        gauche->setFixedHeight(32);
        connect(gauche,SIGNAL(clicked()),this,SLOT(precedent()));

        droite = new QPushButton();
        droite->setVisible(false);
        droite->setIcon(QIcon(dir.relativeFilePath("../icones/forward.png")));
        droite->setFixedWidth(32);
        droite->setFixedHeight(32);
        connect(droite,SIGNAL(clicked()),this,SLOT(suivant()));

        annuler = new QPushButton("Annuler transaction");
        annuler->setFixedWidth(taille_boutons);
        connect(annuler,SIGNAL(clicked()),this,SLOT(annuler_transaction()));


        terminer = new QPushButton("Terminer");
        terminer->setFixedWidth(taille_boutons);
<<<<<<< HEAD
        connect(terminer,SIGNAL(clicked()),this,SLOT(afficherTableau()));
        connect(terminer,SIGNAL(clicked()),parent,SLOT(deleteLater())); // On supprime la fenetre de CSVViewer
        connect(terminer,SIGNAL(clicked()),this,SLOT(deleteLater())); // On supprime l'objet CSVViewer
=======
        connect(terminer,SIGNAL(clicked()),this,SLOT(afficherTableau())); // Permet de réaficher le 1er menu lorsqu'on appuis sur Quitter
        //connect(terminer,SIGNAL(clicked()),parent,SLOT(deleteLater())); // On supprime la fenetre de CSVViewer
        //connect(terminer,SIGNAL(clicked()),this,SLOT(deleteLater())); // On supprime l'objet CSVViewer
>>>>>>> origin/elise1406

    // INDICATEURS

        // Avoir séparer les étapes de créations des indicateurs ici permet de rendre l'ajout de nouveaux indicateurs simples par reproduction de ce qui a déjà été fait pour ceux déjà présent
        // La démarche est expliquée à titre indicatif en commentaires

        // CREATION DES OBJETS

        // Il faut bien passer le EvolutionCours original pour que les indicateurs soient dans le bon ordre pour chaque bougie
        indic = new IndicTechnique(e);
        rsi_obj = new RSI(indic);
        ema_obj = new EMA(indic);
        macd_obj = new MACD(indic);
        // votreindic_obj = new NomIndic(indic);

        // ELEMENT DU QGROUPBOX

        groupeIndic = new QGroupBox("Indicateurs");
        rsi = new QLineEdit();
        rsil = new QLabel("RSI");
        ema = new QLineEdit();
        emal = new QLabel("EMA");
        macd = new QLineEdit();
        macdl = new QLabel("MACD");
        // votreindic = new QLineEdit();
        // votreindicl = new QLabel("NomIndic");

        // TAILLE DES LINEEDIT
        rsi->setFixedWidth(taille_LineEdit);
        ema->setFixedWidth(taille_LineEdit);
        macd->setFixedWidth(taille_LineEdit);
        // votreindic->setFixedWidth(taille_LineEdit);

        // LAYOUT DES QLABEL + QLINEEDIT

        crsi = new QHBoxLayout();
        crsi->addWidget(rsil);
        crsi->addWidget(rsi);
        cema = new QHBoxLayout();
        cema->addWidget(emal);
        cema->addWidget(ema);
        cmacd = new QHBoxLayout();
        cmacd->addWidget(macdl);
        cmacd->addWidget(macd);
        // cvotreindic = new QHBoxLayout();
        // cvotreindic->addWidget(votreindic);
        // cvotreindic->addWidget(votreindicl);

        layoutIndic = new QVBoxLayout();
        partieDroiteDroite = new QVBoxLayout();
        layoutIndic->addLayout(crsi);
        layoutIndic->addLayout(cema);
        layoutIndic->addLayout(cmacd);
        // layoutIndic->addLayout(votreindic);

        groupeIndic->setLayout(layoutIndic);

        partieDroiteDroite->addWidget(groupeIndic);

        // AJOUT DES BOUTONS DE TRANSACTIONS PAS A PAS
        if(mode_affichage == "pasapas"){
            partieDroiteDroite->addWidget(boutonsTransac);
        }

    // TABLEAU DE TRANSACTIONS

        Tableau = Tableau::createTable();  // crée un Tableau de transaction
        // Le 2ème "Tableau" est le namespace défini à la fin de strategietrading.h

    // LAYOUT

        // PARTIE DROITE (Boutons/LineEdit/Label/Indicateurs)

        copen = new QHBoxLayout;
        chigh = new QHBoxLayout;
        clow = new QHBoxLayout;
        cclose = new QHBoxLayout;
        cdate = new QHBoxLayout;
        cforme = new QHBoxLayout;
        fleches = new QHBoxLayout;
        partieDroite = new QVBoxLayout;

        copen->addWidget(openl);
        copen->addWidget(open);

        chigh->addWidget(highl);
        chigh->addWidget(high);

        clow->addWidget(lowl);
        clow->addWidget(low);

        cclose->addWidget(closel);
        cclose->addWidget(close);

        cdate->addWidget(datel);
        cdate->addWidget(date);

        cforme->addWidget(formel);
        cforme->addWidget(forme);

        fleches->addWidget(gauche);
        fleches->addWidget(droite);

        // RAPPEL : l'ordre d'ajout des Widgets est aussi leur ordre d'affichage dans le layout
        groupeInfos = new QGroupBox("Infos Bougie");
        layoutInfos = new QVBoxLayout();

        layoutInfos->addLayout(copen);
        layoutInfos->addLayout(chigh);
        layoutInfos->addLayout(clow);
        layoutInfos->addLayout(cclose);
        layoutInfos->addLayout(cdate);
        layoutInfos->addLayout(cforme);

        groupeInfos->setLayout(layoutInfos);

        partieDroite->addWidget(groupeInfos);
        partieDroite->addWidget(annuler);
        partieDroite->addWidget(details);
        partieDroite->addLayout(fleches);
        partieDroite->addWidget(terminer);

        // PARTIE GAUCHE (Graphes)

        partieGauche = new QVBoxLayout;

        partieGauche->addWidget(chartView_haut);
        partieGauche->addWidget(chartView_bas);

    fenetre = new QHBoxLayout;

    fenetre->addLayout(partieGauche);
    fenetre->addLayout(partieDroite);
    fenetre->addLayout(partieDroiteDroite);

    final= new QVBoxLayout;
    final->addLayout(fenetre);
    final->addWidget(Tableau);

    this->setLayout(final);

    // On fixe les tailles pour éviter le redimensionnement
    Tableau->setFixedSize(static_cast<int>(largeur*0.99),static_cast<int>(hauteur*0.17));
    this->setFixedSize(largeur,hauteur);
    parent->setFixedSize(largeur,hauteur);

}

void CSVViewer::remplissageBougiesManuel(){
    categorieBougie.clear(); // On vide la catégorie avant de rajouter des bougies
    for(EvolutionCours::iterator it = eCSV->begin() ; it!=eCSV->end() ; ++it) { //
        CoursOHLC& cours = *it;
        // On créer une bougie correspondante pour chaque coursOHLC
        Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
        connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
        connect(bougie,SIGNAL(doubleClickedBougie(Bougie*)),this,SLOT(effectuerTransaction(Bougie*))); // permet d'ouvrir la fenêtre de transaction
        categorieBougie << QString::number(it->getIndice() + 1); // A chaque bougie correspondra un n° pour les repérer : la postion du CoursOHLC dans EvolutionCours, soit le n° de ligne du fichier .csv +1, pour commencer à 1 et non à 0
        series->append(bougie);
    }
}

void CSVViewer::remplissageBougiesPasAPas(){
    categorieBougie.clear(); // On vide la catégorie avant de rajouter des bougies

    // BOUGIES NORMALES (Avant date ou Transactions effectuées)
    auto  it = eCSV->begin();
    while(it->getDate()!=param->getDate()){

        CoursOHLC& cours = *it;

        // CREATION BOUGIES
        Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
        connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
        categorieBougie << QString::number(it->getIndice() + 1);
        series->append(bougie);
        it++;
    }

    // MISE A JOUR DES BOUTONS
    // On transmet le cours de la prochaine transaction
    achat_pap->setCours(it);
    vente_pap->setCours(it);

    // BOUGIES PLATES (Transactions à venir)
    while(it!=eCSV->end()){

        CoursOHLC& cours = *it;

        // CREATION BOUGIES
        Bougie *bougie = new Bougie(cours.getOpen(),cours.getOpen(),cours.getOpen(),cours.getOpen(),&cours);
        categorieBougie << QString::number(it->getIndice() + 1);
        series->append(bougie);
        it++;
   }
}

void CSVViewer::remplissageVolume(){
    categorieVolume.clear();
    QBarSet *set = new QBarSet("Volumes");
    for(auto it=eCSV->begin(); it != eCSV->end(); ++it)
    {
        *set << it->getVolume();
        categorieVolume << QString::number(it->getIndice() + 1);
        barseries->append(set);
    }
}

// Permet de zoomer sur le graphe en créeant un sous_eCSV de 30 coursOHLC

void CSVViewer::vueDetails(){

    // Par sécurité, on empêche la vue en détail d'un échantillon déjà affiché dans sa globalité
    if(eCSV->getNbCours() < nbBougiesDetails){
        QMessageBox::warning(this,"Impossible","L'échantillon actuel est déjà afiché dans sa globalité.");
    }

    else{

        // REMPLISSAGE sous_eCSV + MODIF DES BOUTONS < >

        if(sous_eCSV != nullptr){ // Permet de supprimer sous_eCSV pour passer à l'échantillon suivant / précédent avec les boutons
            delete sous_eCSV;
        }

        if(mode_affichage == "pasapas"){
            sous_eCSV = new EvolutionCours(*eCSV, nbBougiesDetails, param->getDate()); // On affiche les bougies autour de la bougie actuellement en pleine transaction
        }
        else
            sous_eCSV = new EvolutionCours(*eCSV, nbBougiesDetails, part); // Initialisation avec les données de la vue en détails par surcharge du constructeur par recopie.

        if(sous_eCSV->getNbMaxCours() == 0){
            QMessageBox::warning(this,"Impossible","Impossible d'aller plus loin. \nVous avez atteint la limite de l'échantillon.");
            part--; // car on l'avait incrémenté avant d'aller dans cette fonction, or on ne peut pas continuer donc il doit garder sa valeur avant l'appel du slot "suivant()".
        }
        else{
            details->setText("Affichage global"); // Le bouton "Voir en détails" se transforme pour revenir à l'affichage global
            connect(details,SIGNAL(clicked()),this,SLOT(vueGlobale()));

            // On va donc mettre à jour les données des graphes avec ce sous_eCSV

            // MISE A JOUR DES DONNEES

                // CANDLESTICK SERIES

                // On vide les données du graphe précédent
                graphe_haut->removeSeries(series);
                series->clear();
                categorieBougie.clear();

                // Puis on en ajoute de nouvelles
                if(mode_affichage!="pasapas"){

                    // MODE MANUEL :
                    for(EvolutionCours::iterator it = sous_eCSV->begin() ; it!=sous_eCSV->end() ; ++it) { //
                        CoursOHLC& cours = *it;
                        Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
                        connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*)));
                        connect(bougie,SIGNAL(doubleClickedBougie(Bougie*)),this,SLOT(effectuerTransaction(Bougie*)));
                        categorieBougie << QString::number(it->getIndice() + 1);
                        series->append(bougie);
                    }
                    // On affiche les boutons <>, maintenant actifs !
                    // La navigation est seulement autorisée pour mode manuel
                    gauche->setVisible(true);
                    droite->setVisible(true);
                }

                else {

                        // MODE PAS A PAS :
                        auto  it = sous_eCSV->begin();

                        // BOUGIES NORMALES (Avant date ou Transactions effectuées)
                        while(it->getDate()!=param->getDate()){

                            CoursOHLC& cours = *it;

                            // CREATION BOUGIES
                            Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
                            connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
                            categorieBougie << QString::number(it->getIndice() + 1);
                            series->append(bougie);
                            it++;
                        }

                        // MISE A JOUR DES BOUTONS
                        // On transmet le cours de la prochaine transaction
                        achat_pap->setCours(it);
                        vente_pap->setCours(it);

                        // BOUGIES PLATES (Transactions à venir)
                        while(it!=sous_eCSV->end()){

                            CoursOHLC& cours = *it;

                            // CREATION BOUGIES
                            Bougie *bougie = new Bougie(cours.getOpen(),cours.getOpen(),cours.getOpen(),cours.getOpen(),&cours);
                            categorieBougie << QString::number(it->getIndice() + 1);
                            series->append(bougie);
                            it++;
                       }
                } // fin else mode pas à pas

                // GRAPHE CHANDELIER
                // Le titre va diverger selon le mode
                QString titre;
                if(mode_affichage == "pasapas"){
                    QString dateTransactionActuelle = param->getDate().toString("dd.MM.yyyy");
                    titre = sous_eCSV->getobjetsducours().displayTitre() + " - Transaction pour le " + dateTransactionActuelle;
                }
                else{
                    QString partie = QString::number(part+1); // On va afficher le n° de la partie actuellement affichée dans le titre du graphe
                    titre = sous_eCSV->getobjetsducours().displayTitre() + " - " + partie;
                }
                graphe_haut->setTitle(titre);
                graphe_haut->addSeries(series);

                // RECREATION DES AXES
                graphe_haut->createDefaultAxes();
                QBarCategoryAxis *axisXBougie = new QBarCategoryAxis();
                axisXBougie->append(categorieBougie);
                graphe_haut->setAxisX(axisXBougie,series);


                // BAR SERIES

                // On vide les données du graphe précédent
                graphe_bas->removeSeries(barseries);
                barseries->clear(); // On supprime la série de Barres de Volume précédente pour lui faire prendre de nouveaux volumes
                categorieVolume.clear(); // on vide la catégorie précédente pour afficher la nouvelle

                // Puis on en ajoute de nouvelles
                QBarSet *set = new QBarSet("Volume");
                for(auto it=sous_eCSV->begin(); it != sous_eCSV->end(); ++it)
                {
                    *set << it->getVolume();
                    categorieVolume << QString::number(it->getIndice()+1);
                    barseries->append(set);
                }

                // GRAPHE VOLUME
                graphe_bas->addSeries(barseries);
                graphe_bas->createDefaultAxes();
                QBarCategoryAxis *axisXVolume = new QBarCategoryAxis();
                axisXVolume->append(categorieVolume);
                graphe_bas->setAxisX(axisXVolume,barseries);

        } // fin else eCSV vide
    } // fin else echantillon déjà affiché au global
}

// Permet de passer au sous ensemble sous_eCSV

void CSVViewer::suivant(){
    if(sous_eCSV != nullptr){
        part++;
        vueDetails(); // On rappelle la méthode de modification
    }
}

// Permet de passer au sous ensemble sous_eCSV précédent
void CSVViewer::precedent(){
    if(sous_eCSV != nullptr && part > 0 ){
        part--;
        vueDetails(); // On rappelle la méthode de modification
    }
}

// Permet de revenir à l'affichage globale après une vue en détails
// Si on souhaite implémenter d'autres modes de vues, on doit avoir au moins un bouton appellant ce slot

void CSVViewer::vueGlobale(){

    // MISE A JOUR DES BOUTONS

        // Le bouton "Voir en détails" revient
        details->setText("Voir en détails");
        connect(details,SIGNAL(clicked()),this,SLOT(vueDetails()));

        // Les boutons <> s'en vont
        gauche->setVisible(false);
        droite->setVisible(false);

    // MISE A JOUR DES DONNEES

        // CANDLESTICK SERIES
        graphe_haut->removeSeries(series);
        series->clear();

        // On rappelle les remplissages par défaut
        if(mode_affichage == "pasapas")
            remplissageBougiesPasAPas();
        else
            remplissageBougiesManuel();

        // GRAPHE CHANDELIER
        QString titre = eCSV->getobjetsducours().displayTitre();
        if(mode_affichage == "pasapas"){
            QString dateTransactionActuelle = param->getDate().toString("dd.MM.yyyy");
            titre += " - Transaction pour le " + dateTransactionActuelle;
        }
        graphe_haut->addSeries(series); // Ajout de la nouvelle série
        graphe_haut->setTitle(titre);

        // RECREATION DES AXES
        graphe_haut->createDefaultAxes();
        QBarCategoryAxis *axisXBougie = new QBarCategoryAxis();
        axisXBougie->append(categorieBougie);
        graphe_haut->setAxisX(axisXBougie,series);

        // BAR SERIES
        graphe_bas->removeSeries(barseries);
        barseries->clear();

        remplissageVolume();

        // GRAPHE VOLUME
        graphe_bas->addSeries(barseries);
        graphe_bas->createDefaultAxes();

}

// Pour gérer l'affichage des données des bougies si cliquées
// On peut facilement rajouter des éléments si besoin

void CSVViewer::viewCoursOHLC(Bougie* b){
    QString str;
    // setNum prend en paramètre une valeur et la transforme en QString
    // OPEN
    str.setNum(b->getCoursOHLC().getOpen());   // On récupère la valeur Open de notre cours
    open->setText(str);                        // On met la valeur récupérée dans le champ texte "open"
    // CLOSE
    str.setNum(b->getCoursOHLC().getClose());
    close->setText(str);
    // LOW
    str.setNum(b->getCoursOHLC().getLow());
    low->setText(str);
    // HIGH
    str.setNum(b->getCoursOHLC().getHigh());
    high->setText(str);
    // DATE
    QDate tmp = b->getCoursOHLC().getDate();
    str = tmp.toString("dd.MM.yyyy");
    date->setText(str);
    // FORME
    forme->setText(b->getForme());
    // INDICATEURS
        // Vu que la fonction get(RSI)) des indicateurs attend la position du cours en unsigned int, on lui renvoit l'indice du CoursOHLC de la bougie.
        // RSI
        rsi->setText(rsi_obj->getRSI(static_cast<unsigned int>(b->getCoursOHLC().getIndice())));
        // EMA
        ema->setText(ema_obj->getEMA(static_cast<unsigned int>(b->getCoursOHLC().getIndice())));
        // MACD
        macd->setText(macd_obj->getMACD(static_cast<unsigned int>(b->getCoursOHLC().getIndice())));
        /* Rajouter autres indicateurs ici si définit */
}

void CSVViewer::afficherTableau(){
    QTableWidget* historique = Tableau::createTable();
    // On copie grâce à cette boucle
    // On a pas voulu créer de constructeur par recopie juste pour une boucle
    for(int i=0; i < Tableau->rowCount(); i++)
        Tableau::addElement(static_cast<unsigned int>(i),historique,transac);
    historique->setFixedSize(800,400);
    historique->show();
    EditTexte *edit = new EditTexte(historique,chemin);
    edit->show();
}

// TRANSACTIONS

void CSVViewer::effectuerTransaction(Bougie* b){

    // On va tout crée dynamiquement ici car on veut une fenêtre indépendante

    this->setEnabled(false); // On empêche l'utilisation de la fenêtre affichage pendant que la petite fenêtre de transaction est ouverte

    CoursOHLC *coursActuel = new CoursOHLC(b->getCoursOHLC());

    // FENETRE

    QWidget *fen = new QWidget();
    fen->setWindowTitle("Transaction : " + coursActuel->getDate().toString("dd/MM/yyyy"));
    fen->setWindowFlags(Qt::WindowStaysOnTopHint);
    // La fenêtre pour effectuer la transaction doit toujours rester au dessus de la fenêtre d'affichage pour plus de confort

    // QDOUBLESPINBOX MONTANT

    QLabel *message = new QLabel("Veuillez insérer le montant de la Transaction:");
    QDoubleSpinBox *montant = new QDoubleSpinBox;
    montant->setRange(0.00,1000000.00);
    montant->setValue(35.00);

    // BOUTONS
    boutonTransaction *achat = new boutonTransaction("Achat",coursActuel,montant);
    boutonTransaction *vente = new boutonTransaction("Vente",coursActuel,montant);
    // On passe un pointeur vers le QDoubleSpinBox, car impossible de passer un pointeur vers la valeur (double) du QDoubleSpinBox seulement
    QPushButton *rienFaire = new QPushButton("Ne rien faire");

    achat->setFixedWidth(150);
    vente->setFixedWidth(150);
    rienFaire->setFixedWidth(150);

    connect(achat,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(achat(CoursOHLC*,QDoubleSpinBox*)));
    connect(achat,SIGNAL(clicked()),fen,SLOT(deleteLater())); // On supprime la fenetre quand on clique pour revenir à affichage
    connect(vente,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(vente(CoursOHLC*,QDoubleSpinBox*)));
    connect(vente,SIGNAL(clicked()),fen,SLOT(deleteLater()));

    if(mode_affichage == "pasapas")
        // MODE PAS A PAS : le bouton "Ne rien faire" permet de passer à la bougie suivante
        connect(rienFaire,SIGNAL(clicked()),this,SLOT(suivant()));
    else
        // MODE MANUEL : le bouton "Ne rien faire" permet de revenir à l'affichage
        connect(rienFaire,SIGNAL(clicked()),fen,SLOT(deleteLater()));

    // Si on ferme la fenetre de transaction, affichage redevient actif
    connect(fen,SIGNAL(destroyed()),this,SLOT(actif()));

    // Layout pour les boutons

       QHBoxLayout *lay = new QHBoxLayout;
       lay->addWidget(achat);
       lay->addWidget(vente);
       lay->addWidget(rienFaire);
       QWidget *lay_widget = new QWidget;
       lay_widget->setLayout(lay);

    // Layout principal

       QVBoxLayout *layFinal = new QVBoxLayout;
       layFinal->addWidget(message);
       layFinal->addWidget(montant);
       layFinal->addWidget(lay_widget);

    // Fenetre finale

    fen->setLayout(layFinal);
    fen->show();

}

void CSVViewer::vente(CoursOHLC *c, QDoubleSpinBox *m)
{

    double montant = m->value();

    if (nbTransactions==0){

        if (param->getContrepartie()>100){
            transac.push_back(Transaction(param->getBase(),param->getContrepartie(),c->getDate(),*c,"vente",param->getPourcentage(),montant));
            transac[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),*c));
            Tableau::addElement(nbTransactions,Tableau,transac);
            nbTransactions++;
        }
        else
            QMessageBox::warning(this,"Impossible","Impossible d'effectuer la transaction. \nVous n'avez pas assez d'argent.");
    }
    else {

        if (transac[nbTransactions-1].getMontantContre()>100){

            transac.push_back(Transaction(transac[nbTransactions-1].getMontantBase(),transac[nbTransactions-1].getMontantContre(),c->getDate(),*c,"vente",param->getPourcentage(),montant));
            transac[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),*c));
            Tableau::addElement(nbTransactions,Tableau,transac);
            nbTransactions++;
        }
        else
            QMessageBox::warning(this,"Impossible","Impossible d'effectuer la transaction. \nVous n'avez pas assez d'argent.");
    }
}


// Fonction qui réalise un achat

void CSVViewer::achat(CoursOHLC *c, QDoubleSpinBox *m){

    double montant = m->value();

    if (nbTransactions==0){

        if (param->getBase()>25){
            transac.push_back(Transaction(param->getBase(),param->getContrepartie(),c->getDate(),*c,"achat",param->getPourcentage(),montant));
            transac[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),*c));
            Tableau::addElement(nbTransactions,Tableau,transac);
            nbTransactions++;
        }
        else
            QMessageBox::warning(this,"Impossible","Impossible d'effectuer la transaction. \nVous n'avez pas assez d'argent.");
    }
    else {

        if (transac[nbTransactions-1].getMontantBase()>25){

            transac.push_back(Transaction(transac[nbTransactions-1].getMontantBase(),transac[nbTransactions-1].getMontantContre(),c->getDate(),*c,"achat",param->getPourcentage(),montant));
            transac[nbTransactions].setRoi(param->getContrepartie()+Transaction::ConversionBaseCotation(param->getBase(),param->getPourcentage(),*c));
            Tableau::addElement(nbTransactions,Tableau,transac);
            nbTransactions++;
        }
        else
            QMessageBox::warning(this,"Impossible","Impossible d'effectuer la transaction. \nVous n'avez pas assez d'argent.");
    }
}

void CSVViewer::annuler_transaction(){
    if(nbTransactions == 0)
        QMessageBox::warning(this,"Impossible","Il n'y a aucune transaction dans votre historique");
    else{
        if(mode_affichage == "pasapas"){
            param->setDatePrecedente(); // On revient une bougie en arrière
            if(details->text() == "Affichage global"){
                vueDetails();
            }
            else
                vueGlobale();
        }
        transac.pop_back();
        nbTransactions -= 1;
        Tableau::removeElement(nbTransactions,Tableau);
    }
}

// Slot simple pour rendre la fenêtre d'affichage inactive (notamment pendant une transaction)

void CSVViewer::inactif(){
    this->setEnabled(false);
}

// Slot simple pour remettre l'affichage actif

void CSVViewer::actif(){
    this->setEnabled(true);
}
