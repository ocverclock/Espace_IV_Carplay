# ESP32 CAN Gateway — firmware actif

Statut : **NEXT IMPLEMENTATION TARGET — 2026-09-18**

Ce dossier est la cible logicielle active pour remplacer le banc SPI/MCP2518FD par le contrôleur TWAI natif de l'ESP32.

## Profils prévus

### Profil A — ESP32 classique / CSW

Un seul bus :

```text
TWAI0 -> transceiver -> CSW-2000R
```

Configuration :

```text
CAN 2.0 classique
500 kbit/s
mode normal
accept all au début
```

Objectif :

- fournir ACK ;
- recevoir `0x681` ;
- afficher timestamp / ID / DLC / data ;
- supprimer les répétitions idle ;
- comparer les payloads à la baseline `F0 0A 0A 01 FF FF FF FF`.

### Profil B — ESP32-C6 / double CAN

Deux bus :

```text
TWAI0 -> transceiver -> CSW privé
TWAI1 -> transceiver -> CAN véhicule
```

Objectif :

- deux instances TWAI simultanées ;
- taguer chaque trame avec son numéro de canal ;
- transférer vers Raspberry Pi ;
- recevoir du Raspberry des demandes d'émission ciblées ;
- listen-only par défaut sur le CAN véhicule ;
- mode normal sur le bus privé CSW.

## Liaison Raspberry

Voie prioritaire : UART direct.

Format binaire à définir avec :

- magic/version ;
- type ;
- canal ;
- timestamp ;
- ID ;
- flags ;
- DLC ;
- données ;
- checksum/CRC léger ou framing robuste.

USB CDC peut être gardé comme mode de développement.

## Sécurité

Aucune émission arbitraire sur le CAN véhicule.

Le bus privé CSW peut être utilisé en mode normal pour fournir ACK et, plus tard, émuler les trames connues du système multimédia d'origine.

## Références

- `docs/CAN_GATEWAY_ESP32.md`
- `docs/CAN_RESEARCH.md`
- `docs/CSW2000R.md`
