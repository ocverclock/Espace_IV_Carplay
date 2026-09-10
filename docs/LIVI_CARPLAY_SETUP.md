# LIVI / CarPlay Setup

## Cible

- Raspberry Pi 4
- Raspberry Pi OS / Debian 13 Trixie 64 bits
- LIVI
- iPhone
- Apple CarPlay natif
- Roole Map
- commandes physiques
- écran HDMI 7" non tactile

## Pré-requis plateforme

La documentation actuelle de LIVI indique que Pi 4 / CM4 / Pi 5 / CM5 nécessitent Trixie pour OpenGL ES 3.x.

Décision projet : voir `D013` dans `docs/DECISIONS.md`.

Avant installation :

```bash
cat /etc/os-release
uname -m
```

Attendu :

```text
Debian/Raspberry Pi OS 13 (Trixie)
arm64 / aarch64
```

## Dépendance critique : MFi

CarPlay natif nécessite un coprocesseur MFi physique. LIVI communique directement avec lui par I²C.

Sans MFi :

- LIVI peut démarrer ;
- l’affichage peut être validé ;
- les entrées clavier/D-Pad/HID peuvent être préparées ;
- CarPlay natif n’est pas disponible.

Le câblage électrique de référence est documenté dans :

`docs/MFI_WIRING.md`

Décisions associées : `D012`, `D014`, `D015`.

## Configuration I²C LIVI cible — non activée sur le banc sans MFi

LIVI utilise actuellement :

```text
carPlayMfiI2cBus = 2
carPlayMfiPowerGpio = 21
```

Son installation Raspberry Pi configure :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
```

Notre câblage :

```text
GPIO19 = SDA
GPIO26 = SCL
GPIO21 = EN du load-switch MFi
3.3 V → load-switch → MFI_VCC
```

GPIO21 n’est pas considéré comme une alimentation directe de la pin VCC dans l’architecture de référence.

## Ordre de validation sur établi

### Étape 1 — OS / LIVI sans CarPlay

- [x] Trixie 64 bits confirmé ;
- [x] LIVI s’installe ;
- [x] LIVI démarre ;
- [ ] affichage HDMI physique stable ;
- [x] démarrage automatique ;
- [x] affichage distant Wayland/VNC validé ;
- [x] LIVI configuré en plein écran dans ses paramètres ;
- [ ] navigation clavier fonctionne ;
- [ ] événements HID externes fonctionnent.

### Étape 2 — carte MFi sans iPhone

Composant cible :

- `MFI343S00177-L`

Valider dans l’ordre :

- [ ] orientation / footprint contrôlés ;
- [ ] absence de court-circuit VCC/GND ;
- [ ] load-switch testé ;
- [ ] GPIO21 coupe et active `MFI_VCC` ;
- [ ] `MFI_VCC` mesuré ;
- [ ] courant MFi mesuré ;
- [ ] SDA/SCL au bon niveau de repos ;
- [ ] `/dev/i2c-2` présent ;
- [ ] adresse I²C `0x10` détectée ;
- [ ] logs LIVI sans erreur MFi bloquante.

Toutes les mesures vont dans `docs/TEST_LOG.md`.

### Étape 3 — CarPlay natif

Une fois le MFi validé :

- [ ] iPhone se connecte en filaire ;
- [ ] CarPlay apparaît ;
- [ ] Roole Map fonctionne ;
- [ ] navigation clavier/HID pilote correctement CarPlay ;
- [ ] audio média fonctionne ;
- [ ] appel entrant/sortant ;
- [ ] micro ;
- [ ] Siri ;
- [ ] reconnexion après reboot.

### Étape 4 — CarPlay sans fil

Après réussite du filaire :

- [ ] Bluetooth fonctionnel ;
- [ ] interface Wi-Fi dédiée ou stratégie réseau choisie ;
- [ ] appairage sans fil ;
- [ ] reconnexion automatique ;
- [ ] stabilité après plusieurs cycles démarrage/arrêt.

## Installation LIVI — banc validé le 2026-09-10

Source officielle figée : [installateur desktop v8.3.0](https://github.com/f-io/LIVI/blob/v8.3.0/scripts/install/desktop/install.sh) et [bibliothèque common.sh v8.3.0](https://github.com/f-io/LIVI/blob/v8.3.0/scripts/install/common.sh).

L'installation via main avec la release v8.3.0 a échoué : la fonction de génération sudoers de main ne remplaçait plus `__PYTHON__`, présent dans le modèle embarqué de v8.3.0. `visudo` a rejeté le fichier temporaire avant activation. La reprise avec les scripts et l'AppImage v8.3.0 a terminé avec succès.

Procédure utilisée depuis SSH, comme utilisateur pi, sans sudo devant le script :

```bash
mkdir -p ~/installation-livi
cd ~/installation-livi
curl -fL -o install-desktop-v8.3.0.sh https://raw.githubusercontent.com/f-io/LIVI/v8.3.0/scripts/install/desktop/install.sh
LIVI_INSTALLER_BRANCH=v8.3.0 bash install-desktop-v8.3.0.sh \
  https://github.com/f-io/LIVI/releases/download/v8.3.0/LIVI-8.3.0-linux-arm64.AppImage
```

Options prévues pour ce banc : pas de configuration MFi sans matériel, pas de splash, pas de modification du pilote pour écran RGB/VGA basse fréquence. Cette commande retélécharge l'AppImage. Ne pas la relancer simplement pour diagnostiquer une installation déjà terminée.

L'installateur ajoute les dépendances (dont pymobiledevice3 dans cette version), les règles udev/sudoers et l'autostart desktop. Le redémarrage a confirmé le lancement effectif dans la session graphique.

Pour une future mise à jour, vérifier ensemble la version de l'application et de l'installateur. Le mode desktop est conservé pour le banc ; l'absence temporaire d'écran n'a pas entraîné de conversion au mode kiosk/headless.

## Accès bureau distant WayVNC — validé le 2026-09-10

Le Pi fonctionne en session Wayland avec `labwc` sur `wayland-0`. WayVNC est utilisé uniquement pour le travail et le diagnostic à distance.

### Autostart WayVNC

Le lancement automatique est ajouté dans :

```text
~/.config/labwc/autostart
```

avec :

```bash
wayvnc 0.0.0.0 5900 &
```

Après reboot, la présence des deux processus est vérifiée avec :

```bash
pgrep -af 'labwc|wayvnc'
```

État validé après redémarrage :

```text
/usr/bin/labwc -m
wayvnc 0.0.0.0 5900
```

Il n’existe pas de `wayvnc.service` systemd utilisateur dans cette configuration ; WayVNC est volontairement lancé par l’autostart de `labwc`, après disponibilité de la session Wayland.

### Connexion VNC via tunnel SSH

Depuis le PC client :

```bash
ssh -N -L 15900:127.0.0.1:5900 pi@raspberry-carplay.local
```

Laisser ce terminal ouvert pendant la session VNC.

Le client VNC se connecte ensuite à :

```text
127.0.0.1:15900
```

ou :

```text
localhost:15900
```

Cette méthode évite d’utiliser directement le port VNC du Pi depuis le PC : le trafic VNC passe dans le tunnel SSH.

### Affichage LIVI

Après accès VNC, LIVI a été réglé dans ses propres paramètres pour occuper tout l’écran. Après reboot, LIVI démarre automatiquement et reprend le plein écran. Ce point est considéré comme validé côté session graphique distante ; l’écran HDMI physique reste à valider quand il sera disponible.

## Checkpoint actif — 2026-09-10 : LIVI + accès distant opérationnels

- Banc Raspberry Pi 4 sur microSD, image avec bureau préparée avec Raspberry Pi Imager 1.7.2.
- SSH fonctionnel : `ssh pi@raspberry-carplay.local`.
- OS mesuré : Debian GNU/Linux 13 (trixie), DEBIAN_VERSION_FULL=13.5 ; `uname -m = aarch64`.
- LIVI v8.3.0 installé dans `/home/pi/LIVI/LIVI.AppImage` (environ 309 Mio).
- Installateur desktop et AppImage épinglés à v8.3.0 après incompatibilité entre installateur main et release (marqueur sudoers `__PYTHON__` non remplacé). Validation sudoers réussie avec l'installateur correspondant.
- Après redémarrage : `graphical.target`, display-manager actif, session Wayland active ; lancement automatique par `/home/pi/.config/autostart/LIVI.desktop`.
- `labwc` démarre correctement sur la session Wayland.
- WayVNC est installé, démarre automatiquement via `~/.config/labwc/autostart` et reste présent après reboot.
- Accès VNC validé via tunnel SSH local `15900 -> 127.0.0.1:5900`, client VNC sur `127.0.0.1:15900`.
- LIVI a été réglé depuis son interface pour démarrer/occuper tout l’écran ; comportement validé après reboot via VNC.
- Logs : `/home/pi/.xsession-errors`. Le dossier `~/.config/LIVI/log/` était vide. `journalctl --user` ne retournait rien ; `sudo journalctl -b _UID=1000` permettait la lecture.
- Initialisation graphique confirmée : OpenGL ES 3.1, Mesa 26.2.1, pilote v3d / Broadcom V3D 4.2.14.0. Compositeur LIVI sur wayland-1 au-dessus du bureau wayland-0, sortie logique main 1280×752, fenêtre LIVI affectée à main.
- GStreamer 1.28.4 embarqué détecte H.264/H.265 matériels et logiciels ; aucun flux réel encore validé.
- Assistant Python et surveillance CarPlay filaire démarrés ; zéro iPhone actif.
- MFi non raccordé/configuré pour ce test : erreur d'initialisation FileNotFoundError attendue dans ce contexte. CarPlay natif NON validé. Les GPIO/bus MFi documentés restent une cible de câblage, pas une configuration matérielle validée.
- Sans-fil désactivé (aaWireless=false, cpWireless=false), Bluetooth et point d'accès inactifs.
- Avertissements VA-API/Vulkan, RTKit et portail Wayland observés : pas de blocage du démarrage démontré ; ne pas modifier au hasard la pile graphique.
- Connexion du RP2040 au Pi explicitement reportée par David. Le prototype boutons/mute/molette du 8 septembre reste acquis ; USB HID pas encore implémenté.
- Prochaine étape : poursuivre les essais d’interface/navigation sur LIVI depuis VNC, puis préparer la validation MFi + iPhone + CarPlay filaire.
- Restent à valider : écran HDMI physique, navigation clavier/HID, vidéo projetée, audio/micro/Siri, MFi et CarPlay. Aucun de ces résultats ne doit être déduit de la seule présence des processus.

## Critère de sortie de phase

La phase CarPlay sur établi est terminée seulement lorsque :

1. le Pi démarre proprement sous Trixie ;
2. LIVI démarre automatiquement ;
3. le MFi est détecté de manière reproductible ;
4. CarPlay filaire fonctionne ;
5. audio, micro, Siri et HID sont validés ;
6. au moins plusieurs redémarrages complets réussissent sans intervention manuelle.
