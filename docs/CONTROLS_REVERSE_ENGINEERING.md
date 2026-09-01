# Controls Reverse Engineering

## Objectif

Transformer les commandes Renault en événements fiables pour Linux/LIVI sans dégrader les pièces d’origine.

## Workflow commun

1. identifier la référence exacte ;
2. photographier ;
3. mesurer sans alimentation ;
4. identifier alimentation/masse si électronique ;
5. documenter chaque broche ;
6. seulement ensuite alimenter ;
7. enregistrer les séquences ;
8. implémenter un décodeur ;
9. exposer les événements en USB HID.

## Format des relevés

Pour chaque action, noter :

- état avant ;
- action ;
- broches concernées ;
- résistance ;
- forme d’onde ;
- durée ;
- rebond ;
- séquence ;
- état après.

Les captures brutes sont conservées dans `captures/`.
