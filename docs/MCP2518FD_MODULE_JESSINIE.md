# Module CAN-FD Jessinie — MCP2518FD / ATA6563

Date d'archivage : `2026-09-18`

> **Architecture secondaire depuis le 2026-09-18.** Ce module reste documenté et disponible, mais la voie CAN active est TWAI natif ESP32 + transceiver : `docs/CAN_GATEWAY_ESP32.md`.

## Statut de la source

Documentation fournisseur transmise par l'utilisateur sous forme de captures d'écran du produit / tableau d'interface.

Cette page documente **le module physique acheté pour le projet**, et non le seul circuit MCP2518FD.

Statut : **USER-SUPPLIED MODULE DOCUMENTATION**.

## Dimensions mécaniques

D'après l'illustration fournisseur :

```text
PCB : 40 mm × 20 mm
entraxe horizontal des trous : 24 mm
entraxe vertical des trous : 16 mm
```

## Connecteur P1 — interface logique / alimentation

Le tableau fournisseur donne :

| P1 | Signal | Fonction |
|---:|---|---|
| 1 | `nCS` | Chip Select SPI |
| 2 | `CLK` | sortie horloge / indicateur de début de trame selon configuration |
| 3 | `SDO` | sortie SPI du module, MISO vers le maître |
| 4 | `INT0` | interruption émission ou GPIO |
| 5 | `SDI` | entrée SPI du module, MOSI depuis le maître |
| 6 | `INT1` | interruption réception ou GPIO |
| 7 | `SCK` | horloge SPI |
| 8 | `GND` | masse |
| 9 | `INT` | interruption principale |
| 10 | `3V3` | alimentation 3,3 V |
| 11 | `GND` | masse |
| 12 | `5V` | alimentation 5 V |

### Correspondance ESP32 du banc

```text
ESP32 GPIO5   -> P1-1  nCS
ESP32 GPIO19  <- P1-3  SDO / MISO
ESP32 GPIO23  -> P1-5  SDI / MOSI
ESP32 GPIO18  -> P1-7  SCK
ESP32 GPIO27  <- P1-9  INT
ESP32 GND     -> P1-8 ou P1-11
```

Les broches `INT0` et `INT1` sont exposées séparément mais ne sont pas utilisées par le firmware de banc actuel.

## P2 — sélection d'alimentation / niveau logique

La documentation fournisseur indique que P2 est utilisé lorsque le maître travaille en logique 5 V :

- maître logique 3,3 V : alimentation 3,3 V utilisée sur P1-10 ; P2 non ponté ;
- maître logique 5 V : P2 court-circuité selon la notice et alimentation 5 V sur P1-12.

**Ne pas considérer P2 comme un signal CAN ou une sortie RX/TX.**

Avant toute modification d'alimentation, vérifier la sérigraphie et le câblage réel de la carte.

## P3 — résistance de terminaison CAN

P3 commande la résistance de terminaison intégrée :

```text
P3 ponté   -> terminaison 120 ohms activée
P3 ouvert  -> terminaison 120 ohms désactivée
```

Conséquence pour le banc :

- mesurer H ↔ L hors tension avant d'ajouter une résistance externe ;
- ne pas cumuler sans raison la terminaison intégrée avec une terminaison externe ;
- sur le véhicule, ne pas ajouter une terminaison supplémentaire à un bus déjà correctement terminé.

## Bornier CAN

Le tableau fournisseur donne :

| Broche | Signal | Fonction |
|---:|---|---|
| 1 | `G` | masse |
| 2 | `H` | CAN-H |
| 3 | `L` | CAN-L |

Pour le CSW-2000R :

```text
module G -> CSW GND / CN1-8
module H -> CSW CANH / CN1-5 ou CN1-6
module L -> CSW CANL / CN1-11 ou CN1-12
```

## Point important : RXCAN / TXCAN

Le connecteur P1 **n'expose pas directement** les signaux internes `RXCAN` et `TXCAN` entre le MCP2518FD et le transceiver.

En particulier :

```text
SDI != RXCAN
SDO != TXCAN
```

`SDI/SDO` sont uniquement les lignes de données **SPI** entre l'ESP32 et le MCP2518FD.

Pour observer `RXCAN/TXCAN`, il faudrait accéder aux pistes/pattes internes reliant le contrôleur CAN au transceiver.

## Diagnostic PulseView recommandé

Sans oscilloscope, les points accessibles utiles sont :

```text
nCS = P1-1
SDO = P1-3
SDI = P1-5
SCK = P1-7
INT = P1-9
GND = P1-8 ou P1-11
```

Pour vérifier la liaison SPI :

```text
PulseView SPI :
CLK  -> P1-7 SCK
MOSI -> P1-5 SDI
MISO -> P1-3 SDO
CS   -> P1-1 nCS
```

La ligne `INT` est active uniquement si le MCP2518FD est correctement initialisé et qu'une condition d'interruption configurée se produit.

## Observation de banc du 2026-09-18

Montage PulseView :

```text
D0 = CSW PCA82C250 TXD
D5 = CSW PCA82C250 RXD
D4 = module P1-9 INT
```

Observation :

- D0 et D5 montrent le trafic CAN émis par le CSW ;
- PulseView décode le trafic CSW ;
- `INT` sur D4 reste statique sur la capture fournie.

Interprétation actuelle :

**on ne peut pas encore conclure que le MCP2518FD reçoit les trames.**

Les causes à vérifier dans cet ordre sont :

1. alimentation du module ;
2. communication SPI ESP32 ↔ MCP2518FD ;
3. initialisation correcte du MCP2518FD ;
4. configuration de `INT` ;
5. continuité H/L/GND ;
6. terminaison P3 / terminaison externe ;
7. réception CAN à 500 kbit/s.

La prochaine mesure logique prioritaire est donc le SPI `nCS + SCK + SDI + SDO`, pas CAN-H/CAN-L directement.
