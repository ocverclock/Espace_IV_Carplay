# Architecture

## Objectif

Construire un système multimédia moderne sans dépendre d’un autoradio Android fermé et sans perdre l’ergonomie Renault.

## Hiérarchie documentaire

- `PROJECT_STATE.md` : source de vérité / checkpoint global ;
- `docs/DECISIONS.md` : décisions verrouillées et raisons ;
- `docs/TEST_LOG.md` : mesures et essais réels ;
- documents de sous-système : conception et procédures ;
- `captures/` et `references/` : preuves brutes et sources externes.

Une mesure réelle doit remplacer une hypothèse ancienne.

## Blocs

### Raspberry Pi 4

Rôle :
- Raspberry Pi OS / Debian 13 Trixie 64 bits ;
- LIVI / CarPlay ;
- affichage ;
- audio ;
- service caméra ;
- SocketCAN ;
- dashboard futur.

LIVI actuel nécessite OpenGL ES 3.x ; sa documentation indique Trixie pour Pi 4/CM4/Pi 5/CM5.

### RP2040

Rôle :
- entrées temps réel ;
- commande au volant ;
- CSW si nécessaire ;
- reverse ;
- ACC / illumination ;
- USB HID.

Le RP2040 ne fait pas le CAN principal et ne porte pas la pile CarPlay.

### MFi

Cible : `MFI343S00177-L` / CP3.0 pour authentification CarPlay native.

Interface de référence :

```text
Pi GPIO19 ───────────── SDA ───────────── MFi pin 5
Pi GPIO26 ───────────── SCL ───────────── MFi pin 6
Pi GPIO21 ───────────── EN load-switch
Pi 3.3 V ── load-switch ── MFI_VCC ───── MFi pin 8
GND ───────────────────────────────────── MFi 1/4/7/9
```

Les pull-up SDA/SCL sont alimentées depuis `MFI_VCC` commuté.

Document de référence : `docs/MFI_WIRING.md`.

### CAN

Deux contrôleurs séparés afin de pouvoir écouter simultanément deux réseaux.

Architecture visée :

```text
Pi SPI → MCP2518FD #1 → transceiver automobile → CAN véhicule
Pi SPI → MCP2518FD #2 → transceiver automobile → CAN secondaire
```

L’écoute passive précède toute émission active.

### K-Line

Optionnelle ; peuplée seulement si les mesures montrent qu’elle apporte une fonction utile.

### Caméra de recul

Doit fonctionner indépendamment de l’iPhone et idéalement rester disponible même si LIVI rencontre un problème.

### Alimentation automobile

Le Pi n’est pas alimenté directement depuis un +12 V véhicule brut.

Architecture cible :

```text
12 V véhicule
→ protections automobile
→ buck 5 V
→ Raspberry Pi
```

ACC/contact pilote une logique de shutdown propre et de coupure temporisée.

## Interfaces de responsabilité

- Linux/LIVI : interface utilisateur, CarPlay, audio, caméra, services haut niveau ;
- RP2040 : entrées physiques déterministes et HID ;
- MFi : authentification Apple uniquement ;
- contrôleurs CAN : accès réseau véhicule ;
- étage d’alimentation : protections, démarrage/arrêt et rails propres.

Le Raspberry ne doit pas dépendre du RP2040 pour afficher la caméra si une stratégie de secours plus robuste est nécessaire. Cette séparation sera réévaluée lors du prototype caméra.

Le CAN n’est pas utilisé pour les commandes physiques si une lecture directe plus simple et plus sûre est disponible.
