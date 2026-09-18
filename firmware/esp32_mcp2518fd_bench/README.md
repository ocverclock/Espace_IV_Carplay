# ESP32 + MCP2518FD — décodeur de banc CSW-2000R

> **LEGACY / dépriorisé depuis le 2026-09-18.** La cible active est `firmware/esp32_can_gateway/` avec TWAI natif + transceiver. Ce dossier reste conservé pour documenter le matériel MCP2518FD déjà acheté et les essais effectués.

Objectif : utiliser l'ESP32 comme hôte SPI du module MCP2518FD/ATA6563 pour identifier les commandes émises par le Renault CSW-2000R.

## Données déjà confirmées

Mesures du projet :

- contrôleur : MCP2518FD ;
- transceiver : ATA6563 ;
- oscillateur du module : 40 MHz ;
- CAN classique 2.0B ;
- débit CSW : 500 kbit/s ;
- ID idle observé : 0x681 ;
- DLC : 8 ;
- payload idle :

    F0 0A 0A 01 FF FF FF FF

Le CSW seul ne termine pas le bus : environ 37 kOhm ont été mesurés entre CAN-H et CAN-L hors tension.

## Câblage ESP32

    ESP32 GPIO18 SCK  -> SCK
    ESP32 GPIO23 MOSI -> SDI
    ESP32 GPIO19 MISO <- SDO
    ESP32 GPIO5  CS   -> nCS
    ESP32 GPIO27 INT  <- INT
    ESP32 GND         -> GND

Côté CAN :

    H -> CSW CANH = CN1-5 ou CN1-6
    L -> CSW CANL = CN1-11 ou CN1-12
    G -> CSW GND  = CN1-8

Vérifier la sérigraphie d'alimentation exacte du module avant branchement.

## Terminaison de banc

Le CSW n'intègre pas de terminaison fixe de 120 ohms.

Sur notre banc, deux résistances de 220 ohms en parallèle ont donné environ 110 ohms entre CAN-H et CAN-L et ont permis le décodage propre à 500 kbit/s.

Avant d'ajouter une résistance, mesurer la résistance déjà présente entre H et L sur le module MCP2518FD reçu.

Ne pas transposer cette terminaison de banc sur le véhicule sans vérifier la topologie réelle du réseau.

## Librairie

Le sketch utilise ACAN2517 de Pierre Molinaro.

ACAN2517 gère le MCP2517FD/MCP2518FD en CAN classique 2.0B et fonctionne sur ESP32.

Configuration utilisée :

    Oscillator = OSC_40MHz
    Bitrate    = 500000
    Mode banc  = Normal20B

## Pourquoi Normal20B sur le banc ?

Sur le banc, le CSW et notre MCP2518FD sont les deux seuls nœuds.

Le CSW a besoin qu'un autre nœud valide ses trames avec le bit ACK. En ListenOnly, le MCP2518FD reçoit mais n'acquitte pas : le CSW retransmet alors continuellement la même trame.

Le sketch reste donc en Normal20B sur le banc, mais il n'appelle aucune fonction d'émission applicative.

Pour écouter un bus véhicule déjà actif, passer :

    BENCH_MODE = false

Le sketch utilisera alors ListenOnly.

## Fonctionnement du décodeur

Le firmware connaît la baseline idle :

    ID=681 DLC=8 DATA=F0 0A 0A 01 FF FF FF FF

Les répétitions de cette trame ne sont plus imprimées une par une.

Lorsqu'un payload différent apparaît, le terminal affiche par exemple :

    EVENT t=0012345678 ID=681 DLC=8 DATA=F0 0A 1A 01 FF FF FF FF DIFF=[B2:0A>1A]

Ici :

- B2 signifie octet d'index 2, donc troisième octet ;
- 0A est la valeur idle ;
- 1A est la nouvelle valeur.

Quand le CSW revient à la baseline :

    IDLE t=0012456789 ID=681 DATA=F0 0A 0A 01 FF FF FF FF

Les retransmissions identiques d'un même événement sont également masquées. Cela évite de saturer le port série tout en conservant chaque changement réel de payload, notamment pour la molette.

Une ligne STAT est affichée chaque seconde :

    STAT frames=... idle=... events=... other_id=... rx_peak=... rx_overflow=... mode=...

Si aucune trame n'est vue pendant plus de deux secondes, NO_CAN_TRAFFIC apparaît.

## Protocole pour cartographier le CSW

1. Démarrer le CSW sans toucher aux commandes.
2. Vérifier que les compteurs STAT augmentent.
3. Appuyer sur une seule commande.
4. Copier la ou les lignes EVENT.
5. Relâcher et vérifier le retour IDLE.
6. Refaire l'essai séparément pour chaque bouton, joystick et rotation.
7. Associer ensuite chaque payload à sa fonction dans la documentation du projet.

Ne pas essayer d'injecter des trames arbitraires pendant cette phase.
