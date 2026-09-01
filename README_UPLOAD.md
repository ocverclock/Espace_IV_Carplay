# Mise à jour GitHub — correction MFi

L’écriture directe via l’intégration GitHub a été refusée avec une erreur 403, malgré la lecture correcte du dépôt.

## Fichiers complets à remplacer

Uploader/remplacer directement :

- `AI_HANDOFF.md`
- `ROADMAP.md`
- `docs/DECISIONS.md`
- `docs/LIVI_CARPLAY_SETUP.md`

## `PROJECT_STATE.md`

Ne pas remplacer ce fichier par un fichier partiel.

Ouvrir `PROJECT_STATE_MFI_UPDATE.md` et appliquer les quatre modifications indiquées dans le `PROJECT_STATE.md` existant.

## Correction apportée

Le MFi n’est pas une étape à acheter après validation de CarPlay natif.

Ordre correct :

1. installer/tester LIVI hors CarPlay ;
2. intégrer un MFi de laboratoire ;
3. seulement ensuite valider iPhone / CarPlay / Roole Map / audio / micro / Siri.

Source :
https://github.com/f-io/LIVI/blob/main/README.md#mfi-authentication
