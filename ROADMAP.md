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
- [x] numérotation 1–12 verrouillée
- [x] identifier GND : pins 2 et 8
- [x] identifier contacts CAN dupliqués : `5=6` et `11=12`
- [x] identifier `IC3` : Philips/NXP `PCA82C250`
- [x] protocole physique CSW : **CAN confirmé**
- [x] identifier polarité : `CN1-5/6 = CANH`, `CN1-11/12 = CANL`
- [ ] mesurer éventuelle terminaison entre CANH et CANL
- [ ] identifier alimentation d'entrée et rail 5 V
- [ ] identifier exactement MCU NEC
- [ ] suivre TXD/RXD entre PCA82C250 et MCU
- [ ] première alimentation labo limitée en courant
- [ ] déterminer bitrate CAN
- [ ] capturer trames au repos / boutons / joystick
- [ ] documenter IDs et payloads

### M1.2 — commande au volant
- [ ] recevoir `7701049643`
- [ ] photos / inspection
- [ ] cartographie 6 broches commodo
- [ ] décodage complet de la molette

### M1.3 — interface Linux
- [ ] prototype USB HID sur RP2040

## M2 — LIVI / CarPlay sur établi

### M2.1 — plateforme
- [x] installer Raspberry Pi OS / Debian 13 Trixie 64 bits
- [x] vérifier arm64
- [x] installer LIVI v8.3.0
- [x] valider démarrage automatique
- [x] valider accès graphique distant WayVNC via tunnel SSH
- [x] configurer LIVI en plein écran
- [x] remplacer l'overlay d'attente natif par heure + date dans `Projection.tsx`
- [x] conserver `Home.tsx` d'origine ; abandonner l'ancienne piste Home personnalisée
- [x] valider la chaîne `telemetry-sim -> telemetry:push -> LIVI -> dashboards`
- [x] réaffecter Dash4 à la télémétrie Espace IV
- [x] valider Dash4 avec données simulées moteur / FAP / turbo / électrique / carburant
- [x] utiliser une scène 1280×720 mise à l'échelle pour un rendu exploitable sur l'écran
- [x] externaliser le style de l'écran d'attente et de Dash4 dans `~/.config/LIVI/custom/espace-ui.css`
- [x] installer et valider la police Inter pour l'interface personnalisée
- [x] valider `git diff --check`, typecheck, build applicatif et build ARM64
- [x] exporter et archiver le patch LIVI exact contre v8.3.0
- [x] enregistrer taille + SHA256 de l'AppImage personnalisée et du patch
- [ ] valider avec CarPlay réel que l'overlay heure/date disparaît à la projection
- [ ] valider affichage HDMI physique stable
- [ ] valider commandes clavier/HID hors CarPlay

Checkpoint : `docs/LIVI_UI_TELEMETRY_CHECKPOINT_2026-09-10.md`.

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

## M3 — GPS / navigation autonome

Objectif : conserver une navigation routière de secours sans iPhone / CarPlay.

- [ ] choisir module GNSS NMEA-0183 ; cible de travail u-blox NEO-M9N ou équivalent
- [ ] valider réception GNSS dans le véhicule
- [ ] valider heure/date/fuseau après coupure totale
- [ ] tester Navit comme navigateur hors ligne principal de secours
- [ ] tester Organic Maps Linux comme alternative
- [ ] choisir stratégie de distribution GNSS entre LIVI et navigation de secours
- [ ] télécharger cartes hors ligne utiles
- [ ] valider calcul/recalcul d'itinéraire sans Internet
- [ ] valider bascule navigation principale CarPlay / navigation secours
- [ ] valider commandes physiques sur écran non tactile

Document : `docs/GPS_NAVIGATION.md`.

## M4 — Écran et façade
- [ ] choisir écran 7"
- [ ] mesurer logement complet
- [ ] CAO façade
- [ ] prototype imprimé
- [ ] validation visibilité soleil/nuit

## M5 — Caméra de recul
- [ ] choisir caméra
- [ ] choisir capture
- [ ] mesurer latence
- [ ] récupérer signal reverse
- [ ] bascule automatique < 1 s si possible
- [ ] restauration automatique écran précédent
- [ ] vérifier fonctionnement sans iPhone

## M6 — Audio / alimentation
- [ ] valider AUX Renault réellement accessible
- [ ] valider état AUX après reboot / coupure batterie
- [ ] prévoir commande automatique AUX si nécessaire
- [ ] définir fallback si AUX OEM inutilisable
- [ ] DAC
- [ ] lecture musique locale Raspberry
- [ ] micro
- [ ] ACC
- [ ] buck automobile
- [ ] shutdown propre
- [ ] redémarrage autonome après coupure totale
- [ ] consommation véhicule arrêté

## M7 — PCB V1

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

## M8 — CAN Renault
- [ ] topologie bus
- [ ] écoute passive
- [ ] logs
- [ ] corrélation trames/événements
- [x] valider le chemin logiciel télémétrie LIVI avec données simulées
- [ ] brancher le futur décodeur CAN réel sur le même contrat télémétrie
- [ ] requêtes diagnostic uniquement si nécessaire

## M9 — Intégration véhicule / mise en service
- [ ] faisceau réversible
- [ ] écran final
- [ ] caméra
- [ ] commandes
- [ ] audio
- [ ] alimentation
- [ ] tests démarrage/arrêt
- [ ] tests après déconnexion/reconnexion batterie
- [ ] tests longs trajets
- [ ] configurer plusieurs réseaux Wi-Fi via NetworkManager avec priorités d'autoconnexion
- [ ] utiliser de préférence un SSID dédié au Raspberry CarPlay avec VLAN limité
- [ ] vérifier la protection root-only des profils Wi-Fi et documenter la limite en cas d'accès physique à la microSD
