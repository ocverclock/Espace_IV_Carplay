# Module CAN SN65HVD230 — prototype

Date : `2026-09-18`

## Statut

**USER-SUPPLIED MODULE DOCUMENTATION / PROTOTYPE**

Ce document décrit le petit module CAN basé sur `SN65HVD230` montré par l'utilisateur.

Il est destiné aux essais avec ESP32/TWAI. Pour le PCB final en environnement automobile, un transceiver automobile qualifié reste préférable.

## Alimentation

Le module est prévu pour :

```text
VCC = 3,3 V
```

Il est donc directement compatible en niveau logique avec un ESP32 3,3 V.

## Brochage côté logique

La sérigraphie visible du module donne :

```text
3.3V
GND
RX
TX
```

Correspondance fonctionnelle :

```text
module TX = entrée driver CAN du SN65HVD230
          = à relier au TX du contrôleur TWAI de l'ESP32

module RX = sortie receiver CAN du SN65HVD230
          = à relier au RX du contrôleur TWAI de l'ESP32
```

Donc :

```text
ESP32 TWAI_TX  -> module TX
ESP32 TWAI_RX  <- module RX
ESP32 3V3      -> module 3.3V
ESP32 GND      -> module GND
```

## Côté bus

Le bornier expose :

```text
CANH
CANL
```

À raccorder directement à la paire CAN du bus concerné.

Pour le CSW-2000R :

```text
module CANH -> CSW CN1-5 ou CN1-6
module CANL -> CSW CN1-11 ou CN1-12
GND commun  -> CSW CN1-8
```

## Terminaison 120 ohms

Le schéma fourni montre une résistance de `120 Ω` entre CANH et CANL.

Le module possède également un cavalier jaune près du bornier CAN, très probablement destiné à activer/désactiver cette terminaison.

**À confirmer au multimètre avant usage :**

- module hors tension ;
- mesurer CANH ↔ CANL cavalier retiré ;
- mesurer CANH ↔ CANL cavalier installé.

Valeurs attendues si le cavalier commande bien la terminaison :

```text
cavalier ouvert  -> résistance élevée
cavalier fermé   -> environ 120 Ω
```

Sur bus véhicule déjà terminé, laisser la terminaison du module désactivée.

Sur le banc privé CSW à deux nœuds, viser une topologie correctement terminée aux deux extrémités.

## Brochage interne SN65HVD230 visible sur le schéma

```text
1 D      = entrée driver / CAN_TX
2 GND
3 VCC    = 3,3 V
4 R      = sortie receiver / CAN_RX
5 Vref
6 CANL
7 CANH
8 Rs
```

Le schéma montre aussi `Rs` relié à la masse via une résistance de `10 kΩ`, ce qui correspond à une configuration de contrôle de pente / mode du transceiver.

## Important : ne pas reprendre les valeurs marketing de l'image

Les mentions visibles en bas de l'image comme :

```text
Standby mode 370MA
Waveform impedance 0-100kΩ
```

ne sont pas considérées comme données techniques fiables.

Pour les caractéristiques électriques précises, utiliser la datasheet Texas Instruments du `SN65HVD230`.

## Usage projet

### Banc CSW

```text
ESP32 TWAI0
   |
   +-- TX -> module TX
   +-- RX <- module RX
   |
SN65HVD230
   |
   +-- CANH -> CSW CANH
   +-- CANL -> CSW CANL
```

Configuration :

```text
CAN classique
500 kbit/s
mode normal
```

Objectif : fournir l'ACK au CSW et recevoir proprement les trames `0x681`.

### ESP32-C6 double CAN

Deux modules identiques peuvent être utilisés :

```text
TWAI0 -> SN65HVD230 #1 -> bus privé CSW
TWAI1 -> SN65HVD230 #2 -> CAN véhicule
```

Un module/transceiver par bus physique.
