#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <gd.h>
#include <curl/curl.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <typeinfo>

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

int main(int, char**)
{
    // Codes des stations en hauts-de-france (32)
//  urlBase + "referentiel/stations?fields=code_station&code_region=32");

    // Dernière valeur de H de la station H208000104 à Creil
//    string respStr1 = request(urlBase + "observations_tr?code_entite=H208000104&size=1&pretty=&grandeur_hydro=H&fields=date_obs,resultat_obs");
//    string respStr2 = request(urlBase + "referentiel/stations?fields=code_station&code_region=32");
    string respStr3 = request(urlBase + "observations_tr?code_entite=H208000104&size=10&grandeur_hydro=H&fields=date_obs,resultat_obs");

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
    bleu =    gdImageColorAllocate(im, 10, 50, 255);
    vert =    gdImageColorAllocate(im, 50, 255, 50);

    json j = json::parse(respStr3);
    auto &results = j["data"];
    for(auto &result : results) {
        int h = static_cast<int>(result["resultat_obs"]);
        cout << "Hauteur = " << result["resultat_obs"] << " | h = " << h/2 << endl;
        int x = marginX+(barWidth+spacing)*i;
        gdImageFilledRectangle(im,x,imgsizeY-marginY,x+barWidth,imgsizeY-marginY-h/2,vert);
        i++;
    }

    pngout = fopen("graphe1", "wb");
    gdImagePng(im, pngout);
    fclose(pngout);
    gdImageDestroy(im);

    return 0;
}
