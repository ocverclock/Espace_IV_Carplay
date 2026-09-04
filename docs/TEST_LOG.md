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
CANH = CN1-5 + CN1-6
CANL = CN1-11 + CN1-12
```

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

### Mesure de terminaison CAN

Module seul, hors tension et débranché du véhicule :

```text
R(CANH, CANL) = 37 kΩ
```

Interprétation : le CSW ne possède pas de terminaison CAN fixe locale de `120 Ω`.

### Traçage alimentation — CN1-7 / D1 / C2

Carte hors tension :

- le pad supérieur près du gros condensateur est relié à `CN1-7` ;
- le pad inférieur est relié à la masse `CN1-2/8` ;
- `D1` est insérée entre `CN1-7` et le rail du gros condensateur `C2 220 µF / 25 V` ;
- mesure en mode diode à travers `D1` :

```text
Vf(D1) = 0,551 V
```

Interprétation : `CN1-7` est un candidat très fort pour l'entrée positive d'alimentation ; `D1` est très probablement une protection série.

### IC1 / C3 / rail 5 V

Nouvelle mesure utilisateur :

- `IC1` alimente le `PCA82C250` ;
- `C3 47 µF / 16 V` est raccordé sur la sortie de `IC1` ;
- cette sortie rejoint `PCA82C250 pin 3 = VCC`.

Le PCA82C250 étant alimenté nominalement en 5 V, la fonction de `IC1` est maintenant établie :

```text
CN1-7 → D1 → C2 220 µF / 25 V → IC1 → C3 47 µF / 16 V → rail logique 5 V → PCA82C250 pin 3
```

Conclusion : **IC1 est le régulateur / étage de régulation 5 V du module**.

Statut : **FUNCTION CONFIRMED / EXACT PART NUMBER TBD — 2026-09-04**.

Toujours inconnu :

- référence exacte et pinout de `IC1` ;
- plage admissible à l'entrée de `IC1` ;
- tension nominale réelle appliquée à `CN1-7` dans le véhicule.

**Ne pas appliquer 12 V tant que l'entrée admissible de IC1 n'est pas validée.**

Document détaillé : `docs/CSW2000R.md`.

## Prochains tests CSW-2000R

1. relever le marquage exact de `IC1` ;
2. confirmer ses broches `VIN`, `VOUT`, `GND` ;
3. suivre le rail 5 V vers le MCU ;
4. suivre `TXD pin 1` et `RXD pin 4` vers le MCU NEC ;
5. seulement après validation de l'alimentation : alimentation de laboratoire limitée en courant ;
6. écoute CAN passive et détermination du bitrate ;
7. captures trames par bouton / joystick / rotation.

## Prochains tests commande volant

- inspection / ouverture si nécessaire ;
- cartographie des 6 broches ;
- boutons ;
- décodage molette.
