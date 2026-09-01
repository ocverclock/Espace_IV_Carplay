# AI / New Chat Handoff

Ce fichier est destiné à toute reprise du projet après changement de chat, de machine ou de contributeur.

## Ordre de lecture obligatoire

1. `PROJECT_STATE.md`
2. `docs/TEST_LOG.md`
3. `docs/DECISIONS.md`
4. le document du sous-système actuellement travaillé
5. les captures / mesures associées

## Hiérarchie de confiance

1. mesures faites sur notre Espace IV ou nos pièces ;
2. schémas constructeur Renault ;
3. datasheets fabricants ;
4. documentation/code des projets tiers ;
5. reverse engineering public documenté ;
6. forums ;
7. hypothèses de travail.

Une mesure nouvelle doit remplacer une hypothèse ancienne.

## Règle de reprise

Avant de proposer une modification matérielle ou logicielle :

- vérifier le dernier état dans `PROJECT_STATE.md` ;
- vérifier les décisions déjà prises dans `docs/DECISIONS.md` ;
- vérifier les dernières mesures dans `docs/TEST_LOG.md`.

Ne pas repartir de zéro et ne pas réintroduire une solution déjà écartée sans raison nouvelle.

## Points importants au checkpoint initial

- Raspberry Pi 4 déjà disponible.
- iPhone obligatoire ; Android Auto n’est pas la cible.
- Roole Map est l’application de navigation habituelle via CarPlay.
- écran 7" non tactile prévu.
- façade d’écran refaite en impression 3D pour exploiter l’écran complet.
- commande centrale `8200326970 / CSW-2000R` achetée pour laboratoire.
- commande volant `7701049643 / 34442201AF` achetée pour laboratoire.
- la molette du commodo doit être récupérée, pas abandonnée.
- MFi direct via `MFI343S00177-L` est la piste privilégiée.
- Carlinkit n’est pas la cible finale.
- RP2040 prévu pour les commandes physiques.
- double CAN matériel prévu sur le PCB final.
- ELS27 à ~150 € volontairement reportée.
- caméra de recul obligatoire dans la cible.
