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

## Configuration I²C LIVI

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

- [ ] Trixie 64 bits confirmé ;
- [ ] LIVI s’installe ;
- [ ] LIVI démarre ;
- [ ] affichage HDMI stable ;
- [ ] démarrage automatique ;
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

## Installation LIVI

Utiliser en priorité la procédure officielle actuelle du dépôt LIVI.

Pour une cible automobile kiosk, l’installation headless est la piste naturelle ; pour le premier débogage, une installation desktop peut rester utile.

Ne pas copier définitivement une commande d’installation dans ce dépôt sans vérifier qu’elle correspond encore à la branche/release LIVI utilisée.

## Critère de sortie de phase

La phase CarPlay sur établi est terminée seulement lorsque :

1. le Pi démarre proprement sous Trixie ;
2. LIVI démarre automatiquement ;
3. le MFi est détecté de manière reproductible ;
4. CarPlay filaire fonctionne ;
5. audio, micro, Siri et HID sont validés ;
6. au moins plusieurs redémarrages complets réussissent sans intervention manuelle.
