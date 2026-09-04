# Test Log

Journal chronologique des mesures et observations réalisées sur notre matériel.

## 2026-08-31 — XTOOL A30M

- connexion Bluetooth Linux réussie ;
- périphérique : `A30M-WE86VF7`
- Bluetooth Serial Port Profile présent ;
- RFCOMM channel observé : 3 ;
- commandes ELM `ATI/ATDP` non exploitables ;
- conclusion : protocole propriétaire XTOOL, ne pas considérer comme ELM327.

## 2026-08-31 — ELM327 Wi-Fi ancien

- PC : `192.168.0.11`
- interface : `192.168.0.10`
- TCP : `35000`
- connexion TCP réussie ;
- `ATI` → `ELM327 v1.5`
- `ATDP` → `ISO 15765-4 (CAN 11/500)`
- `010C` → `CAN ERROR`
- `0105` → `CAN ERROR`
- `010B` → `CAN ERROR`

Conclusion : interface non fiable pour la suite.

## 2026-09-04 — CSW-2000R ouvert / inspection photographique

Pièce :

```text
Renault 8200326970
CSW-2000R
Commande Centrale Multimedia
Xanavi Informatics Corporation
```

### Observations physiques

- boîtier ouvert sans destruction ;
- PCB principal et face boutons photographiés ;
- connecteur principal `CN1` = **12 contacts, 2 × 6** ;
- ancienne hypothèse « 6 voies » invalidée ;
- microcontrôleur principal NEC Japan en QFP ;
- quartz `X1` ;
- électronique active avec protections / conditionnement près de CN1 ;
- boutons `SWx`, LEDs et joystick central sur la carte avant.

### Numérotation CN1 corrigée

Vue dans `CN1`, détrompeur en bas :

```text
7  8  9 10 11 12
1  2  3  4  5  6
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

L'ancienne représentation avec `1..6` sur la rangée du haut est invalidée.

### Mesure GND

Carte hors tension :

```text
pin 2 = GND
pin 8 = GND
```

Statut : **MEASURED / USER CONFIRMED**.

### Traçage pins 6 et 12

Nouvelle mesure utilisateur :

```text
CN1 pin 6  → TDK ZJY2401
CN1 pin 12 → TDK ZJY2401
```

Le `TDK ZJY2401` est un filtre de mode commun 4 broches. Les pins `6` et `12` constituent donc très probablement les deux conducteurs d'une même paire de communication filtrée.

Interprétation :

- paire différentielle : **LIKELY** ;
- CAN : **possible / plausibilité renforcée** ;
- CAN confirmé : **NON**.

La prochaine mesure doit suivre les deux sorties du ZJY2401 côté électronique pour identifier leur destination, en particulier `VR1`, `VR2`, `IC3` ou un éventuel transceiver.

### Toujours inconnu

- entrée positive / alimentation ;
- tension nominale ;
- référence exacte MCU ;
- fonction exacte de `IC3` ;
- destination interne des sorties du ZJY2401 ;
- protocole réel : CAN / LIN / UART / autre ;
- fonction des pins `1,3,4,5,7,9,10,11`.

Conclusion : **ne pas alimenter le CSW avant identification du chemin d'alimentation.**

Document détaillé : `docs/CSW2000R.md`.

## Prochains tests

### Commande volant

- inspection à réception / ouverture si nécessaire ;
- cartographie des 6 broches ;
- boutons ;
- décodage molette.

### CSW-2000R

1. suivre les deux sorties internes du `TDK ZJY2401` ;
2. vérifier si elles arrivent sur `IC3` et, si oui, sur quelles broches ;
3. obtenir une macro lisible du marquage `IC3` ;
4. en parallèle, rechercher l'entrée positive via le condensateur `220 µF / 25 V` ;
5. identifier protections / régulation ;
6. seulement ensuite alimentation de laboratoire limitée en courant ;
7. analyse des lignes de communication.
