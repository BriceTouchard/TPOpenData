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
#include <boost/range/adaptor/reversed.hpp>
#include <math.h>
#include "graphe.h"
#include "requete.h"
#include "boost/date_time/posix_time/posix_time.hpp"

using namespace std;
using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace curlpp::options;
using namespace boost::posix_time;

int main(int argc, char *argv[])
{
    Requete r;
    Graphe g1(800,600,50,100,100,100,10);
    Graphe g2(1600,600,50,100,850,100,10);

    // Message à l'utilisateur s'il a oublié un ou plusieurs arguments
    if(argc < 3) cout << "Veuillez saisir tous les arguments requis : grandeur souhaitée (H ou Q) et le code station. Exemple : H H208000104" << endl;
    if(argc == 4) g2.date_obs = argv[3];
    g1.grandeur_hydro = argv[1];
    g2.grandeur_hydro = argv[1];
    string code_station = argv[2];

    // --- Requêtes HTTP GET ------
    // 10 dernières valeurs de H ou Q pour une station donné
    json jDataObs = r.requestData(r.urlBase + "observations_tr?code_entite=" + code_station + "&size=10&grandeur_hydro=" + g1.grandeur_hydro + "&fields=date_obs,resultat_obs");

    // Libellé d'un site pour un code station donné
    json jDataLibelleSiteByCode = r.requestData(r.urlBase + "referentiel/stations?code_station=" + code_station + "&fields=libelle_site");

    // On crée un json JDataAllVal dans lequel on push back toutes les valeurs (trop de valeurs pour une seule requête)
    json jDataAllVal;

    if(argc < 4){ // si il n'y a pas de date en argument
        // On prend la date du jour moins 3 heures (délai de maj des opendata)
        boost::posix_time::ptime timeLocal = boost::posix_time::second_clock::local_time();
        string timeLocalstr = to_iso_extended_string(timeLocal-hours(3));

        // On ôte les secondes, ce qui est nécessaire pour la requête
        g2.date_obs = timeLocalstr.substr(0, 16);
     }

    /* On effectue 24 requêtes dont on push back les résultats dans un seul json jDataAllVal pour éviter le code 206.
     * On catégorise les requêtes par la première lettre du code entité, on a donc 24 patterns de recherche
     * (il n'y a pas de code_entite commençant par C ou T).*/
    vector< string > patternsVect = {"A*", "B*", "D*", "E*", "F*", "G*", "H*", "I*", "J*", "K*", "L*", "M*", "*N",
                                     "O*", "P*", "Q*", "R*", "S*", "U*", "V*", "W*", "X*", "Y*", "Z*"};
    for (string pattern : patternsVect) {
        json jDataPattern;
        if(g2.grandeur_hydro == "H"){
            jDataPattern = r.requestData(r.urlBase + "observations_tr?code_entite=" + pattern + "&grandeur_hydro=" + g2.grandeur_hydro + "&fields=resultat_obs,code_station&date_debut_obs=" + g2.date_obs + "&date_fin_obs=" + g2.date_obs);
        }else{ // avec Q les code_station sont souvent null, on utilise alors les code_site
            jDataPattern = r.requestData(r.urlBase + "observations_tr?code_entite=" + pattern + "&grandeur_hydro=" + g2.grandeur_hydro + "&fields=resultat_obs,code_site&date_debut_obs=" + g2.date_obs + "&date_fin_obs=" + g2.date_obs);
        }
        for (int i = 0; i < jDataPattern.size() ; ++i) {
            jDataAllVal.push_back(jDataPattern[i]);
        }
    }
    // Classement dans l'ordre décroissant des résultats observés (resultat_obs)
    // avec une expression lambda comme comparateur
    sort(jDataAllVal.begin(), jDataAllVal.end(), [](auto &left, auto &right) {
        return left["resultat_obs"] > right["resultat_obs"] ;
    });

    // On conserve le top 10 en supprimant toutes les autres valeurs
    jDataAllVal.erase(jDataAllVal.begin()+10, jDataAllVal.end());

    if(jDataObs.empty()){
        cout << endl;
        cout << "Pas de données pour la grandeur hydrométrique sélectionnée. Veuillez choisir une autre station ou une autre grandeur." <<  endl;
    }
    // Extraction de la date d'observation et du libellé du site
    g1.date_obs = jDataObs[0]["date_obs"];
    g1.libelle_site = jDataLibelleSiteByCode[0]["libelle_site"];

    // Création du nom de fichier (fileName1). Exemple : H_H208000104_2023-09-19T08:10:00Z.png
    string fileName1Str = g1.grandeur_hydro + "_"  + code_station + "_" + g1.date_obs + ".png";
    const char* fileName1 = fileName1Str.c_str();
    string fileName2Str = "Top10_" + g2.grandeur_hydro + "_" + g2.date_obs + ".png";
    const char* fileName2 = fileName2Str.c_str();

    // On crée les échelles pour les différents graphes
    g1.makeScale(jDataObs);
    g2.makeScale(jDataAllVal);

    // -- Libgd -----------
    gdImagePtr im1;
    gdImagePtr im2;
    FILE *pngout1;
    FILE *pngout2;
    im1 = gdImageCreate(g1.imgsizeX, g1.imgsizeY);
    im2 = gdImageCreate(g2.imgsizeX, g2.imgsizeY);

    int bckgrd, noir, bleu, vert;
    bckgrd = gdImageColorAllocate(im1, 240, 240, 255);
    noir =   gdImageColorAllocate(im1, 0, 0, 0);
    bleu =    gdImageColorAllocate(im1, 50, 100, 255);
    vert =    gdImageColorAllocate(im1, 106, 204, 96);
    bckgrd = gdImageColorAllocate(im2, 240, 240, 255);
    noir =   gdImageColorAllocate(im2, 0, 0, 0);
    bleu =    gdImageColorAllocate(im2, 50, 100, 255);
    vert =    gdImageColorAllocate(im2, 106, 204, 96);
    int colors[4] = {bckgrd, noir, bleu, vert};

    vector<int> colorsList = {bckgrd, noir, bleu, vert};

    gdFontPtr fonts[5];
    fonts[0] = gdFontGetTiny ();
    fonts[1] = gdFontGetSmall ();
    fonts[2] = gdFontGetMediumBold ();
    fonts[3] = gdFontGetLarge ();
    fonts[4] = gdFontGetGiant ();

    // Dessin des graphes
    g1.dessinHistogramme(jDataObs, im1, colorsList);
    g2.dessinGraphe2(jDataAllVal, im2, colorsList);

    // Création des fichiers
    pngout1 = fopen(fileName1, "wb");
    gdImagePng(im1, pngout1);
    fclose(pngout1);
    gdImageDestroy(im1);

    pngout2 = fopen(fileName2, "wb");
    gdImagePng(im2, pngout2);
    fclose(pngout2);
    gdImageDestroy(im2);

    return 0;
}
