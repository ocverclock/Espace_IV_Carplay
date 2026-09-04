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
- connecteur principal repéré `CN1` ;
- **CN1 possède 12 contacts, organisés en 2 × 6** ;
- l'ancienne hypothèse « connecteur 6 voies » est invalidée ;
- microcontrôleur principal NEC Japan en QFP visible ;
- quartz externe `X1` visible près du MCU ;
- composants de protection / alimentation visibles à proximité de CN1 (`ZD4`, `ZD5`, `VR3`, transistors et passifs) ;
- face avant équipée de boutons `SWx`, LEDs et joystick central ;
- la commande contient donc une électronique active et n'est pas une simple matrice passive directement exposée au faisceau.

### Correction de la numérotation CN1

Une première interprétation photographique avait été enregistrée ainsi :

```text
1  2  3  4  5  6
7  8  9 10 11 12
```

**Cette interprétation était fausse.**

Le relevé utilisateur effectué directement sur la pièce physique fixe la numérotation correcte, vue dans `CN1` avec le détrompeur en bas :

```text
7  8  9 10 11 12
1  2  3  4  5  6
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

La convention du projet devient donc :

- rangée du haut, gauche → droite : `7, 8, 9, 10, 11, 12` ;
- rangée du bas, gauche → droite : `1, 2, 3, 4, 5, 6` ;
- détrompeur en bas.

Toute documentation antérieure montrant les deux rangées inversées est supersédée.

### Ce qui n'est PAS encore mesuré

- masse ;
- alimentation ;
- tension nominale ;
- référence exacte MCU ;
- fonction de `IC3` ;
- fonction électrique des 12 pins ;
- protocole de sortie ;
- CAN / UART / LIN / autre.

Conclusion : **ne pas alimenter le CSW avant cartographie hors tension des 12 broches.**

Document détaillé : `docs/CSW2000R.md`.

## Prochains tests

### Commande volant

- inspection à réception / ouverture si nécessaire ;
- cartographie des 6 broches ;
- boutons ;
- décodage molette.

### CSW-2000R

1. chercher la ou les broches GND par continuité, carte hors tension ;
2. cartographier les 12 broches hors tension ;
3. identifier le chemin d'alimentation ;
4. macros lisibles MCU / IC3 / composants d'entrée ;
5. seulement ensuite alimentation de laboratoire limitée en courant ;
6. analyse des lignes de communication.
