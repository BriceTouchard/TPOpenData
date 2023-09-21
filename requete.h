#ifndef REQUETE_H
#define REQUETE_H

#include <string>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;
using namespace nlohmann::literals;

class Requete
{
public:
    // Constructeur(s)
    Requete();

    // Attributs
    string urlBase;

    // Méthodes
    string request(string url);

    json requestData(string url);


};

#endif // REQUETE_H
