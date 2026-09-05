# Module SPI vers CAN FD MCP2518FD — banc CSW-2000R

## Identification d'après la notice fournisseur

Module : SPI vers CAN FD autour du `MCP2518FD`.

Caractéristiques utiles documentées :

- contrôleur : `MCP2518FD` ;
- transceiver physique : `ATA6563` ;
- compatible CAN classique `CAN 2.0B` et CAN FD ;
- conforme ISO11898-1:2015 ;
- débit d'arbitrage jusqu'à 1 Mbit/s ;
- alimentation module : 3,3 V à 5 V ;
- interface microcontrôleur : SPI ;
- bornes bus : `H = CANH`, `G = GND`, `L = CANL` ;
- compatible STM32 / ESP32 / Arduino selon la notice.

Pinout SPI documenté :

```text
5V / 3V3 = alimentation
GND       = masse
nCS       = chip select SPI
SCK       = horloge SPI
SDI       = entrée données SPI (MOSI côté MCU)
SDO       = sortie données SPI (MISO côté MCU)
INT/INT0/INT1 = interruptions
CLK       = sortie horloge
```

## Usage prévu pour le projet

Le module sera relié à un ESP32 et configuré en :

```text
CAN classique
500 kbit/s
mode normal
```

Objectifs :

1. fournir l'ACK CAN qui manque actuellement au CSW-2000R sur banc ;
2. lire directement les trames CAN et les afficher via le port série de l'ESP32 ;
3. relever automatiquement timestamp, ID, DLC et octets de données ;
4. comparer idle / appui / maintien / relâchement sans devoir décoder manuellement chaque capture PulseView.

## PulseView après ajout du module

PulseView ne sera plus nécessaire pour la lecture courante des trames CAN une fois le MCP2518FD opérationnel. Il restera utile comme outil de diagnostic bas niveau :

- vérifier TXD/RXD ;
- mesurer les timings ;
- diagnostiquer une erreur de bitrate ou de terminaison ;
- observer une anomalie physique du bus.

## Point encore à vérifier sur le module reçu

La notice fournie ne documente pas explicitement une terminaison `120 Ω` intégrée/commutable. Ne pas l'assumer. À réception, mesurer hors tension la résistance entre `CANH` et `CANL` du module seul avant de choisir la terminaison de banc.

## Correction de référence transceiver

La notice fournie identifie le transceiver comme **ATA6563**. Toute mention précédente d'`ATA6560` pour ce module précis est à corriger.
