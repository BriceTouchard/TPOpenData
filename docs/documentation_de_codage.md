# Documentation de codage projet Linux embarqué hydrométrie AJC
Voir également les autres documents disponibles : 
- [Documentation utilisateur](documentation_utilisateur.md)
- [README](README.md)

### Périmètre du projet
L'objectif était de créer un programme en C++ qui génère des images de graphes représentant des données régulièrement mise à jour d'une opendata choisie au préalable.

### Données : open data Hydrométrie Hub'eau
#### Ressources 
Site des données de l'API hydrométrie :
[Hydrométrie | Hubeau](https://hubeau.eaufrance.fr/page/api-hydrometrie)
Le visualiseur permet de chercher un site et de trouver son code station [Démonstrateur Hydrométrie Temps Réel Hub'Eau](https://hubeau.eaufrance.fr/sites/default/files/api/demo/hydro_tr.htm)

#### Présentation 
Les données de l'API "Hydrométrie" sont issues de la plate-forme HYDRO Centrale (PHyC), opérée par le Service Central d’Hydrométéorologie et d’Appui à la Prévision des Inondations (SCHAPI).

Les données diffusées par Hub'Eau sont les mesures quasi temps-réel provenant du réseau de mesure français (environ 3000 stations hydrométriques) opéré par les Directions Régionales de l’Environnement de l’Aménagement et du Logement (DREAL) et autres producteurs (collectivités, etc.), ainsi que deux types d'observations élaborées (débits moyens journaliers et débits moyens mensuels).

L'API Hub'Eau - hydrométrie permet de réaliser différents types de requêtes HTTP GET. Ci-dessous la liste des actions possible et l'URL partielle correspondante à concaténer à l'url de base (`https://hubeau.eaufrance.fr/api`). 
- Lister les observations hydrométriques ave `/v1/hydrometrie/observations_tr`
- Lister les sites hydrométriques `/v1/hydrometrie/referentiel/sites`
- Lister les stations hydrométriques `/v1/hydrometrie/referentiel/stations`
- Lister les observations hydrométriques élaborées avec `/v1/hydrometrie/obs_elab`

La liste de observations élaborée (`obs_elab`) n'a pas été utilisée pour le projet, les informations accessibles via `/observation_tr` étant suffisantes.

##### Exemples 
- Liste de toutes les valeurs observées : https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr
- Liste de toutes les stations hydrométriques :  https://hubeau.eaufrance.fr/api/v1/hydrometrie/referentiel/stations
- Liste de toutes le sites hydrométriques  https://hubeau.eaufrance.fr/api/v1/hydrometrie/referentiel/stations
- 10 dernières valeurs du débit Q pour la station K490003010 (La Loire à Tours)
	https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr?code_entite=K490003010&size=10&grandeur_hydro=Q&fields=date_obs,resultat_obs
- Libelle du site pour la station O972001001 (la Garonne à Bordeaux)
	https://hubeau.eaufrance.fr/api/v1/hydrometrie/referentiel/stations?code_station=O972001001&fields=libelle_site
- Hauteurs observé à un instant t (2023-09-20 à 8h00) pour les stations dont le code stations commence par "H"
	https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr?code_entite=H*&grandeur_hydro=H&fields=resultat_obs,code_station&date_debut_obs=2023-09-20T08:00:00Z&date_fin_obs=2023-09-20T08:00:00Z

##### Paramètres utilisés dans les réquêtes
`/Observations_tr`
- `code_entite` : Code unique identifiant la station hydrométrique, correspond au `code_station` du référentiel des stations.
- `date_debut_obs` et `date_fin_obs` : définis un fourchette pour la `date_obs` souhaitée.
- `grandeur_hydro` : "H" pour la hauteur d'eau ou "Q" pour le débit.

`/sites`
- `code_site` 
- `libelle_site` libellé du site avec le nom du cours d'eau et la ville.

`/stations`
- `code_stations` : Code unique identifiant la station hydrométrique, corresponds au `code_entite` des observations.
- `libelle_stations` libellé de la station, différent du libellé du site s'il y a plusieurs stations sur un même site.
- `libelle_site` libellé du site avec le nom du cours d'eau et la ville.

Valables pour toutes les adresses
- `fields` :  permet de choisir les champs que l'on souhaite avoir dans la réponse.
- `size` : limite la réponse à un nombre d'objets.

Plus de paramètres et plus de détails sont disponibles sur le site [Hydrométrie | Hubeau](https://hubeau.eaufrance.fr/page/api-hydrometrie#console)
#### Composants
Ci-dessous la liste des libraries utilisées :
##### libcurl 
Une librairie de transfert de fichier multiprotocol, wrappée par curlpp. Doc : https://curl.se/libcurl/
##### curlpp
Un wrapper de libcurl qui permet de créer des requêtes http simplement. Doc : http://www.curlpp.org/
##### Json for modern C++ (JSON nlohmann)
Permet de faciliter la manipulation des données JSON. Doc : https://json.nlohmann.me/
##### LibGD 
Pour la création de graphes au format image. Doc : https://libgd.github.io/ 
##### Boost
Référentiel complémentaire à la bibliothèque standard C++. 
Utilisation de l'adaptator reverse, de ptime et de replace_all.

#### Architecture du code
Liste des fichiers 
- main.cpp
- Classe Requete
	- requete.h
	- requete.cpp
- Class graphe : 
	- graphe.h
	- graphe.cpp

La classe requête contient deux méthodes : 
- `string request(string url);` qui effectue une requête avec l'url de type string en entrée, et renvoie la réponse de type string en sortie. 
- `json requestData(string url);`  similaire à la méthode précédente, mais qui en plus d'effectuer une requête, parse le résultat en Json et extrait la partie donnée de la réponse qui est renvoyé avec le type `json` implémenté par JSON for modern C++ (nlohmann).


#### Principe des requêtes
##### Graphe 1 : les 10 dernière valeurs 
*tbd* 24 requêtes.

#### Affichage des graphes
##### Dimensions
```C++
// Dimensions
int imgsizeX;
int imgsizeY;
int barWidth;
int marginX;
int marginXright;
int marginY;
int spacing;
```
On définit les dimensions utilisées comme montré sur le schéma "schema_dimensions.png". Ce sont les attributs de la classe Graphe, que l'on peut indiquer dans le constructeur lors de l'instanciation. Exemple : `Graphe g1(800,600,50,100,100,100,10);`

##### Echelle
*tbd* Comment est défini l'échelle




