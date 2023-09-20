#include <iostream>
#include <nlohmann/json.hpp>
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
#include <math.h>

using namespace std;
using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace curlpp::options;

string urlBase = "https://hubeau.eaufrance.fr/api/v1/hydrometrie/";

// Dimensions du graphe
int imgsizeX = 800;
int imgsizeY = 600;
int barWidth = 50;
int marginX = 100;
int marginY = 100;
int spacing = 10;

int i = 0;
int h, hMax, hMin, ecart;
float coeff;
string grandeur_hydro;

string request(string url){
    string respStr;
    try
    {
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;
        myRequest.setOpt<Url>(url);
        myRequest.setOpt<cURLpp::Options::SslVerifyPeer>(false);
        ostringstream os;
        myRequest.perform();
        os << myRequest;
        respStr = os.str();
    }
    catch(curlpp::RuntimeError & e)
    {
        cout << e.what() << endl;
    }
    catch(curlpp::LogicError & e)
    {
        cout << e.what() << endl;
    }
    return respStr;
}

int y(int val){
    int y = imgsizeY-marginY-(val-hMin+ecart/5)*coeff;
    return y;
}

unsigned char* floatToUcEtoile(float f){
    int i = static_cast<int>(f);
    string str = to_string(i);
    const char* ccEtoile = str.c_str();
    unsigned char* ucEtoile;
    ucEtoile = (unsigned char*) ccEtoile;
    return ucEtoile;
}

void dessinHistogramme(json resultsObs, gdImagePtr im, int* colors){
    for (json &result : boost::adaptors::reverse(resultsObs)){   // la requête sort dans l'ordre anti-chronologique, donc on inverse
        h = static_cast<int>(result["resultat_obs"]);
        if(grandeur_hydro == "Q") h=h/100; // conversion en m³/s
        cout << "Hauteur = " << result["resultat_obs"] << " | Date et heure  = " << result["date_obs"].get<string>() << endl;

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
}

int main(int argc, char *argv[])
{
    // Message à l'utilisateur s'il a oublié un ou plusieurs arguments
    if(argc < 3) cout << "Veuillez saisir tous les arguments requis : grandeur souhaitée (H ou Q) et le code station. Exemple : H H208000104" << endl;

    grandeur_hydro = argv[1];
    string station = argv[2];

    // Requêtes HTTP GET
    // 10 dernières valeurs de H ou Q pour une station donné
    string respStrObs = request(urlBase + "observations_tr?code_entite=" + station + "&size=10&grandeur_hydro=" + grandeur_hydro + "&fields=date_obs,resultat_obs");
    cout << endl;
    // Libellé d'un site pour une station donnée
    string respStrStation = request(urlBase + "referentiel/stations?code_station=" + station + "&fields=libelle_site");

    // Liste des code stations d'une région
    string code_region = "32"; // Hauts-de-France
    string reqCodesByRegion = request(urlBase + "referentiel/stations?fields=code_station&code_region=" + code_region);

    cout << endl;
    cout << "reqCodesByRegion = " << reqCodesByRegion << endl;

    // Libgd
    gdImagePtr im;
    FILE *pngout;

    im = gdImageCreate(imgsizeX, imgsizeY);

    int bckgrd, noir, bleu, vert;
    bckgrd = gdImageColorAllocate(im, 240, 240, 255);
    noir =   gdImageColorAllocate(im, 0, 0, 0);
    bleu =    gdImageColorAllocate(im, 50, 100, 255);
    vert =    gdImageColorAllocate(im, 50, 255, 50);
    int colors[4] = {bckgrd, noir, bleu, vert};

    gdFontPtr fonts[5];
    fonts[0] = gdFontGetTiny ();
    fonts[1] = gdFontGetSmall ();
    fonts[2] = gdFontGetMediumBold ();
    fonts[3] = gdFontGetLarge ();
    fonts[4] = gdFontGetGiant ();

    // Extraction des données (data)
    json jObs = json::parse(respStrObs);
    json jStations = json::parse(respStrStation);
    json jCodesByRegion = json::parse(reqCodesByRegion);

    json &resultsObs = jObs["data"];
    json &resultsStations = jStations["data"];
    json &resCodesByRegion = jCodesByRegion["data"];

    // Get de la dernière valeur de H pour chaque station des hauts-de-france
    cout << endl;
    cout << "code station = " << endl;
    for (json &res : resCodesByRegion) {
//        cout << res["code_station"].get<string>() << ", ";
//        string reqVal = request(urlBase + "observations_tr?code_entite=" + res["code_station"].get<string>() + "&size=1&grandeur_hydro=H&fields=resultat_obs");

    }
    cout << endl;

    if(resultsObs.empty()){
        cout << endl;
        cout << "Pas de données pour la grandeur hydrométrique sélectionnée. Veuillez choisir une autre station ou une autre grandeur." <<  endl;
    }
    // Extraction de la date d'observation
    json jObs0 = resultsObs[0];
    string date_obs = jObs0["date_obs"];

    // Extraction du libellé du site
    json jStation0 = resultsStations[0];
    string libelle_site = jStation0["libelle_site"];

    // Création du nom de fichier (fileName). Exemple : H_H208000104_2023-09-19T08:10:00Z.png
    string fileNameStr = grandeur_hydro + "_"  + station + "_" + date_obs + ".png";
    const char* fileName = fileNameStr.c_str();

    // On détermine les hauteurs minimale et maximale, hMin et hMax, ainsi que l'écart entre les deux
    // la requête sort dans l'ordre chronologique descendant, donc on inverse avec l'adaptateur reverse de la librairie Boost

    for (json &result : boost::adaptors::reverse(resultsObs)){
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
    cout << "hMin = " << hMin << endl;
    ecart = hMax - hMin;
    coeff = 300.0f/ecart; // l'écart entre valeur min et valeur max s'affiche sur 300 px.

    // On calcule les valeurs d'échelle min et max, respectivement scaleMin et scaleMax
    float scaleMin, scaleMax;
    float hMinF = hMin;
    float hMaxF = hMax;
    hMinF = hMinF/100;
    hMaxF = hMaxF/100;

    // ScaleMin
    if(round(hMinF) > floor(hMinF)){
        scaleMin = (floor(hMinF)+0.5)*100;
    } else if(round(hMinF) == floor(hMinF)){
        scaleMin = floor(hMinF)*100;
    }
    cout << "scaleMin = " << scaleMin << endl;
    if(y(scaleMin) > imgsizeY){ // Si avec la valeur minimale arrondie on est hors cadre, on n'arrondi pas
        scaleMin = hMin;
    }
    cout << "scaleMin = " << scaleMin << endl;
    cout << "y(scaleMin) = " << y(scaleMin) << " | imgSizeY = " << imgsizeY << endl;

    // ScaleMax. Valeur arrrondie à 100, 50 ou 25. Si l'écart on affiche une valeur exacte.
    cout << "hMax = " << hMax << endl;

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
    // Dessin du graphe
    dessinHistogramme(resultsObs, im, colors);

    // Axe (abcisse et ordonnée)
    gdImageLine(im,marginX,imgsizeY-marginY,imgsizeX-marginX,imgsizeY-marginY,noir); // abcisse
    gdImageLine(im,marginX,imgsizeY-marginY*0.5,marginX,marginY,noir); // ordonnée
    gdImageLine(im,imgsizeX-marginX,imgsizeY-marginY,imgsizeX-marginX-8,imgsizeY-marginY-6,noir); // trait flèche abcisse 1
    gdImageLine(im,imgsizeX-marginX,imgsizeY-marginY,imgsizeX-marginX-8,imgsizeY-marginY+6,noir); // trait flèche abcisse 2
    gdImageLine(im,marginX,marginY,marginX-6,marginY+8,noir); // trait flèche ordonnée 1
    gdImageLine(im,marginX,marginY,marginX+6,marginY+8,noir); // trait flèche ordonnée 2

    // Ligne d'échelle min et max
    int yMin =y(scaleMin);
    int yMax =y(scaleMax);
    cout << "scaleMin = " << scaleMin << " | scaleMax = " << scaleMax << endl;
    cout << "yMin = " << yMin << " | yMax = " << yMax << endl;

    gdImageLine(im,marginX-5,yMin,marginX+5,yMin,noir);
    gdImageLine(im,marginX-5,yMax,marginX+5,yMax,noir);

    // Affichage des valeurs min et max de l'échelle
    unsigned char* scaleMinUc = floatToUcEtoile(scaleMin);
    int lengthScaleMin = strlen((char*) scaleMinUc);
    gdImageString(im,fonts[4], marginX-lengthScaleMin*15, yMin-10 , scaleMinUc, noir);

    unsigned char* scaleMaxUc = floatToUcEtoile(scaleMax);
    int lengthScaleMax = strlen((char*) scaleMaxUc);
    gdImageString(im,fonts[4], marginX-lengthScaleMax*15, yMax-10 , scaleMaxUc, noir);

    // Affichage du titre
    unsigned char* titre = (unsigned char*) libelle_site.c_str();
    int lengthTitre = strlen((char*) titre);
    gdImageString(im,fonts[4], imgsizeX/2-lengthTitre*5, marginY/2, titre , noir);

    unsigned char* grandeurDesc;
    if(grandeur_hydro == "H"){
        grandeurDesc = (unsigned char*) "Hauteur H en mm";
    }else if(grandeur_hydro == "Q"){
        grandeurDesc = (unsigned char*) "Debit Q en m3/s";
    }
    int lengthGrandeurDesc = strlen((char*) grandeurDesc);
    gdImageString(im,fonts[4], imgsizeX/2-lengthGrandeurDesc*5, marginY/3, grandeurDesc , noir);

    // Affichage de la date
    string date_jour = date_obs.substr (0,10);

    gdImageString(im,fonts[4], imgsizeX/2-date_jour.length()*6, imgsizeY-marginY/2, (unsigned char*) date_jour.c_str(), noir);

    pngout = fopen(fileName, "wb");
    gdImagePng(im, pngout);
    fclose(pngout);
    gdImageDestroy(im);

    return 0;
}
