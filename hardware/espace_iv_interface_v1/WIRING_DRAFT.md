# Espace IV Interface V1 — Wiring Draft

Statut : **DRAFT / à valider sur véhicule avant routage PCB**.

Ce document prépare le câblage du banc puis de l'installation finale. Il distingue volontairement le bus privé du `CSW-2000R` des réseaux CAN Renault d'origine.

## 1. Principe d'architecture des commandes

Objectif : les commandes Renault doivent être consommées par le Raspberry Pi uniquement, sans être interprétées en parallèle par l'ancien système multimédia.

Architecture visée :

```text
CSW-2000R
  │ CAN privé 500 kbit/s
  ▼
MCP2518FD + ATA6563
  │ SPI
  ▼
Raspberry Pi 4
  │
  └── service Linux / SocketCAN → commandes LIVI / CarPlay

Commande au volant 7701049643
  │ contacts secs
  ▼
RP2040
  │ USB HID
  ▼
Raspberry Pi 4
```

Les deux commandes sont donc **physiquement retirées du chemin de commande OEM**.

## 2. Isolement du CSW-2000R du réseau multimédia Renault

Brochage CSW mesuré/documenté :

```text
CN1-5 / CN1-6   = CANH
CN1-11 / CN1-12 = CANL
CN1-7            = + batterie protégé
CN1-8            = GND
CN1-1            = wake multimédia
CN1-9            = rhéostat / éclairage
CN1-10           = + éclairage
```

Dans l'installation finale, les lignes CAN du **côté CSW** ne doivent plus être reliées au réseau multimédia OEM :

```text
CSW CN1-5/6   ─────► CANH_PRIVÉ
CSW CN1-11/12 ─────► CANL_PRIVÉ
```

Les conducteurs correspondants du **côté faisceau Renault** restent isolés et ne sont pas pontés vers le bus privé.

Les lignes alimentation / masse / wake / éclairage peuvent rester issues du véhicule :

```text
véhicule ─► CN1-7   + batterie protégé
véhicule ─► CN1-8   GND
véhicule ─► CN1-1   wake
véhicule ─► CN1-9   rhéostat, si conservé
véhicule ─► CN1-10  éclairage
```

Ainsi, l'ancien autoradio ne voit jamais les trames de boutons du CSW.

## 3. Commande au volant

La commande au volant étant passive par contacts secs, elle sera déconnectée du décodeur Renault d'origine et lue directement par le RP2040.

Rappel du connecteur mesuré :

```text
1  2  3
4  5  6
```

Couleurs :

```text
1 blanc
2 pourpre/violet
3 beige
4 marron
5 orange
6 gris
```

Le RP2040 devient le seul équipement connecté à ces contacts. Il produit ensuite des événements USB HID vers le Pi.

## 4. Banc ESP32 + module MCP2518FD

Le premier module reçu servira au banc avec l'ESP32.

Câblage SPI proposé, ESP32 WROOM / VSPI classique :

```text
ESP32 GPIO18 SCK  ─────► module SCK
ESP32 GPIO23 MOSI ─────► module SDI
ESP32 GPIO19 MISO ◄───── module SDO
ESP32 GPIO5  CS   ─────► module nCS
ESP32 GPIO27 INT  ◄───── module INT
ESP32 GND          ───── module GND
```

Alimentation module : la notice autorise `3,3 V à 5 V` et annonce une compatibilité ESP32. **Vérifier la sérigraphie exacte du module reçu avant première alimentation et ne jamais relier simultanément deux entrées d'alimentation.**

Côté CAN :

```text
module H ─────► CSW CN1-5 ou CN1-6
module L ─────► CSW CN1-11 ou CN1-12
module G ─────► CSW GND / CN1-8
```

Le module doit être configuré en :

```text
CAN classique 2.0B
500 kbit/s
mode Normal20B
```

Le mode normal est requis pour fournir l'ACK aux trames du CSW.

## 5. Terminaison du bus privé

Le CSW seul mesure environ `37 kΩ` entre CANH et CANL : il n'a pas de terminaison 120 Ω locale.

Pour un bus privé à deux nœuds :

```text
[120 Ω]                     [120 Ω]
CSW ───────── câble CAN ───── MCP2518FD
```

Résistance attendue hors tension aux bornes du bus complet : environ `60 Ω`.

Sur le banc actuel, deux `220 Ω` en parallèle donnent environ `110 Ω` et peuvent servir temporairement d'une des terminaisons.

Avant de câbler la seconde terminaison, mesurer le module MCP2518FD reçu entre H et L pour vérifier s'il possède déjà une résistance 120 Ω intégrée.

## 6. Câblage final Raspberry Pi 4 → MCP2518FD

Pour la version finale, l'ESP32 n'est pas nécessaire. Le Pi peut piloter directement le MCP2518FD par SPI et l'exposer sous Linux via SocketCAN.

Proposition sur `SPI0` du Raspberry Pi 4 :

```text
Pi GPIO11 / pin 23 / SCLK ─────► MCP2518FD SCK
Pi GPIO10 / pin 19 / MOSI ─────► MCP2518FD SDI
Pi GPIO9  / pin 21 / MISO ◄───── MCP2518FD SDO
Pi GPIO8  / pin 24 / CE0  ─────► MCP2518FD nCS
Pi GPIO25 / pin 22        ◄───── MCP2518FD INT
Pi GND                    ──────► module GND
```

L'oscillateur exact du module (`20 MHz` ou `40 MHz`) reste à relever sur le module physique avant écriture de la configuration Linux finale.

## 7. Deuxième canal CAN véhicule

Le bus privé CSW consomme un canal CAN dédié.

Avec deux MCP2518FD seulement :

```text
CAN0 = bus privé CSW
CAN1 = CAN véhicule principal
```

Conséquence importante : si le projet doit finalement écouter **deux réseaux véhicule distincts en plus du CSW**, il faudra :

- soit ajouter un troisième canal MCP2518FD ;
- soit prévoir un troisième footprint optionnel sur le PCB ;
- soit abandonner l'écoute simultanée d'un des réseaux secondaires.

Ce point reste à décider avant routage du PCB final.

## 8. Ancien autoradio conservé comme amplificateur

Architecture audio de travail :

```text
Raspberry Pi / LIVI
       │
       ▼
DAC / sortie ligne
       │
       ▼
entrée AUX Renault
       │
       ▼
autoradio / étage ampli Renault
       │
       ▼
haut-parleurs d'origine
```

L'ancien autoradio peut rester connecté à ses alimentations et réseaux nécessaires à son fonctionnement, mais il ne reçoit plus les commandes utilisateur :

- CSW isolé sur CAN privé ;
- commande au volant isolée et lue par RP2040 ;
- aucun pont logiciel par défaut entre CAN privé CSW et réseau OEM.

Pour le volume, la piste préférée à tester est :

```text
autoradio laissé sur AUX avec gain fixe sûr
volume utilisateur géré par le Raspberry Pi / DAC
```

À valider sur véhicule :

- l'autoradio reste-t-il sur AUX sans écran/commande OEM ?
- conserve-t-il son dernier niveau de volume ?
- a-t-il besoin d'un message réseau spécifique pour le wake, le mute ou la sélection AUX ?
- risque de bruit, boucle de masse ou niveau de ligne trop élevé ?

Si l'autoradio exige certains messages de service, la solution de secours sera un **gateway à liste blanche** : seules les trames strictement nécessaires à l'ampli seront transférées, jamais les commandes de boutons.

## 9. Règle de sécurité réseau

Ne jamais faire un simple pont transparent entre :

```text
CAN privé CSW ↔ CAN Renault OEM
```

Le principe du projet est l'isolation physique des commandes. Toute passerelle future devra être explicitement filtrée par ID/payload et validée sur banc avant branchement véhicule.
