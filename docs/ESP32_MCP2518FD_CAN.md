# ESP32 + MCP2518FD + ATA6563 — architecture CAN retenue

Date de décision : `2026-09-17`

## Statut

**LEGACY / ARCHITECTURE DÉPRIORISÉE — 2026-09-18**

Cette voie reste documentée parce que le matériel a été acheté et testé, mais elle n'est plus l'architecture CAN principale.

Architecture active : `docs/CAN_GATEWAY_ESP32.md`.

Motif : pour le CAN classique, le contrôleur TWAI est déjà intégré à l'ESP32. Le chemin TWAI + transceiver est plus simple que ESP32 + SPI + MCP2518FD + transceiver.

Cette note fixe l'architecture de travail pour le reverse engineering CAN de l'Espace IV et du CSW-2000R.

## Matériel retenu

Le module acheté est basé sur :

- **MCP2518FD** : contrôleur CAN/CAN-FD externe ;
- **ATA6563** : transceiver physique CAN haute vitesse ;
- **quartz 40 MHz** sur notre module ;
- **ESP32** comme MCU de test / sniffer.

Architecture :

```text
ESP32
  │
  │ SPI
  ▼
MCP2518FD
  │ TXCAN / RXCAN
  ▼
ATA6563
  │
  ├── CAN-H
  └── CAN-L
       │
       ▼
Bus CAN Renault / CSW-2000R
```

## Conséquence importante

L'ESP32 n'utilise pas son contrôleur TWAI interne pour cette voie CAN.

Le protocole CAN est géré par le **MCP2518FD**, auquel l'ESP32 accède en SPI. L'ATA6563 assure uniquement la couche physique différentielle vers CAN-H/CAN-L.

Cette architecture est conservée plutôt que de repartir sur un montage ESP32 + SN65HVD230/TWAI.

## Compatibilité

Le MCP2518FD supporte :

- CAN classique 2.0 ;
- CAN-FD.

Le fait que l'Espace IV utilise du CAN classique ne pose donc aucun problème.

## CAN-H / CAN-L

CAN-H et CAN-L ne sont pas un fil montant et un fil descendant. Ils forment une **paire différentielle bidirectionnelle**.

Tous les nœuds émettent et reçoivent sur cette même paire.

Sur un CAN haute vitesse typique :

```text
récessif : CAN-H ≈ 2,5 V ; CAN-L ≈ 2,5 V

dominant : CAN-H monte ; CAN-L descend
```

Si CAN-H et CAN-L sont inversés au niveau du transceiver, les trames ne seront normalement pas décodées correctement. Une absence totale de réception sur un bus connu actif doit donc faire vérifier immédiatement la polarité H/L.

Pour le CSW-2000R, la polarité a déjà été mesurée :

```text
CN1-5 / CN1-6   = CAN-H
CN1-11 / CN1-12 = CAN-L
```

Voir `docs/CSW2000R.md`.

## Module physique reçu

La documentation spécifique du module Jessinie reçu est archivée dans :

`docs/MCP2518FD_MODULE_JESSINIE.md`

Points désormais documentés :

- P1 complet : `nCS / CLK / SDO / INT0 / SDI / INT1 / SCK / GND / INT / 3V3 / GND / 5V` ;
- P2 : configuration liée à l'alimentation / logique 5 V ;
- P3 : terminaison CAN intégrée, ponté = `120 Ω`, ouvert = désactivée ;
- bornier CAN : `G / H / L`.

## Méthode de travail verrouillée

### Phase 1 — banc CSW isolé

Sur le banc, le CSW et notre interface sont les deux seuls nœuds. Le MCP2518FD doit donc fonctionner en **Normal20B** afin de fournir l'ACK CAN, sans envoyer de trames applicatives arbitraires.

Le firmware doit afficher :

```text
réception
→ timestamp
→ ID CAN
→ DLC
→ données hexadécimales
```

### Phase 2 — écoute véhicule

Sur un réseau véhicule déjà actif, utiliser **ListenOnly** pour une écoute totalement passive.

### Paramètres confirmés pour le CSW

```text
oscillateur module = 40 MHz
CAN classique      = 2.0B
bitrate CSW        = 500000 bit/s
baseline           = ID 0x681, DLC 8
payload idle       = F0 0A 0A 01 FF FF FF FF
```

Avant de chercher les boutons :

1. vérifier alimentation du module ;
2. vérifier SPI ESP32 ↔ MCP2518FD ;
3. vérifier CAN-H/CAN-L et masse commune ;
4. vérifier l'état de P3 / terminaison ;
5. confirmer les interruptions/réceptions du MCP2518FD.

### Phase 3 — reverse engineering du CSW-2000R

Captures à effectuer séparément :

- repos ;
- chaque bouton ;
- appui court ;
- appui long si pertinent ;
- joystick dans chaque direction ;
- rotation molette sens 1 ;
- rotation molette sens 2.

Méthode : comparer les ID et octets qui changent entre état de repos et action utilisateur.

### Phase 4 — émission contrôlée

Seulement après compréhension suffisante du protocole :

- reproduction d'une trame connue ;
- validation sur banc ;
- émission minimale et ciblée ;
- surveillance des erreurs CAN.

Aucune injection aléatoire sur le réseau véhicule.

## Terminaison CAN

### Sur le véhicule

Lorsque le module est branché **en dérivation sur un bus CAN déjà correctement terminé**, ne pas ajouter une résistance de `120 Ω` supplémentaire.

Une résistance supplémentaire modifierait l'impédance du réseau et pourrait dégrader ou perturber les communications.

### Sur banc isolé

Un réseau de test isolé doit disposer d'une terminaison cohérente avec sa topologie. Ne pas extrapoler la règle « pas de 120 Ω » du véhicule à un banc constitué uniquement de deux nœuds et de quelques fils.

Le CSW-2000R lui-même a été mesuré à environ `37 kΩ` entre CAN-H et CAN-L hors tension : il ne constitue donc pas une terminaison fixe de `120 Ω`.

## Bibliothèques / projets de référence

Projets identifiés comme bases utiles :

### FoodyFood / esp32-mcp2518fd-driver

`https://github.com/FoodyFood/esp32-mcp2518fd-driver`

Intérêt :

- ESP32 ;
- MCP2518FD ;
- émission/réception ;
- CAN classique et CAN-FD ;
- gestion FIFO / erreurs.

Le projet utilise un transceiver ATA6561, mais cette différence n'impose pas de changement majeur dans le code pilotant le MCP2518FD.

### caleb-snow-cbm / MCP2518FD-esp32

`https://github.com/caleb-snow-cbm/MCP2518FD-esp32`

Intérêt : exemple ESP32 + MCP2518FD sous ESP-IDF, utile comme référence pour SPI et initialisation.

### Longan-Labs / Longan_CANFD

`https://github.com/Longan-Labs/Longan_CANFD`

Intérêt : bibliothèque MCP2517FD/MCP2518FD, CAN 2.0 / CAN-FD, filtres et masques.

### LilyGO / T-2Can

`https://github.com/Xinyuan-LilyGO/T-2Can`

Intérêt : exemple réel d'une plateforme ESP32 + contrôleur MCP2518FD/MCP2518.

### Marius-U / pican

`https://github.com/Marius-U/pican`

Intérêt : projet utilisant le couple **MCP2518FD + ATA6563** comme interface/analyseur CAN. La plateforme hôte n'est pas un ESP32, mais la chaîne matérielle valide l'association des deux circuits.

## Base logicielle privilégiée

Pour le prototype ESP32, partir en priorité de :

`FoodyFood/esp32-mcp2518fd-driver`

Objectif initial : retirer tout ce qui n'est pas nécessaire et obtenir un firmware minimal fiable d'écoute passive.

Configuration à vérifier impérativement avant utilisation :

```text
OSCILLATOR = 40 MHz
MODE BANC  = Normal20B
MODE AUTO  = ListenOnly
CAN TYPE   = Classical CAN 2.0B
BITRATE    = 500000 bit/s
```

Brochage module utilisé sur le banc :

```text
GPIO5  -> P1-1 nCS
GPIO19 <- P1-3 SDO
GPIO23 -> P1-5 SDI
GPIO18 -> P1-7 SCK
GPIO27 <- P1-9 INT
GND    -> P1-8/P1-11
```

Voir `docs/MCP2518FD_MODULE_JESSINIE.md` pour le brochage complet.

## Règle projet

**Cette voie n'est plus la référence des essais CAN.**

Référence active depuis le 2026-09-18 :

```text
ESP32 TWAI -> transceiver -> CAN
```

Le MCP2518FD reste disponible pour un futur troisième canal, du CAN-FD ou une interface spécifique Raspberry/SPI.
