# Renault Espace IV — CarPlay / OEM+ Multimedia Retrofit

Projet open-source de modernisation du système multimédia d’un **Renault Espace IV équipé du Carminat Navigation & Communication (CNC / Xanavi)**.

L’objectif n’est pas de poser un autoradio Android générique. Le projet vise une intégration **OEM+**, reproductible en totalité ou par sous-ensembles :

- nouvel écran 7" non tactile à l’emplacement du GPS d’origine ;
- Raspberry Pi 4 comme calculateur multimédia ;
- Raspberry Pi OS / Debian 13 Trixie 64 bits ;
- LIVI comme head-unit ;
- Apple CarPlay natif avec iPhone ;
- Roole Map via CarPlay ;
- authentification MFi directe ;
- réutilisation de la commande centrale Renault `CSW-2000R` ;
- réutilisation complète de la commande au volant `7701049643`, y compris sa molette ;
- appels mains libres ;
- caméra de recul avec affichage automatique et fonctionnement indépendant du téléphone ;
- interface CAN double canal pour télémétrie véhicule ;
- K-Line optionnelle ;
- PCB final compact dédié à l’Espace IV ;
- conservation de la prise OBD pour le diagnostic normal.

> **État : prototype / reverse engineering.**  
> Certaines fonctions sont confirmées par documentation, d’autres sont des hypothèses, et les mesures réellement réalisées sur notre matériel sont distinguées explicitement.

## Source de vérité

Pour reprendre le projet :

1. [PROJECT_STATE.md](PROJECT_STATE.md)
2. [docs/TEST_LOG.md](docs/TEST_LOG.md)
3. [docs/DECISIONS.md](docs/DECISIONS.md)
4. [ROADMAP.md](ROADMAP.md)
5. documentation du sous-système concerné

Le fichier [AI_HANDOFF.md](AI_HANDOFF.md) contient les règles de reprise pour un nouveau chat ou un nouveau contributeur.

## Architecture cible

```text
                                iPhone
                                  │
                         CarPlay filaire / Wi-Fi
                                  │
                         ┌────────▼─────────┐
                         │ Raspberry Pi 4   │
                         │ Trixie + LIVI    │
                         └───┬──────┬───────┘
                             │      │
                           HDMI    Audio
                             │      │
                         écran 7"   └──► audio Renault

                         GPIO / USB / SPI
                                  │
        ┌─────────────────────────▼────────────────────────┐
        │            Espace IV Interface Board            │
        │                                                  │
        │ RP2040                                           │
        │  ├─ commande au volant                           │
        │  ├─ CSW-2000R                                    │
        │  ├─ marche arrière                               │
        │  └─ ACC / illumination                           │
        │                                                  │
        │ 3.3 V → load-switch → MFI343S00177-L             │
        │         ▲                                        │
        │         └── GPIO21 EN                            │
        │ GPIO19 SDA / GPIO26 SCL                          │
        │                                                  │
        │ MCP2518FD #1 + transceiver → CAN véhicule        │
        │ MCP2518FD #2 + transceiver → CAN secondaire      │
        │ L9637D optionnel → K-Line                        │
        └──────────────────────────────────────────────────┘
```

## CarPlay / MFi — règle actuelle

CarPlay natif avec LIVI nécessite un coprocesseur MFi physique.

Ordre de validation :

```text
Pi 4 + Trixie
→ LIVI
→ affichage / clavier / HID
→ prototype MFi
→ détection I²C
→ CarPlay filaire
→ Roole Map / audio / micro / Siri
→ CarPlay sans fil
```

Le schéma de référence n’alimente pas directement le MFi depuis GPIO21 :

```text
3.3 V → load-switch → MFI_VCC
GPIO21 → EN
```

Documentation : [docs/MFI_WIRING.md](docs/MFI_WIRING.md).

## Philosophie

1. **Mesurer avant de supposer.**
2. Les mesures faites sur la voiture et les pièces physiques priment sur les forums.
3. Les datasheets et documentations officielles priment sur les commentaires isolés.
4. Une information externe ne devient pas `MEASURED` avant validation sur notre matériel.
5. Les commandes Renault visibles sont conservées autant que possible.
6. Pas de gros adaptateur OBD installé définitivement.
7. L’écoute CAN passive est privilégiée avant toute requête active.
8. La caméra de recul doit fonctionner sans téléphone.
9. Chaque sous-système doit pouvoir être reproduit séparément.
10. Le PCB final ne doit pas être lancé avant validation des sous-systèmes critiques.

## Sous-projets indépendants

| Sous-système | Utilisable seul |
|---|---|
| Écran 7" + façade imprimée 3D | Oui |
| LIVI + CarPlay sur Raspberry Pi 4 | Oui |
| MFi direct pour LIVI | Oui |
| Commande au volant → USB HID | Oui |
| CSW-2000R → USB HID | Oui, après reverse engineering |
| Caméra de recul | Oui |
| Double CAN / télémétrie | Oui |
| PCB complet Espace IV | Oui, quand stabilisé |

## Pièces Renault de laboratoire

- commande centrale : `Renault 8200326970` / `CSW-2000R`
- commande au volant : `Renault 7701049643` / `34442201AF`

Ces pièces permettent d’ouvrir, mesurer et modifier sur établi sans risquer les éléments encore installés dans le véhicule.

## État actuel

- [x] architecture générale définie ;
- [x] Raspberry Pi 4 disponible ;
- [x] cible Apple CarPlay / iPhone définie ;
- [x] Roole Map retenu ;
- [x] LIVI retenu ;
- [x] Trixie retenu pour le Pi 4 LIVI ;
- [x] nécessité du MFi avant validation CarPlay documentée ;
- [x] architecture de câblage MFi documentée ;
- [x] commande centrale de laboratoire achetée ;
- [x] commande au volant de laboratoire achetée ;
- [x] écran 7" non tactile retenu comme format ;
- [x] caméra de recul intégrée au cahier des charges ;
- [x] RP2040 retenu pour les commandes physiques ;
- [x] double CAN matériel retenu pour le PCB futur ;
- [ ] installation LIVI sur Pi 4 Trixie ;
- [ ] prototype MFi et détection `0x10` ;
- [ ] validation CarPlay ;
- [ ] reverse engineering commande au volant ;
- [ ] reverse engineering CSW-2000R ;
- [ ] choix exact écran 7" ;
- [ ] prototype caméra ;
- [ ] PCB V1 ;
- [ ] décodage CAN véhicule.

## Documentation

### Pilotage projet

- [PROJECT_STATE.md](PROJECT_STATE.md) — checkpoint global / source de vérité
- [BOM.md](BOM.md) — nomenclature et priorités d’achat
- [ROADMAP.md](ROADMAP.md) — étapes et gates
- [AI_HANDOFF.md](AI_HANDOFF.md) — reprise nouveau chat / contributeur
- [docs/DECISIONS.md](docs/DECISIONS.md) — décisions verrouillées
- [docs/TEST_LOG.md](docs/TEST_LOG.md) — mesures et essais réels

### Architecture / CarPlay

- [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- [docs/LIVI_CARPLAY_SETUP.md](docs/LIVI_CARPLAY_SETUP.md)
- [docs/MFI_WIRING.md](docs/MFI_WIRING.md)
- [docs/AUDIO_MIC.md](docs/AUDIO_MIC.md)
- [docs/DISPLAY.md](docs/DISPLAY.md)
- [docs/POWER.md](docs/POWER.md)

### Commandes Renault

- [docs/CONTROLS_REVERSE_ENGINEERING.md](docs/CONTROLS_REVERSE_ENGINEERING.md)
- [docs/CSW2000R.md](docs/CSW2000R.md)
- [docs/STEERING_REMOTE.md](docs/STEERING_REMOTE.md)

### Véhicule

- [docs/REVERSE_CAMERA.md](docs/REVERSE_CAMERA.md)
- [docs/CAN_RESEARCH.md](docs/CAN_RESEARCH.md)
- [docs/REPRODUCTION_GUIDE.md](docs/REPRODUCTION_GUIDE.md)

## Statuts utilisés

- **MEASURED** : mesuré sur notre matériel / véhicule ;
- **CONFIRMED** : confirmé par une source technique fiable ;
- **LIKELY** : fortement probable mais non démontré ;
- **HYPOTHESIS** : hypothèse de travail ;
- **TODO** : non réalisé.

## Sécurité

Un véhicule est un environnement électrique sévère. Le +12 V peut subir surtensions, transitoires, inversion ou chute de tension. Le CAN ne doit pas être perturbé par une terminaison ajoutée au mauvais endroit.

Pendant le développement :

- utiliser une alimentation de laboratoire limitée en courant ;
- ne jamais alimenter une broche inconnue ;
- ne jamais appliquer 5 V au MFi ;
- contrôler orientation et absence de court-circuit avant première alimentation du MFi ;
- ne pas injecter de trames CAN actives avant identification du bus ;
- désactiver les résistances 120 Ω des modules de test lorsqu’elles ne sont pas nécessaires ;
- conserver autant que possible le faisceau Renault intact.

## Licence

La licence définitive du projet n’est pas encore choisie. Elle sera définie avant la première release stable, avec séparation possible entre logiciel, matériel/CAD et documentation.
