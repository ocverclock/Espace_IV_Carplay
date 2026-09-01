# Steering Remote — Renault 7701049643

## Identification

- Renault : `7701049643`
- exemplaire acheté : `34442201AF`
- 6 voies.

## Objectif

Décoder **toutes** les commandes, notamment la molette généralement perdue dans les adaptations autoradio simples.

## Première campagne de mesure

Tester les 15 paires possibles entre 6 broches.

Créer une matrice :

| Action | 1-2 | 1-3 | 1-4 | 1-5 | 1-6 | ... |
|---|---|---|---|---|---|---|
| repos | | | | | | |
| bouton 1 | | | | | | |
| bouton 2 | | | | | | |
| molette + 1 cran | | | | | | |
| molette - 1 cran | | | | | | |

## Molette

Mesurer lentement au multimètre puis à l’analyseur logique si nécessaire.

Chercher :
- quadrature ;
- Gray code ;
- séquence de contacts ;
- impulsions sur une matrice.

Ne pas convertir prématurément le commodo en simple réseau résistif : cela peut perdre l’information de direction de la molette.
