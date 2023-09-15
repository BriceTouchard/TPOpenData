#include <iostream>
#include <nlohmann/json.hpp>
#include <string>
#include <gd.h>
#include <curl/curl.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

using namespace std;
using json = nlohmann::json;
using namespace nlohmann::literals;
using namespace curlpp::options;

int main(int, char**)
{
//    CURL *curl = curl_easy_init();
//    long response_code;
//    if(curl) {
//        CURLcode res;
//        curl_easy_setopt(curl, CURLOPT_URL, "https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr?code_entite=H208000104&size=1&pretty=&grandeur_hydro=H&fields=date_obs,resultat_obs");
//        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
//        res = curl_easy_perform(curl);
//        if(res == CURLE_OK) {
//            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
//          }
//        curl_easy_cleanup(curl);
//    }
//    cout << endl;
//    cout << "response code =" << response_code << endl;

//    CURLcode res;
//    res = curl_easy_perform(curl);
//    cout << "curl = " << curl << endl;
//    cout << "res = " << res << endl;


    string url = "https://hubeau.eaufrance.fr/api/v1/hydrometrie/";
    string restxt;
    try
    {
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;
        myRequest.setOpt<Url>(url + "observations_tr?code_entite=H208000104&size=1&pretty=&grandeur_hydro=H&fields=date_obs,resultat_obs");
        myRequest.setOpt<cURLpp::Options::SslVerifyPeer>(false);
        ostringstream os;
        myRequest.perform();
        os << myRequest;
        restxt = os.str();
    }
    catch(curlpp::RuntimeError & e)
    {
        cout << e.what() << endl;
    }
    catch(curlpp::LogicError & e)
    {
        cout << e.what() << endl;
    }

    size_t pos = restxt.find("[");
//    if (pos != string::npos){
//        cout << "First occurrence is " <<
//        pos << endl;
//    }
    string dataStr = restxt.substr(pos+2);
    dataStr.erase(dataStr.end()-4,dataStr.end());
    cout << endl;
    cout << "dataStr = " << dataStr << endl;

//    string dataStr = R"({"date_obs":"2023-09-14T13:30:00Z","resultat_obs":750.0})";
    auto j = json::parse(dataStr);
    auto h = j.at("resultat_obs");
    cout << "Resultat obs = " << h << endl;

    // Libgd
    gdImagePtr im;
    FILE *pngout;

    int imgsize = 550;
    int bckgrd, noir, bleu; // Noms des couleurs

    im = gdImageCreate(imgsize, imgsize); //création de l'image

    bckgrd = gdImageColorAllocate(im, 240, 240, 255); // background
    noir =   gdImageColorAllocate(im, 0, 0, 0);
    bleu =    gdImageColorAllocate(im, 10,   50, 255);

    gdImageFilledRectangle(im,0,0,100,100,bleu);

    pngout = fopen("titre", "wb");
    gdImagePng(im, pngout);
    fclose(pngout);
    gdImageDestroy(im);

    return 0;
}
