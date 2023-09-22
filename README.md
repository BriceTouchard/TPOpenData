# Readme TPOpenData

## Infos
Deux autres documentations sont disponibles :
- [Documentation utilisateur](https://github.com/BriceTouchard/TPOpenData/blob/main/docs/documentation_utilisateur.md)
- [Documentation de codage](https://github.com/BriceTouchard/TPOpenData/blob/main/docs/documentation_de_codage.md)

Il y a deux fichiers exécutables :
- hydrointel est l'exécutable pour Linux 
- hydroarm est l'exécutable pour Buildroot

## Définition du projet
- OpenData hydrométrie : https://hubeau.eaufrance.fr/page/api-hydrometrie
- Chaque stagiaire devra décrire son projet :
	- En présentant les composants logiciels retenus qui devront être C/C++
	- Les différents états qu’il compte fournir
	- Le projet devra fonctionner sous Linux en mode bureautique mais également sous Buildroot

## Composants
### Libcurl 
Une librairie de transfert de fichier multiprotocol, wrappée par curlpp. Doc : https://curl.se/libcurl/

### Curlpp
Un wrapper de Libcurl qui permet de créer des requêtes http simplement. Doc : http://www.curlpp.org/

### Json for modern C++ (JSON nlohmann)
Permet de faciliter la manipulation des données JSON. Doc : https://json.nlohmann.me/

### LibGD 
Pour la création de graphes au format image. Doc : https://libgd.github.io/ 

### Boost
Référentiel complémentaire à la bibliothèque standard C++. 
Utilisation de l'adaptator reverse, de ptime et de replace_all.

## Les différents états
### Graphiques d’état
- Graphe des 10 plus grandes hauteurs d'eau H à une date et heure donnée.
- Graphe des 10 plus grands débits Q à à une date et heure donnée.

### Graphique d’évolution
- Histogramme des 10 dernières valeurs du débit Q pour une station donnée.
- Histogramme des 10 dernières valeurs de la hauteur d'eau H pour une station donnée.

## Jeu de données
### Code stations
Liste non-exhaustive, il y a 5810 stations dans le référentiel.

| Libelle site                           |    Code    | Region | Département | Périodicité |
|----------------------------------------|------------|--------|-------------|-------------|
|L'Oise à Creil et à Pont-Sainte-Maxence | H208000104 |   32   |     60      |   20 min    |
| La Seine à Paris			 | F700000103 |   11   |     75      |   10 min    |
| La Loire à Tours 			 | K490003010 |   24   |     37      |   10 min    |
| La Garonne à Bordeaux 		 | O972001001 |   75   |     33      |    5 min    |

