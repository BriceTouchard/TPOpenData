#ifndef GRAPHE_H
#define GRAPHE_H

#include <string>
#include <gd.h>
#include <nlohmann/json.hpp>
#include <list>

using namespace std;
using json = nlohmann::json;
using namespace nlohmann::literals;

class Graphe
{
public:
    // Constructeur(s)
    Graphe();
    Graphe(int imgsizeX,int imgsizeY, int barWidth, int marginX, int marginXright, int marginY, int spacing);

    // Attributs
    int imgsizeX;
    int imgsizeY;
    int barWidth;
    int marginX;
    int marginXright;
    int marginY;
    int spacing;
    int h, hMax, hMin, ecart;
    float coeff, scaleMin, scaleMax;
    string grandeur_hydro, libelle_site, date_obs;

    // Méthodes
    int y(int val);

    unsigned char* floatToUcEtoile(float f);

    void dessinHistogramme(json jDataObs, gdImagePtr im, vector<int> colors);

    void dessinGraphe2(json jDataObsH, gdImagePtr im, vector<int> colors);

    void makeScale(json jDataObs);

    void toString();


};

#endif // GRAPHE_H
