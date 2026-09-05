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
- ce rail est la sortie de l'étage autour de `IC1`.

### Q1 / IC1 — correction de topologie

Nouvelle observation utilisateur : `C25+` ne semble pas aller directement à `IC1` ; son chemin paraît passer par `Q1`, situé sur l'autre face de la carte.

État de travail :

```text
CN1-7 → D1 → C25 220 µF / 25 V → étage Q1 / IC1 → C3 → rail 5 V → PCA82C250 pin 3
```

Le détail interne de `Q1 / IC1` reste TBD, mais il n'est plus nécessaire de l'identifier avant les premiers essais CAN puisque le fonctionnement global de l'étage est maintenant validé sur banc.

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
- `CN1-1 = 34HU` identifie le **réveil multimédia** ;
- `CN1-5/6` sont effectivement un même net **CANH** sur notre carte ;
- `CN1-11/12` sont effectivement un même net **CANL** sur notre carte ;
- `CN1-9` et `CN1-10` concernent le rétroéclairage / éclairage véhicule.

Statut : **EXTERNAL DOCUMENTATION CORROBORATED BY BENCH MEASUREMENTS — 2026-09-04**.

### Premier essai d'alimentation / wake exploitable

Correction des mesures précédentes : les valeurs `16 V` et `11,3 V` étaient dues à une **erreur de mesure** et sont invalidées.

Mesures correctes :

```text
alimentation principale = 12,5 V
wake CN1-1 / 34HU = appliqué via résistance série
C3+ = 5,0 V
PCA82C250 pin 3 = 5,0 V
```

Conclusion : l'étage interne fournit correctement son rail 5 V lorsque le module est alimenté à `12,5 V` et que le wake est appliqué.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

### Niveaux CAN au repos

Sous la même alimentation, module réveillé :

```text
CANH ≈ 2,5 V
CANL ≈ 2,5 V
```

Ces niveaux sont cohérents avec le PCA82C250 alimenté et le bus au repos / état récessif.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

### Validation du rétroéclairage

Essai utilisateur :

```text
pont direct CN1-7 → CN1-10
```

Résultat : **les LEDs / le rétroéclairage de la commande centrale s'allument**.

Conclusion : `CN1-10 = LPG` est bien l'entrée positive d'éclairage. `CN1-9 = 19E` reste à tester pour son rôle exact de rhéostat / niveau de luminosité.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

Le module est désormais suffisamment validé côté alimentation, wake, transceiver CAN et éclairage pour préparer l'écoute CAN.

## 2026-09-05 — CSW-2000R CAN dynamique

Montage analyseur logique :

```text
D0 = PCA82C250 pin 1 = TXD
D5 = PCA82C250 pin 4 = RXD
sample rate = 8 MHz
CAN decoder RX = D5
bitrate = 500000 bit/s
```

Deux résistances `220 Ω` en parallèle donnent environ `110 Ω` entre CANH et CANL pour la terminaison de banc.

Avec cette terminaison :

- TXD et RXD suivent la même trame ;
- PulseView décode correctement du CAN classique ;
- débit confirmé : **500 kbit/s** ;
- DLC observé : `8` ;
- première trame lisible :

```text
ID   = 0x681
DATA = F0 0A 0A 01 FF FF FF FF
```

### Capture idle dédiée

Fichier : `csw_idle.sr`

```text
50 000 000 échantillons
8 MHz
6,25 s de capture
aucune action utilisateur
```

Décodage automatique de l'intégralité du fichier :

- `2339` trames classiques valides ;
- toutes ont `ID = 0x681` ;
- toutes ont `DLC = 8` ;
- toutes ont le payload `F0 0A 0A 01 FF FF FF FF` ;
- aucune autre trame / aucun autre payload détecté ;
- trafic organisé en `13` salves ;
- une salve dure environ `49–50 ms` ;
- début des salves espacé d'environ `500 ms` ;
- typiquement ~`179` retransmissions identiques par salve ;
- intervalle entre deux tentatives successives : environ `274 µs`.

Conclusion :

```text
0x681  F0 0A 0A 01 FF FF FF FF
```

est la **baseline CAN au repos** du CSW sur notre banc. La capture initialement appelée `csw_bouton_test_01.sr` ne contient qu'une occurrence de cette baseline et ne permet donc pas d'attribuer ce payload à un bouton.

La forte répétition dans chaque salve est probablement liée à l'absence d'ACK sur le banc, à confirmer avec un second nœud CAN actif.

Statut : **MEASURED FROM CAPTURE FILE / USER CONFIRMED — 2026-09-05**.

### Protocole de capture commandes à partir de maintenant

Pour chaque bouton / joystick / rotation :

1. capturer plusieurs secondes ;
2. laisser environ 1 s au repos ;
3. effectuer une seule action, idéalement maintenue 0,5–1 s ;
4. relâcher ;
5. laisser encore environ 1 s au repos ;
6. sauvegarder en `.sr` avec un nom explicite ;
7. comparer automatiquement avec la baseline idle.

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
A volume −         = 4 + 6
B volume +         = 4 + 1
C source −         = 3 + 5
D source +         = 6 + 5
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
