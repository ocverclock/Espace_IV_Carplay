# CAN Gateway — ESP32 / TWAI

Dernière mise à jour : `2026-09-18`

## Statut

**ARCHITECTURE DE TRAVAIL ACTIVE**

Cette page remplace l'ancienne hypothèse où le banc CAN principal reposait sur :

```text
ESP32 -> SPI -> MCP2518FD -> ATA6563
```

Le module MCP2518FD acheté reste documenté et utilisable, mais il n'est plus la voie prioritaire pour le CAN classique de l'Espace IV.

## Principe retenu

Pour du CAN classique, l'ESP32 possède déjà le contrôleur CAN/TWAI.

Il ne faut donc ajouter qu'un **transceiver CAN physique** par bus :

```text
ESP32 TWAI TX/RX
      |
      v
transceiver 3,3 V
(SN65HVD230 pour prototype ; équivalent automobile à valider pour le PCB final)
      |
      +-- CAN-H
      +-- CAN-L
```

Le transceiver ne décode pas les trames. Il réalise uniquement l'interface électrique différentielle entre le contrôleur TWAI et CAN-H/CAN-L.

## Règle : un canal par bus physique

Un même bus CAN peut contenir de nombreux calculateurs. Un canal TWAI suffit pour écouter tous les nœuds présents sur **ce même bus**.

En revanche, deux bus CAN physiquement séparés nécessitent deux contrôleurs/canaux indépendants et deux transceivers.

```text
1 bus CAN physique
= 1 contrôleur TWAI
+ 1 transceiver
```

Ne jamais relier deux réseaux CAN indépendants en parallèle sur le même transceiver.

## ESP32 classique

L'ESP32 classique utilisé sur le banc possède un seul contrôleur TWAI exploitable.

Il convient parfaitement pour :

```text
ESP32 classique
  |
  +-- TWAI0 -> transceiver -> bus privé CSW
```

C'est la voie la plus simple pour terminer le reverse engineering du CSW-2000R.

## ESP32-C6

Le **ESP32-C6 contient deux contrôleurs TWAI matériels**, `TWAI0` et `TWAI1`.

La documentation Espressif actuelle décrit l'utilisation simultanée de deux instances avec l'API à handles, notamment `twai_driver_install_v2()`.

Les deux contrôleurs sont CAN classique ISO 11898-1 ; ils ne sont pas CAN-FD.

Architecture envisagée :

```text
                    ESP32-C6
                 +-------------+
CSW privé <------| TWAI0       |
 via transceiver |             |
                 |             |---- UART ou USB ----> Raspberry Pi 4
CAN véhicule <---| TWAI1       |
 via transceiver +-------------+
```

Conséquence : si le projet final n'a besoin que de :

1. bus privé CSW ;
2. un bus CAN véhicule ;

alors **un seul ESP32-C6 + deux transceivers suffit** pour la partie CAN.

## Communication ESP32-C6 <-> Raspberry Pi

### Option préférée : UART direct

Pour un seul ESP32-C6, l'UART est la solution la plus simple et évite d'ajouter un hub USB.

```text
ESP32-C6 TX  ----> Raspberry RX
ESP32-C6 RX  <---- Raspberry TX
GND          ----- GND
```

UART est bidirectionnel : le Pi reçoit les trames CAN décodées et peut aussi demander au C6 d'émettre une trame sur un bus donné.

Le protocole applicatif devra transporter au minimum :

```text
type
canal
timestamp
ID
flags
DLC
data[0..8]
```

### Option alternative : USB CDC

L'ESP32-C6 possède également USB Serial/JTAG et peut apparaître sous Linux comme port série CDC-ACM.

Cette solution est pratique pour le développement, mais plusieurs ESP connectés simultanément peuvent conduire à utiliser un hub USB. Elle n'est donc pas prioritaire tant qu'un seul C6 suffit.

## Réception ET émission

TWAI fonctionne dans les deux sens.

En mode normal, l'ESP32 peut :

- recevoir ;
- émettre ;
- arbitrer normalement ;
- fournir l'ACK CAN ;
- détecter les erreurs et bus-off.

En mode listen-only, il écoute sans influencer le bus.

Règle projet :

- réseau véhicule inconnu : `listen-only` d'abord ;
- bus privé CSW : mode normal autorisé pour fournir l'ACK ;
- aucune injection aléatoire sur le réseau véhicule.

## Application au CSW-2000R

Faits déjà mesurés :

```text
CAN classique
bitrate = 500000 bit/s
ID idle = 0x681
DLC = 8
payload idle = F0 0A 0A 01 FF FF FF FF
CANH = CN1-5 / CN1-6
CANL = CN1-11 / CN1-12
R(CANH,CANL) CSW seul ~ 37 kOhm
```

Sur le banc isolé, un contrôleur TWAI en mode normal est utile car le CSW a besoin d'un autre nœud pour acquitter correctement ses trames.

## Nombre de bus dans le véhicule

L'Espace IV utilise plusieurs réseaux / domaines de communication. Une prise OBD ne doit pas être supposée exposer automatiquement **tous** les bus physiques présents dans le véhicule.

Ce que nous considérons actuellement :

- un CAN diagnostic/véhicule est accessible sur OBD 6/14 ;
- un CAN multimédia distinct existe sur le véhicule ;
- l'accès éventuel de ce CAN multimédia sur OBD 12/13 est une piste documentaire à **vérifier physiquement sur notre Espace**, pas encore un fait mesuré.

Voir `docs/CAN_RESEARCH.md`.

## Quand le MCP2518FD redevient utile

Le MCP2518FD reste pertinent si :

- CAN-FD devient nécessaire ;
- un troisième bus CAN doit être ajouté sans changer de MCU ;
- le Raspberry doit disposer directement d'interfaces SPI/SocketCAN ;
- une contrainte de PCB justifie un contrôleur externe.

Pour le CSW et les bus CAN classiques connus de l'Espace IV, il n'est pas nécessaire à ce stade.

## Prochaine validation matérielle

1. identifier précisément la carte ESP32-C6 disponible ;
2. relever les GPIO disponibles ;
3. connecter un premier transceiver au TWAI0 ;
4. valider le CSW à 500 kbit/s ;
5. confirmer ACK + réception de `0x681` ;
6. cartographier les commandes du CSW ;
7. seulement ensuite câbler TWAI1 sur un deuxième bus.


## Module SN65HVD230 de prototype documenté

Le module utilisateur reçu/documenté expose :

```text
3.3V
GND
RX
TX
CANH
CANL
```

Câblage :

```text
TWAI_TX -> TX
TWAI_RX <- RX
3V3     -> 3.3V
GND     -> GND
```

La terminaison 120 Ω semble commutable par cavalier ; la vérifier au multimètre avant usage.

Document : `docs/SN65HVD230_MODULE.md`.
