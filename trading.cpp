#include "trading.h"
#include "affichage.h"
#include "choixparam.h"
#include "edittexte.h"
#include "indictechnique.h"
#include "modeauto.h"
#include "modemanuel.h"
#include "modepasapas.h"
#include "strategietrading.h"
#include "import.h"
#include "sauvegarde_simulation_xml.h"

using namespace std;

// TD

Devise::Devise(const QString& c, const QString& m, const QString& z) :
    monnaie(m), zone(z) {
    /*
    if (c.size() != 3) throw TradingException("code devise incorrect");
    for (unsigned int i = 0; i < 3; i++) {
        if (c[i] <= 'A' || c[i] >= 'Z') throw TradingException("code devise incorrect");

    }*/
    code=c;
}
Action::Action( const QString& e,const QString& c, const QString& z ) :code(c),entreprise(e),zone(z){
    //if (c.size() != 3) throw TradingException("code devise incorrect");

}




PaireDevises::PaireDevises(Devise& b,  Devise& c,  const QString &s) :
    base(&b), contrepartie(&c), surnom(s) {}


void CoursOHLC::setCours(double o, double h, double l, double c, double vol, QDate d, int i) {
    if (o < 0 || h < 0 || l < 0 || c<0 || vol<0 || l>h) throw TradingException("cours OHLC incorrect");
        open = o;
        high = h;
        low = l;
        close = c;
        volume = vol;
        date = d;
        indice = i;
}

CoursOHLC::CoursOHLC(double o, double h, double l, double c, double vol, QDate d, int i){
    if (o < 0 || h < 0 || l < 0 || c<0 || vol<0 || l>h) throw TradingException("cours OHLC incorrect");
        open = o;
        high = h;
        low = l;
        close = c;
        volume = vol;
        date = d;
        indice = i;
}


void EvolutionCours::addCours(double o, double h, double l, double c, double vol, QDate d, int indice) {
    if (nbMaxCours == nbCours) { // agrandissement du tableau
        CoursOHLC* newtab = new CoursOHLC[nbMaxCours + 1];
        for (unsigned int i = 0; i < nbCours; i++) newtab[i] = cours[i];
        // mise � jour des attributs
        CoursOHLC* old = cours;
        cours = newtab;
        nbMaxCours += 1;
        delete[] old; // destruction de l'ancien tableau
    }
    // ajout du cours
    cours[nbCours].setCours(o,h,l,c,vol,d,indice);
    nbCours++;
}

EvolutionCours::~EvolutionCours() {
    delete[] cours;
}

EvolutionCours::EvolutionCours(EvolutionCours& e) :
    financialProduct(e.financialProduct),
    cours(new CoursOHLC[e.nbCours]),
    nbCours(e.nbCours),
    nbMaxCours(e.nbCours)
{
    for (unsigned int i = 0; i < nbCours; i++)
        cours[i] = e.cours[i];
}

// Constructeur par recopie surchargé pour la fonction Affichage (sous_eCSV manuel)

EvolutionCours::EvolutionCours(const EvolutionCours& e, const unsigned int nbCoursTotal, const unsigned int part) :
    financialProduct(e.financialProduct),
    cours(new CoursOHLC[1])
    // On crée un tableau de 1 cours, puis avec le addcours() le tableau ajoutera dynamiquement des éléments au tableau afin d'avoir la taille parfaite au final
{
    // PLACEMENT DE L'ITERATEUR SUR LE 1ER COURS DE E A COPIER
    auto it = e.begin();
    for(unsigned int i = 0; (i < part*nbCoursTotal) && (it != e.end()); i++)
        it++;
    if(it != e.end()){
        // AJOUT DES COURS
        int pos;
        for(unsigned int i = 0 ; (i < nbCoursTotal) && (it != e.end()) ; i++){
            // Il y aura nbCoursTotal itérations, ou moins si on atteint plus tôt la fin du EvolutionCours e
            pos = static_cast<int>(i+part*nbCoursTotal);
            //cours[i] = e.cours[pos]; // On prend les valeurs de la tranche considérée
            addCours(e.cours[pos].getOpen(), e.cours[pos].getHigh(),e.cours[pos].getLow(),e.cours[pos].getClose(), e.cours[pos].getVolume(), e.cours[pos].getDate(), e.cours[pos].getIndice());
            it++;
        }
    }
}

// Constructeur par recopie surchargé pour la fonction Affichage (sous_eCSV pas à pas)

EvolutionCours::EvolutionCours(const EvolutionCours& e, const unsigned int nbCoursTotal, const QDate date):
    financialProduct(e.financialProduct),
    cours(new CoursOHLC[1])
    // On crée un tableau de 1 cours, puis avec le addcours() le tableau ajoutera dynamiquement des éléments au tableau afin d'avoir la taille parfaite au final
{
    // PLACEMENT DE L'ITERATEUR SUR LE 1ER COURS DE E A COPIER
    // On veut que la bougie correspondant à la date passé en paramètre soit tout à droite pour ne pas donner d'information sur les cours futurs
    // On commence donc au nbCoursTotal-ème cours avant la date, et on affiche les nbCoursTotal prochaines bougies dans la mesure du possible
    // RAPPEL : Le eCSV passé en paramètre de mode pas à pas est l'EvolutionCours entier initial
    auto it = e.begin();
    auto it2 = e.begin();
    int ecart = static_cast<int>(nbCoursTotal-1); // Changer cet écart pour changer l'affichage si besoin
    // AUTRE ECARTS POSSIBLES:
    // = 1 -> avoir la bougie actuelle à la fin
    // = nbCoursTotal/2 -> avoir la bougie actuelle au milieu
    // = nbCoursTotal-1 -> avoir la bougie actuelle au début
    unsigned int i;
    for(i = 0; it2->getDate() != date; i++)
        it2++;
    if(i >= nbCoursTotal) // Tant qu'on ne peut pas afficher 29 bougies avant la bougie actuelle, on fait une vue par défaut normale pour ne pas faire planter le programme
        for(unsigned int j = 0; it->getDate() != date.addDays(-ecart); j++)
            it++;
    if(it != e.end()){
        // AJOUT DES COURS
        int pos;
        for(unsigned int k = 0 ; (k < nbCoursTotal) && (it != e.end()) ; k++){
            pos = distance(e.begin(),it);
            // Il y aura nbCoursTotal itérations, ou moins si on atteint plus tôt la fin du EvolutionCours e
            //cours[i] = e.cours[pos]; // On prend les valeurs de la tranche considérée
            addCours(e.cours[pos].getOpen(), e.cours[pos].getHigh(),e.cours[pos].getLow(),e.cours[pos].getClose(), e.cours[pos].getVolume(), e.cours[pos].getDate(), e.cours[pos].getIndice());
            it++;
        }
    }
}

// Constructeur par recopie surchargé pour la fonction ModeManuel (eManuel)

EvolutionCours::EvolutionCours(const EvolutionCours& e, const unsigned int pos):
    financialProduct(e.financialProduct),
    cours(new CoursOHLC[e.getNbCours() - pos]),
    // nbCours = nbMaxCours car on veut que cette sous_partie ait exactement un nombre précis de coursOHLC
    nbCours(e.getNbCours() - pos),
    nbMaxCours(e.getNbCours() - pos)
{
    unsigned int j=0; // Compteur pour cours[]
    for(unsigned int i = pos ; i < e.getNbCours() ; i++){ // Il y aura nbCoursTotal itérations
        cours[j] = e.cours[i];
        j++;
    }
}

const Devise& DevisesManager::creationDevise(const QString& c, const QString& m, const QString& z) {
    if (nbDevises == nbMaxDevises) { // agrandissement tableau
        Devise** newtab = new Devise*[nbMaxDevises + 30];
        for (unsigned int i = 0; i < nbDevises; i++) newtab[i] = devises[i];
        // mise a jour des attributs
        Devise** old = devises;
        nbMaxDevises += 30;
        devises = newtab;
        delete[] old;
    }
    // c�ration et ajout de la nouvelle devise...
    devises[nbDevises] = new Devise(c, m, z);
    return *devises[nbDevises++];
}

const Action &DevisesManager:: creationAction(const QString& c, const QString& m,
                             const QString& z){
    if (nbDevises == nbMaxDevises) { // agrandissement tableau
        Action** newtab = new Action*[nbMaxDevises + 30];
        for (unsigned int i = 0; i < nbDevises; i++) newtab[i] = actions[i];
        // mise a jour des attributs
        Action** old = actions;
        nbMaxDevises += 30;
        actions = newtab;
        delete[] old;
    }
    // c�ration et ajout de la nouvelle devise...
    actions[nbDevises] = new Action(c, m, z);
    return *actions[nbDevises++];
}



 Devise& DevisesManager::getDevise(const QString& c) {
    for (unsigned int i = 0; i < nbDevises; i++)
        if (devises[i]->getCode() == c) return *devises[i];
    throw TradingException("devise inexistante");
}

 Action &DevisesManager::getAction(const QString &c)const{
    for (unsigned int i = 0; i < nbDevises; i++)
        if (actions[i]->getCode() == c) return *actions[i];
    throw TradingException("action inexistante");
}


 PaireDevises& DevisesManager::getPaireDevises( QString & c1,  QString & c2)  {
     Devise& d1 = getDevise(c1);
     Devise& d2 = getDevise(c2);
    for (unsigned int i = 0; i < nbPaires; i++)
        if (paires[i]->getBase().getCode() == c1 &&
            paires[i]->getContrepartie().getCode() == c2)
            return *paires[i];
    // si la paire de devises n'est pas trouv�e, il faut la cr�er
    if (nbPaires == nbMaxPaires) { // agrandissement du tableau
        PaireDevises** newtab = new PaireDevises*[nbMaxPaires + 10];
        for (unsigned int i = 0; i < nbPaires; i++) newtab[i] = paires[i];
        PaireDevises** old = paires;
        nbMaxPaires += 10;
        paires = newtab;
        delete[] old;
    }
    paires[nbPaires] = new PaireDevises(d1, d2);
    return *paires[nbPaires++];
}

DevisesManager::~DevisesManager() {
    for (unsigned int i = 0; i < nbPaires; i++) delete paires[i];
    for (unsigned int i = 0; i < nbDevises; i++) delete devises[i];
    delete[] paires; delete[] devises;
}

DevisesManager::Handler DevisesManager::handler=
                                DevisesManager::Handler();



InterfaceFinale::InterfaceFinale(){

    // MESSAGE BIENVENUE
    bienvenue = new QLabel("Bienvenue sur le programme de simulation de trading. \nDans ce programme, vous serez guidé afin de pouvoir effectuer des simulation de transactions \nà partir de données CoursOHLC. Pour commencer la simulation, cliquez sur Suivant.");

    // BOUTONS
    precedent = new QPushButton("Précédent");
    suivant = new QPushButton("Suivant");
    quitter = new QPushButton("Quitter");
        // TAILLE
        precedent->setFixedWidth(taille_boutons);
        suivant->setFixedWidth(taille_boutons);
        quitter->setFixedWidth(taille_boutons);

        // ACTIONS
        connect(precedent,SIGNAL(clicked()),this,SLOT(precedent_itf())); // Revient à l'objet précédent
        connect(suivant,SIGNAL(clicked()),this,SLOT(suivant_itf())); // Passe à l'objet suivant
        connect(quitter,SIGNAL(clicked()),this,SLOT(quitter_itf())); // Pour quitter le programme
        precedent->setEnabled(false); // Le bouton précédent ne peut être actif sur la 1ère page

    // SOUS FENETRE & BIENVENUE

        fenetreCentre = new QWidget(); // On crée la sous_fenêtre du centre qui accueillera les futurs divers objets
        fenetreCentre->setFixedSize(largeur,hauteur-100);
        // SOUS LAYOUT (acceuillant bienvenue pour le moment, puis ceux du SWITCH avec etat)
        layout_objets = new QVBoxLayout(); // fenetreCentre a besoin d'un layout pour accueillir des objets
        layout_objets->addWidget(bienvenue); // Ajout du message de bienvenue dans le layout de base
        fenetreCentre->setLayout(layout_objets);


    // LAYOUT PRINCIPAL

        boutons = new QHBoxLayout();
        boutons->addWidget(precedent);
        boutons->addWidget(suivant);
        boutons->addWidget(quitter);

        // On a besoin de ce Layout afin de placer optimalement les boutons
        fenetreBas = new QWidget(); // On crée la sous_fenêtre qui accueillera le layout pour les boutons du bas de fenêtre
        fenetreBas->setFixedSize(largeur,50);
        fenetreBas->setLayout(boutons);

        layout = new QVBoxLayout();
        layout->addWidget(fenetreCentre);
        layout->addWidget(fenetreBas);

    // REGLAGE FENETRE PRINCIPALE (on y fait appel avec this)
        this->setWindowTitle("Simulation de Trading");
        this->setFixedSize(largeur,hauteur); // On fixe la taille de la fenêtre à une résolution standard
        this->setLayout(layout); // On met layout comme layout principal de la fenêtre
}

void InterfaceFinale::LayoutUpdate(){
    // DEROULEMENT :
    // On fait un switch selon la valeur d'état pour savoir quel Layout doit être affiché actuellement
    // On met à jour dynamiquement notre layout principal : layout_objets
    // Pour cela, on va addWidget() tous les objets à ce layout.
    // On fait ensuite des hide() de tous les autres widgets qui ne nous intéressent pas, pour ne show() que le widget intéressant !
    // Chaque étape se décompose en 2 parties:
    //  - HIDE DES AUTRES WIDGETS
    //  - CREATION / AFFICHAGE DU NOUVEAU WIDGET
    // Optionnel : On gère la modification ou non des boutons suivant / precedent dans certains cas spécifique (ex: on ne peut pas faire suivant si on a pas importer de fichier .csv)

    switch (etat) {

        // 0 - BIENVENUE
        case 0 :
            import->hide(); // Import était le seul widget actif quand on revient en 0
            bienvenue->show(); // On show() le widget qui nous intéresse
            suivant->setEnabled(true); // on remet le bouton suivant actif au cas où on reviendrait sur le 1er écran sans avoir importé de fichier.csv
            precedent->setEnabled(false); // Le bouton précédent ne peut être actif sur la 1ère page
        break;

        // 1 - IMPORT
        case 1 :

            // HIDE DES AUTRES WIDGETS
            // On a besoin de hide() seulement les widgets de l'étape etat=0 et etat=2
            bienvenue->hide();
            precedent->setEnabled(true); // On réactive le bouton précédent
            if (choix != nullptr) // Tant que choix n'est pas créé, on ne va pas le hide() !
                choix->hide();

            // CREATION / AFFICHAGE DU NOUVEAU WIDGET
            // Si le widget n'a pas encore été créé, on le crée grâce à la méthode, sinon on l'affiche juste avec show() dans le layout
            if (import == nullptr)
                creerImport();
            else
                import->show();
            if(e == nullptr)
                suivant->setEnabled(false); // On désactive le bouton suivant tant qu'on a pas importé un fichier .csv avec succès pour empêcher la progression !
        break;

        // 2 - CHOIXPARAM
        case 2 :

            // MAJ DU BOUTON DEMARRER -> SUIVANT (quand on vient de etat=3)
            suivant->setText("Suivant");
            suivant->disconnect(); // On enlève le connect précédent
            connect(suivant,SIGNAL(clicked()),this,SLOT(suivant_itf()));

            // HIDE DES AUTRES WIDGETS
            import->hide(); // import est forcément créé si on arrive ici
            if(mode != nullptr)
                mode->hide();

            // CREATION / AFFICHAGE DU NOUVEAU WIDGET
            if(choix == nullptr)
                creerChoixParam();
            else
                choix->show();
        break;

        // 3 - CHOIXMODE
        case 3 :

            // MAJ DU BOUTON SUIVANT -> DEMARRER
            suivant->setText("Démarrer");
            suivant->disconnect(); // On enlève le connect précédent
            connect(suivant,SIGNAL(clicked()),this,SLOT(lancerSimulation()));

            // HIDE DES AUTRES WIDGETS
            choix->hide();

            // CREATION / AFFICHAGE DU NOUVEAU WIDGET
            if(mode == nullptr)
                creerChoixMode();
            else
                mode->show();
        break;

        default :
            QMessageBox::critical(this,"Erreur","Une erreur est survenue. \nLe programme va maintenant s'arrêter.");
            QApplication::quit(); // on quitte l'application si la variable etat rencontre une valeur inconnue
    }
}

void InterfaceFinale::creerImport(){

    import = new QWidget();
    import->setFixedSize(largeur-margeX,hauteur-margeY);
    // On rajoute des soustractions pour un affichage correcte suivant les marges de la fenêtre

    // QLABEL (Message)
    message_import = new QLabel("Veuillez importer un fichier au format .csv contenant les cours d'une Action ou Paire de Devises.");

    // BOUTONS
    ouvrir = new QPushButton("Ouvrir");
    ouvrir->setFixedWidth(taille_boutons);
    connect(ouvrir,SIGNAL(clicked()),this,SLOT(import_itf())); // Appel au slot import

    message_XML = new QLabel("Vous pouvez aussi charger une simulation déjà effectuée afin d'afficher ses informations.");

    charger = new QPushButton("Charger une sauvegarde XML");
    connect(charger,SIGNAL(clicked()),this,SLOT(charger_itf())); // Passe à l'objet suivant

    // QLINEEDIT
    affiche_chemin = new QLineEdit();
    affiche_chemin->setFixedWidth(largeur-120);
    affiche_chemin->setReadOnly(true); // Permet de rendre les QLineEdit non modifiable
    // Il sera complété dynamiquement après l'import d'un fichier .csv

    // LAYOUT
    boutons_import = new QHBoxLayout();
    boutons_import->setSpacing(100);
    boutons_import->addWidget(affiche_chemin);
    boutons_import->addWidget(ouvrir);

    layout_import = new QVBoxLayout();
    layout_import->addWidget(message_import);
    layout_import->addLayout(boutons_import);
    layout_import->addWidget(message_XML);
    layout_import->addWidget(charger);

    import->setLayout(layout_import);

    // Enfin: On ajoute l'import créé au layout principal
    layout_objets->addWidget(import);

}

void InterfaceFinale::creerChoixParam(){

    choix = new ChoixParam(e, this); // choix est bien initialisé comme un ChoixParam !

    // QLABEL (Message)
    // Il est directement mis dans ChoixParam

    // Enfin: On ajoute l'import créé au layout principal
    layout_objets->addWidget(choix);

}

void InterfaceFinale::creerChoixMode(){



    mode = new QWidget();
    mode->setFixedSize(largeur-margeX,hauteur-margeY);

    // QLABEL (Message)
    message_choixmode = new QLabel("Veuillez choisir un mode de fonctionnement du programme parmi les suivants:");
    desc_auto = new QLabel("L'ordinateur va appliquer une stratégie choisie par l'utilisateur correspondant aux objets qu'il va analyser. \nOn parcourt l'ensemble des cours depuis la date de début, et l'ordinateur décide d'acheter, de vendre ou de ne rien faire.");
    desc_manuel = new QLabel("L'utilisateur commence par choisir les paramètres de la simulation ainsi que la date à laquelle \nil voudra commencer la simulation. \nPour chaque cours et de manière libre, l'utilisateur aura le choix entre réaliser une vente, un achat ou ne rien faire. \nL'utilisateur poura également annuler la dernière transaction réalisée. \nPour réaliser une transaction sur un cours précis, \nil suffit de double cliquer sur la bougie correspondante. \nUn bouton 'Terminer' permet de terminer la simulation. Un historique des transactions est à votre disposition pour vous y retrouvez.");
    desc_pasapas = new QLabel("L'utilisateur achète petit à petit des cours sans avoir accès aux informations nécessaires.\nAprès avoir sélectionné les paramètres de sa simulation, il peut choisir de réaliser une vente \nun achat ou ne rien faire. \nL'utilisateur peut également annuler la dernière transaction effectuée. \nUn bouton 'Terminer' permet de terminer la simulation. Un historique des transactions est à votre disposition pour vous y retrouvez.");

    // Vu que mode auto est coché de base, on cache les autres descriptions
    desc_manuel->hide();
    desc_pasapas->hide();

    // QCOMBOBOX (Pour le choix des stratégies en mode auto)

    // BOUTONS
    mode_boutons = new QGroupBox("Choix du Mode");
    automa = new QRadioButton("Mode Automatique");
    manuel = new QRadioButton("Mode Manuel");
    pasapas = new QRadioButton("Mode Pas à Pas");

    // On gère l'afichage de la description du mode quand on clique sur le bouton correspondant
    connect(automa,SIGNAL(clicked()),this,SLOT(afficherDescriptionMode()));
    connect(manuel,SIGNAL(clicked()),this,SLOT(afficherDescriptionMode()));
    connect(pasapas,SIGNAL(clicked()),this,SLOT(afficherDescriptionMode()));

    automa->setChecked(true); // le mode automatique par défaut est séléctionné par défaut

    // COMBO BOX (Mode auto uniquement)

    strategieBoxes = new QGroupBox("Choix de la Strategie");

    layout_choixStrat = new QVBoxLayout(); // Layout qui contiendra le QComboBox pour le mettre dans le QGroupBox

    choixStrategie = new QComboBox; // creation du combobox pour l'intermediaire possible
    choixStrategie->addItem("RSI");
    choixStrategie->addItem("MACD/SMA");
    choixStrategie->addItem("Analyse de la forme");
    choixStrategie->setCurrentIndex(0); // On met une valeur par défaut, pour éviter qu'il n'y en ai pas pour laisser le bouton Démarrer actif
    layout_choixStrat->addWidget(choixStrategie); // On ajoute au Layout
    strategieBoxes->setLayout(layout_choixStrat); // On met le Layout dans le QGroupBox

    // LAYOUT

    // Layout des boutons pour le QGroupBox
    boutons_layout = new QVBoxLayout();
    boutons_layout->addWidget(automa);
    boutons_layout->addWidget(manuel);
    boutons_layout->addWidget(pasapas);
    mode_boutons->setLayout(boutons_layout);

    // Layout simple pour l'ajouter à la fenêtre mode
    mode_layout = new QVBoxLayout();
    mode_layout->addWidget(mode_boutons);
    // On ajoute les description en dessous le combobox, pour les afficher selon le QRadioButton coché
    mode_layout->addWidget(desc_auto);
    mode_layout->addWidget(desc_manuel);
    mode_layout->addWidget(desc_pasapas);
    mode_layout->addWidget(strategieBoxes);
    mode->setLayout(mode_layout);

    // Enfin: On ajoute l'import créé au layout principal
    layout_objets->addWidget(mode);

}

void InterfaceFinale::precedent_itf(){
    if (etat > 0){
        etat--;
        LayoutUpdate();
    }
    // On décrémente etat et on update le layout de la sous-fenêtre à l'état précédent
}

void InterfaceFinale::suivant_itf(){
    etat++;
    LayoutUpdate();
    // Pas de test à réaliser ici car on désactivera les boutons sur l'écran final
}

void InterfaceFinale::quitter_itf(){
    this->setEnabled(false); // On rend la fenêtre principale inactive
    int reponse = QMessageBox::warning(this, "Quitter ?", "Voulez-vous mettre fin à la simulation et quitter l'application ?", QMessageBox::Yes | QMessageBox::No);
    if (reponse == QMessageBox::Yes)
        QApplication::quit(); // on quitte le programme
    else
        this->setEnabled(true); // La fenêtre principale redevient active
    // Si on fait "non", la fenêtre se ferme juste et on retourne à la fenêtre principale
}

void InterfaceFinale::charger_itf(){
    // On fait appel aux fonctions statiques de sauvegarde_simulation_XML
    // On affiche le reste seulement si le chargement réussi
    if(Sauvegarde_XML::LectureDuFichier_XML() == true){
        tab = Sauvegarde_XML::getTab(); // on ne récupère que le tableau de transaction à partir du fichier XML ici
        tab->show();
    }
}

void InterfaceFinale::import_itf(){

    // Si on a déjà importé un EvolutionCours avant et qu'on décide de faire un nouvel import, il faut supprimer le précédent !
    if(e != nullptr)
        delete e;

    DevisesManager& m=DevisesManager::getManager();
    QString filtre(".csv(*.csv)");
    //Vérifier si on a pas déjà un fichier à partir d'une sauvegarde
       QString nomdufichier(chemin);
       QString chemin_Qstr(chemin);
       if(chemin.isNull() || chemin.isEmpty()){
           // Boîte de dialogue "Ouvrir un fichier" renvoyant en QString le chemin vers le fichier
           QFileInfo chemin = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "../",filtre);
           chemin_Qstr=chemin.absoluteFilePath();
           //pour recupere le chemin du fichier.csv pour la sauvegarde XML
           Sauvegarde_XML::setChemin(chemin_Qstr);
           nomdufichier = chemin.fileName(); // Nom du fichier afin de récupérer le nom des Devises ou de l'action
       }
       affiche_chemin->setText(chemin_Qstr); // Met le chemin récupéré dans le QLineEdit lors de l'import

    // On effectue la suite ssi on a bien importé un fichier d'extension .csv
    if(nomdufichier.contains(".csv")){
        string chemin_str=chemin_Qstr.toStdString(); // On utilise un string et non un QString dans notre remplissage, d'où cette conversion
        int positiondupoint=nomdufichier.indexOf(".");
        int longueur=nomdufichier.size();
        nomdufichier=nomdufichier.remove(positiondupoint,longueur-positiondupoint);
        cout<<nomdufichier.toStdString();

        // On reconnait les fichiers .csv de Devises (et non d'Action d'entreprise) s'ils ont l'un des séparateurs suivants.
        // On peut ajouter des séparateurs à notre guise si on veut étendre son fonctionnement !
        // On autorise pas les fichiers avec plusieurs séparateurs : nom invalide

        // SEPARATEUR 1: -
        if(nomdufichier.contains("-")){
           QStringList list;
           list = QString(nomdufichier).split("-");

           QString premieredevise = list.at(0);
           cout<<premieredevise.toStdString()<<endl;
           //list.removeFirst();
           QString deuxiemedevise =list.at(1);
           cout<<deuxiemedevise.toStdString()<<endl;

           m.creationDevise(premieredevise, "","");
           m.creationDevise(deuxiemedevise, "","");
           PaireDevises& notrepaire=m.getPaireDevises(premieredevise,deuxiemedevise);
           e = new EvolutionCours(notrepaire);
        }
        else{
           m.creationAction(nomdufichier,"Entreprise","");
           Action &nom=m.getAction(nomdufichier);
           e = new EvolutionCours(nom);
        }
        csv::Lecteur::RemplirEvolutionCours(dynamic_cast<EvolutionCours*>(e),chemin_str);

        suivant->setEnabled(true); // on réactive le bouton suivant une fois l'import réussi
        ouvrir->setEnabled(false); // on désactive le bouton ouvrir après un import réussi
    }
    else{
        QMessageBox::critical(NULL,"Erreur","Impossible d'ouvrir le fichier. \nVerifiez que l'extension de votre fichier est bien .csv.");
    }
}

void InterfaceFinale::lancerSimulation(){
    if(automa->isChecked()){ // Si on a choisi le mode auto
        ModeAuto *simulationAuto = new ModeAuto(dynamic_cast<ChoixParam*>(choix),affiche_chemin->text(),this);
        // On doit faire un dynamic_cast sur choix vu qu'il est considéré comme QWidget à la base
        simulationAuto->lancer_simulation(choixStrategie->currentText(),e);
        // On commence la simulation en fournissant la stratégie choisie dans choixStrategie et les données de EvolutionCours
        this->hide(); // On cache l'interface principale
    }
    if(manuel->isChecked()){ // Si on a choisi le mode manuel
        ModeManuel *simulationManuel = new ModeManuel(e,dynamic_cast<ChoixParam*>(choix),affiche_chemin->text(),this);
        simulationManuel->lancer_simulation();
        this->hide(); // On cache l'interface principale, dont on a plus besoin vu qu'on bascule sur Affichage
    }
    if(pasapas->isChecked()){ // Si on a choisi le mode pas à pas
        Modepasapas *simulationPasaPas = new Modepasapas(this,dynamic_cast<ChoixParam*>(choix),affiche_chemin->text(),e);
        simulationPasaPas->lancer_simulation();
        this->hide(); // On cache l'interface principale, dont on a plus besoin vu qu'on bascule sur Affichage
    }
}

void InterfaceFinale::afficherDescriptionMode(){
    if(automa->isChecked()){ // Si on a cliqué sur le mode auto, sa description s'affiche uniquement
        desc_pasapas->hide();
        desc_manuel->hide();
        desc_auto->show();
        strategieBoxes->show();
    }
    if(manuel->isChecked()){
        desc_pasapas->hide();
        desc_manuel->show();
        desc_auto->hide();
        strategieBoxes->hide();
    }
    if(pasapas->isChecked()){
        desc_pasapas->show();
        desc_manuel->hide();
        desc_auto->hide();
        strategieBoxes->hide();
    }
}
