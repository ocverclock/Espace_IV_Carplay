# CAN Research — Renault Espace IV

Dernière mise à jour : `2026-09-18`

## Objectif

Comprendre la topologie CAN réelle de notre Espace IV, décoder le CSW-2000R et fournir au Raspberry Pi les données utiles sans perturber les réseaux du véhicule.

## Règle de preuve

Ordre de confiance :

1. mesure sur notre voiture / nos pièces ;
2. documentation constructeur ;
3. datasheets fabricants ;
4. documentation officielle des composants ;
5. sources externes concordantes ;
6. forums / retours utilisateurs ;
7. hypothèse.

Une information externe n'est jamais promue en `MEASURED` sans validation.

## Architecture CAN active

La voie de travail prioritaire est désormais :

```text
bus CAN
  |
transceiver 3,3 V
  |
ESP32 / contrôleur TWAI natif
  |
UART ou USB
  |
Raspberry Pi 4
```

Document : `docs/CAN_GATEWAY_ESP32.md`.

L'ancien banc `ESP32 -> SPI -> MCP2518FD -> ATA6563` reste archivé mais n'est plus la voie prioritaire pour le CAN classique.

## Règle : un canal par bus physique

Un bus peut contenir de nombreux calculateurs : un seul contrôleur CAN suffit pour tous les écouter.

Deux bus CAN physiquement séparés nécessitent deux canaux indépendants.

```text
1 bus physique = 1 contrôleur TWAI + 1 transceiver
```

Le ESP32-C6 est particulièrement intéressant car il possède deux contrôleurs TWAI matériels. Avec deux transceivers, il peut donc servir de passerelle pour deux bus CAN classiques simultanément.

## CSW-2000R — faits mesurés

Le CSW possède un transceiver `PCA82C250`.

Brochage mesuré :

```text
CN1-5 / CN1-6   = CANH
CN1-11 / CN1-12 = CANL
```

Résistance CSW seul hors tension :

```text
R(CANH,CANL) ~= 37 kOhm
```

Le CSW n'intègre donc pas de terminaison fixe 120 ohms.

Capture logique validée :

```text
CAN classique
bitrate = 500000 bit/s
ID = 0x681
DLC = 8
idle = F0 0A 0A 01 FF FF FF FF
```

Sur la capture idle dédiée :

- 2339 trames valides ;
- uniquement ID `0x681` ;
- uniquement le payload idle ci-dessus ;
- répétitions massives probablement dues à l'absence d'ACK sur le banc.

Prochaine étape CSW : fournir un ACK avec un vrai nœud CAN puis comparer les payloads bouton par bouton.

## Réseaux CAN de l'Espace IV

### Fait acquis

Le véhicule ne doit pas être considéré comme ayant un unique bus CAN universel.

Le système de navigation dispose au minimum de lignes distinctes documentées pour :

```text
133B / 133C = CAN tableau de bord / véhicule
34DZ / 34GA = CAN multimédia 1
```

Le calculateur navigation Renault 662 est documenté avec ces deux paires distinctes.

Source externe utilisée :
`https://diagnostdata.com/renault/espace/iv-2002-2006/remont/connector-pinouts/connectors-600-699/connector-pinout-662-navigation-computer/`

Statut : **EXTERNAL WIRING DOCUMENTATION**.

### Retour Espace IV Phase 1

Un reverse engineering public sur Espace IV 2004 rapporte :

```text
OBD pin 6  = CAN-H diagnostic
OBD pin 14 = CAN-L diagnostic
bitrate observé = 250 kbit/s

CAN intérieur / multimédia :
accessible au connecteur radio
bitrate observé = 500 kbit/s
```

Source :
`https://www.canhack.de/viewtopic.php?t=145`

Statut : **EXTERNAL REVERSE ENGINEERING / SAME VEHICLE FAMILY / NOT YET MEASURED ON OUR CAR**.

## Prise OBD — ce qu'il faut retenir

La prise OBD n'est **pas supposée exposer automatiquement tous les bus CAN du véhicule**.

### OBD 6 / 14

Pins normalisées CAN :

```text
6  = CAN-H
14 = CAN-L
```

Sur Espace IV, des retours externes les décrivent comme accès au CAN diagnostic/véhicule.

Le bitrate exact doit être mesuré sur notre véhicule avant de le figer.

### OBD 12 / 13 — piste CAN multimédia

Plusieurs ressources Renault / DDT4All décrivent l'utilisation de 12/13 pour accéder à un second CAN, souvent appelé CAN2 ou CAN multimédia, avec des adaptateurs qui redirigent les interfaces ELM vers ces pins.

Sources de travail :

- `https://github.com/cedricp/ddt4all/discussions/970`
- `https://www.espace-freunde.info/forum/showthread.php?tid=820`
- `https://pinoutguide.com/reports/renault_obd2_diag.shtml`

Mais ces sources couvrent plusieurs Renault/générations et ne suffisent pas à prouver le brochage de **notre** Espace.

Statut actuel :

```text
OBD 6/14     = CAN diagnostic/véhicule : LIKELY / à mesurer
OBD 12/13    = accès CAN multimédia     : HYPOTHESIS FOR OUR CAR / à mesurer
```

Ne pas connecter les deux paires ensemble.

## Test OBD à réaliser

Avant toute émission :

1. contact coupé, mesurer résistance entre 6 et 14 ;
2. mesurer résistance entre 12 et 13 ;
3. vérifier qu'il n'existe pas de continuité directe entre les deux paires ;
4. contact mis, écouter 6/14 en listen-only ;
5. déterminer bitrate et IDs ;
6. répéter sur 12/13 ;
7. comparer les trames 12/13 avec le CAN multimédia / CSW connu à 500 kbit/s.

Ce test dira combien de bus intéressants sont réellement accessibles depuis la prise OBD.

## Architecture cible provisoire

Si deux bus suffisent :

```text
ESP32-C6
  |
  +-- TWAI0 -> transceiver -> bus privé CSW 500 kbit/s
  |
  +-- TWAI1 -> transceiver -> CAN véhicule
  |
  +-- UART bidirectionnel -> Raspberry Pi 4
```

Si le CAN multimédia OEM doit être écouté **en plus** du CSW privé et du CAN véhicule principal, trois canaux physiques seront nécessaires.

À ce moment seulement, choisir entre :

- second ESP32 ;
- ESP32 disposant de davantage de contrôleurs CAN ;
- MCP2518FD externe ;
- autre interface multi-CAN.

Ne pas ajouter de matériel avant d'avoir mesuré le besoin réel.

## Liaison avec Raspberry Pi

Pour un seul C6, priorité à l'UART direct afin d'éviter un hub USB :

```text
C6 TX -> Pi RX
C6 RX <- Pi TX
GND    = GND
```

La liaison est bidirectionnelle : réception CAN vers le Pi et demandes d'émission Pi vers CAN.

USB CDC reste une alternative de développement.

## Matériel / logiciels

### Chemin actif

- ESP32 classique : 1 TWAI, idéal pour banc CSW ;
- ESP32-C6 : 2 TWAI matériels, candidat final si disponible ;
- transceiver 3,3 V par bus, prototype possible avec SN65HVD230 ;
- ESP-IDF avec API TWAI multi-instance sur C6.

Documentation Espressif :
- `https://documentation.espressif.com/esp32-c6_technical_reference_manual_en.pdf`
- `https://docs.espressif.com/projects/esp-idf/en/release-v5.2/esp32c6/api-reference/peripherals/twai.html`

### Ancien chemin MCP2518FD

Le module Jessinie `MCP2518FD + ATA6563` acheté reste documenté :

- `docs/MCP2518FD_MODULE_JESSINIE.md`
- `docs/ESP32_MCP2518FD_CAN.md`
- `firmware/esp32_mcp2518fd_bench/`

Observation du 2026-09-18 : aucune activité visible sur la ligne `INT` pendant que le CSW émettait. La communication SPI n'a pas été démontrée. Cette voie est donc **dépriorisée**, pas déclarée matériellement défectueuse.

## Historique interfaces OBD

### XTOOL A30M

Bluetooth SPP présent, protocole propriétaire. Non retenu comme interface PyRen.

### ELM327 Wi-Fi

```text
192.168.0.10:35000
ATI  -> ELM327 v1.5
ATDP -> ISO 15765-4 (CAN 11/500)
PID moteur -> CAN ERROR
```

Cet essai ne permet pas de conclure que le bus principal de notre Espace est réellement à 500 kbit/s : l'interface n'a pas communiqué correctement avec les ECU.

### ELS27

Achat d'environ 150 EUR reporté. Le développement direct TWAI permet d'abord de comprendre les bus sans cet achat.
