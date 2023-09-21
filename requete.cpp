#include "requete.h"

#include <iostream>
#include <sstream>
#include <string>
#include <curl/curl.h>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

using namespace std;
using namespace curlpp::options;
using json = nlohmann::json;
using namespace nlohmann::literals;

Requete::Requete()
{
    urlBase = "https://hubeau.eaufrance.fr/api/v1/hydrometrie/";
}

/**
 * @brief Requete::request Effectue une requête HTTP get
 * @param url
 * @return le résultat de la requête de type string
 */
string Requete::request(string url)
{
    string respStr;
    try
    {
        curlpp::Cleanup myCleanup;
        curlpp::Easy myRequest;
        myRequest.setOpt<Url>(url);
        myRequest.setOpt<SslVerifyPeer>(false);
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

/**
 * @brief Requete::requestData. Fait une requête et en extrait les données (data) au format json
 * @param url
 * @return jData, les donnéees de type json
 */
json Requete::requestData(string url)
{
        string req = request(url);
        json jReq = json::parse(req);
        json &jData = jReq["data"];
        return jData;
}
