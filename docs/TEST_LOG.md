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

## 2026-09-04 — CSW-2000R ouvert / inspection et reverse engineering

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

Mesure utilisateur :

```text
CN1 pin 6  → TDK ZJY2401
CN1 pin 12 → TDK ZJY2401
```

Le `TDK ZJY2401` est un filtre de mode commun 4 broches placé sur une paire de lignes.

### Identification IC3 — étape décisive

Marquage relevé directement sur `IC3` :

```text
A82C250
5K3A3
N5464
PHILIPS
```

Ce marquage correspond au **Philips/NXP PCA82C250**, transceiver CAN haute vitesse compatible ISO 11898.

Pinout datasheet utile :

```text
1 TXD
2 GND
3 VCC (5 V nominal)
4 RXD
5 Vref
6 CANL
7 CANH
8 Rs
```

Conclusion :

```text
CSW-2000R utilise un bus CAN = CONFIRMÉ
CN1-6 / CN1-12 = paire CAN via ZJY2401 = CONFIRMÉ
```

Ce qui reste à déterminer :

```text
CN1-6  = CANH ou CANL ?
CN1-12 = CANL ou CANH ?
```

Statut de la polarité H/L : **NOT YET MEASURED**.

### Alimentation

Le PCA82C250 confirme l'existence d'un rail interne d'environ 5 V (`VCC pin 3`, plage nominale datasheet 4,5–5,5 V).

Mais l'entrée positive du module côté `CN1` et sa tension nominale véhicule ne sont toujours pas identifiées.

Conclusion : **ne pas appliquer 12 V tant que le chemin d'alimentation n'est pas tracé.**

Document détaillé : `docs/CSW2000R.md`.

## Prochains tests

### Commande volant

- inspection à réception / ouverture si nécessaire ;
- cartographie des 6 broches ;
- boutons ;
- décodage molette.

### CSW-2000R

1. carte hors tension : suivre `PCA82C250 pin 6 = CANL` à travers le ZJY2401 jusqu'à CN1 ;
2. suivre `PCA82C250 pin 7 = CANH` à travers le ZJY2401 jusqu'à CN1 ;
3. fixer définitivement `CN1-6` et `CN1-12` en CANH/CANL ;
4. suivre `PCA82C250 pin 3 = VCC` vers le rail 5 V et son régulateur ;
5. remonter du régulateur jusqu'à l'entrée positive CN1 ;
6. suivre `TXD pin 1` et `RXD pin 4` vers le MCU NEC ;
7. seulement après identification alimentation : alimentation de laboratoire limitée en courant ;
8. écoute CAN passive et détermination du bitrate ;
9. captures trames par bouton / joystick / rotation.
