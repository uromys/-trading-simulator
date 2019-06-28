#include <fstream>
#include <sstream>
#include <iomanip>
#include "import.h"



namespace csv {

    Lecteur::Lecteur(const string &data, char sep) : _sep(sep) {

        string ligne;
        _fichier = data;
        ifstream importe(_fichier.c_str()); //flux fichier
        if (importe.is_open())
        {
            while (importe.good())
            {
                getline(importe, ligne);
                if (ligne != "")
                    originefichier.push_back(ligne);
            }
            importe.close();

            if (originefichier.size() == 0) {csvException(string("Il faut ouvrir un fichier pour utiliser le programme"));}
            else{
                lecteurtete();
                Lecteurcontenu();
            }


        }
        else{ csvException(string("impossible d'ouvrir le fichier").append(_fichier));}

    }

    Lecteur::~Lecteur()
    {
        vector<Col *>::iterator it;
        for (it = contenu.begin(); it != contenu.end(); it++)
            delete *it;
    }


    void Lecteur::lecteurtete()
    {
        stringstream ss(originefichier[0]);
        string item;

        while (getline(ss, item, _sep))
            trading.push_back(item);
    }

    //lit chaque ligne , et a chaque "," push dans un vector
    void Lecteur::Lecteurcontenu(){
        vector<string>::iterator it;

        for (it = originefichier.begin(); it != originefichier.end(); it++){
            unsigned int debut = 0;
            unsigned int i = 0;
            Col *col = new Col(trading);

            for (; i != it->length(); i++){
                if (it->at(i) == ','){
                  col->push(it->substr(debut, i - debut));
                  debut = i + 1;
                }
            }

            col->push(it->substr(debut, it->length() - debut));

            if (col->size() != trading.size()) { csvException("probleme contient une ligne vide"); }
            else { contenu.push_back(col); }
       }
    }

    Col &Lecteur::getCol(unsigned int colPosition) const
    {
        if (colPosition < contenu.size())
            return *(contenu[colPosition]);
        throw csvException("la colonne existe pas ");
    }

    Col &Lecteur::operator[](unsigned int colPosition) const //permet d'obtenir une ligne
    {
        return Lecteur::getCol(colPosition);
    }

    unsigned int Lecteur::nbligne() const
    {
        return static_cast<unsigned int>(contenu.size());
    }

    unsigned int Lecteur::NombreColonne() const
    {
        return static_cast<unsigned int>(trading.size());
    }

    vector<string> Lecteur::getpremiereligne() const
    {
        return trading;
    }

    const string Lecteur::GetTrading(unsigned int position) const
    {
        if (position >= trading.size()){
            csvException("la premiere ligne n'a rien ");
        return "";}
        else {
        return trading[position];}
    }

    bool Lecteur::RemplirEvolutionCours(EvolutionCours *e, string chemin){

           Lecteur fichier2 = csv::Lecteur(chemin);
           unsigned int i;
           for (i=1 ; i < fichier2.nbligne() ; i++){
               //on lit pas la premiere ligne, contient les headers
               //le 2 [] correspond a chaque colonne
               // OPEN
               QString openi=QString::fromStdString(fichier2[i][1]);// conversion string to Qstring, puis en double pour chaque OHLC
               bool ok=false;
               double open2=openi.toDouble(&ok);

               // HIGH
               QString high=QString::fromStdString(fichier2[i][2]);
               double high2 =high.toDouble(&ok);

               // CLOSE
               QString low=QString::fromStdString(fichier2[i][3].c_str());
               double low2=low.toDouble(&ok);

               // CLOSE
               QString close=QString::fromStdString(fichier2[i][4].c_str());
               double close2=close.toDouble(&ok);

               // VOLUME
               QString volume=QString::fromStdString(fichier2[i][6]);
               double volume2=volume.toInt(&ok);

               // DATE
               QString date=QString::fromStdString(fichier2[i][0]);
               QDate datel=QDate::fromString(date, "yyyy-MM-dd");

               // REMPLISSAGE EVOLUTION COURS
               e->addCours(open2,high2,low2,close2,volume2,datel,static_cast<int>(i-1));
               // On pense à faire i-1 pour l'indice vu qu'on commence la boucle for à i=1

          }
          if(i == fichier2.nbligne())
            return true;
          else
            return false;


    }

  const string &Lecteur::getfichier() const
  {
      return _fichier;
  }


  Col::Col(const vector<string> &tete)
      : trading(tete) {}

  Col::~Col() {}

  unsigned int Col::size() const
  {
    return static_cast<unsigned int>(valeurs.size());
    // On fait un static cast car un size of renvoie un élément de type size_t
  }

  void Col::push(const string &valeur)
  {
    valeurs.push_back(valeur);
  }



  string Col::operator[](unsigned int valeurPosition) const
  {
       if (valeurPosition < valeurs.size())
           return valeurs[valeurPosition];
       throw csvException("la valeur existe pas ");
  }

  string Col::operator[](const string &devise) const//permet de chercher directement une devise
  {
      vector<string>::const_iterator it;
      int position = 0;

      for (it = trading.begin(); it != trading.end(); it++)
      {
          if (devise == *it)
              return valeurs[position];
          position++;
      }

      throw csvException("cette valeur existe pas ");
  }

  ostream &operator<<(ostream &os, const Col &col)
  {
      for (unsigned int i = 0; i != col.valeurs.size(); i++)
          os << col.valeurs[i] << " , ";

      return os;
  }

  ofstream &operator<<(ofstream &os, const Col &col)
  {
    for (unsigned int i = 0; i != col.valeurs.size(); i++)
    {
        os << col.valeurs[i];
        if (i < col.valeurs.size() - 1)
          os << ",";
    }
    return os;
  }

} // fin du namespace csv
