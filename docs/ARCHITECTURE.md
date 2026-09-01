# Architecture

## Objectif

Construire un système multimédia moderne sans dépendre d’un autoradio Android fermé et sans perdre l’ergonomie Renault.

## Blocs

### Raspberry Pi 4
Rôle :
- Linux ;
- LIVI / CarPlay ;
- affichage ;
- audio ;
- service caméra ;
- SocketCAN ;
- dashboard futur.

### RP2040
Rôle :
- entrées temps réel ;
- commodo ;
- CSW si nécessaire ;
- reverse ;
- ACC ;
- USB HID.

### MFi
`MFI343S00177-L` envisagé pour authentification CarPlay native.

### CAN
Deux contrôleurs séparés afin de pouvoir écouter simultanément deux réseaux.

### K-Line
Optionnelle ; peuplée seulement si les mesures montrent qu’elle apporte une fonction utile.

## Interfaces de responsabilité

Le Raspberry ne doit pas dépendre du RP2040 pour afficher la caméra si une stratégie de secours plus robuste est nécessaire. Cette séparation sera réévaluée lors du prototype caméra.

Le RP2040 ne fait pas le CAN principal.

Le CAN n’est pas utilisé pour les commandes physiques si une lecture directe plus simple et plus sûre est disponible.
