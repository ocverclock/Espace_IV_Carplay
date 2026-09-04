# Espace IV Modernisation Multimédia — PROJECT_STATE

Dernière mise à jour : 2026-09-04

Ce fichier est le **checkpoint global et la source de vérité principale** du projet.

Il contient l’architecture, les faits mesurés, les décisions actives et les prochaines étapes. Les détails vivent dans les documents de sous-système.

## 1. Objectif

Moderniser le système multimédia d’un Renault Espace IV CNC/Xanavi en conservant une intégration OEM+ :

- Raspberry Pi 4 ;
- Apple CarPlay natif avec iPhone ;
- Roole Map via CarPlay ;
- écran 7" non tactile ;
- commandes Renault d’origine réutilisées ;
- appels mains libres ;
- caméra de recul automatique ;
- futur dashboard véhicule via CAN ;
- installation propre et réversible ;
- prise OBD laissée libre dans l’installation finale.

## 2. Hiérarchie documentaire

Ordre de lecture pour reprendre le projet :

1. `PROJECT_STATE.md`
2. `docs/TEST_LOG.md`
3. `docs/DECISIONS.md`
4. `ROADMAP.md`
5. document du sous-système travaillé
6. `captures/` / `references/`

Hiérarchie de confiance :

1. mesures sur notre véhicule / nos pièces ;
2. documentation constructeur Renault ;
3. datasheets fabricants ;
4. documentation/code officiel des projets utilisés ;
5. plusieurs sources tierces indépendantes concordantes ;
6. reverse engineering public ;
7. forums/commentaires isolés ;
8. hypothèses.

Une mesure réelle remplace une hypothèse ancienne.

## 3. Véhicule / système d’origine

Système : Renault Carminat Navigation & Communication / CNC, génération Xanavi.

Éléments conservés autant que possible :

- système audio Renault ;
- commande centrale ;
- commande multimédia au volant ;
- ergonomie visuelle OEM.

L’ancienne navigation est destinée à être abandonnée fonctionnellement.

## 4. Commande centrale CSW-2000R — état acquis

Pièce laboratoire :

```text
Renault 8200326970
CSW-2000R
Commande Centrale Multimedia
Xanavi Informatics Corporation
```

### Connecteur CN1

`CN1` possède **12 voies, 2 × 6**.

Vue dans `CN1`, détrompeur en bas :

```text
haut : 7  8  9 10 11 12
bas  : 1  2  3  4  5  6
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

Les anciennes mentions « 6 voies » ou les schémas avec `1..6` sur la rangée supérieure sont invalidés.

### Masse

```text
CN1-2 = GND
CN1-8 = GND
```

Statut : **MEASURED**.

### Bus CAN confirmé

Le PCB contient :

- filtre de mode commun `TDK ZJY2401` ;
- `IC3 = Philips/NXP PCA82C250`, transceiver CAN haute vitesse ISO 11898.

Pinout utile du PCA82C250 :

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

Conclusion :

```text
CSW-2000R utilise un bus CAN = CONFIRMÉ
```

### Brochage CAN définitif

Mesures utilisateur :

```text
CN1-5 = CN1-6
CN1-11 = CN1-12
```

Continuité confirmée jusqu’aux sorties du PCA82C250 :

```text
PCA82C250 pin 7 = CANH → CN1-5 / CN1-6
PCA82C250 pin 6 = CANL → CN1-11 / CN1-12
```

Donc :

```text
CN1-5  = CANH
CN1-6  = CANH
CN1-11 = CANL
CN1-12 = CANL
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

### Terminaison CAN locale

Module seul, hors tension :

```text
R(CANH, CANL) = 37 kΩ
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

Interprétation : le CSW **ne possède pas de terminaison CAN fixe locale de 120 Ω**. La valeur de `37 kΩ` est une impédance interne élevée, pas une terminaison de bus.

Conséquence : ne pas ajouter arbitrairement `120 Ω` au niveau du CSW. La terminaison doit rester conforme à la topologie réelle du réseau et uniquement aux extrémités prévues.

### Alimentation — état actuel

Le chemin mesuré côté entrée est :

```text
CN1-7 → D1 → C25 220 µF / 25 V
```

- `CN1-7` rejoint le rail de `C25` à travers `D1` ;
- `Vf(D1) = 0,551 V` en mode diode ;
- `C25-` est à la masse `CN1-2/8` ;
- `C3 47 µF / 16 V` est sur le rail qui alimente `PCA82C250 pin 3`, donc rail logique ≈5 V ;
- le chemin entre `C25` et `C3` implique l’étage `Q1 / IC1`, dont la topologie exacte reste à déterminer ;
- l’ancien raccourci `C25 → IC1 → 5 V` est invalidé car non démontré.

État de travail :

```text
CN1-7 → D1 → C25 → étage Q1 / IC1 TBD → C3 → ≈5 V → PCA82C250 pin 3
```

`CN1-7` est donc un **très fort candidat pour l’entrée positive d’alimentation**, mais sa tension nominale côté véhicule reste à confirmer.

**Ne pas appliquer 12 V tant que la topologie Q1/IC1 et la plage d'entrée admissible ne sont pas suffisamment établies.**

### Table CN1 actuelle

| Pin | Fonction / net | Statut |
|---:|---|---|
| 1 | TBD | non mesuré |
| 2 | **GND** | MEASURED |
| 3 | TBD | non mesuré |
| 4 | TBD | non mesuré |
| 5 | **CANH**, relié à 6 | MEASURED |
| 6 | **CANH**, relié à 5 | MEASURED |
| 7 | **entrée alimentation probable via D1 → C25** | MEASURED PATH / tension à confirmer |
| 8 | **GND** | MEASURED |
| 9 | TBD | non mesuré |
| 10 | TBD | non mesuré |
| 11 | **CANL**, relié à 12 | MEASURED |
| 12 | **CANL**, relié à 11 | MEASURED |

### MCU

MCU principal NEC Japan en QFP + quartz `X1` visibles.

Le marquage semble compatible avec une famille Renesas/NEC `78K0/Kx2`, possiblement `µPD78F052xA`, mais la référence exacte n’est pas encore verrouillée.

Document détaillé : `docs/CSW2000R.md`.

## 5. Commande au volant — reverse engineering acquis

Référence :

```text
Renault 7701049643
34442201AF
connecteur 6 voies
```

Convention de mesure utilisateur, détrompeur à droite :

```text
1  2  3
4  5  6
```

Couleurs :

```text
1 blanc
2 pourpre / violet
3 beige
4 marron
5 orange
6 gris
```

### Boutons

```text
volume −         = 4 + 6
volume +         = 4 + 1
source −         = 3 + 5
source +         = 6 + 5
bouton inférieur = 2 + 4
```

La fonction OEM exacte du bouton inférieur reste inconnue.

### Molette

- non cliquable ;
- `pin 2` = commun permanent ;
- séquence mesurée dans un sens : `2+6 → 2+3 → 2+1` ;
- sens inverse : `2+1 → 2+3 → 2+6`.

Conclusion : **commande passive par contacts secs**, y compris la molette. La direction peut être décodée par l’ordre des états.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

Conséquence : lecture directe par RP2040 réaliste, avec entrées numériques, pull-ups/pull-downs adaptés et debounce logiciel. Pas de conversion analogique requise à ce stade.

Documents :

- `docs/STEERING_REMOTE.md`
- `docs/CONTROLS_REVERSE_ENGINEERING.md`

## 6. Architecture cible

```text
                                 iPhone
                                   │
                          CarPlay filaire/Wi-Fi
                                   │
                          ┌────────▼─────────┐
                          │ Raspberry Pi 4   │
                          │ Trixie + LIVI    │
                          └───┬──────┬───────┘
                              │      │
                            HDMI    Audio
                              │      │
                          écran 7"   └──► audio Renault

                         GPIO / USB / SPI
                                   │
        ┌──────────────────────────▼─────────────────────────┐
        │             Espace IV Interface Board             │
        │                                                   │
        │ RP2040                                            │
        │  ├─ commande au volant                            │
        │  ├─ reverse                                       │
        │  └─ ACC / illumination                            │
        │                                                   │
        │ 3.3 V → load-switch → MFI343S00177-L              │
        │ GPIO21 → EN                                       │
        │ GPIO19 SDA / GPIO26 SCL                           │
        │                                                   │
        │ MCP2518FD #1 + transceiver → CAN véhicule         │
        │ MCP2518FD #2 + transceiver → CAN secondaire       │
        │ L9637D optionnel → K-Line                         │
        └───────────────────────────────────────────────────┘
```

Le CSW étant confirmé CAN, la stratégie prioritaire est de **conserver son électronique d’origine et décoder ses trames**.

Fallback : RP2040 interne uniquement si le protocole Xanavi devient trop coûteux à reproduire.

## 7. Raspberry Pi / LIVI

Décision active :

- Raspberry Pi 4 ;
- Raspberry Pi OS / Debian 13 Trixie 64 bits ;
- LIVI ;
- cible kiosk automobile à terme.

Décision : `D013`.

Document : `docs/LIVI_CARPLAY_SETUP.md`.

## 8. CarPlay / MFi

CarPlay natif LIVI nécessite un coprocesseur MFi physique.

Ordre correct :

```text
Pi + Trixie
→ LIVI
→ affichage / clavier / HID
→ prototype MFi
→ détection I²C
→ CarPlay filaire
→ Roole Map / audio / micro / Siri
→ CarPlay sans fil
```

Décision : `D012`.

Composant cible :

```text
Microchip MFI343S00177-L
Authentication Coprocessor 3.0
LCSC C33770534
```

Configuration LIVI actuelle :

```text
I²C bus = 2
SDA = GPIO19
SCL = GPIO26
Power control = GPIO21
```

Architecture alimentation :

```text
Pi 3.3 V → load-switch → MFI_VCC
GPIO21 → EN du load-switch
```

Décision : `D014`.

Pinout de travail :

```text
1 GND
2 NC
3 NC
4 GND
5 SDA
6 SCL
7 GND
8 VCC
9 PAD/GND
```

Pas encore MEASURED sur notre prototype. Décision : `D015`.

Adresse I²C attendue CP3.0 : `0x10`, à confirmer.

Document : `docs/MFI_WIRING.md`.

## 9. Écran

Décisions : 7", non tactile, IPS préféré, HDMI préféré, luminosité élevée et nouvelle façade imprimée 3D.

Cible : idéalement ≥500 nits si prix raisonnable.

Document : `docs/DISPLAY.md`.

## 10. RP2040

Rôle prévu : commande au volant, debounce, molette, reverse, ACC/illumination et USB HID vers Pi.

La commande au volant étant maintenant cartographiée comme contacts secs, son interface RP2040 devient simple à prototyper.

Pour le CSW, le RP2040 n’est plus la voie principale tant que le décodage CAN d’origine reste réaliste.

## 11. Audio / mains libres

```text
Pi → DAC / ligne → AUX Renault → amplification OEM
```

Micro : essayer d’abord le micro Renault d’origine ; fallback micro automobile discret.

Document : `docs/AUDIO_MIC.md`.

## 12. Caméra de recul

Obligatoire : bascule automatique, retour automatique, faible latence et fonctionnement sans iPhone.

Technologie ouverte : CVBS / AHD / USB UVC.

Document : `docs/REVERSE_CAMERA.md`.

## 13. CAN véhicule / architecture finale

Architecture finale envisagée :

```text
Pi SPI → MCP2518FD #1 → transceiver → CAN véhicule
Pi SPI → MCP2518FD #2 → transceiver → CAN secondaire / multimédia
```

La présence du PCA82C250 dans le CSW confirme l’intérêt du second canal CAN pour le réseau multimédia.

Règles : écoute passive d’abord, aucune émission active avant compréhension, aucune terminaison supplémentaire sans validation de la topologie.

Document : `docs/CAN_RESEARCH.md`.

## 14. Alimentation automobile

```text
12 V véhicule
→ protections automobile
→ buck 5 V
→ Raspberry Pi
```

ACC/contact : détection protégée, shutdown Linux propre, temporisation, coupure finale, faible consommation à l’arrêt.

Document : `docs/POWER.md`.

## 15. PCB final

Objectif : une carte unique Espace IV intégrant RP2040, MFi + load-switch, double CAN, K-Line optionnelle, reverse, ACC/illumination, protections, connecteurs et points de test.

Ne pas lancer PCB V1 tant que commandes, MFi, alimentation et stratégie écran ne sont pas suffisamment validés.

## 16. Priorités actuelles

### P0-A — CSW-2000R

État :

```text
GND = pins 2 et 8
CANH = pins 5 et 6
CANL = pins 11 et 12
CAN transceiver = PCA82C250
CAN = CONFIRMED
R(CANH,CANL) = 37 kΩ
local 120 Ω termination = ABSENT
CN1-7 → D1 → C25 = measured power path
C3 → PCA82C250 VCC ≈ 5 V
Q1 / IC1 topology = TBD
```

Prochaines étapes :

1. cartographier les trois broches de `Q1` ;
2. suivre `C25+ → Q1` puis `Q1 ↔ IC1` ;
3. identifier les marquages exacts `Q1` / `IC1` si possible ;
4. suivre TXD/RXD vers le MCU ;
5. seulement après validation de l’alimentation : alimentation labo limitée en courant ;
6. déterminer bitrate CAN ;
7. capturer trames boutons / joystick / rotation.

### P0-B — banc LIVI / CarPlay

1. Pi 4 sous Trixie ;
2. LIVI ;
3. affichage HDMI temporaire ;
4. clavier/HID ;
5. MFi prototype ;
6. détection `0x10` ;
7. CarPlay filaire ;
8. audio/micro/Siri.

### P1 — commande au volant

Cartographie électrique acquise. Reste :

1. fonction du bouton inférieur ;
2. sens fonctionnel de la molette ;
3. test rebonds / transitions rapides ;
4. prototype RP2040.

Autres P1 : écran final, caméra, audio, alimentation automobile.

### P2

- PCB V1 ;
- double CAN prototype.

### P3

- télémétrie CAN avancée ;
- ELS27 uniquement si nécessaire.

## 17. Achats court terme

Voir `BOM.md`.

Actuellement : écran 7" à sélectionner, RP2040 prototype, MFi `MFI343S00177-L`, passifs MFi, load-switch MFi et petit matériel de laboratoire.

## 18. Prochaine action concrète

### CSW

**Carte hors tension.**

Cartographier `Q1` : déterminer quelle patte rejoint `C25+`, quelles pattes rejoignent `IC1` et si une liaison existe vers `C3+`.

Ne pas alimenter le module avant compréhension suffisante de cet étage.

### Commande au volant

Le brochage passif est acquis. Prochaine validation utile : déterminer quel sens physique de la molette correspond à `2+6 → 2+3 → 2+1`, puis tester les rebonds avant implémentation RP2040.

### En parallèle

Banc Raspberry Pi 4 + LIVI sous Trixie avec écran HDMI temporaire, puis prototype MFi selon `docs/MFI_WIRING.md`.
