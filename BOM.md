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
| Commande centrale | Renault `8200326970`, `CSW-2000R` | 1 | BOUGHT | P0 | acheté |
| Commande volant | Renault `7701049643`, `34442201AF` | 1 | BOUGHT | P0 | acheté |
| Écran | 7" IPS HDMI, non tactile, forte luminosité | 1 | SELECT | P1 | 40–80 € |
| RP2040 prototype | RP2040-Zero ou Pico | 2 | SELECT | P1 | 5–20 € |
| MFi CP3.0 | Microchip `MFI343S00177-L`, LCSC `C33770534` | 2 conseillé | SELECT | P1 | quelques € |
| Passifs MFi | 4.7 kΩ ×2, 33 Ω ×2, 100 nF, 1 µF | 1 lot | SELECT | P1 | <5 € |
| Commutation MFi | load-switch 3.3 V actif haut, EN compatible GPIO 3.3 V | 1 | DESIGN | P1 | <5 € |
| CAN prototype | MCP2518FD + transceiver auto | 2 | WAIT | P2 | 15–30 € |
| K-Line | L9637D ou équivalent | 1 | WAIT | P3 | quelques € |
| Caméra recul | à choisir | 1 | SELECT | P1 | 20–50 € |
| Capture vidéo | selon caméra CVBS/AHD/USB | 1 | SELECT | P1 | 10–40 € |
| DAC audio | USB/I2S sortie ligne | 1 | WAIT | P1 | 10–30 € |
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
- acheter les modules CAN de test seulement après réception/relevé des commandes Renault ;
- ne pas commander l’écran avant d’avoir validé dimensions extérieures, luminosité et connectique ;
- pour le PCB MFi final, préférer assemblage professionnel du DFN/XDFN.

## Références MFi

- LIVI : `docs/LIVI_CARPLAY_SETUP.md`
- câblage MFi : `docs/MFI_WIRING.md`
- source fournisseur : LCSC `C33770534`
