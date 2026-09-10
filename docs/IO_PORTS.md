# I/O et budget de ports — Raspberry Pi Espace IV

Objectif : éviter une architecture « pieuvre » et attribuer explicitement chaque fonction à une interface avant intégration finale.

## Règle de conception

Toute nouvelle fonction doit préciser avant validation :

1. son alimentation ;
2. son interface avec le Raspberry Pi ;
3. le port réellement consommé ;
4. son débit / criticité ;
5. si elle doit fonctionner contact coupé ;
6. si elle peut être intégrée à la future carte interface au lieu d'utiliser un boîtier USB externe.

## Répartition cible actuelle

| Fonction | Interface Pi cible | USB consommé | Statut / remarque |
|---|---|---:|---|
| Écran 7" | HDMI | 0 | cible principale |
| MFi natif LIVI | I²C + GPIO | 0 | si architecture MFi native retenue |
| CAN privé CSW | SPI -> MCP2518FD | 0 | SocketCAN côté Linux |
| CAN véhicule | SPI -> MCP2518FD | 0 | SocketCAN côté Linux |
| CAN véhicule supplémentaire | SPI -> contrôleur optionnel | 0 | seulement si besoin démontré |
| GNSS autonome | UART de préférence | 0 | éviter USB si possible |
| RP2040 commandes | USB HID | 1 | commande volant / HID ; ne porte pas le CAN principal |
| Caméra de recul | capture vidéo UVC USB si caméra analogique | 1 | port à réserver si capture USB retenue |
| Audio + micro | interface audio USB combinée de préférence | 1 | une seule interface pour sortie ligne + entrée micro |
| CarPlay filaire natif | USB iPhone | 1 | si MFi natif LIVI retenu |
| Dongle CarPlay USB | USB | 1 | alternative au chemin MFi natif ; ne pas compter simultanément avec l'iPhone USB natif sans besoin démontré |
| Stockage système | microSD High Endurance | 0 | solution de base actuelle |
| SSD externe | USB 3 | +1 | optionnel ; à justifier par besoin réel |
| Maintenance réseau | Wi-Fi intégré Pi | 0 | seulement lorsque le Pi est alimenté |

## Budget USB sans SSD

Architecture native MFi :

```text
USB #1 -> iPhone CarPlay filaire
USB #2 -> RP2040 HID
USB #3 -> capture caméra
USB #4 -> interface audio + micro
```

Les fonctions CAN, MFi, GNSS et écran n'occupent alors aucun port USB.

Si un dongle CarPlay USB est retenu à la place du chemin MFi natif, il prend le port dédié au CarPlay ; il ne doit pas être considéré comme un périphérique supplémentaire tant que les deux architectures ne sont pas utilisées simultanément.

## SSD

Le SSD n'est pas requis à ce stade.

Avantages possibles :

- meilleure endurance pour écritures intensives ;
- stockage local important ;
- logs ou bibliothèque musicale volumineux.

Inconvénients :

- consomme un port USB 3 ;
- ajoute boîtier / adaptateur / câble ;
- augmente la consommation et les points de panne ;
- rend probablement nécessaire un hub si les quatre autres périphériques USB restent requis.

Décision actuelle : conserver une microSD High Endurance avec shutdown propre et sauvegarde reproductible. Réévaluer le SSD seulement si stockage local ou volume d'écritures le justifie.

## Hub USB

Pas obligatoire dans l'architecture actuelle sans SSD.

Si un hub devient nécessaire :

- préférer un hub compact et alimenté de façon maîtrisée ;
- éviter de faire passer caméra et SSD sur le même chemin si cela crée des problèmes de débit ou d'alimentation ;
- privilégier les périphériques faibles débits sur le hub : RP2040 HID et éventuellement audio / dongle CarPlay selon essais ;
- réserver si possible un port direct pour le stockage USB et/ou la capture vidéo ;
- vérifier alimentation, reprise après boot et stabilité après plusieurs cycles véhicule.

## Carte interface future

La future carte Espace IV doit réduire le nombre de boîtiers externes en intégrant autant que raisonnablement possible :

- RP2040 ;
- MFi + load-switch si chemin natif retenu ;
- contrôleur(s) CAN + transceivers ;
- entrées ACC / illumination / reverse ;
- PhotoMOS / émulation commandes OEM ;
- protections automobiles ;
- logique de gestion d'alimentation.

Le DAC / interface audio et la capture vidéo peuvent rester des modules USB séparés tant que leur intégration sur PCB n'apporte pas un gain clair de fiabilité ou d'encombrement.

## Principe de validation PCB

Ne pas passer directement d'un schéma global à un PCB complexe non testé.

Ordre recommandé :

```text
prototypes fonctionnels séparés
        ↓
mesures électriques réelles
        ↓
sous-blocs validés un par un
        ↓
schéma complet
        ↓
ERC + revue croisée
        ↓
PCB V1 avec points de test / options / protections
        ↓
assemblage partiel et tests bloc par bloc
        ↓
assemblage final
```

Un premier PCB doit être considéré comme un prototype, pas comme une garantie de fonctionnement sans retouche.

## Références

- `docs/ARCHITECTURE.md`
- `docs/POWER.md`
- `docs/AUDIO_MIC.md`
- `docs/MFI_WIRING.md`
- `docs/MCP2518FD_MODULE.md`
- `BOM.md`
