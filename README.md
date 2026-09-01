# Renault Espace IV — CarPlay / OEM+ Multimedia Retrofit

Projet open-source de modernisation du système multimédia d’un **Renault Espace IV équipé du Carminat Navigation & Communication (CNC / Xanavi)**.

L’objectif n’est pas de simplement poser un autoradio Android. Le projet vise une intégration **OEM+**, reproductible en totalité ou par sous-ensembles :

- nouvel écran 7" non tactile à l’emplacement du GPS d’origine ;
- Raspberry Pi 4 comme calculateur multimédia ;
- Apple CarPlay avec iPhone ;
- Roole Map via CarPlay ;
- réutilisation de la commande centrale Renault `CSW-2000R` ;
- réutilisation complète de la commande au volant `7701049643`, y compris sa molette ;
- appels mains libres ;
- caméra de recul avec affichage automatique ;
- interface CAN double canal pour télémétrie véhicule ;
- K-Line optionnelle ;
- PCB final compact dédié à l’Espace IV ;
- conservation de la prise OBD pour le diagnostic normal.

> **Etat : prototype / reverse engineering.**  
> Certaines fonctions sont confirmées, d’autres sont encore des hypothèses à mesurer. Le dépôt distingue explicitement les deux.

## Architecture cible

```text
                                iPhone
                                  │
                         CarPlay Wi-Fi / BT
                                  │
                         ┌────────▼─────────┐
                         │ Raspberry Pi 4   │
                         │ Linux + LIVI     │
                         └───┬──────┬───────┘
                             │      │
                           HDMI    Audio
                             │      │
                         écran 7"   └──► audio Renault
                             │
        ┌────────────────────┴─────────────────────┐
        │                                          │
┌───────▼──────────────────────────────────────────▼──────┐
│             Espace IV Interface Board                  │
│                                                       │
│ RP2040                                                │
│  ├─ commande au volant                                │
│  ├─ CSW-2000R                                         │
│  ├─ marche arrière                                    │
│  └─ ACC / illumination                                │
│                                                       │
│ MFI343S00177-L ── authentification Apple CarPlay      │
│                                                       │
│ MCP2518FD #1 + transceiver ── CAN véhicule            │
│ MCP2518FD #2 + transceiver ── CAN secondaire          │
│ L9637D optionnel ───────────── K-Line                  │
└───────────────────────────────────────────────────────┘
```

## Philosophie

1. **Mesurer avant de supposer.**
2. Les mesures faites sur la voiture et les pièces physiques priment sur les forums.
3. Les commandes Renault visibles sont conservées autant que possible.
4. Pas de gros adaptateur OBD installé définitivement.
5. L’écoute CAN passive est privilégiée avant toute requête active.
6. Les fonctions importantes doivent fonctionner sans téléphone quand cela a du sens, notamment la caméra de recul.
7. Chaque sous-système doit pouvoir être reproduit séparément.

## Sous-projets indépendants

Le dépôt est organisé pour permettre une reproduction partielle :

| Sous-système | Utilisable seul |
|---|---|
| Écran 7" + façade imprimée 3D | Oui |
| LIVI + CarPlay sur Raspberry Pi 4 | Oui |
| Commande au volant → USB HID | Oui |
| CSW-2000R → USB HID | Oui, après reverse engineering |
| Caméra de recul | Oui |
| Double CAN / télémétrie | Oui |
| PCB complet Espace IV | Oui, quand stabilisé |

## Pièces Renault déjà achetées pour laboratoire

- commande centrale : `Renault 8200326970` / `CSW-2000R`
- commande au volant : `Renault 7701049643` / marquage `34442201AF`

Ces pièces servent au reverse engineering sur établi afin de ne pas risquer les éléments encore installés dans le véhicule.

## Etat actuel

Voir **[PROJECT_STATE.md](PROJECT_STATE.md)** pour le checkpoint complet et à jour.

Résumé :

- [x] architecture générale définie ;
- [x] Raspberry Pi 4 disponible ;
- [x] cible Apple CarPlay / iPhone définie ;
- [x] Roole Map retenu pour la navigation ;
- [x] commande centrale de laboratoire achetée ;
- [x] commande au volant de laboratoire achetée ;
- [x] écran 7" non tactile retenu comme format ;
- [x] caméra de recul ajoutée au cahier des charges ;
- [x] RP2040 retenu pour les commandes physiques ;
- [x] double CAN matériel retenu pour le PCB futur ;
- [ ] reverse engineering commande au volant ;
- [ ] reverse engineering CSW-2000R ;
- [ ] choix exact de l’écran 7" ;
- [ ] prototype LIVI + MFi ;
- [ ] prototype caméra ;
- [ ] PCB V1 ;
- [ ] décodage CAN véhicule.

## Documentation

- [PROJECT_STATE.md](PROJECT_STATE.md) — source de vérité et checkpoint de reprise
- [BOM.md](BOM.md) — nomenclature et priorités d’achat
- [ROADMAP.md](ROADMAP.md) — étapes du projet
- [AI_HANDOFF.md](AI_HANDOFF.md) — ordre de lecture pour reprendre le projet
- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- [docs/REPRODUCTION_GUIDE.md](docs/REPRODUCTION_GUIDE.md)
- [docs/DECISIONS.md](docs/DECISIONS.md)
- [docs/CONTROLS_REVERSE_ENGINEERING.md](docs/CONTROLS_REVERSE_ENGINEERING.md)
- [docs/CSW2000R.md](docs/CSW2000R.md)
- [docs/STEERING_REMOTE.md](docs/STEERING_REMOTE.md)
- [docs/LIVI_CARPLAY_SETUP.md](docs/LIVI_CARPLAY_SETUP.md)
- [docs/DISPLAY.md](docs/DISPLAY.md)
- [docs/AUDIO_MIC.md](docs/AUDIO_MIC.md)
- [docs/REVERSE_CAMERA.md](docs/REVERSE_CAMERA.md)
- [docs/POWER.md](docs/POWER.md)
- [docs/CAN_RESEARCH.md](docs/CAN_RESEARCH.md)
- [docs/TEST_LOG.md](docs/TEST_LOG.md)

## Statuts utilisés dans la documentation

- **MEASURED** : mesuré sur notre matériel / véhicule.
- **CONFIRMED** : confirmé par une source technique fiable mais pas encore mesuré ici.
- **LIKELY** : fortement probable, à vérifier.
- **HYPOTHESIS** : hypothèse de travail.
- **TODO** : non réalisé.

## Sécurité

Un véhicule est un environnement électrique sévère. Le +12 V peut subir surtensions, transitoires, inversion ou chute de tension. Le CAN ne doit pas être perturbé par une terminaison ajoutée au mauvais endroit.

Pendant le développement :

- utiliser une alimentation de laboratoire limitée en courant ;
- ne jamais alimenter une broche inconnue ;
- ne pas injecter de trames CAN actives avant identification du bus ;
- désactiver les résistances 120 Ω des modules de test lorsqu’elles ne sont pas nécessaires ;
- conserver autant que possible le faisceau Renault intact.

## Licence

La licence définitive du projet n’est pas encore choisie. Elle sera définie avant la première release stable, avec séparation possible entre logiciel, matériel/CAD et documentation.
