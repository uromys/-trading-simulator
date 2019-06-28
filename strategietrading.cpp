#include "strategietrading.h"
#include "affichage.h"
#include "sauvegarde_simulation_xml.h"
#include <cstdlib>

// 3 Strategie déjà: Une sur l'indicateur RSI,  une sur l'indictateur MACD et EMA, une sur la forme analysé (AnalyseForme)



StrategieTrading::~StrategieTrading()=default;
QString StrategieRSI::procedure(unsigned int pos){
    if(rsi->getRSI(pos)=="NA") return "rien";
    if(rsi->getRSI(pos).toDouble()>70) return "vente";
    if(rsi->getRSI(pos).toDouble()<30) return "achat";
    return "rien";
}


QString StrategieMACD::procedure(unsigned int pos){
    if(pos==0 || macd->getMACD(pos-1)=="NA") return "rien";
    if(macd->getSignal(pos-1).toDouble()>macd->getMACD(pos-1).toDouble() && macd->getSignal(pos).toDouble()<macd->getMACD(pos).toDouble()) return "achat"; // si on coupe le signal depuis le bas
    if(macd->getSignal(pos-1).toDouble()<macd->getMACD(pos-1).toDouble() && macd->getSignal(pos).toDouble()>macd->getMACD(pos).toDouble()) return "vente"; // si on coupe le signal depuis le haut
    return "rien";
}

QString StrategieBougie::procedure(unsigned int pos){
    Bougie* bougie= new Bougie(cours->getCours(pos).getOpen(),cours->getCours(pos).getHigh(),cours->getCours(pos).getLow(),cours->getCours(pos).getHigh(), new CoursOHLC(cours->getCours(pos)));
    if(bougie->getForme()=="Marteau") return "achat";
    if(bougie->getForme()=="Etoile Filante") return "vente";
    return "rien";
}


double Transaction::getMontantBase() const{    //récupère le montant de base en float à partir de l'attributs en string
    QStringList list = MontantBase.split("(");
    return list[0].toDouble();
}

double Transaction::getMontantContre() const{
    QStringList list = MontantContre.split("(");
    return list[0].toDouble();
}

double Transaction::ConversionBaseCotation(double base, double pourcent, CoursOHLC cours, unsigned int mode){  //le mode permet de choisir le moment de conversion : si c'est fin de journée alors mode = 0, si c'est durant la transaction alors mode !0
    if (mode==0) return base*cours.getClose();
    return (1-pourcent)*base*cours.getOpen();
}

double Transaction::ConversionCotationBase(double cotation, double pourcent, CoursOHLC cours, unsigned int mode){
    if (mode==0) return cotation/cours.getClose();
    return (1-pourcent)*cotation/cours.getOpen();
}

void Transaction::setRoi(double d){
    roi=MontantTotal/d;
}
Transaction::Transaction (double mb, double mc,  QDate d, CoursOHLC cours,QString t, double pourcent, double montant): date(d),type(t){


    QString strmb,strmc, montantTransac, conversion;           //string qui vont nous permettre d'initialiser nos attributs
    double transaction;                                        // pour le calcul de la transaction
    if(t=="achat"){

        transaction = ConversionBaseCotation(montant,pourcent, cours,1); // si on fait un achat on converti des bases en contreparties.

        strmb.setNum(mb-montant);
        strmb.append("(-"+montantTransac.setNum(montant)+")");   //on a donc pris un montant de base

        strmc.setNum(mc+transaction);                           // et on recoit cette argent converti lors de la transaction en contrepartie
        strmc.append("(+"+conversion.setNum(transaction)+")");

        MontantBase=strmb;                                       // on peut alors initialiser nos attributs
        MontantContre=strmc;
        MontantTotal = getMontantContre()+ConversionBaseCotation(getMontantBase(),pourcent, cours);
    }
    else if(t=="vente"){

        transaction=ConversionCotationBase(montant,pourcent,cours,1); //meme histoire mais dans le cas d'une vente

        strmb.setNum(mb+transaction);
        strmb.append("(+"+conversion.setNum(transaction)+")");

        strmc.setNum(mc-montant);
        strmc.append("(-"+montantTransac.setNum(montant)+")");

        MontantBase=strmb;
        MontantContre=strmc;
        MontantTotal = getMontantContre()+ConversionBaseCotation(getMontantBase(),pourcent, cours);
    }
}
void Tableau::addElement(unsigned int pos, QTableWidget* w, std::vector<Transaction> t){

    //Ajoute une ligne dans le Tableau

    w->insertRow(static_cast<int>(pos));

    //Création des objets qui vont etre créé dans le tableau w

    QTableWidgetItem* date = new QTableWidgetItem();
    QTableWidgetItem* montantBase = new QTableWidgetItem();
    QTableWidgetItem* montantContre = new QTableWidgetItem();
    QTableWidgetItem* roi = new QTableWidgetItem();
    QTableWidgetItem* type = new QTableWidgetItem();

    //Alignement central de ces objets

    date->setTextAlignment(Qt::AlignCenter);
    montantBase->setTextAlignment(Qt::AlignCenter);
    montantContre->setTextAlignment(Qt::AlignCenter);
    roi->setTextAlignment(Qt::AlignCenter);
    type->setTextAlignment(Qt::AlignCenter);

    //Récupération des valeurs de ces objets

    date->setText(t[pos].getDate().toString("yyyy.MM.dd"));
    montantBase->setText(t[pos].getMontantB());
    montantContre->setText(t[pos].getMontantC());
    roi->setText(QString::number(t[pos].getRoi()));
    type->setText(t[pos].getType());

    //Chargement des valeurs de ces objets dans le tableau w

    w->setItem(static_cast<int>(pos),0,date);
    w->setItem(static_cast<int>(pos),1,montantBase);
    w->setItem(static_cast<int>(pos),2,montantContre);
    w->setItem(static_cast<int>(pos),3,roi);
    w->setItem(static_cast<int>(pos),4,type);

    Sauvegarde_XML::setTableau(w);
}

void Tableau::removeElement(unsigned int pos,QTableWidget* w){
    w->removeRow(static_cast<int>(pos));
     Sauvegarde_XML::setTableau(w);
}

QTableWidget* Tableau::createTable(){
    QTableWidget* Tableau = new QTableWidget();
    Tableau->setEditTriggers(QAbstractItemView::NoEditTriggers);
    Tableau->setColumnCount(5);
    QStringList list = {"date","montant de Base","montant de Contrepartie","ROI","type"};
    Tableau->setHorizontalHeaderLabels(list);
    Tableau->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    Sauvegarde_XML::setTableau(Tableau);

    return Tableau;
}
