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

> Important : sur les photos du module reçu, le marquage des circuits intégrés n'est pas suffisamment lisible pour identifier les références optiquement. Les noms `MCP2518FD` et `ATA6563` viennent de la notice fournisseur et sont corroborés par la topologie/pinout mesurés.

## Vocabulaire matériel

Pour éviter les ambiguïtés pendant le diagnostic :

```text
ESP32
  ↕ SPI
MCP2518FD = gros contrôleur CAN du module
  ↕ TXCAN / RXCAN (logique)
ATA6563 = petit transceiver SO8 près du bornier CAN
  ↕ CANH / CANL (physique)
CSW-2000R
```

Le `MCP2518FD` fabrique/décode les trames CAN. Le `ATA6563` convertit les niveaux logiques TX/RX en niveaux électriques différentiels CANH/CANL.

## Photos de référence du module reçu — 2026-09-17

Deux nouvelles photos utilisateur ont été prises et analysées :

- `IMG_7215.jpeg` : face composants ;
- `IMG_7216.jpeg` : face soudures / sérigraphie du connecteur 2×6.

### Face composants — repères visuels

Dans l'orientation de `IMG_7215.jpeg`, bornier vert à gauche :

- bornier CAN 3 voies sérigraphié, de haut en bas : `G`, `H`, `L` ;
- petit circuit `SO8` immédiatement à droite du bornier : transceiver CAN `ATA6563` d'après la notice ;
- grand circuit rectangulaire à nombreuses pattes vers la droite : contrôleur `MCP2518FD` ;
- quartz / oscillateur entre les deux circuits ; une photo antérieure a permis de lire `40.000M`, donc oscillateur **40 MHz** ;
- LED verte d'alimentation visible et fonctionnelle ;
- cavalier de terminaison CAN présent sur le module.

Si la carte est retournée horizontalement avec le bornier à droite, l'ordre visuel du bornier devient `L`, `H`, `G` de haut en bas. Toujours se fier aux lettres sérigraphiées plutôt qu'à une position absolue.

### Connecteur 2×6 du module — sérigraphie confirmée par photo

Face soudures telle que visible sur `IMG_7216.jpeg` :

```text
GND | 5V
INT | 3V3
SCK | GND
SDI | INT1
SDO | INT0
nCS | CLK
```

Ce connecteur est appelé ici **P1 du module MCP**, afin de ne pas le confondre avec **CN1 du CSW-2000R**.

Correspondance SPI utilisée avec l'ESP32 NodeMCU-32S :

```text
ESP32 GPIO18 SCK  → module SCK
ESP32 GPIO23 MOSI → module SDI
ESP32 GPIO19 MISO ← module SDO
ESP32 GPIO5  CS   → module nCS
ESP32 GPIO27 INT  ← module INT   (débranché lors des essais polling)
GND commun
```

## Transceiver SO8 — brochage de travail et mesures

Brochage ATA6563 utilisé pour le diagnostic :

```text
1 = TXD
2 = GND
3 = VCC
4 = RXD
5 = VIO
6 = CANL
7 = CANH
8 = STBY
```

Mesures / observations utilisateur confirmées :

- pin 2 → GND : `≈ 0 Ω` ;
- pin 6 → CANL : piste PCB visible et fonction cohérente ;
- pin 7 → CANH : piste PCB visible et fonction cohérente ;
- pin 2 ↔ pin 8 : environ `15 kΩ`, donc la pin 8 présente un chemin de l'ordre de 15 kΩ vers GND sur le module ;
- pin 8 mesurée initialement sous tension : environ `2,5 V` ;
- ajout temporaire `4,7 kΩ` entre pin 8 et GND : environ `1,45 V` ;
- ajout temporaire `1 kΩ` entre pin 8 et GND : environ `0,76 V`.

Ces résistances additionnelles sont des **essais de diagnostic** et ne constituent pas encore le câblage final.

### Signaux logiques observés au transceiver

Avec PulseView à `8 MHz` :

- activité CAN visible sur `RXD` (pin 4) lorsque le CSW émet ;
- capture `13_45` : `D0 = pin 1 TXD` et `D5 = pin 4 RXD` du petit SO8 ;
- les deux formes observées sont très proches, ce qui n'est pas le comportement attendu si pin 1 et pin 4 sont bien respectivement TXD et RXD ;
- la mesure ohmique directe TXD ↔ RXD a donné un comportement instable du multimètre (`0 Ω` ↔ circuit ouvert), donc elle n'est pas considérée comme une preuve de court-circuit.

**Prochaine vérification matérielle prioritaire :** tracer hors tension les nets logiques entre le SO8 et le MCP2518FD pour confirmer directement :

```text
ATA pin 1 TXD ↔ MCP2518FD TXCAN
ATA pin 4 RXD ↔ MCP2518FD RXCAN
```

et vérifier l'absence de court-circuit croisé.

## Alimentation du module reçue — état validé

Pour ce module précis avec l'ESP32 3,3 V :

- alimentation `5V` du module raccordée au rail 5 V/VIN de l'ESP32 ;
- alimentation `3V3` du module raccordée au 3,3 V de l'ESP32 ;
- GND commun ;
- configuration matérielle de banc utilisée : `P2 ouvert` ;
- tension mesurée module `5V ≈ 4,76 V` ;
- tension mesurée module `3V3 ≈ 3,30 V` ;
- CANH/CANL au repos autour de `2,5 V`.

Les anciennes mesures aberrantes (environ 10,47 V sur le 5 V, 7,37 V sur le 3,3 V, etc.) provenaient d'un multimètre en défaut temporaire et sont invalidées.

## Terminaison CAN du module reçu

Mesures réelles hors tension :

```text
P3 ouvert  : R(CANH,CANL) ≈ 29,2 kΩ
P3 fermé   : R(CANH,CANL) ≈ 128 Ω
```

Conclusion : le cavalier `P3` active bien la terminaison CAN du module, nominalement proche de `120 Ω`.

Avec `P3` fermé côté module et une résistance externe de `120 Ω` à l'autre extrémité côté CSW, la résistance globale mesurée sur le bus est d'environ :

```text
Rbus ≈ 61 Ω
```

Valeur cohérente avec deux terminaisons d'extrémité en parallèle.

## Usage prévu pour le projet

Le module sera relié à un ESP32 puis, à terme, au Raspberry Pi, et configuré en :

```text
CAN classique
500 kbit/s
mode normal
```

Objectifs :

1. fournir l'ACK CAN qui manque actuellement au CSW-2000R sur banc ;
2. lire directement les trames CAN et les afficher via le port série ;
3. relever automatiquement timestamp, ID, DLC et octets de données ;
4. comparer idle / appui / maintien / relâchement sans devoir décoder manuellement chaque capture PulseView.

## ESP32 / ACAN2517 — état au 2026-09-17

Carte ESP32 : `ESP-32S V1.1 101010 NodeMCU`, Arduino IDE en `NodeMCU-32S`.

Bibliothèque : `ACAN2517` de Pierre Molinaro.

Configuration validée côté SPI :

```text
oscillateur = 40 MHz
bitrate demandé = 500000 bit/s
mode = Normal20B
```

Le mode interruption avec GPIO27 a provoqué des resets watchdog ESP32. Le banc actuel utilise donc le mode polling / sans INT (`inINT = 255`).

Le contrôleur répond correctement sur SPI :

```text
CAN INIT OK
Bitrate reel : 500000
Bitrate exact : OUI
```

Ce succès prouve la communication SPI et la configuration du MCP2518FD, mais **ne prouve pas encore la réception CAN correcte**.

### Sorties série anormales observées

Le moniteur série a produit successivement :

```text
ID=000 DLC=0 DATA=
```

puis, lors d'un essai ultérieur :

```text
ID=1FFFFFFF DLC=15 DATA=FF FF FF FF FF FF FF FF ...
```

Ces valeurs ne sont **pas** considérées comme des trames CSW valides. La baseline CSW connue et décodée par PulseView reste :

```text
ID 0x681
DLC 8
F0 0A 0A 01 FF FF FF FF
```

Le firmware de diagnostic doit aussi vérifier la valeur de retour de `receive()` avant d'imprimer une trame et ne jamais traiter plus de 8 octets en CAN classique.

## PulseView après ajout du module

PulseView reste l'outil de référence bas niveau tant que le MCP2518FD n'acquitte pas proprement le CSW :

- vérifier TXD/RXD ;
- mesurer les timings ;
- diagnostiquer une erreur de bitrate ou de terminaison ;
- observer l'ACK et les retransmissions.

Le CSW montre encore les salves de retransmission d'environ `49–50 ms` toutes les `~500 ms`, caractéristiques de l'absence d'ACK sur notre banc.

## État de diagnostic actuel

Faits acquis :

```text
CSW émet à 500 kbit/s                         OK
terminaison globale ≈ 61 Ω                   OK
module alimenté 5 V + 3,3 V                  OK
MCP2518FD communique en SPI                   OK
bitrate MCP calculé exactement à 500 kbit/s   OK
signal CAN atteint le transceiver RXD         OBSERVÉ
ACK valide du MCP2518FD                       NON
lecture de vraies trames 0x681 par ESP32      NON
```

Le prochain test doit rester ciblé : confirmer la continuité `TXCAN/RXCAN` entre le MCP2518FD et le transceiver SO8 avant toute nouvelle modification de résistance ou de configuration logicielle.
