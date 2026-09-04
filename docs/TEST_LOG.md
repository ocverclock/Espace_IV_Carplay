# Test Log

Journal chronologique des mesures et observations réalisées sur notre matériel.

## 2026-08-31 — XTOOL A30M

- connexion Bluetooth Linux réussie ;
- périphérique : `A30M-WE86VF7` ;
- Bluetooth Serial Port Profile présent ;
- RFCOMM channel observé : 3 ;
- commandes ELM `ATI/ATDP` non exploitables ;
- conclusion : protocole propriétaire XTOOL, ne pas considérer comme ELM327.

## 2026-08-31 — ELM327 Wi-Fi ancien

- PC : `192.168.0.11` ;
- interface : `192.168.0.10` ;
- TCP : `35000` ;
- `ATI` → `ELM327 v1.5` ;
- `ATDP` → `ISO 15765-4 CAN 11/500` ;
- `010C`, `0105`, `010B` → `CAN ERROR`.

Conclusion : interface non fiable pour la suite.

## 2026-09-04 — CSW-2000R ouvert / reverse engineering

Pièce :

```text
Renault 8200326970
CSW-2000R
Commande Centrale Multimedia
Xanavi Informatics Corporation
```

### Connecteur CN1

- `CN1` = **12 contacts, 2 × 6** ;
- ancienne hypothèse « 6 voies » invalidée ;
- numérotation confirmée sur la pièce, vue dans CN1 détrompeur en bas :

```text
7  8  9 10 11 12
1  2  3  4  5  6
```

Statut : **MEASURED / USER CONFIRMED**.

### Masse

Carte hors tension :

```text
CN1-2 = GND
CN1-8 = GND
```

Statut : **MEASURED / USER CONFIRMED**.

### Identification du bus CAN

Composants relevés :

- filtre de mode commun `TDK ZJY2401` ;
- `IC3` marqué :

```text
A82C250
5K3A3
N5464
PHILIPS
```

`IC3` est identifié comme **Philips/NXP PCA82C250**, transceiver CAN haute vitesse ISO 11898.

Pinout utile :

```text
1 TXD
2 GND
3 VCC 5 V nominal
4 RXD
5 Vref
6 CANL
7 CANH
8 Rs
```

Conclusion : **CSW-2000R utilise un bus CAN = CONFIRMÉ**.

### Contacts CAN dupliqués

Mesure utilisateur :

```text
CN1-5 = CN1-6
CN1-11 = CN1-12
```

Les deux nets côté connecteur sont donc :

```text
NET H = CN1-5 + CN1-6
NET L = CN1-11 + CN1-12
```

Les anciennes notes ne mentionnant que `CN1-6` et `CN1-12` sont incomplètes et supersédées.

### Polarité CAN confirmée

Continuité vérifiée jusqu'au `PCA82C250` :

```text
PCA82C250 pin 7 = CANH → CN1-5 / CN1-6
PCA82C250 pin 6 = CANL → CN1-11 / CN1-12
```

Brochage CAN définitif :

```text
CN1-5  = CANH
CN1-6  = CANH
CN1-11 = CANL
CN1-12 = CANL
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

### Alimentation

Le PCA82C250 confirme un rail interne 5 V, mais l'entrée positive du module côté CN1 et la tension véhicule ne sont toujours pas identifiées.

**Ne pas appliquer 12 V tant que le chemin d'alimentation n'est pas tracé.**

Document détaillé : `docs/CSW2000R.md`.

## Prochains tests CSW-2000R

1. mesurer, module seul et hors tension, la résistance entre `CANH (CN1-5/6)` et `CANL (CN1-11/12)` pour vérifier une éventuelle terminaison ;
2. suivre `PCA82C250 pin 3` vers le rail 5 V puis le régulateur ;
3. remonter jusqu'à l'entrée positive CN1 ;
4. suivre `TXD pin 1` et `RXD pin 4` vers le MCU NEC ;
5. seulement après identification alimentation : alimentation de laboratoire limitée en courant ;
6. écoute CAN passive et détermination du bitrate ;
7. captures trames par bouton / joystick / rotation.

## Prochains tests commande volant

- inspection / ouverture si nécessaire ;
- cartographie des 6 broches ;
- boutons ;
- décodage molette.
