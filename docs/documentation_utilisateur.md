# Documentation utilisateur Hydrométrie
Voir également les autres documents disponibles : 
- [Documentation de codage](documentation_de_codage.md)
- [README](README.md)

### Généralités
Les type des graphiques :
##### Graphique d’évolution (graphe 1)
- Histogramme des 10 dernières valeurs du débit Q pour une station donnée.
- Histogramme des 10 dernières valeurs de la hauteur d'eau H pour une station donnée.
##### Graphiques d’état (graphe 2)
- Graphe des 10 plus grandes hauteurs d'eau H à une date et heure donnée.
- Graphe des 10 plus grands débits Q à à une date et heure donnée.

Le programme créer deux images au format png :
1. la première image représente le graphe 1 avec comme nom de fichier `grandeurHydro_codeEntite_dateEtHeure.png`. Exemples : `H_F700000103_2023-09-20T08:00:00Z.png` ou encore `Q_F700000103_2023-10-13T12:30:00Z.png`
2. La deuxième image représente le graphe 2 avec comme nom de fichier `Top10_grandeurHydro_dateEtHeure.png`. Exemple : `Top10_Q_2023-09-22T08:57.png`

### Compilation
*tbd*

### Lancement
*tbd*
Condition sur les arguments : date de moins d'un mois

Commande de lancement : `./hydroarm H H208000104`

 Avec 
	- `grandeurHydro` la grandeur choisie, soit le débit "Q" soir la hauteur d'eau "H".
	- `codeEntite` Code unique identifiant la station hydrométrique
