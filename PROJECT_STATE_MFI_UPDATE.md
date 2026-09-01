# Correction à appliquer dans `PROJECT_STATE.md`

`PROJECT_STATE.md` reste la source de vérité principale. Il contient encore une formulation qui place le MFi dans « A acheter après validation ». Cette formulation doit être corrigée.

Source technique de la correction :
- LIVI README / MFi Authentication
- https://github.com/f-io/LIVI/blob/main/README.md#mfi-authentication

Le README de LIVI indique qu’un coprocesseur MFi matériel est requis pour CarPlay natif et que, sans ce coprocesseur, CarPlay natif est indisponible.

## 1. Section 5 — Apple CarPlay / MFi

Après le paragraphe présentant `MFI343S00177-L`, ajouter :

```text
[CONFIRME PAR LIVI]
Pour CarPlay natif, le coprocesseur MFi est une dépendance matérielle obligatoire.
LIVI peut être installé et son interface/HID testés sans MFi, mais la connexion
iPhone et la validation réelle de CarPlay natif ne sont pas possibles sans
coprocesseur MFi fonctionnel relié en I²C.

Source :
https://github.com/f-io/LIVI/blob/main/README.md#mfi-authentication

Conséquence projet :
le MFi de laboratoire doit être disponible AVANT les tests iPhone / CarPlay /
Roole Map / audio CarPlay / appels / micro / Siri.
```

## 2. Section 21 — Matériel disponible / prévu

Dans `### A acheter à court terme`, ajouter :

```text
- MFi de laboratoire pour validation CarPlay native ;
- composants minimum nécessaires à son alimentation et à sa liaison I²C.
```

Dans `### A acheter après validation`, supprimer :

```text
- MFi `MFI343S00177-L`
- composants/PCB MFi ;
```

Le MFi n’est plus un achat « après validation CarPlay » : il est nécessaire pour effectuer cette validation.

## 3. Section 23 — Phase B — CarPlay sur établi

Remplacer l’ordre actuel par :

```text
### Phase B — CarPlay sur établi

1. Pi 4 ;
2. LIVI ;
3. écran HDMI temporaire ;
4. validation démarrage / affichage / navigation clavier-HID hors CarPlay ;
5. intégrer un MFi de laboratoire ;
6. valider alimentation, I²C et détection MFi par LIVI ;
7. connecter l’iPhone ;
8. valider CarPlay natif ;
9. valider Roole Map ;
10. valider commandes HID dans CarPlay ;
11. valider audio ;
12. valider microphone ;
13. valider Siri ;
14. valider reconnexion après reboot.

Livrable :
`docs/LIVI_CARPLAY_SETUP.md`
```

## 4. Section 28 — Etat du projet

Ajouter au résumé :

```text
- MFi de laboratoire : requis avant validation iPhone / CarPlay natif ;
- tests LIVI sans MFi : limités à l’application, l’affichage et les entrées locales/HID.
```

## Règle de priorité

La nouvelle décision `D012` dans `docs/DECISIONS.md` supersède toute ancienne formulation contradictoire jusqu’à mise à jour complète de `PROJECT_STATE.md`.
