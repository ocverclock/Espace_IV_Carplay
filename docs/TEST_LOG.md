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

### Traçage alimentation — CN1-7 / D1 / C25

Correction utilisateur : le gros électrolytique `220 µF / 25 V` est sérigraphié **C25**, et non C2.

Carte hors tension :

- le pad supérieur près de `C25` est relié à `CN1-7` ;
- le pad inférieur est relié à la masse `CN1-2/8` ;
- `D1` est insérée entre `CN1-7` et le rail de `C25 220 µF / 25 V` ;
- mesure en mode diode à travers `D1` :

```text
Vf(D1) = 0,551 V
```

Interprétation : `CN1-7` est un candidat très fort pour l'entrée positive d'alimentation ; `D1` est très probablement une protection série.

### C3 / rail 5 V

Mesures utilisateur :

- `C3 47 µF / 16 V` est raccordé sur le rail qui alimente `PCA82C250 pin 3 = VCC` ;
- ce rail est la sortie de l'étage autour de `IC1` ;
- le PCA82C250 étant alimenté nominalement en 5 V, ce rail est attendu autour de 5 V en fonctionnement normal.

```text
C3+ → rail logique attendu ≈5 V → PCA82C250 pin 3
```

### Q1 / IC1 — correction de topologie

Nouvelle observation utilisateur : `C25+` ne semble pas aller directement à `IC1` ; son chemin paraît passer par `Q1`, situé sur l'autre face de la carte.

L'ancien raccourci :

```text
C25 → IC1 → 5 V
```

est donc considéré **non démontré**.

État de travail :

```text
CN1-7 → D1 → C25 220 µF / 25 V → étage Q1 / IC1 à déterminer → C3 → rail attendu ≈5 V → PCA82C250 pin 3
```

Fonction exacte de `Q1` : **TBD**. Hypothèses possibles : transistor série / pass transistor, interrupteur high-side, ou transistor de commande de l'étage IC1.

### Brochage externe retrouvé — composant Renault 1657

Recherche documentaire sur le composant Renault `1657`, désigné clavier / contrôleur multimédia de l'Espace IV.

Brochage véhicule publié pour Espace IV :

```text
CN1-1  = 34HU  = réveil multimédia
CN1-5  = 107W  = ligne multimédia H
CN1-6  = 34DZ  = ligne multimédia 1
CN1-7  = BCP4  = + batterie protégé / mémoire
CN1-8  = MV    = masse audiosystème
CN1-9  = 19E   = rhéostat / éclairage
CN1-10 = LPG   = + veilleuses / position protégé
CN1-11 = 107X  = ligne multimédia L
CN1-12 = 34GA  = ligne multimédia 1
```

Pins `2`, `3`, `4` non listées comme conducteurs utilisés dans le faisceau externe consulté.

Corrélation avec nos mesures :

- `CN1-7 = BCP4 + batterie protégé` confirme le chemin mesuré `CN1-7 → D1 → C25` ;
- `CN1-1 = 34HU` identifie le **réveil multimédia**, à tracer maintenant vers `Q1 / IC1` ;
- `CN1-5/6` sont les deux branches documentées côté H et sont effectivement un même net **CANH** sur notre carte ;
- `CN1-11/12` sont les deux branches documentées côté L et sont effectivement un même net **CANL** sur notre carte ;
- `CN1-9` et `CN1-10` concernent le rétroéclairage / éclairage véhicule.

Statut : **EXTERNAL DOCUMENTATION CORROBORATED BY BENCH MEASUREMENTS — 2026-09-04**.

### Premier essai d'alimentation / wake sur banc

Essai utilisateur :

- alimentation branchée sur les deux pads d'alimentation près de CN1 ;
- alimentation marquée `12 V`, mais tension réellement mesurée à vide / au banc : **16 V** ;
- wake appliqué via la résistance série proposée ;
- le « second condensateur » a alors été mesuré à **11,3 V** ; l'identité exacte de ce condensateur doit être confirmée avant interprétation définitive.

**Important : essai à arrêter et ne pas répéter avec cette alimentation 16 V.**

Si le « second condensateur » est bien `C3`, cette mesure est anormale et incompatible avec le rail nominal 5 V attendu sur `PCA82C250 pin 3`. Le PCA82C250 fonctionne normalement avec `VCC = 4,5 à 5,5 V`; sa limite absolue de VCC est 9 V. Une mesure de 11,3 V sur le même net imposerait donc un risque réel de dommage.

Statut : **MEASURED / TEST INCONCLUSIVE — OVERVOLTAGE SOURCE — 2026-09-04**.

Prochaine étape avant toute nouvelle mise sous tension :

1. confirmer si le condensateur mesuré à `11,3 V` est bien `C3` ;
2. vérifier que la source utilisée est bien **DC** et non AC ;
3. utiliser ensuite une alimentation DC régulée, idéalement de laboratoire, réglée autour de `9 V` avec limitation de courant ;
4. mesurer séparément `C25+`, `C3+` et `PCA82C250 pin 3` avant et après wake.

Document détaillé : `docs/CSW2000R.md`.

## Prochains tests CSW-2000R

1. confirmer l'identité du condensateur ayant mesuré `11,3 V` ;
2. ne plus utiliser la source nominale 12 V qui délivre 16 V ;
3. reprendre avec une alimentation DC régulée limitée en courant ;
4. mesurer `C25+`, `C3+`, `PCA82C250 pin 3` avant/après `CN1-1 = 34HU` ;
5. tracer `CN1-1 = 34HU` vers `Q1`, `IC1` et les résistances associées si le 5 V reste absent ;
6. seulement après validation du rail 5 V : écoute CAN et détermination du bitrate.

## 2026-09-04 — Commande au volant 7701049643

Pièce :

```text
Renault 7701049643
34442201AF
connecteur 6 voies
```

Convention utilisateur, détrompeur à droite :

```text
1  2  3
4  5  6
```

Couleurs relevées :

```text
1 blanc
2 pourpre / violet
3 beige
4 marron
5 orange
6 gris
```

### Boutons

Mesures de continuité :

```text
A volume −        = 4 + 6
B volume +        = 4 + 1
C source −        = 3 + 5
D source +        = 6 + 5
E bouton inférieur = 2 + 4
```

Correction utilisateur : `D = source +` est `6 + 5`; l'ancien relevé `2 + 5` est invalide.

Fonction exacte du bouton inférieur `E` : **TBD**.

### Molette

- molette non cliquable ;
- `pin 2` = commun permanent ;
- séquence mesurée dans un sens :

```text
2+6 → 2+3 → 2+1
```

- sens inverse : séquence inverse :

```text
2+1 → 2+3 → 2+6
```

Conclusion : commande **passive par contacts secs** ; la direction de la molette est décodable par ordre des états, sans conversion analogique.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

Document détaillé : `docs/STEERING_REMOTE.md`.

## Prochains tests commande volant

1. identifier la fonction OEM du bouton inférieur ;
2. associer le sens fonctionnel de la molette à la séquence `6 → 3 → 1` ;
3. vérifier les transitions rapides / rebonds ;
4. préparer la lecture numérique par RP2040.
