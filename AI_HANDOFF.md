# AI / New Chat Handoff

## Checkpoint actif — 2026-09-10 : LIVI installé et démarré

- Banc Raspberry Pi 4 sur microSD, image avec bureau préparée avec Raspberry Pi Imager 1.7.2.
- SSH fonctionnel : `ssh pi@raspberry-carplay.local`. Aucun écran physique disponible pour le moment.
- OS mesuré : Debian GNU/Linux 13 (trixie), DEBIAN_VERSION_FULL=13.5 ; `uname -m = aarch64`.
- LIVI v8.3.0 installé dans `/home/pi/LIVI/LIVI.AppImage` (environ 309 Mio).
- Installateur desktop et AppImage épinglés à v8.3.0 après incompatibilité entre installateur main et release (marqueur sudoers `__PYTHON__` non remplacé). Validation sudoers réussie avec l'installateur correspondant.
- Après redémarrage : `graphical.target`, display-manager actif, session Wayland active ; lancement automatique par `/home/pi/.config/autostart/LIVI.desktop`.
- Logs : `/home/pi/.xsession-errors`. Le dossier `~/.config/LIVI/log/` était vide. `journalctl --user` ne retournait rien ; `sudo journalctl -b _UID=1000` permettait la lecture.
- Initialisation graphique confirmée : OpenGL ES 3.1, Mesa 26.2.1, pilote v3d / Broadcom V3D 4.2.14.0. Compositeur LIVI sur wayland-1 au-dessus du bureau wayland-0, sortie logique main 1280×752, fenêtre LIVI affectée à main.
- GStreamer 1.28.4 embarqué détecte H.264/H.265 matériels et logiciels ; aucun flux réel encore validé.
- Assistant Python et surveillance CarPlay filaire démarrés ; zéro iPhone actif.
- MFi non raccordé/configuré pour ce test : erreur d'initialisation FileNotFoundError attendue dans ce contexte. CarPlay natif NON validé. Les GPIO/bus MFi documentés restent une cible de câblage, pas une configuration matérielle validée.
- Sans-fil désactivé (aaWireless=false, cpWireless=false), Bluetooth et point d'accès inactifs.
- Avertissements VA-API/Vulkan, RTKit et portail Wayland observés : pas de blocage du démarrage démontré ; ne pas modifier au hasard la pile graphique.
- Connexion du RP2040 au Pi explicitement reportée par David. Le prototype boutons/mute/molette du 8 septembre reste acquis ; USB HID pas encore implémenté.
- Prochaine étape proposée : préparer un accès au bureau à distance sans écran pour voir/configurer LIVI, en vérifiant d'abord la compatibilité Wayland et l'absence de sortie physique. Aucun accès graphique distant n'a encore été installé ou validé.
- Restent à valider : image réelle, navigation, vidéo projetée, audio/micro/Siri, MFi et CarPlay, puis HID. Aucun de ces résultats ne doit être déduit de la seule présence des processus.


Ce fichier est destiné à toute reprise du projet après changement de chat, de machine ou de contributeur.

## Ordre de lecture obligatoire

1. `PROJECT_STATE.md`
2. `docs/TEST_LOG.md`
3. `docs/DECISIONS.md`
4. `ROADMAP.md`
5. document du sous-système actuellement travaillé
6. captures / mesures associées

Pour CarPlay/MFi, lire obligatoirement :

- `docs/LIVI_CARPLAY_SETUP.md`
- `docs/MFI_WIRING.md`

## Hiérarchie de confiance

1. mesures faites sur notre Espace IV ou nos pièces ;
2. schémas constructeur Renault ;
3. datasheets fabricants ;
4. documentation/code des projets tiers ;
5. plusieurs schémas tiers indépendants concordants ;
6. reverse engineering public documenté ;
7. forums / commentaires isolés ;
8. hypothèses de travail.

Une mesure nouvelle doit remplacer une hypothèse ancienne.

## Règle de reprise

Avant de proposer une modification matérielle ou logicielle :

- vérifier `PROJECT_STATE.md` ;
- vérifier les décisions dans `docs/DECISIONS.md` ;
- vérifier les dernières mesures dans `docs/TEST_LOG.md` ;
- vérifier le document du sous-système ;
- ne pas promouvoir une information externe en `MEASURED` tant qu’elle n’a pas été vérifiée sur notre matériel.

Ne pas repartir de zéro et ne pas réintroduire une solution déjà écartée sans raison nouvelle.

## Décisions CarPlay / MFi à ne pas perdre

### D012 — MFi avant CarPlay natif

CarPlay natif LIVI nécessite un coprocesseur MFi physique.

Ordre :

1. OS + LIVI + affichage/HID hors CarPlay ;
2. MFi de laboratoire ;
3. détection I²C et validation LIVI ;
4. iPhone / CarPlay ;
5. Roole Map / audio / micro / Siri.

### D013 — Pi 4 sous Trixie

Pour LIVI actuel : Raspberry Pi OS / Debian 13 Trixie 64 bits sur Pi 4.

Ne pas proposer une ancienne base Bullseye/Bookworm sans nouvelle vérification explicite de LIVI.

### D014 — GPIO21 n’alimente pas directement le MFi dans le schéma de référence

Architecture :

```text
Pi 3.3 V → load-switch → MFI_VCC
GPIO21 → EN du load-switch
```

Les pull-up SDA/SCL vont vers `MFI_VCC` commuté.

### D015 — pinout MFi encore à confirmer physiquement

Pinout de travail recoupé publiquement :

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

Ce pinout n’est pas `MEASURED` tant que notre première carte n’a pas été inspectée/testée.

## Configuration LIVI MFi actuelle

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

Adresse CP3.0 attendue d’après WACResearch : `0x10`, à confirmer sur notre prototype.

## Points importants du projet

- Raspberry Pi 4 disponible ;
- iPhone / CarPlay cible principale ;
- Roole Map via CarPlay ;
- écran 7" non tactile ;
- façade écran refaite en impression 3D ;
- commande centrale `8200326970 / CSW-2000R` achetée ;
- commande volant `7701049643 / 34442201AF` achetée ;
- molette du commodo obligatoire ;
- MFi direct `MFI343S00177-L` cible principale ;
- Carlinkit pas cible finale ;
- RP2040 pour commandes physiques ;
- double CAN matériel futur ;
- ELS27 reportée ;
- caméra de recul obligatoire et indépendante du téléphone ;
- PCB final interdit avant mesures des commandes et validation des sous-systèmes critiques.

## Règle de documentation

Chaque résultat réel doit être ajouté immédiatement dans `docs/TEST_LOG.md`, puis répercuté dans `PROJECT_STATE.md` et/ou `docs/DECISIONS.md` s’il change une décision ou une hypothèse.
