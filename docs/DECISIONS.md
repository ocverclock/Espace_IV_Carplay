# Decision Log

## D001 — Raspberry Pi 4
**Décision :** utiliser le Pi 4 déjà disponible.  
**Raison :** coût nul, performances suffisantes, bon support vidéo.

## D002 — iPhone / Apple CarPlay
**Décision :** CarPlay est la cible principale.  
**Raison :** téléphone utilisé au quotidien ; Roole Map disponible via CarPlay.

## D003 — LIVI plutôt qu’OpenAuto
**Décision :** privilégier LIVI pour le prototype CarPlay.  
**Raison :** besoin Apple CarPlay, pas Android Auto.

## D004 — MFi direct
**Décision :** viser une puce MFi sur le PCB plutôt qu’un boîtier Carlinkit final.  
**Raison :** coût, compacité, contrôle de l’architecture.  
**Condition :** prototype à valider avant intégration finale.

## D005 — Écran 7" non tactile
**Décision :** écran non tactile.  
**Raison :** écran éloigné ; commandes physiques préférables.

## D006 — Nouvelle façade 3D
**Décision :** ne pas conserver la fenêtre 130×70 mm comme limite.  
**Raison :** impression 3D d’un nouveau cache permettant d’exploiter le 7" complet.

## D007 — Réutiliser les commandes Renault
**Décision :** reverse engineering des commandes plutôt que remplacement esthétique.  
**Raison :** résultat OEM+ et ergonomie existante.

## D008 — Molette au volant obligatoire
**Décision :** ne pas reproduire les adaptations qui abandonnent la molette.  
**Raison :** RP2040 permet un décodage de séquence plus complet.

## D009 — ELS27 reportée
**Décision :** ne pas dépenser ~150 € maintenant.  
**Raison :** CAN/télémétrie n’est pas la première valeur d’usage.

## D010 — Double CAN sur PCB
**Statut : SUPERSEDED BY D018/D019.**  
La nécessité de plusieurs bus reste vraie, mais l'implémentation n'est plus figée autour de MCP2518FD. Le nombre réel de canaux sera dimensionné après mesure de la topologie OBD/véhicule.

## D011 — Caméra de recul
**Décision :** fait partie du cahier des charges de base.  
**Raison :** forte valeur d’usage ; doit fonctionner sans téléphone.

## D012 — MFi requis avant validation CarPlay native
**Décision :** intégrer et valider un coprocesseur MFi de laboratoire avant de considérer la connexion iPhone / CarPlay native comme testable avec LIVI.  
**Raison :** LIVI indique que CarPlay natif nécessite un coprocesseur MFi matériel relié en I²C et qu’il est indisponible sans ce coprocesseur.  
**Source :** `https://github.com/f-io/LIVI` — section `MFi Authentication`.  
**Impact :** LIVI, l’affichage et les entrées HID peuvent être préparés sans MFi, mais pas CarPlay, Roole Map, l’audio CarPlay, les appels, le micro ou Siri.

## D013 — Trixie / Debian 13 pour le Raspberry Pi 4 LIVI
**Décision :** le banc et la cible Raspberry Pi 4 utilisent Raspberry Pi OS / Debian 13 Trixie 64 bits pour LIVI.  
**Raison :** la documentation LIVI actuelle exige OpenGL ES 3.x et précise que Pi 4 / CM4 / Pi 5 / CM5 nécessitent Trixie.  
**Impact :** ne pas construire la cible LIVI sur une ancienne Raspberry Pi OS Bullseye/Bookworm sans nouvelle validation en amont.  
**Source :** `https://github.com/f-io/LIVI` — section Installation.

## D014 — GPIO21 commande l’alimentation MFi via un load-switch
**Décision :** sur le schéma de référence, GPIO21 est une commande `EN`; l’alimentation du MFi provient du rail 3.3 V à travers un load-switch.  
**Raison :** LIVI expose `carPlayMfiPowerGpio=21`, mais la consommation maximale publique du coprocesseur n’est pas suffisamment documentée pour considérer l’alimentation directe par GPIO comme une architecture finale sûre. Le switch évite également la réalimentation parasite via les pull-up I²C.  
**Architecture :** `3.3 V → load-switch → MFI_VCC`, `GPIO21 → EN`, pull-up SDA/SCL vers `MFI_VCC`.  
**Statut :** décision d’architecture ; référence exacte du load-switch à choisir après mesure du courant du prototype.  
**Document :** `docs/MFI_WIRING.md`.

## D015 — Pinout MFI343S00177-L non promu en MEASURED avant prototype
**Décision :** utiliser pour le prototype le pinout recoupé publiquement `1/4/7/9=GND`, `5=SDA`, `6=SCL`, `8=VCC`, `2/3=NC`, mais le conserver au statut de preuve externe jusqu’au contrôle physique de notre première carte.  
**Raison :** plusieurs schémas publics concordent, mais la documentation fabricant complète n’est pas publiquement disponible.  
**Condition avant PCB :** revérifier orientation/footprint fournisseur et faire un contrôle de continuité avant première alimentation.

## D016 — Commandes Renault arbitrées par notre électronique
**Décision :** les commandes utilisateur Renault ne doivent pas rester reliées passivement en parallèle au chemin de commande OEM.

Architecture retenue :

- `CSW-2000R` isolé du CAN multimédia Renault et raccordé à un **bus CAN privé** vers la passerelle CAN du système ;
- commande au volant `7701049643` déconnectée du décodeur OEM et lue directement par le RP2040 ;
- aucun pont transparent entre le bus privé CSW et le CAN Renault ;
- le RP2040 peut toutefois reproduire volontairement certaines commandes vers le chemin OEM sous forme de fermetures de contacts synthétiques.

**Raison :** éviter qu’une même pression soit interprétée simultanément et sans contrôle par LIVI et par l’ancien autoradio/CNC. Notre électronique doit décider explicitement de la destination de chaque commande.

**Document :** `hardware/espace_iv_interface_v1/WIRING_DRAFT.md`.

## D017 — Volume OEM via proxy RP2040 et contacts statiques
**Observation :** le niveau de volume de l'autoradio Renault n'est pas conservé au démarrage.

**Décision :** conserver le volume général dans l'électronique Renault et utiliser le RP2040 comme proxy de la commande au volant.

Contacts mesurés :

```text
VOL+ = 4 ↔ 1
VOL- = 4 ↔ 6
```

Le RP2040 lit la commande physique puis ferme électroniquement les mêmes paires **sur le côté faisceau/décodeur OEM**.

La cible finale n'est pas le relais mécanique mais un **relais statique optiquement commandé** de famille PhotoMOS/OptoMOS, à sortie MOSFET flottante et bidirectionnelle. Le relais mécanique reste seulement un moyen de validation de prototype si nécessaire.

**Progression volume :** reproduire d'abord la durée réelle de l'appui. Tant que le bouton physique est maintenu, le contact OEM reste fermé. Si le décodeur Renault gère l'auto-répétition comme attendu, la progression reste entièrement OEM.

**Audio Raspberry :**

- master de sortie Pi/DAC fixe et calibré ;
- volume utilisateur global = électronique Renault ;
- mixeur logiciel Raspberry uniquement pour les rapports entre sources : musique, navigation, Siri/appels, sons système, alertes ;
- ducking/priorités/mute/limiteur gérés côté Raspberry ;
- éviter une double variation simultanée du master Pi et du volume OEM.

**Condition avant choix du relais statique :** mesurer tension, courant et éventuelle fréquence de balayage sur les lignes OEM.

**Documents :** `docs/STEERING_REMOTE.md`, `docs/AUDIO_MIC.md`, `hardware/espace_iv_interface_v1/WIRING_DRAFT.md`.


## D018 — TWAI natif prioritaire pour le CAN classique
**Décision :** pour le reverse engineering et l'intégration des bus CAN classiques de l'Espace IV, utiliser en priorité le contrôleur TWAI natif d'un ESP32 avec un transceiver externe par bus.

**Raison :** le contrôleur CAN est déjà intégré à l'ESP32 ; le MCP2518FD ajoute SPI, interruptions et une couche logicielle inutile pour un bus CAN 2.0 classique.

**Conséquence :**
- ESP32 classique = 1 bus CAN ;
- un transceiver est requis par bus ;
- le module MCP2518FD acheté reste disponible mais n'est plus la voie critique.

**Document :** `docs/CAN_GATEWAY_ESP32.md`.

## D019 — ESP32-C6 candidat passerelle CAN 2 voies
**Décision :** si la carte ESP32-C6 disponible est confirmée, la retenir comme candidat principal pour agréger deux bus CAN classiques.

**Fait matériel :** l'ESP32-C6 possède deux contrôleurs TWAI matériels (`TWAI0`, `TWAI1`). Les API ESP-IDF multi-instance permettent d'exploiter les deux contrôleurs avec un transceiver distinct par bus.

**Architecture de travail :**

```text
TWAI0 -> transceiver -> bus privé CSW
TWAI1 -> transceiver -> CAN véhicule
ESP32-C6 <-> Raspberry Pi
```

**Limite :** CAN classique uniquement, pas CAN-FD.

**Condition :** identifier la carte C6 réellement disponible et valider ses GPIO avant de figer le PCB.

## D020 — Liaison CAN gateway vers Raspberry par UART direct
**Décision :** pour un seul ESP32-C6, privilégier une liaison UART bidirectionnelle directe avec le Raspberry Pi plutôt qu'un hub USB.

**Raison :** moins de matériel, liaison simple, débit largement suffisant pour deux bus CAN classiques si le protocole est binaire et correctement tamponné.

**Alternative :** USB CDC pour développement ou si le câblage UART devient contraignant.

**Protocole minimal à définir :** canal, timestamp, ID, flags, DLC, données, commandes d'émission et état d'erreur.

## D021 — Ne pas supposer que l'OBD expose tous les CAN
**Décision :** mesurer la topologie réelle avant de dimensionner le nombre final de canaux CAN.

**État actuel :**
- OBD 6/14 est la paire CAN diagnostic/véhicule attendue ;
- un CAN multimédia distinct existe sur l'Espace IV ;
- l'accès du CAN multimédia via OBD 12/13 est documenté sur des Renault et rapporté sur des Espace, mais n'est pas encore mesuré sur notre véhicule.

**Impact :** aucun troisième canal CAN ne sera acheté ou routé avant le test physique des paires OBD.

**Document :** `docs/CAN_RESEARCH.md`.

## D022 — MCP2518FD conservé mais dépriorisé
**Décision :** conserver le module Jessinie MCP2518FD + ATA6563 comme matériel disponible et piste de secours.

**Observation :** lors du test du 2026-09-18, aucune activité n'a été observée sur `INT` alors que le CSW émettait. La communication SPI n'a pas été démontrée.

**Conclusion :** ne pas déclarer le module défectueux sans test SPI isolé, mais ne pas bloquer le projet dessus.
