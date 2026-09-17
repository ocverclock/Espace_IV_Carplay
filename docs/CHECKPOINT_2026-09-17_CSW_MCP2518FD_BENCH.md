# Checkpoint — 2026-09-17 — CSW-2000R + MCP2518FD sur banc

## But

Faire fonctionner le CSW-2000R sur un bus CAN privé avec un second nœud actif `ESP32 + MCP2518FD + ATA6563`, obtenir l'ACK CAN et lire les vraies trames du CSW sans dépendre du décodage manuel PulseView.

## Terminologie verrouillée

```text
CSW-2000R
  ↕ CANH / CANL
ATA6563 = petit SO8, transceiver CAN physique
  ↕ TXD / RXD
MCP2518FD = gros contrôleur CAN du module
  ↕ SPI
ESP32 NodeMCU-32S
```

Ne pas confondre :

- `CN1` = connecteur 12 voies du CSW-2000R ;
- `P1` = connecteur 2×6 du module MCP2518FD.

## Photos module reçues ce jour

Références utilisateur :

- `IMG_7215.jpeg` : face composants ;
- `IMG_7216.jpeg` : face soudures / sérigraphie P1.

Relevés visuels :

- face composants, bornier CAN à gauche : `G`, `H`, `L` de haut en bas ;
- petit SO8 immédiatement à droite du bornier = transceiver ATA6563 d'après la notice fournisseur ;
- grand circuit rectangulaire vers la droite = MCP2518FD ;
- oscillateur 40 MHz déjà confirmé sur photo précédente (`40.000M`) ;
- P1 face soudures :

```text
GND | 5V
INT | 3V3
SCK | GND
SDI | INT1
SDO | INT0
nCS | CLK
```

## Câblage CSW-2000R de banc verrouillé

```text
CN1-7  = +12,5 V principal
CN1-8  = GND
CN1-1  = 34HU wake
CN1-10 = illumination / veilleuses
CN1-5/6   = CANH
CN1-11/12 = CANL
```

Wake de banc :

```text
CN1-7 (+12,5 V)
   │
   └── 3,3 kΩ (orange-orange-rouge) ── CN1-1 / 34HU
```

La résistance de `3,3 kΩ` est notre simulation du wake véhicule. Sans cette liaison, le trafic CAN du CSW devient nul. Le CSW est donc bien réveillé par `CN1-1 / 34HU`.

Éclairage de banc :

```text
CN1-7 ── fil direct ── CN1-10
```

Ce pont allume le rétroéclairage / les LED. Il est distinct du wake.

## CAN CSW — référence connue

PulseView :

```text
500 kbit/s
ID  = 0x681
DLC = 8
DATA = F0 0A 0A 01 FF FF FF FF
```

Sans ACK, le CSW retransmet en salves :

- durée typique `~49–50 ms` ;
- salves espacées de `~500 ms` ;
- tentatives espacées d'environ `274 µs`.

Ce comportement est encore observé aujourd'hui : le second nœud ne fournit donc pas encore un ACK CAN valide.

## Module MCP2518FD — alimentation et terminaison

Module alimenté avec les deux rails :

```text
5V  ≈ 4,76 V
3V3 ≈ 3,30 V
GND commun
```

CAN au repos : CANH/CANL autour de `2,5 V`.

Terminaison :

```text
P3 ouvert  → H-L ≈ 29,2 kΩ
P3 fermé   → H-L ≈ 128 Ω
```

Avec une résistance externe `120 Ω` à l'autre extrémité du bus :

```text
Rbus ≈ 61 Ω
```

Valeur cohérente avec deux terminaisons de bus en parallèle.

## ATA6563 — brochage de travail

```text
1 TXD
2 GND
3 VCC
4 RXD
5 VIO
6 CANL
7 CANH
8 STBY
```

Relevés :

- pin 2 → GND : `≈ 0 Ω` ;
- pin 6 = CANL et pin 7 = CANH confirmées visuellement par les pistes PCB ;
- pin 2 ↔ pin 8 : environ `15 kΩ` ;
- pin 8 sous tension, sans résistance temporaire : `~2,5 V` ;
- pin 8 + `4,7 kΩ` vers GND : `~1,45 V` ;
- pin 8 + `1 kΩ` vers GND : `~0,76 V`.

Les résistances ajoutées sur STBY sont temporaires pour diagnostic. Ne pas les considérer comme solution définitive.

## PulseView côté ATA6563

Avec pin 8 abaissée à environ `0,76 V` :

- activité CAN observée sur `RXD` pin 4 ;
- capture `13_45` : `D0 → pin 1 TXD`, `D5 → pin 4 RXD` ;
- les deux formes sont presque identiques ;
- ce comportement est anormal si le pinout ATA6563 est bien celui ci-dessus : TXD ne devrait pas simplement reproduire RXD pendant toute la trame ;
- test ohmique direct TXD ↔ RXD : multimètre instable entre `0 Ω` et circuit ouvert, résultat non interprétable comme preuve de court-circuit.

Prochaine mesure prioritaire : continuité physique directe entre le petit SO8 et le MCP2518FD :

```text
ATA pin 1 TXD ↔ MCP2518FD TXCAN
ATA pin 4 RXD ↔ MCP2518FD RXCAN
```

puis contrôle des deux liaisons croisées.

## ESP32 / ACAN2517

ESP32 : `ESP-32S V1.1 101010 NodeMCU`, board Arduino `NodeMCU-32S`.

SPI :

```text
GPIO18 SCK  → SCK
GPIO23 MOSI → SDI
GPIO19 MISO ← SDO
GPIO5  CS   → nCS
```

Le mode interruption sur GPIO27 a provoqué des resets watchdog. Diagnostic actuel en polling sans INT.

Initialisation :

```text
CAN INIT OK
Bitrate reel : 500000
Bitrate exact : OUI
```

Cela valide la communication SPI avec le MCP2518FD et son calcul de bitrate, pas la réception CAN correcte.

Sorties série anormales observées :

```text
ID=000 DLC=0 DATA=
```

puis :

```text
ID=1FFFFFFF DLC=15 DATA=FF FF FF FF FF FF FF FF ...
```

Ces valeurs sont invalides pour notre CAN classique et ne correspondent pas à la baseline `0x681 / DLC 8` du CSW.

Le sketch doit à terme vérifier le booléen retourné par `receive()` et borner l'impression des données à 8 octets en CAN classique.

## État de fin de checkpoint

```text
CSW réveillé et émet à 500 kbit/s             OUI
baseline 0x681 connue                          OUI
terminaison globale correcte (~61 Ω)           OUI
MCP2518FD accessible en SPI                     OUI
bitrate configuré exactement à 500 kbit/s      OUI
signal atteint RXD du transceiver               OUI
ACK valide fourni au CSW                        NON
trame 0x681 lue correctement par ESP32          NON
```

### Reprise recommandée

1. ne plus modifier le câblage CSW, qui est validé ;
2. ne plus changer les résistances de STBY avant d'avoir tracé les nets ;
3. identifier physiquement la pin 1 du MCP2518FD ;
4. vérifier continuité `ATA TXD ↔ MCP TXCAN` ;
5. vérifier continuité `ATA RXD ↔ MCP RXCAN` ;
6. vérifier absence de liaison croisée ;
7. seulement ensuite revenir au firmware / FIFO / mode CAN si le routage est correct.
