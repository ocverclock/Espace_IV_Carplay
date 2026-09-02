# Espace IV Modernisation Multimédia — PROJECT_STATE

Dernière mise à jour : 2026-09-02

Ce fichier est le **checkpoint global et la source de vérité principale** du projet.

Il ne doit pas contenir tous les détails techniques : ceux-ci vivent dans les documents de sous-système. Ici doivent rester l’architecture, les décisions actives, les états mesurés et les prochaines étapes.

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

## 4. Matériel déjà disponible / acheté

### Disponible

- Raspberry Pi 4 ;
- PC Linux ;
- imprimante 3D ;
- XTOOL A30M pour diagnostic classique ;
- ancien ELM327 Wi-Fi non retenu pour le projet.

### Acheté pour reverse engineering

Commande centrale :

```text
Renault 8200326970
CSW-2000R
Xanavi Informatics Corporation
connecteur 6 voies
```

Commande au volant :

```text
Renault 7701049643
34442201AF
connecteur 6 voies
```

Ces pièces sont destinées au laboratoire pour éviter de risquer les pièces du véhicule.

## 5. Architecture cible

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
        │  ├─ CSW-2000R                                     │
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

## 6. Raspberry Pi / LIVI

Décision active :

- Raspberry Pi 4 ;
- Raspberry Pi OS / Debian 13 Trixie 64 bits ;
- LIVI ;
- cible kiosk automobile à terme.

Raison : LIVI actuel requiert OpenGL ES 3.x et documente Trixie pour Pi 4/CM4/Pi 5/CM5.

Décision : `D013`.

Document : `docs/LIVI_CARPLAY_SETUP.md`.

## 7. CarPlay / MFi

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

Il est incorrect de planifier « valider CarPlay puis acheter le MFi ».

Décision : `D012`.

### Composant cible

```text
Microchip MFI343S00177-L
Authentication Coprocessor 3.0
LCSC C33770534
```

### Configuration LIVI actuelle

```text
I²C bus = 2
SDA = GPIO19
SCL = GPIO26
Power control = GPIO21
```

Overlay LIVI actuel :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
```

### Architecture alimentation MFi

Le schéma de référence n’utilise pas GPIO21 pour alimenter directement VCC.

```text
Pi 3.3 V → load-switch → MFI_VCC
GPIO21 → EN du load-switch
```

Les pull-up SDA/SCL vont vers `MFI_VCC` commuté.

Décision : `D014`.

### Pinout de travail

Recoupé par plusieurs schémas publics :

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

Ce pinout n’est **pas encore MEASURED** sur notre prototype.

Avant PCB : revérifier orientation/footprint fournisseur et contrôler la première carte hors tension.

Décision : `D015`.

Adresse I²C CP3.0 attendue selon WACResearch :

```text
0x10
```

À confirmer sur notre propre composant.

Document : `docs/MFI_WIRING.md`.

## 8. Écran

Décisions :

- 7 pouces ;
- non tactile ;
- IPS préféré ;
- HDMI préféré ;
- luminosité élevée ;
- nouvelle façade imprimée 3D.

L’ancienne fenêtre visible d’environ `130 × 70 mm` n’est plus considérée comme limite définitive.

Critères avant achat :

- dimensions externes ;
- connecteurs ;
- luminosité ;
- angle de vision ;
- consommation ;
- température ;
- résolution ;
- possibilité de gérer proprement le rétroéclairage.

Cible de luminosité : idéalement ≥500 nits si prix raisonnable.

Document : `docs/DISPLAY.md`.

## 9. Commande au volant

Référence : `7701049643 / 34442201AF`.

État :

- pièce achetée ;
- reverse engineering à faire ;
- probablement passive / contacts-matrice ;
- molette obligatoire dans la reproduction.

Ne pas supposer son codage sans mesure.

Plan :

1. inspection ;
2. tester les 15 couples de broches ;
3. relever tous les boutons ;
4. décoder la séquence de la molette ;
5. prototype RP2040 USB HID.

Documents :

- `docs/STEERING_REMOTE.md`
- `docs/CONTROLS_REVERSE_ENGINEERING.md`

## 10. Commande centrale CSW-2000R

Référence : `8200326970 / CSW-2000R`.

État :

- pièce achetée ;
- électronique interne confirmée ;
- protocole, alimentation et pinout inconnus ;
- ne pas supposer CAN.

Plan :

1. ouvrir ;
2. photographier PCB ;
3. relever circuits intégrés ;
4. continuité des 6 broches ;
5. identifier alimentation ;
6. alimentation labo limitée en courant ;
7. analyser protocole.

Fallback : conserver mécanique/joystick ALPS et remplacer l’électronique interne par RP2040 si nécessaire.

Document : `docs/CSW2000R.md`.

## 11. RP2040

Rôle :

- commandes physiques ;
- debounce ;
- molette ;
- éventuellement CSW ;
- reverse ;
- ACC/illumination ;
- USB HID vers Pi.

Le RP2040 ne fait pas le CAN principal.

Prototype : RP2040-Zero/Pico.

## 12. Mapping fonctionnel provisoire

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

## 13. Audio / mains libres

Architecture cible :

```text
Pi → DAC / ligne → AUX Renault → amplification OEM
```

Micro :

1. essayer micro Renault d’origine ;
2. fallback micro automobile externe discret.

À valider :

- présence/activation AUX ;
- niveau ligne ;
- bruit de masse ;
- isolateur éventuel ;
- micro ;
- appels/Siri.

Document : `docs/AUDIO_MIC.md`.

## 14. Caméra de recul

Obligatoire.

Exigences :

- automatique en marche arrière ;
- retour automatique ;
- faible latence ;
- fonctionne sans iPhone ;
- idéalement <1 s ;
- watchdog / stratégie robuste si LIVI plante.

Technologie encore ouverte : CVBS / AHD / USB UVC.

Document : `docs/REVERSE_CAMERA.md`.

## 15. CAN véhicule

Objectifs futurs :

- température liquide ;
- RPM ;
- vitesse ;
- pression admission/turbo ;
- rail ;
- FAP ;
- EGR ;
- autres données Renault.

### Tests déjà réalisés

XTOOL A30M :

- Bluetooth SPP présent ;
- protocole propriétaire ;
- non exploitable comme ELM327 direct.

Ancien ELM327 Wi-Fi :

```text
192.168.0.10:35000
ATI → ELM327 v1.5
ATDP → ISO 15765-4 CAN 11/500
010C / 0105 / 010B → CAN ERROR
```

Conclusion : interface abandonnée pour ce projet.

ELS27 V5/V5.2 Full : utile mais ~150 €, achat reporté.

### Architecture finale

```text
Pi SPI → MCP2518FD #1 → transceiver → CAN véhicule
Pi SPI → MCP2518FD #2 → transceiver → CAN secondaire
```

K-Line optionnelle `L9637D`.

Règles :

- écoute passive d’abord ;
- pas d’émission active avant compréhension ;
- ne pas considérer OBD 12/13 comme CAN secondaire sans preuve ;
- attention aux terminaisons 120 Ω.

Document : `docs/CAN_RESEARCH.md`.

## 16. Alimentation automobile

Ne jamais alimenter naïvement le Pi depuis un +12 V véhicule permanent.

Cible :

```text
12 V véhicule
→ protections automobile
→ buck 5 V
→ Raspberry Pi
```

ACC/contact :

- détection protégée ;
- shutdown Linux propre ;
- délai ;
- coupure finale ;
- faible consommation véhicule arrêté.

Document : `docs/POWER.md`.

## 17. PCB final

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

- commandes Renault non mesurées ;
- MFi non validé sur banc ;
- orientation/footprint MFi non revérifiés ;
- alimentation non validée ;
- stratégie écran non stabilisée.

## 18. Priorités actuelles

### P0 — banc fonctionnel

1. Pi 4 sous Trixie ;
2. LIVI ;
3. affichage HDMI temporaire ;
4. clavier/HID ;
5. MFi prototype ;
6. détection `0x10` ;
7. CarPlay filaire ;
8. audio/micro/Siri.

### P1 — commandes / usage quotidien

- reverse engineering commandes Renault ;
- écran final ;
- caméra ;
- audio ;
- alimentation automobile.

### P2 — intégration électronique

- PCB V1 ;
- double CAN prototype.

### P3 — télémétrie avancée

- CAN Renault ;
- ELS27 uniquement si réellement nécessaire.

## 19. Achats court terme

Voir `BOM.md`.

Actuellement :

- écran 7" à sélectionner ;
- RP2040 prototype ;
- MFi `MFI343S00177-L` ;
- passifs MFi ;
- load-switch MFi à sélectionner après mesure ;
- petit matériel de laboratoire.

Le MFi n’est plus classé en achat « après validation » : il est nécessaire pour effectuer la validation CarPlay.

## 20. Questions ouvertes majeures

### MFi

- consommation réelle ?
- validation 3.3 V sur notre lot ?
- adresse `0x10` confirmée ?
- load-switch exact ?
- authentification CarPlay réussie ?

### Commandes

- pinout CSW ?
- protocole CSW ?
- matrice commande volant ?
- séquence molette ?

### Écran

- modèle final ?
- dimensions module ?
- luminosité réelle ?

### Audio

- AUX Renault ?
- micro OEM réutilisable ?

### Caméra

- CVBS/AHD/UVC ?
- capture Linux faible latence ?

### CAN

- topologie réelle de cet Espace IV ?
- bus secondaire accessible ?
- valeurs passives disponibles ?

### Alimentation

- point +12 V / ACC définitif ?
- délai shutdown ?
- UPS/supercap nécessaire ou non ?

## 21. Prochaine action concrète

**Banc Raspberry Pi 4 + LIVI sous Trixie, avec écran HDMI temporaire.**

Avant CarPlay :

1. vérifier OS ;
2. installer LIVI ;
3. valider affichage ;
4. valider clavier/HID ;
5. construire le prototype MFi selon `docs/MFI_WIRING.md` ;
6. mesurer et documenter dans `docs/TEST_LOG.md`.
