# Documentation utilisateur
Voir également les autres documents disponibles : 
- [Documentation de codage](documentation_de_codage.md)
- [README](https://github.com/BriceTouchard/TPOpenData/blob/main/README.md)
## Sommaire
- [Présentation](#Présentation)
- [Les types de graphiques](#Les-types-de-graphiques)
- [Compilation](#Compilation)
- [Lancement](#Lancement)
- [Jeu de données](#Jeu-de-données)

## Présentation
Ce programme écrit en C++ génère des images de graphes représentant la hauteur d'eau ou le débit d'une ou de plusieurs stations hydrométriques issues des données ouvertes hydrométrique de Hub'Eau (open data).
### Hub'Eau Hydrométrie
Les données diffusées par Hub'Eau sont les mesures quasi temps-réel provenant du réseau de mesure français (environ 3000 stations hydrométriques) opéré par les Directions Régionales de l’Environnement de l’Aménagement et du Logement (DREAL) et autres producteurs (collectivités, etc.), ainsi que deux types d'observations élaborées (débits moyens journaliers et débits moyens mensuels).

## Les types de graphiques
Le programme génère deux graphes dans deux fichiers différents. Pour chaque graphe on peut choisir de représenter la hauteur d'eau ou le débit. 
### Graphique d’évolution (graphe 1)
- Histogramme des 10 dernières valeurs du débit Q pour une station donnée.
- Histogramme des 10 dernières valeurs de la hauteur d'eau H pour une station donnée.
### Graphiques d’état (graphe 2)
- Graphe des 10 plus grandes hauteurs d'eau H à une date et heure donnée.
- Graphe des 10 plus grands débits Q à à une date et heure donnée.

Le programme créer deux images au format png :
1. la première image représente le graphe 1 avec comme nom de fichier : `grandeurHydro_codeEntite_dateEtHeure.png`. Exemples : `H_F700000103_2023-09-20T08:00:00Z.png` ou encore `Q_F700000103_2023-10-13T12:30:00Z.png`
2. La deuxième image représente le graphe 2 avec comme nom de fichier : `Top10_grandeurHydro_dateEtHeure.png`. Exemple : `Top10_Q_2023-09-22T08:57.png`


## Compilation
Voir [Documentation de codage](documentation_de_codage.md#Compilation)

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

