# CAN Research

## Objectif

Lire des informations véhicule et comprendre les réseaux de l’Espace IV.

## Règle

**Passif d’abord.**

Aucune injection de trames arbitraires pendant la phase d’identification.

## Architecture de test retenue — 2026-09-17

La référence pour les essais CAN est désormais :

```text
ESP32 → SPI → MCP2518FD → ATA6563 → CAN-H / CAN-L
```

Le module acheté utilise un **MCP2518FD + ATA6563** avec **quartz 40 MHz**.

Le MCP2518FD gère le protocole CAN/CAN-FD ; l’ATA6563 assure la couche physique différentielle. L’ESP32 n’utilise donc pas son contrôleur TWAI interne pour cette voie.

Le premier firmware doit fonctionner en **LISTEN ONLY** et afficher au minimum : timestamp, ID, DLC et données.

Documentation détaillée et projets de référence :

- [`ESP32_MCP2518FD_CAN.md`](ESP32_MCP2518FD_CAN.md)

Cette architecture est verrouillée comme base de travail et ne doit pas être remplacée par TWAI + transceiver simple sans nouvelle décision documentée.

## Matériel cible final

- MCP2518FD ;
- transceiver automobile ATA6563 ou équivalent validé ;
- 2 canaux prévus au PCB final.

## Linux

Interfaces souhaitées :
- `can0`
- `can1`

Outils :
- `ip link`
- `candump`
- `cansniffer`
- scripts Python `python-can`.

## Vérifications avant branchement

- vitesse bus ;
- CAN-H/CAN-L ;
- terminaison existante ;
- résistance du module de test ;
- masse ;
- topologie ;
- fréquence oscillateur MCP2518FD correctement réglée à **40 MHz** ;
- mode **LISTEN ONLY** pendant l’identification.

## Terminaison

Sur un bus véhicule déjà terminé, le module de sniff branché en dérivation ne doit pas ajouter une résistance `120 Ω` supplémentaire.

Sur banc isolé, la terminaison doit en revanche être adaptée à la topologie du réseau de test.

Le CSW-2000R a été mesuré à environ `37 kΩ` entre CAN-H et CAN-L hors tension ; il ne constitue pas une terminaison fixe `120 Ω`.

## Données cibles

- RPM ;
- vitesse ;
- température eau ;
- MAP / turbo ;
- rail ;
- EGR ;
- FAP ;
- états utiles multimédia ;
- trames et commandes du CSW-2000R.

## Historique interface OBD

### XTOOL A30M
Bluetooth SPP présent, mais protocole propriétaire. Non retenu comme interface PyRen.

### ELM327 Wi-Fi
- `192.168.0.10:35000`
- `ATI` : `ELM327 v1.5`
- `ATDP` : `ISO 15765-4 (CAN 11/500)`
- PID moteur : `CAN ERROR`

Non retenu.

### ELS27
Techniquement intéressante mais achat ~150 € reporté.
