#include "edittexte.h"

EditTexte::EditTexte(QTableWidget* tab, QString c) : QMainWindow(),chemin(c), transac(tab)
{

    setWindowTitle("Editeur de Texte");
    setMinimumSize(largeur, hauteur);

    menuFichier = menuBar()->addMenu("&Fichier");
    // RAPPEL: le & sert à ouvrir le menu en appuyant sur alt + la lettre du clavier suivant le &
    // On ouvrira ici le menu Fichier en appuyant sur alt+F

        //Action du menu Fichier :
        actionNew = menuFichier->addAction("Nouveau");
        actionOpen = menuFichier->addAction("Ouvrir");
        menuFichier->addSeparator(); // sépare en catégorie les actions du menu
        actionSave = menuFichier->addAction("Exporter en txt");
        actionSaveXML = menuFichier->addAction("Exporter en XML");
        menuFichier->addSeparator();
        actionExit = menuFichier->addAction("Quitter");

        // Raccourcis Clavier : Lorsqu'on fera ces commandes, cela déclenchera les actions
        actionNew->setShortcut(QKeySequence("Ctrl+N"));
        actionOpen->setShortcut(QKeySequence("Ctrl+O"));
        actionSave->setShortcut(QKeySequence("Ctrl+S"));
        actionExit->setShortcut(QKeySequence("Ctrl+Q"));


    menuEdition = menuBar()->addMenu("&Edition");

        //Action du menu Edition :
        actionUndo = menuEdition->addAction("Annuler");
        actionRedo = menuEdition->addAction("Rétablir");
        menuEdition->addSeparator();
        actionCut = menuEdition->addAction("Couper");
        actionCopy = menuEdition->addAction("Copier");
        actionPaste = menuEdition->addAction("Coller");
        menuEdition->addSeparator();
        actionSelectAll = menuEdition->addAction("Tout sélectionner");

        // Raccourcis Clavier :
        actionUndo->setShortcut(QKeySequence("Ctrl+Z"));
        actionRedo->setShortcut(QKeySequence("Ctrl+Y"));
        actionCut->setShortcut(QKeySequence("Ctrl+X"));
        actionCopy->setShortcut(QKeySequence("Ctrl+C"));
        actionPaste->setShortcut(QKeySequence("Ctrl+V"));
        actionSelectAll->setShortcut(QKeySequence("Ctrl+A"));

        // Attribution des icones :
        QDir dir("../"); // Permet d'accéder aux ressources depuis un autre dossier que celui de l'executable
        actionNew->setIcon(QIcon(dir.relativeFilePath("../icones/filenew.png")));
        actionOpen->setIcon(QIcon(dir.relativeFilePath("../icones/fileopen.png")));
        actionSave->setIcon(QIcon(dir.relativeFilePath("../icones/filesave.png")));
        actionUndo->setIcon(QIcon(dir.relativeFilePath("../icones/undo.png")));
        actionRedo->setIcon(QIcon(dir.relativeFilePath("../icones/redo.png")));
        actionExit->setIcon(QIcon(dir.relativeFilePath("../icones/exit.png")));
        actionCopy->setIcon(QIcon(dir.relativeFilePath("../icones/copy.png")));
        actionPaste->setIcon(QIcon(dir.relativeFilePath("../icones/paste.png")));
        actionCut->setIcon(QIcon(dir.relativeFilePath("../icones/cut.png")));
        actionSelectAll->setIcon(QIcon(dir.relativeFilePath("../icones/sall.png")));

        // Note : on peut faire une toolbar (accès rapide):
        barre = addToolBar("Bar d'outils");
        barre->addAction(actionNew); // bouton nouveau
        barre->addAction(actionOpen); // bouton ouvrir
        barre->addAction(actionSave); // bouton enregistrer
        barre->addAction(actionUndo); // bouton enregistrer
        barre->addAction(actionRedo); // bouton enregistrer
        barre->addAction(actionExit); // bouton quitter
        // On peut aussi configurer une icone pour avoir des boutons plus classes

    // Création de la zone de texte :
    zoneTexte = new QTextEdit; // Zone de texte vide créée
    zoneTexte->setText(Sauvegarde_XML::getNotes());

    setCentralWidget(zoneTexte);

    // Connexion des actions :
    connect(actionExit, SIGNAL(triggered()), this, SLOT(quitter()));
    connect(actionSave, SIGNAL(triggered()), this, SLOT(saveFile()));
    connect(actionSaveXML, SIGNAL(triggered()), this, SLOT(saveFileXML()));
    connect(actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));
    connect(actionNew, SIGNAL(triggered()), zoneTexte, SLOT(clear()));
    connect(actionUndo, SIGNAL(triggered()), zoneTexte, SLOT(undo()));
    connect(actionRedo, SIGNAL(triggered()), zoneTexte, SLOT(redo()));
    connect(actionCut, SIGNAL(triggered()), zoneTexte, SLOT(cut()));
    connect(actionSelectAll, SIGNAL(triggered()), zoneTexte, SLOT(selectAll()));
    connect(actionCopy, SIGNAL(triggered()), zoneTexte, SLOT(copy()));
    connect(actionPaste, SIGNAL(triggered()), zoneTexte, SLOT(paste()));

}

void EditTexte::openFile(){

    // Filtre des extensions visibles, à mettre en dernier paramètre du getOpenFileName de QFileDialog
    QString filtre("Fichier Texte(*.txt *.ol *.fe)");

    // Boîte de dialogue "Ouvrir un fichier" renvoyant en QString le chemin vers le fichier
    QString path = QFileDialog::getOpenFileName(this, "Ouvrir un fichier", "../", "Fichier Texte(*.txt *.ol *.fe)");

    // Ajout à la zone de texte
    QFile fichier(path); // Ouverture du fichier
    if(!fichier.open(QIODevice::ReadOnly |QIODevice::Text)){
        QMessageBox::critical(this,"Erreur","Impossible d'ouvrir le fichier");
    }

    // Pour tester le message d'erreur, on peut aller dans Debug puis essayer d'ouvrir "trading.exe"
    else{
        QTextStream contenu(&fichier); // On charge le flux de contenu du fichier dans la variable "contenu"
        zoneTexte->setPlainText(contenu.readAll());
        fichier.close();
    }
}

void EditTexte::saveFile()
{ // Filtre des extensions visibles, à mettre en dernier paramètre du getSaveFileName de QFileDialog
    QString filtre("Fichier Texte(*.txt *.ol *.fe)");

    QDir directory= QDir::currentPath();
    directory.cdUp();
    QString path = directory.path()+"/Simulation_du_"+QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss");
    directory.mkpath(path);


    // Sauvegarde dans le stream du fichier
    QFile fichier(path+"/notes.txt"); // Ouverture du fichier


    if(!fichier.open(QIODevice::WriteOnly |QIODevice::Text)){
        QMessageBox::critical(this,"Erreur","Impossible de sauvegarder le fichier");
    }

    // Ouvrir le fichier en WriteOnly permet de supprimer son contenu précédent
    else{
        QFile fichierTab(path+"/Transactions.txt");
        if(!fichierTab.open(QIODevice::WriteOnly |QIODevice::Text)){
            QMessageBox::critical(this,"Erreur","Impossible de sauvegarder le fichier");
        }
        else{
            QFile::copy(chemin,path+"/Cours.csv"); // Stockera le fichier csv initial
            QString Transactions;
            Transactions  += "\n";
            Transactions  += "------------------Tableau------------------";
            Transactions  += "\n";
            for (int k = 0; k <transac->columnCount();k++){
                Transactions  += transac->horizontalHeaderItem(k)->text();  // affiche le nom des colonnes
                Transactions += "||";
            }
            Transactions  += "\n";
            for (int i = 0; i < transac->rowCount(); i++) {
                for (int j = 0; j < transac->columnCount(); j++) {

                        Transactions += transac->item(i,j)->text();
                        Transactions += "||";
                }
                Transactions  += "\n";
                Transactions  += "\n";
            }

            QTextStream contenu(&fichier);
            QTextStream contenuXML(&fichierTab);
            contenu << "------------------Notes------------------";
            contenu << "\n";
            contenu << zoneTexte->toPlainText();
            contenuXML << Transactions;

            fichier.close();
            fichierTab.close();
            QMessageBox::information(this,"Enregistrement","Données sauvegardées dans le dossier suivant: \n" + path);
        }
    }
}
