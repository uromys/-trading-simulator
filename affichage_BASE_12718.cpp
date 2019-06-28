#include "affichage.h"

/* A FAIRE:

 * THEO
    - Afficher l'historique de transactions quand on appuis sur "Terminer" en même temps que l'éditeur de texte

 * MODE PAS A PAS
   - Completer la methode remplissageBougiesPasAPas
        - Afficher les bougies normalement jusqu'à dateDebutChoixParam.
        - Afficher des bougies plates
        if (i < dateDebut())
            Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
        else
            Bougie *bougie = new Bougie(cours.getOpen(),cours.getOpen(),cours.getOpen(),cours.getOpen(),&cours);
   --> A chaque transaction effectué, faire dateDebut() -> dateDebut() + 1 (la bougie où la transaction vient d'être faite n'est plus écrasée)
   --> A chaque transaction annulée, faire dateDebut() -> dateDebut() - 1 (la bougie où la transaction vient d'être faite est de nouveau écrasée)
   - Faire les transactions une à une sur les bougies plates (à partir de la 1ère bougies plates)
   - On s'arrête une fois toutes les bougies traitées seulement
   - Faire une méthode lancer_simulation() dans Mode Pas à Pas comme dans Mode Manuel pour faire le lien avec l'interface (c'est la même chose que Mode Manuel normalement, tu as juste à recopier)
   - Faire la même chose pour remplissageVolumePasAPas()
*/

using namespace std;

// BOUGIE
map<int, Bougie*> Bougie::objects={};
 int Bougie::curID=0;



QString Bougie::calculForme() const {

    // Un switch (case) aurait été optimal pour le calcul de la forme, or son fonctionnement n'étant que par rapport à une seule variable nous empêche d'exprimer simplement nos conditions pour chaque CASE, ce qui aurait résulté en des switch imbriqués pas du tout optimaux.
    // NOTE: On peut imbriquer les if pour rendre le code optimal, mais il est cependant plus lisible de faire un if pour chaque cas de bougie sans imbrications, ce qui rend le code plus lisible et facilite l'ajout de forme ultérieur.
    // WARNING : Pour éviter les warning, on pourrait remplacer les == par des >= && <=, mais c'est plus long et allonge le code pour rien car ces warning ne sont pas dangereux

    // Definition des caractéristiques réutilisées dans les tests suivants:

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
    // abs(cours->getHigh() - cours->getOpen()) représentant la longueur du trait noir au dessus de la bougie
    if(abs(cours->getClose() - cours->getLow()) < abs(cours->getHigh() - cours->getOpen()))
        meche = haute;
    // abs(cours->getClose() - cours->getLow()) représentant la longueur du trait noir en dessous de la bougie
    if(abs(cours->getClose() - cours->getLow()) > abs(cours->getHigh() - cours->getOpen()))
        meche = basse;

    boug = bougie_defaut; // Initialisation par défaut (par sécurité)
    if(abs(cours->getOpen() - cours->getClose()) < abs(cours->getHigh() - cours->getLow()))
        boug = toupie;


    // Deduction de la forme

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

CSVViewer::CSVViewer(EvolutionCours *e, ChoixParam* p, QWidget *parent, QString mode): QWidget(parent), mode_affichage(mode){

    // COPIE DES PARAMETRES

    param = p->clone();

    for(unsigned int i = 0 ; i < e->getNbCours() ; i++){
        COURS.push_back(e->getCours(i));
    }

    // SAUVEGARDE DANS eCSV

    // On fait tourner l'itérateur jusqu'à cette date pour récupérer la position du 1er cours à ajouter

    auto it = e->begin(); // On crée l'itérateur en déhors du for car on va en avoir besoin après
    while(it != e->end() && (*it).getDate() != param->getDate())
        it++;
    // Tant qu'on atteint pas la fin de EvolutionCours ou la date saisie, on incrémente it

    qDebug() << "Echantillon à partir de la " + QString::number(distance(e->begin(),it)) + "ème bougie";

    if(it == e->end()){ // Si on dépasse l'échantillon => date invalide
        QMessageBox::critical(this,"Erreur","La date paramètrée est invalide et dépasse l'échantillon fourni. \nVeuillez revoir votre choix de paramètres. \nLe programme va maintenant s'arrêter.");
        QApplication::quit(); // on quitte l'application si la variable etat rencontre une valeur inconnue
    }
    else{
        int pos = distance(e->begin(),it); // On récupère la position de l'itérateur en int
        if(mode_affichage !="pasapas"){
            // MODE MANUEL : On ajoute seulement les cours à partir de la date donnée dans ChoixParam
            eCSV = new EvolutionCours(*e,static_cast<unsigned int>(pos));
        }
        else{
            // MODE PAS A PAS : On copie l'EvolutionCours initial pour avoir toutes les données.
            eCSV=new EvolutionCours(*e); // On a besoin de tous
        }
    }

    // GRAPHE EN CHANDELIER (au dessus)

        // CANDLESTICK

        series = new QCandlestickSeries();
        series->setName(eCSV->getobjetsducours().displayName()); // Nom de la série de bougies
        series->setIncreasingColor(QColor(Qt::green));
        series->setDecreasingColor(QColor(Qt::red));

        // CREATION DES CATEGORIES & BOUGIES
        Date=param->getDate();
        if(mode_affichage == "pasapas"){
            remplissageBougiesPasAPas();


        }
        else
            remplissageBougiesManuel();

        // Les deux modes à pouvoir afficher les données sont ceux ci
        // Or leur affichage ne diverge que légèrement
        // On décide de garder un seul constructeur et de diverger le remplissage des graphes ainsi

        // CREATION DU GRAPHE

        QString titre = eCSV->getobjetsducours().displayTitre();
        // Permet de récupérer le code des Devises du fichier .csv contenu dans EvolutionCours pour les afficher en tant que titre du graphe

        graphe_haut = new QChart(); // Création du graphe
        graphe_haut->addSeries(series); // On ajoute la série des barres créées
        // Si on arrive à récuperer le titre du fichier CSV, le mettre ici
        graphe_haut->setTitle(titre); // Titre du graphe
        graphe_haut->setAnimationOptions(QChart::SeriesAnimations); // Animation fluide pour l'affichage

        // CREATION DES AXES DU GRAPHE

        graphe_haut->createDefaultAxes();

        QBarCategoryAxis *axisXBougie = new QBarCategoryAxis();
        axisXBougie->append(categorieBougie);
        graphe_haut->setAxisX(axisXBougie,series);

        // Permet de montrer (true) ou non (false) la légende sous le graphe
        graphe_haut->legend()->setVisible(false); // On n'affiche pas la légende, tout est dans le titre du graphe ici
        graphe_haut->legend()->setAlignment(Qt::AlignBottom); // On place la légende sous le graphe (bottom)


        // VUE DU CHART (GRAPHE)

        QChartView *chartView_haut = new QChartView(graphe_haut,this);
        chartView_haut->setRenderHint(QPainter::Antialiasing);
        chartView_haut ->setFixedHeight(static_cast<int>(hauteur*0.4));
        chartView_haut ->setFixedWidth(static_cast<int>(largeur*0.72));

    // GRAPHE EN BARRE VOLUME (en dessous)

        //series->setName("Nom du fichier"); A FAIRE SI TEMPS

        // CREATION DU SET
        // Un graphe en bar est un ensemble de set où chaque set représente une barre
        // On créer donc une QBarSerie à laquelle on append() les différents set qui composeront notre graphe
        // ATTENTION: Il faut bien créer un set pour chaque volume comme une bougie pour chaque CoursOHLC, car la series prend les références vers ces derniers !

        barseries =new QBarSeries(); // Création de la série de set, où chaque sert contiendra un volume

        remplissageVolume(); // le remplissage est le même pour mode manuel ou pas à pas

        // CREATION DU GRAPHE

        graphe_bas = new QChart(); // Création du graphe
        graphe_bas->addSeries(barseries); // On ajoute la série des barres créées
        graphe_bas->setTitle("Volume des coursOHLC"); // Titre du graphe
        graphe_bas->setAnimationOptions(QChart::SeriesAnimations); // Animation fluide pour l'affichage

        // CREATION DES AXES DU GRAPHE

        graphe_bas->createDefaultAxes();
        QBarCategoryAxis *axisXVolume = new QBarCategoryAxis();
        axisXVolume->append(categorieVolume);
        graphe_bas->setAxisX(axisXVolume,barseries);

        // Permet de montrer (true) ou non (false) la légende sous le graphe
        graphe_bas->legend()->setVisible(false); // On n'affiche pas la légende, tout est dans le titre du graphe ici
        graphe_bas->legend()->setAlignment(Qt::AlignBottom); // On place la légende sous le graphe (bottom)

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
        gauche->setIcon(QIcon(dir.relativeFilePath("../icones/backward-inactive.png")));
        gauche->setFixedWidth(32);
        gauche->setFixedHeight(32); // La même Width et Height nous permet d'avoir un bouton carré
        connect(gauche,SIGNAL(clicked()),this,SLOT(precedent()));

        droite = new QPushButton();
        droite->setVisible(false); // Permet de rendre invisible le bouton dans un premier temps
        droite->setIcon(QIcon(dir.relativeFilePath("../icones/forward-inactive.png")));
        droite->setFixedWidth(32);
        droite->setFixedHeight(32);
        connect(droite,SIGNAL(clicked()),this,SLOT(suivant()));

        annuler = new QPushButton("Annuler transaction");
        annuler->setFixedWidth(taille_boutons);
        connect(annuler,SIGNAL(clicked()),this,SLOT(annuler_transaction()));


        terminer = new QPushButton("Terminer");
        terminer->setFixedWidth(taille_boutons);
        connect(terminer,SIGNAL(clicked()),this,SLOT(afficherTableau())); // Permet de réaficher le 1er menu lorsqu'on appuis sur Quitter
        connect(terminer,SIGNAL(clicked()),parent,SLOT(deleteLater())); // On supprime la fenetre de CSVViewer
        connect(terminer,SIGNAL(clicked()),this,SLOT(deleteLater())); // On supprime l'objet CSVViewer

    // INDICATEURS

        // CREATION DES OBJETS

        indic = new IndicTechnique(e); // Il faut bien passer le EvolutionCours original pour que les indicateurs soient dans le bon ordre pour chaque bougie
        rsi_obj = new RSI(indic);
        ema_obj = new EMA(indic);
        macd_obj = new MACD(indic);

        // ELEMENT DU QGROUPBOX

        groupeIndic = new QGroupBox("Indicateurs");
        rsi = new QLineEdit();
        rsil = new QLabel("RSI");
        ema = new QLineEdit();
        emal = new QLabel("EMA");
        macd = new QLineEdit();
        macdl = new QLabel("MACD");

        if(mode_affichage=="pasapas"){


         //on trouve la date correspondant a notre première bougie 
            for(EvolutionCours::iterator it = eCSV->begin() ; it!=eCSV->end() ; ++it){
            unsigned int i=0;
                if(it->getDate()==Date){
                     //coursActuel= eCSV->getCoursOHLCref();
                     //CoursOHLC *coursActuel = new CoursOHLC(b->getCoursOHLC());
                 CoursOHLC *coursActuel =   new CoursOHLC(eCSV->getCours(i));
                 i++;
                 //QLabel *message = new QLabel("Veuillez insérer le montant de la Transaction:");
                 QDoubleSpinBox *montant = new QDoubleSpinBox;
                 montant->setRange(0.00,1000000.00);
                 montant->setValue(35.00);

                 // BOUTONS
                 boutonTransaction *achat = new boutonTransaction("Achat",coursActuel,montant);
                 boutonTransaction *vente = new boutonTransaction("Vente",coursActuel,montant);
                 // On passe un pointeur vers le QDoubleSpinBox, car impossible de passer un pointeur vers la valeur (double) du QDoubleSpinBox seulement
                 QPushButton *suivant = new QPushButton("Ne rien faire");
                 // Appuyer sur "Ne rien faire" va enclencher la bougie suivante

                 achat->setFixedWidth(150);
                 vente->setFixedWidth(150);
                 suivant->setFixedWidth(150);

                 connect(achat,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(achat(CoursOHLC*,QDoubleSpinBox*)));

                 connect(vente,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(vente(CoursOHLC*,QDoubleSpinBox*)));

                 connect(vente,SIGNAL(clicked()),this,SLOT(action()));
                 connect(achat,SIGNAL(clicked()),this,SLOT(action()));
                  connect(suivant,SIGNAL(clicked()),this,SLOT(action()));






                 QLabel *messagePasaPas = new QLabel("Transaction");





                 boxmessage=new QHBoxLayout();
                 boxmessage->addWidget(messagePasaPas);
                 boxmessage->addWidget(montant);
                 boxachat = new QHBoxLayout();
                 boxachat->addWidget(achat);
                 //boxachat->addWidget(vente);
                 boxvente = new QHBoxLayout();
                 boxvente->addWidget(vente);
                // boxvente->addWidget(ventel);
                 boxNeRienFaire = new QHBoxLayout();
                 boxNeRienFaire->addWidget(suivant);
                 //boxNeRienFaire->addWidget(nerienfairel);

                 layoutPasaPas= new QVBoxLayout();
                 layoutPasaPas->addLayout(boxmessage);
                 layoutPasaPas->addLayout(boxachat);
                 layoutPasaPas->addLayout(boxvente);
                 layoutPasaPas->addLayout(boxNeRienFaire);

                 //layoutPasaPas->addLayout(groupePasaPas);

 qDebug()<<param->getDate().toString();
 qDebug()<<coursActuel->getDate().toString();




                }

            }

            
            
            
            
            //CoursOHLC *coursActuel = new CoursOHLC(b->getCoursOHLC());
            



        }



        // TAILLE DES LINEEDIT
        rsi->setFixedWidth(taille_LineEdit);
        ema->setFixedWidth(taille_LineEdit);
        macd->setFixedWidth(taille_LineEdit);

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

        layoutIndic = new QVBoxLayout();
        partieDroiteIndicateur = new QVBoxLayout();
        layoutIndic->addLayout(crsi);
        layoutIndic->addLayout(cema);
        layoutIndic->addLayout(cmacd);

        groupeIndic->setLayout(layoutIndic);

        partieDroiteIndicateur->addWidget(groupeIndic);

    // TABLEAU DE TRANSACTIONS

        Tableau = Tableau::createTable();  // crée un Tableau de transaction
        // Le 2ème Tableau est le namespace défini à la fin de strategietrading.h
        Tableau->setFixedSize(static_cast<int>(largeur*0.99),static_cast<int>(hauteur*0.17));

    // LAYOUT

        // PARTIE DROITE (Boutons/LineEdit/Label/Indicateurs)

        copen = new QHBoxLayout;
        chigh = new QHBoxLayout;
        clow = new QHBoxLayout;
        cclose = new QHBoxLayout;
        cdate = new QHBoxLayout;
        cforme = new QHBoxLayout;
        fleches = new QHBoxLayout;
        partieDroite = new QVBoxLayout; // On créé un Grid ici pour pouvoir gérer l'espacement juste après

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
        groupeInfos = new QGroupBox("Infos Bougie Cliquée");
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

    if(mode_affichage=="pasapas"){
        fenetre->addLayout(layoutPasaPas);

    }


    fenetre->addLayout(partieGauche); // D'abbord la partie gauche
    fenetre->addLayout(partieDroite); // Puis la partie droite
    fenetre->addLayout(partieDroiteIndicateur); // Et enfin les indicateurs tout à droite !


    final= new QVBoxLayout;
    final->addLayout(fenetre);
    final->addWidget(Tableau);

    this->setLayout(final); // On met le layout "fenêtre" comme celui principal de l'objet CSVViewer

    this->setFixedSize(largeur,hauteur); // on fixe la taille de l'objet CSVViewer
    parent->setFixedSize(largeur,hauteur); // on fixe la taille de la fenêtre contenant l'objet

}

void CSVViewer::remplissageBougiesManuel(){
    categorieBougie.clear(); // On vide la catégorie avant de rajouter des bougies
    for(EvolutionCours::iterator it = eCSV->begin() ; it!=eCSV->end() ; ++it) { //
        CoursOHLC& cours = *it; // Iterateur parcourant les coursOHLC de EvolutionCours
        // On créer une bougie correspondante pour chaque coursOHLC
        Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
        connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
        connect(bougie,SIGNAL(doubleClickedBougie(Bougie*)),this,SLOT(effectuerTransaction(Bougie*))); // permet d'ouvrir la fenêtre de transaction quand on double clique sur une bougie
        categorieBougie << QString::number(it->getIndice() + 1); // A chaque bougie correspondra un n° pour les repérer : la postion du CoursOHLC dans EvolutionCours (= n° de ligne du fichier .csv) +1, pour commencer à 1 et non à 0
        series->append(bougie); // on ajoute la bougie créée à la série
    }
}

// On peut append dans un même set pour avoir une même couleur et mettre dans une même catégorie

void CSVViewer::remplissageVolume(){
    categorieVolume.clear();
    QBarSet *set = new QBarSet("Volumes");
    for(auto it=eCSV->begin(); it != eCSV->end(); ++it) // On parcourt le tableau jusqu'à ne plus avoir d'objet de type "CoursOHLC" à lire.
    {
        // Création d'une barre de volume avec le volume pointé par l'itérateur
        *set << it->getVolume(); // On remplie le set
        categorieVolume << QString::number(it->getIndice() + 1);
        // Ajout de la barre de volume à la série de barres à afficher sur le graphe
        barseries->append(set); // On ajoute le set à notre serie
    }
}

void CSVViewer::remplissageBougiesPasAPas(){

    //Date=Date.addDays(1);
    qDebug()<< "la";
//series->clear(); //
    categorieBougie.clear(); // On vide la catégorie avant de rajouter des bougies

 //Bougie::toutDetruire();



    bool DateAtteinte=false;
     vector<CoursOHLC>::iterator it;

     for( it = COURS.begin() ; it!=COURS.end() ; ++it){
        if(it->getDate()==Date)
            DateAtteinte=true;



        if(DateAtteinte==false){
            CoursOHLC& cours = *it;
 qDebug()<< "la ??"<<endl;
            // Iterateur parcourant les coursOHLC de EvolutionCours

            Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
            connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
            categorieBougie << QString::number(it->getIndice() + 1);

            // A chaque bougie correspondra un n° pour les repérer : la postion du CoursOHLC dans EvolutionCours (= n° de ligne du fichier .csv) +1, pour commencer à 1 et non à 0
            series->append(bougie);
        }
        else{//on a dépassé la date, les bougies doivent être plates (pas encore de transaction)
            CoursOHLC& cours = *it;
            Bougie *bougie = new Bougie(cours.getOpen(),cours.getOpen(),cours.getOpen(),cours.getOpen(),&cours);// afin que la bougie soit plate
            //connect(bougie,SIGNAL(doubleClicked(Bougie*)),this,SLOT(effectuerTransaction(Bougie*)));
            categorieBougie << QString::number(it->getIndice() + 1); // A chaque bougie correspondra un n° pour les repérer : la postion du CoursOHLC dans EvolutionCours (= n° de ligne du fichier .csv) +1, pour commencer à 1 et non à 0
            series->append(bougie);

            qDebug()<< "ici ? ??"<<endl;
       } // fin else
        /*
        if(it->indice>eCSV->getNbMaxCours())
            break;// fin for
            */
    }


qDebug()<< "pas là ? ??"<<endl;
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

        sous_eCSV = new EvolutionCours(*eCSV, nbBougiesDetails, part); // Initialisation avec les données de la vue en détails par surcharge du constructeur par recopie.

        if(sous_eCSV->getNbMaxCours() == 0){
            QMessageBox::warning(this,"Impossible","Impossible d'aller plus loin. \nVous avez atteint la limite de l'échantillon.");
            // Si on a ajouté aucun cours dans eCSV avec le constructeur précédent (donc NbMaxCours == 0), on bloque le bouton suivant
            part--; // car on l'avait incrémenté avant d'aller dans cette fonction, or on ne peut pas continuer donc il doit garder sa valeur avant l'appel du slot "suivant()".
        }
        else{
            // Changement de fonction du bouton "Voir en détails" précédent
            details->setText("Affichage global");
            connect(details,SIGNAL(clicked()),this,SLOT(vueGlobale()));
            // On affiche les boutons <>, maintenant actifs !
            gauche->setVisible(true);
            droite->setVisible(true);
            // On change les icones des 2 boutons précédents
            QDir dir("../");
            droite->setIcon(QIcon(dir.relativeFilePath("../icones/forward.png")));
            gauche->setIcon(QIcon(dir.relativeFilePath("../icones/backward.png")));

            // On va donc mettre à jour les données des graphes avec ce sous_eCSV

            // MISE A JOUR DES DONNEES

                // CANDLESTICK SERIES
                graphe_haut->removeSeries(series); // On retire la série précédente
                series->clear(); // On supprime la série de bougies précédente pour lui faire prendre de nouvelle bougies
                categorieBougie.clear(); // On vide la catégorie avant de rajouter des bougies
                if(mode_affichage!="pasapas"){
                    // MODE MANUEL :
                    for(EvolutionCours::iterator it = sous_eCSV->begin() ; it!=sous_eCSV->end() ; ++it) { //
                        CoursOHLC& cours = *it; // Iterateur parcourant les coursOHLC de EvolutionCours
                        // On créer une bougie correspondante pour chaque coursOHLC
                        Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
                        connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
                        connect(bougie,SIGNAL(doubleClickedBougie(Bougie*)),this,SLOT(effectuerTransaction(Bougie*)));
                        categorieBougie << QString::number(it->getIndice() + 1); // On fait +1 pour commencer les labels à 1
                        series->append(bougie); // on ajoute la bougie créée à la série
                    }
                }
                else {
                    // MODE PAS A PAS :
                    bool DateAtteinte=false;
                    for(EvolutionCours::iterator it = sous_eCSV->begin() ; it!=sous_eCSV->end() ; ++it){
                        if(it->getDate()==Date)
                            DateAtteinte=true;

                        if(DateAtteinte==false){
                            CoursOHLC& cours = *it;

                            // Iterateur parcourant les coursOHLC de EvolutionCours

                            Bougie *bougie = new Bougie(cours.getOpen(),cours.getHigh(),cours.getLow(),cours.getClose(),&cours);
                            connect(bougie,SIGNAL(clickBougie(Bougie*)),this,SLOT(viewCoursOHLC(Bougie*))); // Pour gérer l'affichage des bougies si cliquées
                            categorieBougie << QString::number(it->getIndice() + 1);

                            // A chaque bougie correspondra un n° pour les repérer : la postion du CoursOHLC dans EvolutionCours (= n° de ligne du fichier .csv) +1, pour commencer à 1 et non à 0
                            series->append(bougie);
                        }
                        else{//on a dépassé la date, les bougies doivent être plates (pas encore de transaction)
                            CoursOHLC& cours = *it;
                            Bougie *bougie = new Bougie(cours.getOpen(),cours.getOpen(),cours.getOpen(),cours.getOpen(),&cours);// afin que la bougie soit plate
                            //connect(bougie,SIGNAL(doubleClicked(Bougie*)),this,SLOT(effectuerTransaction(Bougie*)));
                            categorieBougie << QString::number(it->getIndice() + 1); // A chaque bougie correspondra un n° pour les repérer : la postion du CoursOHLC dans EvolutionCours (= n° de ligne du fichier .csv) +1, pour commencer à 1 et non à 0
                            series->append(bougie);
                       } // fin else
                    } // fin for
                } // fin mode pas a pas

                // GRAPHE CHANDELIER
                QString partie = QString::number(part+1); // On va afficher le n° de la partie actuellement affichée dans le titre du graphe
                QString titre = sous_eCSV->getobjetsducours().displayTitre();
                graphe_haut->addSeries(series); // Ajout de la nouvelle série
                graphe_haut->setTitle(titre);

                // RECREATION DES AXES
                graphe_haut->createDefaultAxes();
                QBarCategoryAxis *axisXBougie = new QBarCategoryAxis();
                axisXBougie->append(categorieBougie);
                graphe_haut->setAxisX(axisXBougie,series);


                // BAR SERIES
                graphe_bas->removeSeries(barseries); // On retire la série précédente
                barseries->clear(); // On supprime la série de Barres de Volume précédente pour lui faire prendre de nouveaux volumes
                categorieVolume.clear(); // on vide la catégorie précédente pour afficher la nouvelle
                QBarSet *set = new QBarSet("Volume");
                for(auto it=sous_eCSV->begin(); it != sous_eCSV->end(); ++it) // On parcourt le tableau jusqu'à ne plus avoir d'objet de type "CoursOHLC" à lire.
                {
                    *set << it->getVolume(); // On ajoute un volume au set
                    categorieVolume << QString::number(it->getIndice()+1); // on segmente le graphe
                    barseries->append(set); // On ajoute le set à notre serie
                }

                // GRAPHE VOLUME
                graphe_bas->addSeries(barseries); // Ajout de la nouvelle série
                graphe_bas->createDefaultAxes();
                QBarCategoryAxis *axisXVolume = new QBarCategoryAxis();
                axisXVolume->append(categorieVolume);
                graphe_bas->setAxisX(axisXVolume,barseries);
        } // fin else eCSV vide
    } // fin else echantillon déjà affiché au global
}

// Permet de passer au sous ensemble sous_eCSV suivant en incrémentant "part" puis refaisant appel à la méthode vueDetails

void CSVViewer::suivant(){
    if(sous_eCSV != nullptr){ // Nous permet de nous assurer que le bouton "Voir en détails" a bien été cliqué (et donc eCSV rempli), dans le cas contraire les flêches sont inactives. On vérifie aussi qu'on ne peut pas aller au delà de la valeur max possible.
        part++;
        vueDetails(); // On rappelle la méthode de modification
    }
}

// Permet de passer au sous ensemble sous_eCSV suivant en incrémentant "part" puis refaisant appel à la méthode vueDetails

void CSVViewer::precedent(){
    if(sous_eCSV != nullptr && part > 0 ){ // Nous permet de nous assurer que le bouton "Voir en détails" a bien été cliqué (et donc eCSV rempli), dans le cas contraire les flêches sont inactives. On vérifie aussi qu'on ne peut pas aller au delà de la valeur max possible.
        part--;
        vueDetails(); // On rappelle la méthode de modification
    }
}

// Permet de revenir à l'affichage globale après une vue en détails (slot à réutiliser si on souhaite ajouter d'autres modes de vues, afin de revenir à la vue initiale)

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
        graphe_haut->removeSeries(series); // On retire la série précédente
        series->clear(); // On supprime la série de bougies précédente pour lui faire prendre de nouvelle bougies

        if(mode_affichage == "pasapas")
            remplissageBougiesPasAPas();
        else
            remplissageBougiesManuel();

        // GRAPHE CHANDELIER
        QString titre =sous_eCSV->getobjetsducours().displayTitre();
        graphe_haut->addSeries(series); // Ajout de la nouvelle série
        graphe_haut->setTitle(titre);

        // RECREATION DES AXES
        graphe_haut->createDefaultAxes();
        QBarCategoryAxis *axisXBougie = new QBarCategoryAxis();
        axisXBougie->append(categorieBougie);
        graphe_haut->setAxisX(axisXBougie,series);


        // BAR SERIES
        graphe_bas->removeSeries(barseries); // On retire la série précédente
        barseries->clear(); // On supprime la série de Barres de Volume précédente pour lui faire prendre de nouveaux volumes

        remplissageVolume();

        // GRAPHE VOLUME
        graphe_bas->addSeries(barseries); // Ajout de la nouvelle série
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
        // Vu que la fonction get(RSI)) des indicateurs attend la position du cours en unsigned int, on lui renvoit l'indice de la bougie (= position du cours dans eCSV) avec un static cast
        // RSI
        rsi->setText(rsi_obj->getRSI(static_cast<unsigned int>(b->getCoursOHLC().getIndice())));
        // EMA
        ema->setText(ema_obj->getEMA(static_cast<unsigned int>(b->getCoursOHLC().getIndice())));
        // MACD
        macd->setText(macd_obj->getMACD(static_cast<unsigned int>(b->getCoursOHLC().getIndice())));
}

void CSVViewer::afficherTableau(){
    Tableau->setFixedSize(800,400);
    Tableau->show();
    EditTexte * edit = new EditTexte();
    edit->show();
}

// TRANSACTIONS

// ATTENTION ESTELLE & ANAEL

void CSVViewer::effectuerTransaction(Bougie* b){

    this->setEnabled(false); // On empêche l'utilisation de la fenêtre affichage pendant que la petite fenêtre de transaction est ouverte

    // b est un pointeur vers la bougie cliquée pour récupérer ses informations

    CoursOHLC *coursActuel = new CoursOHLC(b->getCoursOHLC());

    // FENETRE

    QWidget *fen = new QWidget();
    fen->setWindowTitle("Transaction : " + coursActuel->getDate().toString("dd/MM/yyyy"));
    fen->setWindowFlags(Qt::WindowStaysOnTopHint); // La fenêtre pour effectuer la transaction doit toujours rester au dessus de la fenêtre d'affichage pour plus de confort

    // QDOUBLESPINBOX MONTANT

    QLabel *message = new QLabel("Veuillez insérer le montant de la Transaction:");
    QDoubleSpinBox *montant = new QDoubleSpinBox;
    montant->setRange(0.00,1000000.00);
    montant->setValue(35.00);

    // BOUTONS
    boutonTransaction *achat = new boutonTransaction("Achat",coursActuel,montant);
    boutonTransaction *vente = new boutonTransaction("Vente",coursActuel,montant);
    // On passe un pointeur vers le QDoubleSpinBox, car impossible de passer un pointeur vers la valeur (double) du QDoubleSpinBox seulement
    QPushButton *suivant = new QPushButton("Ne rien faire");
    // Appuyer sur "Ne rien faire" va enclencher la bougie suivante

    achat->setFixedWidth(150);
    vente->setFixedWidth(150);
    suivant->setFixedWidth(150);

    connect(achat,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(achat(CoursOHLC*,QDoubleSpinBox*)));
    connect(achat,SIGNAL(clicked()),fen,SLOT(deleteLater())); // On supprime la fenetre quand on clique pour revenir à affichage
    connect(vente,SIGNAL(clicAvecCours(CoursOHLC*,QDoubleSpinBox*)),this,SLOT(vente(CoursOHLC*,QDoubleSpinBox*)));
    connect(vente,SIGNAL(clicked()),fen,SLOT(deleteLater())); // On supprime la fenetre quand on clique pour revenir à affichage

    // **NOTE** : Est-ce que delete la fenetre (et donc les boutons) une fois qu'on clique dessus laisse le temps à achat et vente de se faire ? à tester !!
    // Cela peut poser problème... mais aussi fonctionner !

    if(mode_affichage == "pasapas")
        // MODE PAS A PAS : le bouton "Ne rien faire" permet de passer à la bougie suivante
        connect(suivant,SIGNAL(clicked()),this,SLOT(suivant()));
    else
        // MODE MANUEL : le bouton "Ne rien faire" permet de revenir à l'affichage
        connect(suivant,SIGNAL(clicked()),fen,SLOT(deleteLater()));

    // Si on ferme la fenetre de transaction, affichage redevient actif
    connect(fen,SIGNAL(destroyed()),this,SLOT(actif()));

    // Layout pour les boutons

       QHBoxLayout *lay = new QHBoxLayout;
       lay->addWidget(achat);
       lay->addWidget(vente);
       lay->addWidget(suivant);
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


