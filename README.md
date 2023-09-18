# TPOpenData

## Infos
- hydrointel est l'exécutable pour Linux 
- hydroarm est l'exécutable pour Buildroot

## Définition du projet
- OpenData hydrométrie : https://hubeau.eaufrance.fr/page/api-hydrometrie
- Chaque stagiaire devra décrire son projet :
	- En présentant les composants logiciels retenus qui devront être C/C++
	- Les différents états qu’il compte fournir
	- Le projet devra fonctionner sous Linux en mode bureautique mais également sous Buildroot

## Composants
### libcurl 
Une librairie de transfert de fichier multiprotocol, wrappée par curlpp.

### curlpp
Un wrapper de libcurl qui permet de créer des requêtes http simplement. Doc : http://www.curlpp.org/

### Json for modern C++ (JSON nlohmann)
Permet de faciliter la manipulation des données JSON. Doc : https://json.nlohmann.me/

### LibGD 
Pour la création de graphes au format image. Doc : https://libgd.github.io/ 

### Boost
Référentiel complémentaire à la bibliothèque standard C++. Utilisation de l'adaptator reverse.

## Les différents états
On note Q le débit et H la hauteur d'eau mesurés.

Graphiques d’état
- Affichage de Q et H pour un cours d'eau
- Comparaison de plusieurs cours d'eau
- 10 plus grandes hauteurs d'eau à l'instant t
- 10 plus grands débits à l'instant t

Graphique d’évolution
- Courbe d'évolution sur 1 mois de Q et H d’un ou plusieurs cours d'eau (ou histogramme)

Graphiques statistiques en France, par régions ou par départements
- 10 plus grandes et 10 plus petites hauteurs d'eau moyennes mensuelles, hebdomadaires et journalières (barres)
- 10 plus grands et 10 plus petites débits moyens mensuels, hebdomadaires et journalier (barres)
- Idem pour les 10 plus petits
- Camembert : pourcentage du débit total de chaque fleuve. Par exemple 10% Loire, 7% Seine (…) 12% autres.
- Affichage de Q et H moyen par région et département (affichage simultanée de toutes les régions ou départements)

