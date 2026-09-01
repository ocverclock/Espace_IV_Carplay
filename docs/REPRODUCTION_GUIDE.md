# Reproduction Guide

Le projet peut être reproduit entièrement ou par modules.

## Niveau 1 — CarPlay seulement

Nécessaire :
- Raspberry Pi 4 ;
- écran HDMI ;
- iPhone ;
- LIVI ;
- audio.

Résultat :
- CarPlay / Roole Map sans intégration Renault avancée.

## Niveau 2 — Commandes Renault

Ajouter :
- RP2040 ;
- commande au volant ou faisceau d’origine ;
- CSW-2000R une fois décodé.

Résultat :
- contrôle sans tactile.

## Niveau 3 — Caméra de recul

Ajouter :
- caméra ;
- interface vidéo ;
- signal marche arrière protégé.

Résultat :
- bascule automatique indépendante de l’iPhone.

## Niveau 4 — CAN

Ajouter :
- 1 ou 2 contrôleurs MCP2518FD ;
- transceivers automobiles ;
- raccordement réversible au faisceau.

Résultat :
- télémétrie et recherche véhicule.

## Niveau 5 — PCB complet

Regroupe :
- RP2040 ;
- MFi ;
- double CAN ;
- K-Line optionnelle ;
- entrées protégées ;
- gestion ACC/reverse.

## Procédure de validation

Chaque niveau doit fonctionner sur établi avant montage dans le véhicule.

Ne jamais utiliser le véhicule comme première alimentation d’un prototype non validé.
