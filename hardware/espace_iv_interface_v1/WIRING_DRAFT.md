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
  ├── lecture des boutons / molette
  ├── USB HID → Raspberry Pi 4
  └── émulation sélective de contacts → décodeur OEM Renault
```

Le RP2040 fonctionne donc dans les deux sens **fonctionnels** :

- il reçoit les états des commandes physiques ;
- il commande séparément des interrupteurs électroniques qui reproduisent certains contacts vers le système OEM.

Il ne relie pas directement ses GPIO aux lignes Renault : les deux côtés restent séparés par l'étage d'interface approprié.

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
source droite    = 3 + 5
source gauche     = 5 + 6
bouton inférieur = 3 + 4
mute              = volume + et volume - simultanés
molette sens A   = 2+6 → 2+3 → 2+1
molette sens B   = 2+1 → 2+3 → 2+6
```

### Prototype RP2040-Zero validé

Câblage de banc confirmé :

```text
commande 1..6 → RP2040-Zero GP0..GP5
```

Le scanner numérique, le debounce, le nommage des boutons, la combinaison `MUTE` et le décodage incrémental de la molette sont validés sur le matériel réel.

Firmware : `firmware/rp2040/steering_remote_test/steering_remote_test.ino`.

Statut : **BENCH TESTED / USER CONFIRMED — 2026-09-08**.

### Proxy de volume OEM

Le volume de l'autoradio Renault n'est pas conservé au démarrage. Le volume général doit donc continuer à être réellement appliqué par l'électronique Renault.

Le RP2040 reçoit l'état du bouton physique puis commande un **contact électronique distinct côté OEM** :

```text
commande physique
     │
     ▼
   RP2040
     │
     ├── décision / HID Raspberry
     │
     └── commande logique
              │
              ▼
       relais statique flottant
              │
              ├── 4↔1 = VOL+
              └── 4↔6 = VOL-
              │
              ▼
       décodeur Renault OEM
```

La progression du volume doit idéalement reproduire la durée réelle de l'appui :

```text
bouton pressé  → contact OEM fermé
bouton maintenu → contact OEM maintenu fermé
bouton relâché → contact OEM ouvert
```

Si le décodeur Renault possède l'auto-répétition d'origine, cette méthode conserve exactement son comportement et sa vitesse de progression. Ne générer une cadence artificielle qu'en cas de mesure contraire.

### PhotoMOS / OptoMOS

Ici `PhotoMOS` / `OptoMOS` désigne un **relais statique optiquement commandé**, pas un composant qui pilote un relais mécanique.

Structure conceptuelle :

```text
GPIO RP2040
    │
 résistance
    │
    ▼
 LED interne
    ║ isolation optique
    ▼
 paire MOSFET bidirectionnelle
    │
    ▼
contact électronique flottant
```

Pour notre usage, avantages recherchés :

- aucun clic ;
- aucune pièce mobile ;
- sortie flottante ;
- conduction dans les deux sens ;
- commande directe logique via LED + résistance ;
- comportement proche d'un bouton mécanique normalement ouvert.

Un simple optocoupleur à transistor n'est pas équivalent : sa sortie est polarisée et n'imite pas forcément proprement un contact sec bidirectionnel.

Pour le prototype, un relais mécanique reste acceptable uniquement pour valider le principe. Pour le PCB final, on vise un relais statique de cette famille après mesure :

- tension maximale entre lignes ;
- courant de fermeture ;
- résistance ON admissible ;
- fréquence de balayage.

Nombre de canaux :

```text
2 canaux minimum : VOL+ / VOL-
5 canaux : tous les boutons
8 canaux : boutons + trois états de molette
```

## 4. Passerelle CAN active — ESP32 / TWAI

L'architecture CAN principale ne dépend plus du module MCP2518FD pour le CAN classique.

### Banc CSW minimal

Pour terminer le reverse engineering du CSW, un ESP32 classique suffit :

```text
CSW CANH/CANL
      │
      ▼
transceiver 3,3 V
      │ TX/RX logique
      ▼
ESP32 TWAI0
```

Le transceiver de prototype peut être un module `SN65HVD230` 3,3 V ou équivalent compatible. Pour le PCB automobile final, choisir un transceiver adapté à l'environnement automobile.

Configuration :

```text
CAN classique 2.0B
500 kbit/s
mode Normal sur le bus privé CSW
```

Le mode normal est requis pour fournir l'ACK au CSW.

### ESP32-C6 — candidat double CAN

Si la carte C6 disponible est confirmée :

```text
CSW privé
   │
transceiver #1
   │
TWAI0
   │
ESP32-C6
   │
TWAI1
   │
transceiver #2
   │
CAN véhicule
```

Un transceiver est obligatoire par bus physique.

Le ESP32-C6 possède deux contrôleurs TWAI matériels ; cette architecture permet donc deux réseaux CAN simultanés sans MCP2518FD externe.

## 5. Liaison ESP32-C6 vers Raspberry Pi 4

### UART direct — voie préférée

```text
ESP32-C6 TX  ─────► Raspberry Pi RX
ESP32-C6 RX  ◄───── Raspberry Pi TX
GND          ────── GND
```

La liaison est bidirectionnelle.

Le C6 envoie vers le Pi :

- canal CAN source ;
- timestamp ;
- ID ;
- flags ;
- DLC ;
- données ;
- événements d'erreur / bus-off.

Le Pi peut envoyer vers le C6 :

- requête d'émission sur CAN0 ou CAN1 ;
- changement de filtre ;
- changement de mode si autorisé ;
- commandes de diagnostic de la passerelle.

Le protocole série final doit être binaire, délimité et protégé contre les trames partielles/corrompues. Le format exact reste à définir après validation du prototype.

### USB CDC — alternative

Le C6 peut également apparaître comme port série USB sur Linux. Cette solution reste utile en développement mais n'est pas prioritaire si UART direct suffit, afin d'éviter l'ajout d'un hub USB.

## 6. Terminaison CAN

### Bus privé CSW

Le CSW seul mesure environ `37 kΩ` entre CANH et CANL : il n'a pas de terminaison 120 Ω locale.

Sur un banc à deux nœuds, la terminaison doit être cohérente avec la topologie physique.

Valeur cible classique avec deux terminaisons :

```text
120 Ω // 120 Ω ~= 60 Ω
```

Toujours mesurer H ↔ L hors tension avant d'ajouter une résistance.

### Réseau véhicule

Lorsqu'un transceiver est branché en dérivation sur un bus véhicule déjà correctement terminé, **ne pas activer une terminaison 120 Ω supplémentaire**.

## 7. Module MCP2518FD Jessinie — matériel conservé / voie secondaire

Le module acheté reste documenté mais n'est plus la voie prioritaire.

Documentation :

- `docs/MCP2518FD_MODULE_JESSINIE.md`
- `docs/ESP32_MCP2518FD_CAN.md`
- `firmware/esp32_mcp2518fd_bench/`

Observation du 2026-09-18 :

```text
CSW TXD/RXD : trafic visible
MCP module INT : aucune activité visible
SPI ESP32 <-> MCP : non démontré
```

Ne pas conclure que le module est défectueux sans test SPI isolé. Il reste disponible comme :

- contrôleur supplémentaire si un troisième CAN est requis ;
- interface CAN-FD si un futur besoin apparaît ;
- solution de secours pour le Raspberry.

## 8. Ancien autoradio conservé comme amplificateur

Architecture audio de travail :

```text
CarPlay musique ───────┐
Navigation ────────────┤
Siri / appels ─────────┤
sons système ──────────┤→ mixeur logiciel Raspberry → master fixe → DAC
alertes locales ───────┘
                                              │
                                              ▼
                                        AUX Renault
                                              │
                                              ▼
                                      ampli / volume OEM
                                              │
                                              ▼
                                      haut-parleurs
```

Le mixeur logiciel sert à régler les **rapports entre sources** : niveaux relatifs, ducking, priorités, mute, limiteur. Il ne sert pas de volume général quotidien.

Le niveau master Raspberry/DAC doit rester fixe, calibré et avec suffisamment de headroom pour éviter le clipping.

Le volume général est piloté uniquement par l'ampli Renault via les fermetures OEM synthétiques :

```text
VOL+ / VOL-
    ↓
RP2040
    ↓
4↔1 / 4↔6
    ↓
volume Renault
```

À valider sur véhicule :

- tension/courant du balayage de la commande au volant côté décodeur ;
- durée minimale d'une fermeture reconnue ;
- comportement d'un appui maintenu ;
- comportement AUX/wake de l'autoradio ;
- niveau ligne fixe optimal du DAC ;
- bruit, boucle de masse et pops de boot/shutdown.

## 9. Règle de sécurité réseau

Ne jamais faire un simple pont transparent entre :

```text
CAN privé CSW ↔ CAN Renault OEM
```

Le principe du projet reste l'arbitrage des commandes par notre électronique. Une commande n'est reproduite vers l'OEM que si elle est explicitement autorisée, comme `VOL+` et `VOL-` dans l'architecture actuelle.
