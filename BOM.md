# BOM — Bill of Materials

Les prix sont des ordres de grandeur. Ne pas commander les éléments marqués `WAIT` avant validation de l’étape correspondante.

## Statuts

- `HAVE` : déjà disponible ;
- `BOUGHT` : acheté pour le projet ;
- `SELECT` : choix/achat à court terme ;
- `DESIGN` : architecture définie mais composant exact à choisir ;
- `WAIT` : ne pas acheter avant validation d’une étape précédente ;
- `DEFERRED` : volontairement reporté.

| Élément | Référence / type | Qté | Etat | Priorité | Budget cible |
|---|---|---:|---|---|---:|
| Raspberry Pi 4 | existant | 1 | HAVE | P0 | 0 € |
| ESP32 banc | ESP-WROOM-32 / carte de développement existante | 1 | HAVE | P0 | 0 € |
| Commande centrale | Renault `8200326970`, `CSW-2000R` | 1 | BOUGHT | P0 | acheté |
| Commande volant | Renault `7701049643`, `34442201AF` | 1 | BOUGHT | P0 | acheté |
| CAN prototype #1 | module `MCP2518FD + ATA6563`, SPI, H/G/L | 1 | BOUGHT | P0 | acheté |
| Terminaisons CAN privées | `120 Ω`, 1 %, 0,25 W minimum | 4 | SELECT | P0 | <5 € |
| Faisceau / breakout CSW | adaptateur réversible 12 voies pour isoler CAN du CSW | 1 | DESIGN | P0 | à chiffrer |
| Proxy volume OEM prototype | relais SPST-NO, 2 canaux minimum | 2 | SELECT | P0 | <10 € |
| Proxy commandes OEM final | PhotoMOS/OptoMOS bidirectionnels flottants, 2 canaux minimum / 8 si matrice complète | 2 à 8 | DESIGN | P1 | à chiffrer |
| Écran | 7" IPS HDMI, non tactile, forte luminosité | 1 | SELECT | P1 | 40–80 € |
| RP2040 prototype | RP2040-Zero ou Pico | 2 | SELECT | P1 | 5–20 € |
| MFi CP3.0 | Microchip `MFI343S00177-L`, LCSC `C33770534` | 2 conseillé | SELECT | P1 | quelques € |
| Passifs MFi | 4.7 kΩ ×2, 33 Ω ×2, 100 nF, 1 µF | 1 lot | SELECT | P1 | <5 € |
| Commutation MFi | load-switch 3.3 V actif haut, EN compatible GPIO 3.3 V | 1 | DESIGN | P1 | <5 € |
| CAN véhicule #2 | second MCP2518FD + transceiver automobile | 1 | WAIT | P2 | 15–30 € |
| CAN véhicule #3 optionnel | troisième canal si CSW privé + deux réseaux véhicule requis | 1 | WAIT | P3 | 15–30 € |
| K-Line | L9637D ou équivalent | 1 | WAIT | P3 | quelques € |
| Caméra recul | à choisir | 1 | SELECT | P1 | 20–50 € |
| Capture vidéo | selon caméra CVBS/AHD/USB | 1 | SELECT | P1 | 10–40 € |
| DAC audio | USB/I2S sortie ligne | 1 | WAIT | P1 | 10–30 € |
| Isolation audio | transformateur ligne 1:1 / solution anti-boucle de masse, si nécessaire | 1 | WAIT | P2 | 5–20 € |
| Micro | réemploi Renault ou automobile | 1 | WAIT | P1 | 0–20 € |
| Alimentation auto | buck 12→5 V + protections | 1 | DESIGN | P1 | 15–40 € |
| PCB interface V1 | JLCPCB/équivalent | 1 lot | WAIT | P2 | à chiffrer |
| ELS27 | V5/V5.2 Full | 1 | DEFERRED | P4 | ~150 € |

## Règles d’achat

- priorité aux fonctions visibles au quotidien ;
- le MFi n’est plus `WAIT` : il est requis pour valider CarPlay natif avec LIVI ;
- acheter au moins deux MFi si le surcoût reste négligeable, le boîtier étant petit et délicat à souder ;
- ne pas figer le load-switch MFi avant mesure du courant du prototype ;
- aucune interface diagnostic à ~150 € tant qu’elle n’est pas indispensable ;
- le premier module CAN MCP2518FD est acheté pour le banc CSW ;
- mesurer H↔L du module reçu avant d’ajouter une terminaison afin de vérifier la présence éventuelle d’un `120 Ω` intégré ;
- le bus privé CSW et le CAN véhicule restent physiquement séparés ;
- pour le volume OEM, deux relais SPST-NO suffisent au prototype ;
- ne pas choisir la référence PhotoMOS finale avant mesure de la tension/courant de balayage des 6 fils côté décodeur Renault ;
- si toute la matrice doit être réémulée, prévoir jusqu'à 8 contacts électroniques indépendants ;
- ne pas commander le troisième CAN avant d’avoir prouvé le besoin de deux réseaux véhicule simultanés en plus du CSW privé ;
- ne pas commander l’écran avant d’avoir validé dimensions extérieures, luminosité et connectique ;
- pour le PCB MFi final, préférer assemblage professionnel du DFN/XDFN.

## Références

- LIVI : `docs/LIVI_CARPLAY_SETUP.md`
- câblage MFi : `docs/MFI_WIRING.md`
- module CAN : `docs/MCP2518FD_MODULE.md`
- commande volant : `docs/STEERING_REMOTE.md`
- câblage interface : `hardware/espace_iv_interface_v1/WIRING_DRAFT.md`
- source fournisseur MFi : LCSC `C33770534`
