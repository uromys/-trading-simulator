#include "trading.h"
#include "sauvegarde_simulation_xml.h"

using namespace std;

QString Sauvegarde_XML::chemin = "";
QString Sauvegarde_XML::notes = "";
QTableWidget* Sauvegarde_XML::tab = nullptr;
QString Sauvegarde_XML::pathSave = "";

bool Sauvegarde_XML::CreationDuFichier_XML(QString note){

    // PARAMETRES (A modifier à notre convenance)

    QString nomfichier = "/Sauvegarde_Simulation.xml";
    QDir directory= QDir::currentPath();
    directory.cdUp(); // On se place un repertoire plus haut que celui de base, soit dans le même que celui du Build
    QString path = directory.path()+"/Simulation_du_"+QDateTime::currentDateTime().toString("dd_MM_yyyy_hh_mm_ss");
    directory.mkpath(path);

    pathSave = path;

    // FONCTION

    QDomDocument doc("ProjectData");
    QDomElement sauvegarde = doc.createElement("fichier_sauvegarde");
    //point.setAttribute("value","nom"); si pas d attribut entre < >
    doc.appendChild(sauvegarde);

    // insertion en début de document de <?xml version="1.0" ?>
    QDomProcessingInstruction instructions = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"ISO-8859-1\"");
    doc.insertBefore(instructions, doc.firstChild());

     try {
        //creation des noeud
        QDomElement chemin_fichier_de_donnee_balise = doc.createElement("chemin_du_fichier");
        QDomText lien_chemin = doc.createTextNode(chemin);
        chemin_fichier_de_donnee_balise.appendChild(lien_chemin);
        sauvegarde.appendChild(chemin_fichier_de_donnee_balise); //ajout de abscisse à sauvegarde
    } catch (runtime_error) {}

    try {
         QDomElement les_notes_balise = doc.createElement("notes");
         QDomText notes = doc.createTextNode(note);
          les_notes_balise.appendChild(notes);
          sauvegarde.appendChild(les_notes_balise);
    } catch (runtime_error) {}

        try {

           QDomElement tableau_balise = doc.createElement("tableau_des_achats_et_des_ventes");
            sauvegarde.appendChild(tableau_balise);
           QString Colonnes[5]={"date", "montant_total_cotation", "montant_total_base", "ROI", "type"};

           for(unsigned int i=0; i<tab->rowCount(); i++){
                QDomElement Achat_Vente_balise = doc.createElement("achats_ventes");
                 tableau_balise.appendChild(Achat_Vente_balise);
               for(unsigned int j =0; j<tab->columnCount(); j++ ){
                   QDomElement colonne_balise = doc.createElement(Colonnes[j]);
                   QDomText item = doc.createTextNode(tab->item(i,j)->text());
                   colonne_balise.appendChild(item);
                   Achat_Vente_balise.appendChild(colonne_balise);

               }

           }



        } catch (runtime_error) {}

     QFile file(path + nomfichier); // Permet de créer le fichier avec l'emplacement et le nom voulu
     qDebug() << path + nomfichier;
     if ( file.open( QIODevice::WriteOnly | QIODevice::Text ) )
     {
         QTextStream out (&file);
         out.setDevice(&file); // association du flux au fichier
         doc.save(out,2, QDomDocument::EncodingFromDocument);
         file.close();
         return true; // La sauvegarde a réussi
     }

     return false; // la sauvegarde a échouée

}

bool Sauvegarde_XML::LectureDuFichier_XML(){
//chargement
    QString nomFichier = QFileDialog::getOpenFileName(nullptr, "Ouvrir un fichier","../", "XML files (*.xml)");
    QFile file(nomFichier.toUtf8().constData());
       if (!file.open(QIODevice::ReadOnly | QIODevice::Text)){
           QMessageBox::critical(nullptr,"Erreur","Impossible d'ouvrir le ficher XML");
           file.close();
           return false;
       }

    QDomDocument doc;
    doc.setContent(&file);
    file.close();

    //lecture
    QDomElement racine=doc.documentElement(); //prendre la racine
    //recuperer les fils
    QDomElement Noeud_fils=racine.firstChild().toElement();

    while(!Noeud_fils.isNull())
    {
       // verification si le fils a qqch
       if (Noeud_fils.tagName()=="chemin_du_fichier")
       {
           chemin = Noeud_fils.firstChild().toText().data();
       }

       else if (Noeud_fils.tagName()=="notes") {
           notes = Noeud_fils.firstChild().toText().data();
        }

       else if (Noeud_fils.tagName()=="tableau_des_achats_et_des_ventes") {
           QDomElement Achat_Vente_balise= Noeud_fils.firstChild().toElement();
           tab = new QTableWidget();
           tab->setEditTriggers(QAbstractItemView::NoEditTriggers);
           tab->setColumnCount(5);
           QStringList list = {"date","montant de Base","montant de Contrepartie","ROI","type"};
           tab->setHorizontalHeaderLabels(list);
           tab->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
           tab->setFixedSize(static_cast<int>(1300*0.99),static_cast<int>(900*0.17));

           int num_ligne = 0;
           while (!Achat_Vente_balise.isNull()){
               tab->insertRow(num_ligne);
               QDomElement colonne = Achat_Vente_balise.firstChild().toElement();
               // On se repère dans les balises du fichier XML pour les lire
               // Si on ajoute de nouvelles balises à l'avenir, il faudra créé un nouveau if ici
               // On aurait éventuellement pu faire un switch, c'est un choix de conception
               while(!colonne.isNull()){
                   if(colonne.tagName() == "date"){
                    QString date = colonne.firstChild().toText().data();
                    QTableWidgetItem* date_item = new QTableWidgetItem();
                    date_item->setText(date);
                    date_item->setTextAlignment(Qt::AlignCenter);
                    tab->setItem(num_ligne, 0, date_item);
                   }else if (colonne.tagName() == "montant_total_cotation") {
                       QString note = colonne.firstChild().toText().data();
                       QTableWidgetItem* note_item = new QTableWidgetItem();
                       note_item->setText(note);
                       tab->setItem(num_ligne, 1, note_item);
                    }else if (colonne.tagName() == "montant_total_base") {
                       QString mtb = colonne.firstChild().toText().data();
                       QTableWidgetItem* mtb_item = new QTableWidgetItem();
                       mtb_item->setText(mtb);
                       tab->setItem(num_ligne, 2, mtb_item);
                   }else if (colonne.tagName() == "ROI") {
                       QString roi = colonne.firstChild().toText().data();
                       QTableWidgetItem* roi_item = new QTableWidgetItem();
                       roi_item->setText(roi);
                       tab->setItem(num_ligne, 3, roi_item);
                   }else if (colonne.tagName() == "type") {
                       QString type = colonne.firstChild().toText().data();
                       QTableWidgetItem* type_item = new QTableWidgetItem();
                       type_item->setText(type);
                       tab->setItem(num_ligne, 4, type_item);
                   }
                   colonne = colonne.nextSibling().toElement();
               }

               num_ligne++;
               Achat_Vente_balise = Achat_Vente_balise.nextSibling().toElement();
           }
        }

       // Prochain composant
       Noeud_fils = Noeud_fils.nextSibling().toElement();
     }
    return true;

}
