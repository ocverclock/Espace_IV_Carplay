# Roadmap

## M0 — Base documentaire
- [x] architecture générale
- [x] source de vérité
- [x] pièces de commande achetées
- [x] structure GitHub

## M1 — Reverse engineering commandes
- [ ] recevoir `CSW-2000R`
- [ ] recevoir `7701049643`
- [ ] photos recto/verso
- [ ] cartographie 6 broches commodo
- [ ] décodage complet de la molette
- [ ] brochage CSW
- [ ] identification des IC du CSW
- [ ] protocole CSW identifié ou décision de bypass
- [ ] prototype USB HID sur RP2040

## M2 — CarPlay sur établi
- [ ] installer LIVI
- [ ] valider affichage
- [ ] valider Roole Map
- [ ] valider commandes clavier/HID
- [ ] valider audio
- [ ] valider micro
- [ ] valider Siri
- [ ] valider MFi direct

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

## M5 — Audio / alimentation
- [ ] AUX Renault
- [ ] DAC
- [ ] micro
- [ ] ACC
- [ ] buck automobile
- [ ] shutdown propre
- [ ] consommation véhicule arrêté

## M6 — PCB V1
- [ ] RP2040
- [ ] MFi
- [ ] double CAN
- [ ] K-Line optionnelle
- [ ] reverse
- [ ] ACC
- [ ] protections
- [ ] connecteurs

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
