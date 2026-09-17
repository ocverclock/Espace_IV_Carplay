# Checkpoint — 2026-09-17 — CSW-2000R + MCP2518FD sur banc

## But

Faire fonctionner le CSW-2000R sur un bus CAN privé avec un second nœud actif `ESP32 + MCP2518FD + ATA6563`, obtenir l'ACK CAN et lire les vraies trames du CSW sans dépendre du décodage manuel PulseView.

## Correction importante du 17/09

Une confusion a eu lieu pendant les mesures entre les deux transceivers CAN présents dans le montage :

```text
CSW-2000R : PCA82C250 d'origine Renault/Xanavi
module ajouté : ATA6563 associé au MCP2518FD
```

Les mesures suivantes :

- `pin 8 ≈ 2,5 V` sous tension ;
- `pin 2 ↔ pin 8 ≈ 15 kΩ` ;
- ajout temporaire de résistances vers GND ;
- identification de `R54 = 15 kΩ` ;

concernent **le PCA82C250 du CSW**, pas l'ATA6563 du module MCP2518FD.

Toute conclusion antérieure du type « ATA6563 pin 8 / STBY à 2,5 V » basée sur ces mesures est donc **invalidée**.

## Terminologie verrouillée

```text
CSW-2000R
  └── IC3 PCA82C250 = transceiver CAN d'origine
        ↕ CANH / CANL
        ↕ TXD / RXD vers électronique Xanavi

bus CAN privé 500 kbit/s

module ajouté
  └── ATA6563 = transceiver physique du module
        ↕ TXD / RXD
      MCP2518FD = contrôleur CAN SPI
        ↕ SPI
      ESP32 NodeMCU-32S
```

Ne pas confondre :

- `CN1` = connecteur 12 voies du CSW-2000R ;
- `P1` = connecteur 2×6 du module MCP2518FD.

## Photos module MCP2518FD

Références utilisateur :

- `IMG_7215.jpeg` : face composants ;
- `IMG_7216.jpeg` : face soudures / sérigraphie P1.

Relevés :

- bornier CAN : `G`, `H`, `L` ;
- petit SO8 du module = ATA6563 d'après la notice fournisseur ;
- grand circuit = MCP2518FD ;
- oscillateur `40.000M` = 40 MHz ;
- P1 :

```text
GND | 5V
INT | 3V3
SCK | GND
SDI | INT1
SDO | INT0
nCS | CLK
```

## Câblage CSW-2000R de banc — état actuel

```text
CN1-7  = +12,5 V principal
CN1-8  = GND
CN1-1  = 34HU wake
CN1-10 = illumination / veilleuses
CN1-5/6   = CANH
CN1-11/12 = CANL
```

### Wake de banc

Ancien essai :

```text
CN1-7 (+12,5 V) ── 3,3 kΩ ── CN1-1 / 34HU
```

État actuel :

```text
CN1-7 (+12,5 V) ── 4,7 kΩ ── CN1-1 / 34HU
```

Le CSW continue à se réveiller et à émettre avec `4,7 kΩ`. Sans liaison de wake sur `CN1-1`, le trafic CAN devient nul.

Conclusion : `4,7 kΩ` est suffisant pour le wake de banc actuel. La valeur exacte d'origine du véhicule reste à documenter ; la voiture pilote `34HU` comme ligne de réveil multimédia.

### Éclairage

```text
CN1-7 ── fil direct ── CN1-10
```

Ce pont allume les LEDs / le rétroéclairage. Il est distinct du wake.

## PCA82C250 du CSW — pin 8 Rs / R54

Le transceiver du CSW est :

```text
IC3 = Philips/NXP PCA82C250
```

Brochage utile :

```text
1 TXD
2 GND
3 VCC 5 V
4 RXD
5 Vref
6 CANL
7 CANH
8 Rs = slope control / standby
```

### Mesures corrigées

Carte hors tension :

```text
PCA82C250 pin 8 Rs ── R54 ≈ 15 kΩ ── GND
```

Mesure rail `+5 V ↔ GND` en ohms : **circuit ouvert**. Il n'y a donc pas de court-circuit franc entre le rail 5 V et la masse.

Sous tension principale mais **sans wake CN1-1** :

```text
PCA82C250 pin 8 Rs ≈ 0,4 mV
```

Avec wake appliqué et rail interne 5 V présent :

```text
PCA82C250 pin 3 VCC ≈ 5,0 V
PCA82C250 pin 8 Rs  ≈ 2,5 V
```

### Interprétation

`R54 = 15 kΩ` est reliée de `Rs` vers GND. La valeur `Rs ≈ 2,5 V` lorsque le PCA82C250 est alimenté est cohérente avec le **mode slope-control** du PCA82C250 ; elle ne constitue pas une preuve de standby.

Le passage d'environ `0 V` sans wake à `2,5 V` avec wake s'explique naturellement par l'apparition du rail 5 V du PCA82C250. Il n'est pas nécessaire de supposer qu'IC2 fabrique directement les 2,5 V.

Une résistance temporaire ajoutée extérieurement sur cette pin 8 a été retirée : elle n'apportait aucune amélioration du comportement CAN et ne correspond pas au montage d'origine.

Conclusion : **ne plus modifier R54 / Rs pour chercher l'ACK CAN**. La partie PCA82C250 du CSW est cohérente.

## Connecteur CN1 — rappel

Vue dans le connecteur, détrompeur en bas :

```text
haut : 7  8  9 10 11 12
bas  : 1  2  3  4  5  6
```

État documenté / mesuré :

```text
1  = 34HU wake multimédia
2  = GND sur PCB ; non câblé dans le faisceau externe documenté
3  = non câblé dans le faisceau externe documenté
4  = non câblé dans le faisceau externe documenté
5  = CANH
6  = CANH
7  = + batterie protégé
8  = GND / masse audio
9  = rhéostat / éclairage
10 = + veilleuses / éclairage
11 = CANL
12 = CANL
```

Les photos / documents de faisceau retrouvés confirment que le connecteur physique possède 12 positions mais que `2`, `3` et `4` ne sont pas utilisées comme conducteurs du faisceau externe consulté.

## CAN CSW — référence connue

PulseView :

```text
500 kbit/s
ID   = 0x681
DLC  = 8
DATA = F0 0A 0A 01 FF FF FF FF
```

Sans ACK :

- salves de `~49–50 ms` ;
- salves espacées de `~500 ms` ;
- retransmissions espacées d'environ `274 µs`.

### Capture 14_32 après modifications de banc

Modifications avant capture :

- wake `3,3 kΩ` remplacé par `4,7 kΩ` ;
- résistance temporaire sur la pin 8 Rs du PCA82C250 retirée.

Résultat : comportement pratiquement inchangé par rapport à `13_45` :

- salve autour de `50 ms` ;
- toujours des retransmissions nombreuses ;
- toujours pas d'ACK valide.

Conclusion : ces deux modifications n'ont pas résolu le problème d'ACK.

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

Avec une résistance externe `120 Ω` à l'autre extrémité :

```text
Rbus ≈ 61 Ω
```

Valeur cohérente avec deux terminaisons en parallèle.

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

Initialisation obtenue :

```text
CAN INIT OK
Bitrate reel : 500000
Bitrate exact : OUI
```

Cela valide la communication SPI avec le MCP2518FD et le calcul du bitrate, pas la réception CAN correcte.

Sorties série anormales observées :

```text
ID=000 DLC=0 DATA=
```

puis :

```text
ID=1FFFFFFF DLC=15 DATA=FF FF FF FF FF FF FF FF ...
```

Ces valeurs sont invalides pour notre CAN classique et ne correspondent pas à la baseline `0x681 / DLC 8` du CSW.

## État de fin de checkpoint

```text
CSW réveillé avec 4,7 kΩ sur 34HU              OUI
rail PCA82C250 5 V avec wake                    OUI
PCA82C250 Rs/R54 compris comme slope-control    OUI
baseline CAN 0x681 connue                       OUI
terminaison globale correcte (~61 Ω)            OUI
MCP2518FD accessible en SPI                     OUI
bitrate configuré à 500 kbit/s                  OUI
ACK valide fourni au CSW                        NON
trame 0x681 lue correctement par ESP32          NON
```

## Reprise recommandée

La partie `CSW / wake / PCA82C250 / R54` ne doit plus être modifiée sans nouvel élément.

La recherche doit maintenant se concentrer sur **le module ajouté MCP2518FD + ATA6563** :

1. identifier précisément les nets `TXCAN`, `RXCAN` et la commande de standby de l'ATA6563 sur le PCB du module ;
2. vérifier physiquement les continuités MCP2518FD ↔ ATA6563 ;
3. vérifier le mode réel du MCP2518FD et les registres d'état / erreurs ;
4. corriger le sketch pour vérifier le retour de `receive()` et ne jamais imprimer plus de 8 octets en CAN classique ;
5. refaire une capture et rechercher la disparition des salves de retransmission lorsque l'ACK fonctionnera.
