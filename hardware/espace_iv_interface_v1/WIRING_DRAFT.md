# Espace IV Interface V1 — Wiring Draft

Statut : **DRAFT / à valider sur véhicule avant routage PCB**.

Ce document prépare le câblage du banc puis de l'installation finale. Il distingue volontairement le bus privé du `CSW-2000R` des réseaux CAN Renault d'origine.

## 1. Principe d'architecture des commandes

Objectif : les commandes Renault sont d'abord lues et arbitrées par notre électronique. Elles ne restent pas reliées passivement en parallèle au système OEM.

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
  ├── USB HID → Raspberry Pi 4
  └── émulation sélective de contacts → décodeur OEM Renault
```

Le point important est que la commande physique au volant est retirée du décodeur OEM. Le RP2040 devient l'arbitre : il peut consommer une commande uniquement côté Raspberry, ou reproduire volontairement certaines fermetures de contacts vers le décodeur Renault.

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

Ainsi, l'ancien système ne voit jamais directement les trames de boutons du CSW.

## 3. Commande au volant — lecture + proxy OEM

La commande au volant est passive par contacts secs. Elle sera déconnectée du décodeur Renault d'origine et lue directement par le RP2040.

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

Contacts mesurés :

```text
volume -         = 4 + 6
volume +         = 4 + 1
source -         = 3 + 5
source +         = 6 + 5
bouton inférieur = 2 + 4
molette sens A   = 2+6 → 2+3 → 2+1
molette sens B   = 2+1 → 2+3 → 2+6
```

### Proxy de volume OEM

Le volume de l'autoradio Renault **n'est pas conservé au démarrage** sur le véhicule utilisateur. La stratégie "volume OEM fixe + variation uniquement côté Pi" n'est donc plus la stratégie principale.

À la place, le RP2040 pourra reproduire électriquement les fermetures de contacts d'origine **du côté du décodeur OEM** :

```text
commande physique au volant
        │
        ▼
      RP2040
        │
        ├── décision logique / éventuel HID vers Pi
        │
        └── sorties de commande
             │
             ├── contact électronique 4↔1 = VOL+
             └── contact électronique 4↔6 = VOL-
                        │
                        ▼
             faisceau côté décodeur Renault
                        │
                        ▼
             tableau de bord / afficheur OEM
                        │
                        ▼
                  système audio Renault
```

Le décodeur Renault voit donc exactement la même chose qu'avec le bouton d'origine, mais **uniquement quand le RP2040 décide de reproduire la fermeture**.

### Technologie de commutation

Pour le prototype :

- deux petits relais `SPST-NO` peuvent parfaitement simuler `VOL+` et `VOL-` ;
- c'est électriquement la solution la plus universelle tant que la tension/polarité de balayage OEM n'est pas mesurée.

Pour le PCB final, préférer :

- relais statiques **PhotoMOS / OptoMOS à sortie MOSFET bidirectionnelle**, ou
- interrupteurs analogiques bilatéraux compatibles avec la tension de balayage réellement mesurée.

Éviter de figer un simple transistor NPN/NMOS à la masse avant mesure : la matrice peut être balayée avec une polarité ou une référence qui ne correspond pas à cette hypothèse.

Un PhotoMOS agit comme un contact flottant et bidirectionnel, ce qui reproduit beaucoup mieux un bouton mécanique.

Nombre de canaux :

```text
2 canaux minimum : VOL+ / VOL-
5 canaux : tous les boutons
8 canaux : boutons + trois états de molette
```

La référence exacte des commutateurs ne sera choisie qu'après mesure sur le faisceau OEM :

- tension maximale entre les 6 lignes au repos ;
- courant de fermeture d'un bouton ;
- fréquence éventuelle du balayage.

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

Le volume général doit, à ce stade, rester piloté par l'électronique Renault puisque son niveau n'est pas mémorisé au démarrage.

Principe retenu :

```text
VOL+ / VOL- physiques
        ↓
      RP2040
        ↓
fermetures OEM synthétiques 4↔1 / 4↔6
        ↓
décodeur Renault d'origine
        ↓
commande volume système audio
```

Le Raspberry reste maître des entrées utilisateur, mais il autorise explicitement les deux commandes de volume à être reproduites vers le chemin OEM.

Le niveau de sortie du Pi/DAC sera donc plutôt maintenu à une valeur de ligne stable et sûre ; éviter de cumuler simultanément une variation logicielle importante et une variation OEM du volume.

À valider sur véhicule :

- tension/courant du balayage de la commande au volant côté décodeur ;
- durée minimale d'une fermeture reconnue pour VOL+/VOL- ;
- répétition attendue lors d'un appui long ;
- comportement AUX/wake de l'autoradio ;
- niveau ligne optimal du DAC ;
- bruit, boucle de masse et pops de boot/shutdown.

## 9. Règle de sécurité réseau

Ne jamais faire un simple pont transparent entre :

```text
CAN privé CSW ↔ CAN Renault OEM
```

Le principe du projet reste l'arbitrage des commandes par notre électronique. Une commande n'est reproduite vers l'OEM que si elle est explicitement autorisée, comme `VOL+` et `VOL-` dans l'architecture actuelle.
