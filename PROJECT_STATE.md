# Espace IV Modernisation Multimédia — PROJECT_STATE

Dernière mise à jour : 2026-09-04

Ce fichier est le **checkpoint global et la source de vérité principale** du projet.

Il contient uniquement l’architecture, les faits mesurés, les décisions actives et les prochaines étapes. Les détails techniques vivent dans les documents de sous-système.

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

### Mécanique / connecteur

`CN1` possède **12 voies, 2 × 6**.

Vue dans `CN1`, détrompeur en bas :

```text
haut : 7  8  9 10 11 12
bas  : 1  2  3  4  5  6
```

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

Les anciennes mentions « 6 voies » ou les schémas avec `1..6` sur la rangée supérieure sont invalidés.

### Masse

Carte hors tension :

```text
CN1 pin 2 = GND
CN1 pin 8 = GND
```

Statut : **MEASURED**.

### Paire CAN

Traçage utilisateur :

```text
CN1 pin 6  → TDK ZJY2401
CN1 pin 12 → TDK ZJY2401
```

Le `TDK ZJY2401` est un filtre de mode commun placé sur une paire de lignes.

`IC3` porte le marquage :

```text
A82C250
5K3A3
N5464
PHILIPS
```

Ce composant est identifié comme **Philips/NXP PCA82C250**, transceiver CAN haute vitesse ISO 11898.

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

Conclusion :

```text
CSW-2000R utilise un bus CAN = CONFIRMÉ
CN1-6 / CN1-12 = paire CAN via ZJY2401 = CONFIRMÉ
```

Reste à déterminer :

```text
CN1-6  = CANH ou CANL ?
CN1-12 = CANL ou CANH ?
```

### MCU

MCU principal NEC Japan en QFP + quartz `X1` visibles.

Le marquage semble compatible avec une famille Renesas/NEC `78K0/Kx2`, possiblement `µPD78F052xA`, mais la référence exacte n’est pas encore verrouillée.

### Alimentation

Le `PCA82C250` confirme l’existence d’un rail interne **5 V** (`VCC pin 3`, nominal 4,5–5,5 V).

L’entrée positive côté `CN1` et sa tension véhicule ne sont **pas encore identifiées**.

**Ne pas appliquer 12 V tant que ce chemin n’est pas tracé.**

Document détaillé : `docs/CSW2000R.md`.

## 5. Commande au volant

Référence :

```text
Renault 7701049643
34442201AF
connecteur 6 voies
```

État :

- pièce achetée ;
- reverse engineering à faire ;
- probablement passive / contacts-matrice ;
- molette obligatoire dans la reproduction.

Plan :

1. inspection ;
2. tester les 15 couples de broches ;
3. relever les boutons ;
4. décoder la molette ;
5. prototype RP2040 USB HID.

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

Le CSW étant désormais confirmé CAN, deux stratégies restent possibles :

1. conserver le CSW d’origine comme nœud CAN et décoder ses trames ;
2. fallback RP2040 interne uniquement si le protocole CAN Xanavi est trop coûteux à reproduire.

La stratégie 1 devient prioritaire.

## 7. Raspberry Pi / LIVI

Décision active :

- Raspberry Pi 4 ;
- Raspberry Pi OS / Debian 13 Trixie 64 bits ;
- LIVI ;
- cible kiosk automobile à terme.

Décision : `D013`.

Document : `docs/LIVI_CARPLAY_SETUP.md`.

## 8. CarPlay / MFi

### Règle fondamentale

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

### Composant cible

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

Overlay :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
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

Pas encore MEASURED sur notre prototype.

Décision : `D015`.

Adresse I²C attendue CP3.0 : `0x10`, à confirmer.

Document : `docs/MFI_WIRING.md`.

## 9. Écran

Décisions :

- 7 pouces ;
- non tactile ;
- IPS préféré ;
- HDMI préféré ;
- luminosité élevée ;
- nouvelle façade imprimée 3D.

L’ancienne fenêtre visible d’environ `130 × 70 mm` n’est plus une limite définitive.

Cible : idéalement ≥500 nits si prix raisonnable.

Document : `docs/DISPLAY.md`.

## 10. RP2040

Rôle prévu :

- commande au volant ;
- debounce ;
- molette ;
- reverse ;
- ACC/illumination ;
- USB HID vers Pi.

Pour le CSW, le RP2040 n’est plus la voie principale tant que le décodage CAN d’origine reste réaliste.

Le RP2040 ne fait pas le CAN principal.

Prototype : RP2040-Zero/Pico.

## 11. Mapping fonctionnel provisoire

À valider après tests LIVI :

- `MAP 2D/3D` → CarPlay / Roole Map ;
- `INFO/ROUTE` → dashboard véhicule ;
- `MENU/SET` → menu système ;
- `BACK` → retour ;
- `DEST/HOME` → accueil/navigation ;
- `REPEAT/MUTE` → mute/média ;
- `LIGHT/DARK` → jour/nuit ;
- joystick → navigation UI ;
- molette → navigation/next-prev ;
- clic → validation ;
- appui long à définir → Siri.

## 12. Audio / mains libres

Architecture cible :

```text
Pi → DAC / ligne → AUX Renault → amplification OEM
```

Micro :

1. essayer micro Renault d’origine ;
2. fallback micro automobile externe discret.

Document : `docs/AUDIO_MIC.md`.

## 13. Caméra de recul

Obligatoire.

Exigences :

- automatique en marche arrière ;
- retour automatique ;
- faible latence ;
- fonctionne sans iPhone ;
- idéalement <1 s ;
- watchdog / stratégie robuste si LIVI plante.

Technologie ouverte : CVBS / AHD / USB UVC.

Document : `docs/REVERSE_CAMERA.md`.

## 14. CAN véhicule / architecture finale

Objectifs futurs : température, RPM, vitesse, pression admission/turbo, rail, FAP, EGR et autres données Renault.

Tests antérieurs :

- XTOOL A30M : Bluetooth SPP mais protocole propriétaire ;
- ancien ELM327 Wi-Fi : `ATI` OK, `ATDP` CAN 11/500, PID standard → `CAN ERROR` ; interface abandonnée ;
- ELS27 V5/V5.2 Full : achat reporté (~150 €).

Architecture finale envisagée :

```text
Pi SPI → MCP2518FD #1 → transceiver → CAN véhicule
Pi SPI → MCP2518FD #2 → transceiver → CAN secondaire / multimédia
```

La découverte du `PCA82C250` dans le CSW confirme maintenant qu’au moins la commande centrale utilise physiquement un **bus CAN**.

Cela renforce l’intérêt d’un second canal CAN dédié au réseau multimédia.

Règles :

- écoute passive d’abord ;
- aucune émission active avant compréhension ;
- aucune hypothèse sur OBD 12/13 sans preuve ;
- attention aux terminaisons 120 Ω.

Document : `docs/CAN_RESEARCH.md`.

## 15. Alimentation automobile

Cible :

```text
12 V véhicule
→ protections automobile
→ buck 5 V
→ Raspberry Pi
```

ACC/contact : détection protégée, shutdown Linux propre, temporisation, coupure finale, faible consommation à l’arrêt.

Document : `docs/POWER.md`.

## 16. PCB final

Objectif : une carte unique Espace IV intégrant :

- RP2040 ;
- MFi + load-switch ;
- double CAN ;
- K-Line optionnelle ;
- reverse ;
- ACC/illumination ;
- protections ;
- connecteurs ;
- points de test.

### Gate avant lancement PCB V1

Ne pas envoyer le PCB tant que :

- commandes Renault suffisamment mesurées ;
- MFi validé sur banc ;
- orientation/footprint MFi revérifiés ;
- alimentation validée ;
- stratégie écran stabilisée.

## 17. Priorités actuelles

### P0-A — CSW-2000R

Etat :

```text
GND = pins 2 et 8
CAN pair = pins 6 et 12
CAN transceiver = PCA82C250
CAN = CONFIRMED
```

Prochaines étapes :

1. déterminer `CN1-6` / `CN1-12` = CANH / CANL via PCA82C250 pins 6/7 ;
2. suivre `PCA82C250 pin 3` vers le rail 5 V ;
3. identifier le régulateur et l’entrée positive CN1 ;
4. suivre TXD/RXD vers le MCU ;
5. alimenter en labo avec limitation de courant ;
6. déterminer bitrate CAN ;
7. capturer trames au repos / boutons / joystick ;
8. documenter IDs et payloads.

### P0-B — banc LIVI / CarPlay

1. Pi 4 sous Trixie ;
2. LIVI ;
3. affichage HDMI temporaire ;
4. clavier/HID ;
5. MFi prototype ;
6. détection `0x10` ;
7. CarPlay filaire ;
8. audio/micro/Siri.

### P1 — usage quotidien

- commande au volant ;
- écran final ;
- caméra ;
- audio ;
- alimentation automobile.

### P2 — intégration électronique

- PCB V1 ;
- double CAN prototype.

### P3 — télémétrie avancée

- CAN Renault ;
- ELS27 uniquement si nécessaire.

## 18. Achats court terme

Voir `BOM.md`.

Actuellement :

- écran 7" à sélectionner ;
- RP2040 prototype ;
- MFi `MFI343S00177-L` ;
- passifs MFi ;
- load-switch MFi à sélectionner après mesure ;
- petit matériel de laboratoire.

## 19. Prochaine action concrète

### CSW

**Carte hors tension.**

Le prochain test le plus utile est :

```text
PCA82C250 pin 6 = CANL → suivre vers CN1
PCA82C250 pin 7 = CANH → suivre vers CN1
```

Objectif : fixer définitivement :

```text
CN1-6  = CANH ou CANL
CN1-12 = CANL ou CANH
```

Puis suivre `PCA82C250 pin 3 = VCC 5 V` vers l’alimentation.

### En parallèle

Banc Raspberry Pi 4 + LIVI sous Trixie avec écran HDMI temporaire, puis prototype MFi selon `docs/MFI_WIRING.md`.
