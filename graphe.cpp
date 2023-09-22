#include "graphe.h"

#include <iostream>
#include <nlohmann/json.hpp>
#include <algorithm>
#include <string>
#include <curl/curl.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <gd.h>
#include "gdfontt.h"
#include "gdfonts.h"
#include "gdfontmb.h"
#include "gdfontl.h"
#include "gdfontg.h"
#include <list>
#include <iostream>
#include <boost/range/adaptor/reversed.hpp>
#include <boost/algorithm/string.hpp>
#include <math.h>
#include "graphe.h"
#include "requete.h"

using namespace std;
using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace curlpp::options;

Graphe::Graphe()
{

}

Graphe::Graphe(int imgsizeX, int imgsizeY, int barWidth, int marginX, int marginXright,int marginY, int spacing)
{
    this->imgsizeX = imgsizeX;
    this->imgsizeY = imgsizeY;
    this->barWidth = barWidth;
    this->marginX = marginX;
    this->marginXright = marginXright;
    this->marginY = marginY;
    this->spacing = spacing;
    this->h = 0;
    this->hMax = 0;
    this->hMin = 0;
    this->ecart = 0;
    this->coeff = 0.0;
    this->scaleMin = 0;
    this->scaleMax = 0;
    this->grandeur_hydro = "";
}

/**
 * @brief Graphe::y
 * @param val
 * @return l'ordonnée du haut des barres de l'histogramme
 */
int Graphe::y(int val)
{
    int y = imgsizeY-marginY-(val-hMin+ecart/5)*coeff;
    return y;
}

/**
 * @brief Graphe::floatToUcEtoile
 * @param f
 * @return le casting d'un float en unsigned char*
 */
unsigned char *Graphe::floatToUcEtoile(float f)
{
    int i = static_cast<int>(f);
    string str = to_string(i);
    const char* ccEtoile = str.c_str();
    unsigned char* ucEtoile;
    ucEtoile = (unsigned char*) ccEtoile;
    return ucEtoile;
}

/**
 * @brief Graphe::dessinHistogramme
 * @param jDataObs
 * @param im
 * @param colors
 */
void Graphe::dessinHistogramme(json jDataObs, gdImagePtr im, vector<int> colors)
{
    toString();
    int i=0;
    for (json &result : boost::adaptors::reverse(jDataObs)){   // la requête sort dans l'ordre anti-chronologique, donc on inverse
        h = static_cast<int>(result["resultat_obs"]);
        if(grandeur_hydro == "Q") h=h/100; // conversion en m³/s

        // Dessin des barres de l'histogramme
        int x = marginX+(barWidth+spacing)*i;
        gdImageFilledRectangle(im,x,imgsizeY-marginY,x+barWidth,y(h),colors[2]);

        // Formattage de la grandeur (H ou Q) et des heures
        string dateStr = result["date_obs"].get<string>();
        for (int i = 0; i < 4; ++i) { // On ôte les secondes (":00Z")
            dateStr.pop_back();
        }
        string heure = dateStr.substr(11); // conserve l'heure au format hh:mm
        string resObsStr = to_string(result["resultat_obs"]); // float to string
        if(grandeur_hydro == "H"){
            for (int i = 0; i < 2; ++i) resObsStr.pop_back(); // On ôte le ".0"" la décimale est toujours nulle
        }else if (grandeur_hydro == "Q"){
            for (int i = 0; i < 4; ++i) resObsStr.pop_back(); // On ôte le "00.0"" la décimale est toujours nulle et on passe en m³/s
        }
        // Affichage des hauteurs (en mm) puis des heures (hh:mm)
        int lengthH = resObsStr.length();
        gdImageString(im,gdFontGetGiant (),x+(27-lengthH*5),y(h)-16,(unsigned char*) resObsStr.c_str(),colors[1]);
        gdImageString(im,gdFontGetGiant (),x+5,imgsizeY-marginY+5,(unsigned char*) heure.c_str(),colors[1]);
        i++;
    }
    // Axe (abcisse et ordonnée)
    gdImageLine(im,marginX,imgsizeY-marginY,imgsizeX-marginXright,imgsizeY-marginY,colors[1]); // abcisse
    gdImageLine(im,marginX,imgsizeY-marginY*0.5,marginX,marginY,colors[1]); // ordonnée
    gdImageLine(im,imgsizeX-marginX,imgsizeY-marginY,imgsizeX-marginXright-8,imgsizeY-marginY-6,colors[1]); // trait flèche abcisse 1
    gdImageLine(im,imgsizeX-marginX,imgsizeY-marginY,imgsizeX-marginXright-8,imgsizeY-marginY+6,colors[1]); // trait flèche abcisse 2
    gdImageLine(im,marginX,marginY,marginX-6,marginY+8,colors[1]); // trait flèche ordonnée 1
    gdImageLine(im,marginX,marginY,marginX+6,marginY+8,colors[1]); // trait flèche ordonnée 2

    // Ligne d'échelle min et max
    int yMin =y(scaleMin);
    int yMax =y(scaleMax);

    gdImageLine(im,marginX-5,yMin,marginX+5,yMin,colors[1]);
    gdImageLine(im,marginX-5,yMax,marginX+5,yMax,colors[1]);

    // Affichage des valeurs min et max de l'échelle
    unsigned char* scaleMinUc = floatToUcEtoile(scaleMin);
    int lengthScaleMin = strlen((char*) scaleMinUc);
    gdImageString(im,gdFontGetGiant(), marginX-lengthScaleMin*15, yMin-10 , scaleMinUc, colors[1]);

    unsigned char* scaleMaxUc = floatToUcEtoile(scaleMax);
    int lengthScaleMax = strlen((char*) scaleMaxUc);
    gdImageString(im,gdFontGetGiant(), marginX-lengthScaleMax*15, yMax-10 , scaleMaxUc, colors[1]);

    // Affichage du titre
    boost::replace_all(libelle_site,"à","a");
    boost::replace_all(libelle_site,"é","e");
    boost::replace_all(libelle_site,"è","e");
    boost::replace_all(libelle_site,"ê","e");
    boost::replace_all(libelle_site,"ô","o");
    unsigned char* titre = (unsigned char*) libelle_site.c_str();
    int lengthTitre = strlen((char*) titre);
    gdImageString(im,gdFontGetGiant(), imgsizeX/2-lengthTitre*5, marginY/2, titre , colors[1]);

    unsigned char* grandeurDesc;
    if(grandeur_hydro == "H"){
        grandeurDesc = (unsigned char*) "Hauteur H en mm";
    }else if(grandeur_hydro == "Q"){
        grandeurDesc = (unsigned char*) "Debit Q en m3/s";
    }
    int lengthGrandeurDesc = strlen((char*) grandeurDesc);
    gdImageString(im,gdFontGetGiant(), imgsizeX/2-lengthGrandeurDesc*5, marginY/3, grandeurDesc , colors[1]);

    // Affichage de la date
    string date_jour = date_obs.substr (0,10);

    gdImageString(im,gdFontGetGiant(), imgsizeX/2-date_jour.length()*6, imgsizeY-marginY/2, (unsigned char*) date_jour.c_str(), colors[1]);
}

/**
 * @brief Graphe::dessinGraphe2. Top 10 de H ou Q du jour à une heure donnée
 * @param jDataObs
 * @param im
 * @param colors
 */
void Graphe::dessinGraphe2(json jDataAllVal, gdImagePtr im, vector<int> colors)
{
    int i = 0;
    for (json &j : jDataAllVal){
        h = j["resultat_obs"];
        if(grandeur_hydro == "Q") h=h/100; // conversion en m³/s

        int x = marginX+(barWidth+spacing)*i;
        gdImageFilledRectangle(im,x,imgsizeY-marginY,x+barWidth,y(h),colors[3]);

        // Affichage des valeurs au-dessus des barres et du numéro en abcisse
        string valStr = to_string(h); // int to string

        int lengthH = valStr.length();
        int lengthNum = to_string(i+1).length();
        gdImageString(im,gdFontGetGiant (),x+(27-lengthH*5),y(h)-16,(unsigned char*) valStr.c_str(),colors[1]);
        gdImageString(im,gdFontGetGiant (),x+barWidth/2-lengthNum*5,imgsizeY-marginY+5,(unsigned char*) to_string(i+1).c_str(),colors[1]);

        // Libellé d'un site pour un code station donné
        Requete r;
        string reqLibelleSiteByCode;
        if(grandeur_hydro == "H"){
            reqLibelleSiteByCode = r.request(r.urlBase + "referentiel/stations?code_station=" + j["code_station"].get<string>() + "&fields=libelle_site");
        }else{
            reqLibelleSiteByCode = r.request(r.urlBase + "referentiel/sites?code_site=" + j["code_site"].get<string>() + "&fields=libelle_site");
        }
        json jStations = json::parse(reqLibelleSiteByCode);
        json &jDataStations = jStations["data"];

        // Affichage du libellé
        string libelle = to_string(i+1) + " " + jDataStations[0]["libelle_site"].get<string>();

        // La fonction gdImageString n'affiche pas correctement les caractères spéciaux, donc on les remplace
        boost::replace_all(libelle,"à","a");
        boost::replace_all(libelle,"é","e");
        boost::replace_all(libelle,"è","e");
        boost::replace_all(libelle,"ê","e");
        boost::replace_all(libelle,"ô","o");
        gdImageString(im,gdFontGetGiant(),imgsizeX-marginXright,marginY+20*i,(unsigned char*) libelle.c_str(),colors[1]);
        i++;
    }

//    const char* fontlist = "Arial;Segoe UI";
//    int brect[8] = {0,0,50,0,50,20,0,20};
//    const char* text = "Hello, it's me.";
//    gdImageStringFT(im,brect,colors[1],fontlist,12.0,0.0,imgsizeX-marginXright,marginY+20*i,text);

    // Axe (abcisse et ordonnée)
    gdImageLine(im,marginX,imgsizeY-marginY,imgsizeX-marginXright,imgsizeY-marginY,colors[1]); // abcisse
    gdImageLine(im,marginX,imgsizeY-marginY*0.5,marginX,marginY,colors[1]); // ordonnée
    gdImageLine(im,imgsizeX-marginXright,imgsizeY-marginY,imgsizeX-marginXright-8,imgsizeY-marginY-6,colors[1]); // trait flèche abcisse 1
    gdImageLine(im,imgsizeX-marginXright,imgsizeY-marginY,imgsizeX-marginXright-8,imgsizeY-marginY+6,colors[1]); // trait flèche abcisse 2
    gdImageLine(im,marginX,marginY,marginX-6,marginY+8,colors[1]); // trait flèche ordonnée 1
    gdImageLine(im,marginX,marginY,marginX+6,marginY+8,colors[1]); // trait flèche ordonnée 2

    // Ligne d'échelle min et max
    int yMin =y(scaleMin);
    int yMax =y(scaleMax);

    gdImageLine(im,marginX-5,yMin,marginX+5,yMin,colors[1]);
    gdImageLine(im,marginX-5,yMax,marginX+5,yMax,colors[1]);

    // Affichage des valeurs min et max de l'échelle
    unsigned char* scaleMinUc = floatToUcEtoile(scaleMin);
    int lengthScaleMin = strlen((char*) scaleMinUc);
    gdImageString(im,gdFontGetGiant(), marginX-lengthScaleMin*15, yMin-10 , scaleMinUc, colors[1]);

    unsigned char* scaleMaxUc = floatToUcEtoile(scaleMax);
    int lengthScaleMax = strlen((char*) scaleMaxUc);
    gdImageString(im,gdFontGetGiant(), marginX-lengthScaleMax*15, yMax-10 , scaleMaxUc, colors[1]);

    // Affichage du titre
//    unsigned char* titre = (unsigned char*) libelle_site.c_str();
//    int lengthTitre = strlen((char*) titre);
//    gdImageString(im,gdFontGetGiant(), imgsizeX/2-lengthTitre*5, marginY/2, titre , colors[1]);

    unsigned char* grandeurDesc;
    if(grandeur_hydro == "H"){
        grandeurDesc = (unsigned char*) "Top 10 des hauteurs H en mm";
    }else if(grandeur_hydro == "Q"){
        grandeurDesc = (unsigned char*) "Top 10 des debit Q en m3/s";
    }
    int lengthGrandeurDesc = strlen((char*) grandeurDesc);
    gdImageString(im,gdFontGetGiant(), imgsizeX/2-lengthGrandeurDesc*5, marginY/3, grandeurDesc , colors[1]);

    // Affichage de la date
    string dateJour = date_obs.substr (0,10);
    string heure = date_obs.substr (11,5);
    string dateComplete = "le " + dateJour + " a "  + heure;

    gdImageString(im,gdFontGetGiant(), imgsizeX/2-dateComplete.length()*6, imgsizeY-marginY/2, (unsigned char*) dateComplete.c_str(), colors[1]);
}

/**
 * @brief Graphe::makeScale. On détermine les valeurs mini et maxi, hMin, hMax, l'écart et le coeff. Valeurs utiles à la fonction y().
 * @param jDataObs
 */
void Graphe::makeScale(json jDataObs)
{
    // la requête sort dans l'ordre chronologique descendant, donc on inverse avec l'adaptateur reverse de la librairie Boost
    for (json &result : boost::adaptors::reverse(jDataObs)){
        h = static_cast<int>(result["resultat_obs"]);
        if(grandeur_hydro == "Q") h=h/100; // conversion en m³/s
        if(h > hMax){
            hMax = h;
        }
        if(hMin == 0){
            hMin = h;
        }else if(h <  hMin){
            hMin = h;
        }
    }
    ecart = hMax - hMin;

    if(ecart < 1){
        coeff = 20;
        ecart = 50;
    }else{
        coeff = 300.0f/ecart; // l'écart entre valeur min et valeur max s'affiche sur 300 px.
    }

    // On calcule les valeurs d'échelle min et max, respectivement scaleMin et scaleMax
    float hMinF = hMin;
    float hMaxF = hMax;
    hMinF = hMinF/100;
    hMaxF = hMaxF/100;

    // ScaleMin, valeur d'échellle min
    if(round(hMinF) > floor(hMinF)){
        scaleMin = (floor(hMinF)+0.5)*100;
    } else if(round(hMinF) == floor(hMinF)){
        scaleMin = floor(hMinF)*100;
    }
    if(y(scaleMin) > imgsizeY){ // Si avec la valeur minimale arrondie on est hors cadre, on n'arrondi pas
        scaleMin = hMin;
    }

    // ScaleMax, valeur d'échellle min. Arrrondie à 100, 50 ou 25 ou une valeur exacte si on est hors cadre.
    if(round(hMaxF) != floor(hMinF)){
        scaleMax = round(hMaxF)*100;
    }else{
        if(ecart > 25 && ecart > 50){
            scaleMax = scaleMin+50;
        }else if(ecart > 12 && ecart < 25){
            scaleMax = scaleMin+25;
        }else if (ecart < 12) {
            scaleMax = hMax;
        }
    }
    if(y(scaleMax) < marginY){ // Si avec la valeur maximale arrondie on est hors cadre, on n'arrondi pas
        scaleMax = hMax;
    }
}

/**
 * @brief Graphe::toString. Affichage console des attributs
 */
void Graphe::toString()
{
    cout << "--- toString début -----------------" << endl;
    cout << "imgsizeX = " << imgsizeX << endl;
    cout << "imgsizeY = " << imgsizeY << endl;
    cout << "barWidth = " << barWidth << endl;
    cout << "marginX = " << marginX << endl;
    cout << "marginY = " << marginY << endl;
    cout << "spacing = " << spacing << endl;
    cout << "h = " << h << endl;
    cout << "hMax = " << hMax << endl;
    cout << "hMin = " << hMin << endl;
    cout << "ecart = " << ecart << endl;
    cout << "coeff = " << coeff << endl;
    cout << "scaleMin = " << scaleMin << endl;
    cout << "scaleMax = " << scaleMax << endl;
    cout << "grandeur_hydro = " << grandeur_hydro << endl;
    cout << "--- toString fin -----------------" << endl;
}
