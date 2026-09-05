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
- reverse ;
- ACC / illumination ;
- USB HID.

La commande au volant étant passive par contacts secs, elle doit être lue directement par le RP2040 et ne plus être connectée au décodeur OEM.

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

Le CSW-2000R est maintenant traité comme un nœud CAN à conserver mais à **isoler du réseau multimédia Renault**.

Architecture minimale visée :

```text
Pi SPI → MCP2518FD #1 → CAN privé 500 kbit/s → CSW-2000R
Pi SPI → MCP2518FD #2 → CAN véhicule
```

Le bus privé CSW ne doit pas être ponté directement vers le CAN OEM. Les commandes du CSW sont ainsi exclusives au Raspberry Pi.

Si deux réseaux véhicule distincts doivent être écoutés simultanément en plus du bus privé CSW, un troisième contrôleur CAN ou un troisième footprint optionnel devra être ajouté au PCB final.

L’écoute passive précède toute émission active sur les réseaux véhicule.

### K-Line

Optionnelle ; peuplée seulement si les mesures montrent qu’elle apporte une fonction utile.

### Caméra de recul

Doit fonctionner indépendamment de l’iPhone et idéalement rester disponible même si LIVI rencontre un problème.

### Audio Renault

Piste principale :

```text
Pi / LIVI → DAC / sortie ligne → AUX Renault → ampli / haut-parleurs d'origine
```

L'ancien autoradio est conservé pour la partie audio/amplification, mais il ne doit plus recevoir les commandes utilisateur du CSW ou de la commande au volant.

Le niveau de gain fixe, la sélection AUX, le wake/mute et l'éventuel besoin de messages OEM restent à valider sur véhicule.

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
- CAN privé CSW : commandes centrales Xanavi vers le Pi ;
- CAN véhicule : télémétrie / intégration véhicule ;
- étage d’alimentation : protections, démarrage/arrêt et rails propres.

Le Raspberry ne doit pas dépendre du RP2040 pour afficher la caméra si une stratégie de secours plus robuste est nécessaire. Cette séparation sera réévaluée lors du prototype caméra.

Le CAN n’est pas utilisé pour les commandes physiques lorsqu’une lecture directe plus simple et plus sûre est disponible ; exception : le CSW-2000R conserve son électronique d'origine et est lu sur un bus CAN privé dédié.
