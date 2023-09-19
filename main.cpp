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

void parse(string respStr, string param, string type){
    json j = json::parse(respStr);
    auto &results = j["data"];
    for(auto &result : results) {
        if(type == "string"){
            cout << param << " = " << result[param].get<string>() << endl;
        }else if(type == "float"){
            cout << param << " = " << result[param] << endl;
        }else{
            cout << param << " = " << result[param] << endl;
        }
    }
}

int y(int imgsizeY, int marginY, int hMin, int hMax, int h, int coeff){
    int ecart = hMax - hMin;
    int y = imgsizeY-marginY-(h-hMin+ecart/3)*coeff;
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

int main(int argc, char *argv[])
{
    // Codes des stations en hauts-de-france (32)
//  urlBase + "referentiel/stations?fields=code_station&code_region=32");

    cout << "argv[1] = " << argv[1] << " | argv[2] = " << argv[2] << endl;

    string grandeur_hydro = argv[1];
    string station = argv[2]; // Creil (Oise) = H208000104

//    string grandeur_hydro = "H";
//    string station = "H208000104"; // Oise
    unsigned char monTitre [] = "Hauteur d'eau de l'Oise a Creil";

//    string station = "F700000103"; // Seine à Paris Austerlitz
//    unsigned char monTitre [] = "Hauteur d'eau de la Seine à Paris Austerlitz";

    // Dernière valeur de H de la station H208000104 à Creil
//    string respStr1 = request(urlBase + "observations_tr?code_entite=H208000104&size=1&pretty=&grandeur_hydro=H&fields=date_obs,resultat_obs");
//    string respStr2 = request(urlBase + "referentiel/stations?fields=code_station&code_region=32");
    string respStr3 = request(urlBase + "observations_tr?code_entite=" + station + "&size=10&grandeur_hydro=" + grandeur_hydro + "&fields=date_obs,resultat_obs");

//    parse(respStr1,"resultat_obs","float");
//    parse(respStr2,"code_station","string");
//    parse(respStr3,"resultat_obs","float");
//    parse(respStr3,"date_obs","string");

    // Libgd
    gdImagePtr im;
    FILE *pngout;

    int bckgrd, noir, bleu, vert;
    int imgsizeX = 800;
    int imgsizeY = 600;
    int barWidth = 50;
    int marginX = 100;
    int marginY = 100;
    int spacing = 10;
    int i = 0;
    im = gdImageCreate(imgsizeX, imgsizeY);

    bckgrd = gdImageColorAllocate(im, 240, 240, 255);
    noir =   gdImageColorAllocate(im, 0, 0, 0);
    bleu =    gdImageColorAllocate(im, 50, 100, 255);
    vert =    gdImageColorAllocate(im, 50, 255, 50);

    gdFontPtr fonts[5];
    fonts[0] = gdFontGetTiny ();
    fonts[1] = gdFontGetSmall ();
    fonts[2] = gdFontGetMediumBold ();
    fonts[3] = gdFontGetLarge ();
    fonts[4] = gdFontGetGiant ();

    json j = json::parse(respStr3);
    json &results = j["data"];
    json j0 = results[0];
    string date_obs = j0["date_obs"];

    string fileNameStr = grandeur_hydro + "_"  + station + "_" + date_obs + ".png";
    const char* fileName = fileNameStr.c_str();

    // On détermine le shauteurs minimale et maximale, hMin et hMax, ainsi que l'écart entre les deux
    // la requête sort dans l'ordre anti-chronologique, donc on inverse
    int hMax, hMin, ecart;
    for (json &result : boost::adaptors::reverse(results)){
        int h = static_cast<int>(result["resultat_obs"]);
        if(h > hMax){
            hMax = h;
        }else if(hMin == 0){
            hMin = h;
        }else if(h <  hMin){
            hMin = h;
        }
    }
    ecart = hMax - hMin;

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

    // ScaleMax. Valeur arrrondie à 1000, 50 ou 25. Si l'écart on affiche une valeur exacte.
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

    int coeff = 300/ecart; // l'écart entre valeur min et valeur max s'affiche sur 300 px.

    // Dessin du graphe
    for (json &result : boost::adaptors::reverse(results)){   // la requête sort dans l'ordre anti-chronologique, donc on inverse
        int h = static_cast<int>(result["resultat_obs"]);
        cout << "Hauteur = " << result["resultat_obs"] << " | Date et heure  = " << result["date_obs"].get<string>() << endl;

        // Dessin des barres de l'histogramme
        int x = marginX+(barWidth+spacing)*i;
        gdImageFilledRectangle(im,x,imgsizeY-marginY,x+barWidth,y(imgsizeY,marginY,hMin,hMax,h,coeff),bleu);

        string dateStr = result["date_obs"].get<string>();
        for (int i = 0; i < 4; ++i) { // On ôte les secondes (":00Z")
            dateStr.pop_back();
        }
        string heure = dateStr.substr(11); // conserve l'heure au format hh:mm
        string resObsStr = to_string(result["resultat_obs"]); // float to string
        for (int i = 0; i < 2; ++i) resObsStr.pop_back(); // On ôte le ".0""  à la fin qui est inutile

        // Affichage des hauteurs en mm puis des heures (hh:mm)
        gdImageString(im,fonts[4],x+14,y(imgsizeY,marginY,hMin,hMax,h,coeff)-16,(unsigned char*) resObsStr.c_str(),noir);
        gdImageString(im,fonts[4],x+5,imgsizeY-marginY+5,(unsigned char*) heure.c_str(),noir);
        i++;
    }

    // Axe (abcisse et ordonnée)
    gdImageLine(im,marginX,imgsizeY-marginY,imgsizeX-marginX,imgsizeY-marginY,noir); // abcisse
    gdImageLine(im,marginX,imgsizeY-marginY*0.5,marginX,marginY,noir); // ordonnée
    gdImageLine(im,imgsizeX-marginX,imgsizeY-marginY,imgsizeX-marginX-8,imgsizeY-marginY-6,noir); // trait flèche abcisse 1
    gdImageLine(im,imgsizeX-marginX,imgsizeY-marginY,imgsizeX-marginX-8,imgsizeY-marginY+6,noir); // trait flèche abcisse 2
    gdImageLine(im,marginX,marginY,marginX-6,marginY+8,noir); // trait flèche ordonnée 1
    gdImageLine(im,marginX,marginY,marginX+6,marginY+8,noir); // trait flèche ordonnée 2

    // Ligne d'échelle min et max
    int yMin =y(imgsizeY,marginY,hMin,hMax,scaleMin,coeff);
    int yMax =y(imgsizeY,marginY,hMin,hMax,scaleMax,coeff);
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
    unsigned char* titre = &monTitre[0];
    int length = strlen((char*) titre);
    gdImageString(im,fonts[4], imgsizeX/2-length*5, marginY/2, titre , noir);

    // Affichage de la date

    string date_jour = date_obs.substr (0,10);

//    unsigned char* titre = &monTitre[0];
//    int length = strlen((char*) titre);
//    unsigned char monTitre [] = "Hauteur d'eau de l'Oise a Creil";

    gdImageString(im,fonts[4], imgsizeX/2-date_jour.length()*6, imgsizeY-marginY/2, (unsigned char*) date_jour.c_str(), noir);

    pngout = fopen(fileName, "wb");
    gdImagePng(im, pngout);
    fclose(pngout);
    gdImageDestroy(im);

    return 0;
}
