# CAN Research

## Objectif

Lire des informations véhicule et comprendre les réseaux de l’Espace IV.

## Règle

**Passif d’abord.**

Aucune injection de trames arbitraires pendant la phase d’identification.

## Matériel cible

- MCP2518FD
- transceiver automobile type ATA6560 ou équivalent
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
- topologie.

## Données cibles

- RPM ;
- vitesse ;
- température eau ;
- MAP / turbo ;
- rail ;
- EGR ;
- FAP ;
- états utiles multimédia.

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
