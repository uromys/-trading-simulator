#pragma once
#include <iostream>
# include <stdexcept>
# include <string>
# include <vector>
# include <list>
# include <sstream>

#include "trading.h"


/*! \class csvException
            \brief La classe csvException permet de gérer les erreurs notamment à l'importation d'un fichier au format csv
    */

using namespace std;

namespace csv{

    //gere les erreurs
    class csvException {
        string info;
    public:
        csvException(const string& message) :info(message){
            QMessageBox::critical(NULL,"Erreur",QString::fromStdString(info));
        }
        string getInfo() const { return info; }

    };
    /*! \class Col
                    \brief La classe Col permet de définir un tableau dans lequel seront rangées les valuers du fichier importé
            */

    class Col
    {
        const vector<string> trading; //vector  tableaux c++ avec plein de fonctior intégéré
        vector<string> valeurs;
    public:
        Col(const vector<string> &tete);
        ~Col();

        unsigned int size() const;
        void push(const string &valeur);

        template<typename T> //on fait par template parce que les values sont soit des integers, soit des caractères
        const T getValue(unsigned int position) const{
            if (position < valeurs.size()){
                T res;
                stringstream ss;
                ss << valeurs[position];
                ss >> res;
                return res;
            }
        }

        string operator[](unsigned int) const; // donne le numéro de la ligne
        string operator[](const string &valueName) const; // permet d'obtenir une position
        friend ostream& operator<<(ostream& os, const Col &col);
        friend ofstream& operator<<(ofstream& os, const Col &col);
    };
    /*! \class Lecteur
                    \brief Classe Lecteur permet d'ouvrir, de lire le fichier.csv choisi et de le transformer le en un tableau utilisable dans notre programme
            */
     class Lecteur
     {
        string _fichier;
        const char _sep;
        vector<string> originefichier;
        vector<string> trading;//premiere ligne
        vector<Col *> contenu;
     public:
        Lecteur(const string &data, char sep = ',');//csv donc  on  initialise a ,
        ~Lecteur();
        Col &getCol(unsigned int col) const;
        unsigned int nbligne() const;
        unsigned int NombreColonne() const;
        vector<string> getpremiereligne() const; //si on veux obtenir le type "volume, date, open...
        const string GetTrading(unsigned int position) const;
        const string &getfichier() const;
        Col &operator[](unsigned int col) const; //permet d'obtenir une ligne précise

        static bool RemplirEvolutionCours(EvolutionCours *e, string chemin);

        void lecteurtete();
        void Lecteurcontenu();
    };

}

