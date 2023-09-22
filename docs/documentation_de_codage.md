# Documentation de codage
Voir également les autres documents disponibles : 
- [Documentation utilisateur](documentation_utilisateur.md)
- [README](https://github.com/BriceTouchard/TPOpenData/blob/main/README.md)

## Sommaire
- [Périmètre du TP](#Périmètre-du-TP)
- [Open data Hydrométrie Hub'eau](#Open-data-Hydrométrie-Hub'eau)
- [Composants](#Composants)
- [Architecture du code](#Architecture-du-code)
- [Principe des requêtes](#Principe-des-requêtes)
- [Affichage des graphes](#Affichage-des-graphes)
- [Compilation](#Compilation)
- [Lancement](#Lancement)
- [Jeu de données](Jeu-de-données)

## Périmètre du TP
L'objectif était de créer un programme en C++ qui génère des images de graphes représentant des données régulièrement mise à jour d'une opendata choisie au préalable. Le programme doit pouvoir fonctionner sur Linux et sur une configuration Linux embarquée Buildroot/Qemu. 
Le choix s'est porté sur les données hydrométrique de Hub'Eau.

## Open data Hydrométrie Hub'eau
### Ressources 
Site des données de l'API hydrométrie :
[Hydrométrie | Hubeau](https://hubeau.eaufrance.fr/page/api-hydrometrie)

Le visualiseur permet de chercher un site et de trouver son code station [Démonstrateur Hydrométrie Temps Réel Hub'Eau](https://hubeau.eaufrance.fr/sites/default/files/api/demo/hydro_tr.htm)

### Présentation 
Les données de l'API "Hydrométrie" sont issues de la plate-forme HYDRO Centrale (PHyC), opérée par le Service Central d’Hydrométéorologie et d’Appui à la Prévision des Inondations (SCHAPI).

Les données diffusées par Hub'Eau sont les mesures quasi temps-réel provenant du réseau de mesure français (environ 3000 stations hydrométriques) opéré par les Directions Régionales de l’Environnement de l’Aménagement et du Logement (DREAL) et autres producteurs (collectivités, etc.), ainsi que deux types d'observations élaborées (débits moyens journaliers et débits moyens mensuels).

### API
L'API permet d'interroger le référentiel hydrométrique (sites et stations d'observations du réseau français de mesures) ainsi que les observations de hauteur d'eau (H) et de débit (Q) dites "temps réel", bancarisées toutes les 5 à 60 minutes dans la plateforme PHyC.
L'API est mise à jour à partir de la plateforme PHyC toutes les 2 minutes et maintient un historique d'un mois.

L'API permet de réaliser différents types de requêtes HTTP GET. Ci-dessous la liste des actions possibles et l'URL partielle correspondante associée à concaténer à l'url de base, `https://hubeau.eaufrance.fr/api`. 
- Lister les observations hydrométriques ave `/v1/hydrometrie/observations_tr`
- Lister les sites hydrométriques `/v1/hydrometrie/referentiel/sites`
- Lister les stations hydrométriques `/v1/hydrometrie/referentiel/stations`
- Lister les observations hydrométriques élaborées avec `/v1/hydrometrie/obs_elab`

La liste de observations élaborée (`obs_elab`) n'a pas été utilisée pour le projet, les informations accessibles via `/observation_tr` étant suffisantes.

#### Exemples 
- Liste de toutes les valeurs observées : https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr
- Liste de toutes les stations hydrométriques :  https://hubeau.eaufrance.fr/api/v1/hydrometrie/referentiel/stations
- Liste de toutes le sites hydrométriques  https://hubeau.eaufrance.fr/api/v1/hydrometrie/referentiel/stations
- 10 dernières valeurs du débit Q pour la station K490003010 (La Loire à Tours)
	https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr?code_entite=K490003010&size=10&grandeur_hydro=Q&fields=date_obs,resultat_obs
- Libelle du site pour la station O972001001 (la Garonne à Bordeaux)
	https://hubeau.eaufrance.fr/api/v1/hydrometrie/referentiel/stations?code_station=O972001001&fields=libelle_site
- Hauteurs observé à un instant t (2023-09-20 à 8h00) pour les stations dont le code stations commence par "H"
	https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr?code_entite=H*&grandeur_hydro=H&fields=resultat_obs,code_station&date_debut_obs=2023-09-20T08:00:00Z&date_fin_obs=2023-09-20T08:00:00Z

#### Paramètres utilisés dans les requêtes
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
## Composants
Ci-dessous la liste des libraries utilisées, à activer dans la configuration de Buildroot préalablement au `make`.
### libcurl 
Une librairie de transfert de fichier multiprotocol, wrappée par curlpp. Doc : https://curl.se/libcurl/
### curlpp
Un wrapper de libcurl qui permet de créer des requêtes http simplement. Doc : http://www.curlpp.org/
### Json for modern C++ (JSON nlohmann)
Permet de faciliter la manipulation des données JSON. Doc : https://json.nlohmann.me/
### LibGD 
Pour la création de graphes au format image. Doc : https://libgd.github.io/ 
### Boost
Référentiel complémentaire à la bibliothèque standard C++. 
Utilisation de l'adaptator reverse, de ptime et de replace_all.

## Architecture du code
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

## Principe des requêtes
### Requêtes pour le graphe 1 
Une première requête est effectuée pour récupérer les 10 dernière valeurs de Q ou H ainsi que le code station associé. Pour ce faire on utilise le paramètres `size`/
Exemple :

La seconde requête récupère le libelle du site à l'aide du code station dans le référentiel stations.
### Requêtes pour le graphe 2
La réponse de la requête pour sélectionner toutes les valeurs de H ou Q à un instant t étant trop grande, elle renvoie un code "*206 partial content*". 
Exemple pour le débit Q  à 8h00 le 22 septembre 2023. Notez que seules les données de moins d'un mois sont conservées, veillez à modifier la date en conséquence.
`https://hubeau.eaufrance.fr/api/v1/hydrometrie/observations_tr?grandeur_hydro=Q&date_debut_obs=2023-09-22T08:00&date_fin_obs=2023-09-22T08:00`

On effectue alors une requêtes pour tous les codes stations commençant par chaque lettre de l'alphabet à l'exception de C et T, autrement dit 24 requêtes. Puis on concatène le tout dans un seul json avec la fonction `nlohmann::basic_json::push_back` au fonctionnement similaire de la fonction `std::vector::push_back`  des vecteurs.


## Affichage des graphes
### Dimensions
```C++
int imgsizeX;
int imgsizeY;
int barWidth;
int marginX;
int marginXright;
int marginY;
int spacing;
```
On définit les dimensions utilisées comme montré sur le schéma [schema_dimensions.png](schema_dimensions.png). Ce sont les attributs de la classe Graphe, que l'on peut indiquer dans le constructeur lors de l'instanciation. Exemple : `Graphe g1(800,600,50,100,100,100,10);`
### Echelle
L'écart entre les valeurs minimales et maximale est représenté dynamiquement sur 300 pixels, afin que la représentation graphique des variations de valeurs soit clairement visible.

## Compilation
### Compilation en ligne de commande 
Pour compiler le programme pour Intel (Linux)
`g++ -o hydrointel main.cpp graphe.cpp requete.cpp -lcurl -lcurlpp -lgd
`
Pour compiler le programme pour ARM, sur une configuration Buildroot 2023.08
`/buildroot-2023.08/output/host/bin/aarch64-buildroot-linux-gnu-g++ -o hydroarm main.cpp graphe.cpp requete.cpp -lcurl -lcurlpp -lgd`
## Lancement
### Lancement en ligne de commande 
Il y a trois arguments que l'on peut passer en ligne de commande :
1. La grandeur que l'on souhaite `H` ou `Q` (obligatoire)
2. Le code station, par exemple `F700000103` (obligatoire)
3. La date et heure souhaitée, par exemple `2023-09-22T08:00`. La date ne doit pas être antérieure à plus d'un mois de la date du jour.  Cet argument est facultatif, si aucune date n'est rentrée, la date par défaut est celle d'aujourdhui, il y a trois heure.

Exemple : `./hydroarm H F700000103 2023-09-22T08:00`

## Jeu de données
### Code stations
Liste non-exhaustive, il y a 5810 stations dans le référentiel. Le [visualiseur Hub'Eau](https://hubeau.eaufrance.fr/sites/default/files/api/demo/hydro_tr.htm) permet de chercher un site avec son adresse et de trouver son code station. 

| Libelle site                           |    Code    | Region | Département | Périodicité |
|----------------------------------------|------------|--------|-------------|-------------|
|L'Oise à Creil et à Pont-Sainte-Maxence | H208000104 |   32   |     60      |   20 min    |
| La Seine à Paris			 | F700000103 |   11   |     75      |   10 min    |
| La Loire à Tours 			 | K490003010 |   24   |     37      |   10 min    |
| La Garonne à Bordeaux 		 | O972001001 |   75   |     33      |    5 min    |


