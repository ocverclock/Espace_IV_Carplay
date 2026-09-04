# Roadmap

## M0 — Base documentaire
- [x] architecture générale
- [x] source de vérité
- [x] pièces de commande achetées
- [x] structure GitHub
- [x] ordre MFi / CarPlay corrigé
- [x] câblage MFi documenté

## M1 — Reverse engineering commandes

### M1.1 — CSW-2000R
- [x] recevoir `CSW-2000R`
- [x] ouvrir sans destruction
- [x] photos PCB / face boutons
- [x] confirmer connecteur `CN1` 12 voies (2 × 6)
- [ ] photo CN1 strictement de face et numérotation 1–12 verrouillée
- [ ] cartographie hors tension des 12 broches
- [ ] identification exacte MCU NEC / `IC3` / étage d'entrée
- [ ] identifier GND et alimentation
- [ ] première alimentation labo limitée en courant
- [ ] protocole CSW identifié ou décision de bypass

### M1.2 — commande au volant
- [ ] recevoir `7701049643`
- [ ] photos / inspection
- [ ] cartographie 6 broches commodo
- [ ] décodage complet de la molette

### M1.3 — interface Linux
- [ ] prototype USB HID sur RP2040

## M2 — LIVI / CarPlay sur établi

### M2.1 — plateforme
- [ ] installer Raspberry Pi OS / Debian 13 Trixie 64 bits
- [ ] vérifier arm64
- [ ] installer LIVI
- [ ] valider affichage HDMI
- [ ] valider démarrage automatique
- [ ] valider commandes clavier/HID hors CarPlay

### M2.2 — MFi
- [ ] assembler prototype `MFI343S00177-L`
- [ ] vérifier orientation / footprint
- [ ] valider load-switch 3.3 V
- [ ] valider GPIO21 → EN
- [ ] mesurer `MFI_VCC`
- [ ] mesurer courant MFi
- [ ] valider `/dev/i2c-2`
- [ ] détecter `0x10`
- [ ] vérifier logs LIVI

### M2.3 — CarPlay
- [ ] connecter l’iPhone en filaire
- [ ] valider CarPlay natif
- [ ] valider Roole Map
- [ ] valider commandes clavier/HID dans CarPlay
- [ ] valider audio
- [ ] valider micro
- [ ] valider Siri
- [ ] valider reconnexion après reboot
- [ ] valider CarPlay sans fil ensuite

> Le MFi est un prérequis à la validation réelle de CarPlay natif.

## M3 — Écran et façade
- [ ] choisir écran 7"
- [ ] mesurer logement complet
- [ ] CAO façade
- [ ] prototype imprimé
- [ ] validation visibilité soleil/nuit

## M4 — Caméra de recul
- [ ] choisir caméra
- [ ] choisir capture
- [ ] mesurer latence
- [ ] récupérer signal reverse
- [ ] bascule automatique < 1 s si possible
- [ ] restauration automatique CarPlay
- [ ] vérifier fonctionnement sans iPhone

## M5 — Audio / alimentation
- [ ] AUX Renault
- [ ] DAC
- [ ] micro
- [ ] ACC
- [ ] buck automobile
- [ ] shutdown propre
- [ ] consommation véhicule arrêté

## M6 — PCB V1

**Gate avant lancement PCB :**
- [ ] commandes Renault mesurées
- [ ] MFi validé sur banc
- [ ] pinout / footprint MFi revérifiés
- [ ] écran choisi
- [ ] stratégie alimentation validée

Contenu :
- [ ] RP2040
- [ ] MFi
- [ ] load-switch MFi
- [ ] double CAN
- [ ] K-Line optionnelle
- [ ] reverse
- [ ] ACC
- [ ] protections
- [ ] connecteurs
- [ ] points de test

## M7 — CAN Renault
- [ ] topologie bus
- [ ] écoute passive
- [ ] logs
- [ ] corrélation trames/événements
- [ ] télémétrie
- [ ] requêtes diagnostic uniquement si nécessaire

## M8 — Intégration véhicule
- [ ] faisceau réversible
- [ ] écran final
- [ ] caméra
- [ ] commandes
- [ ] audio
- [ ] alimentation
- [ ] tests démarrage/arrêt
- [ ] tests longs trajets
