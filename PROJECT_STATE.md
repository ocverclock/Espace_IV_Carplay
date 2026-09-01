# Espace IV Modernisation Multimédia — PROJECT_STATE

Dernière mise à jour : 2026-09-01

## 1. But du projet

Moderniser le système multimédia d’un Renault Espace IV en conservant au maximum l’intégration d’origine :

- remplacer l’ancien GPS Carminat par une interface moderne ;
- utiliser un Raspberry Pi 4 déjà disponible ;
- afficher Apple CarPlay sur un nouvel écran 7" non tactile ;
- utiliser un iPhone comme téléphone principal ;
- utiliser Roole Map via CarPlay pour la navigation ;
- conserver et réutiliser la commande centrale Renault ;
- conserver et réutiliser la commande multimédia au volant, y compris sa molette ;
- conserver les appels mains libres ;
- intégrer une caméra de recul avec bascule automatique ;
- préparer l’accès futur aux données véhicule via CAN : température, turbo, régime, FAP, etc. ;
- garder une intégration propre, réversible autant que possible, sans gros boîtier permanent sur la prise OBD.

Le projet est volontairement construit par étapes pour éviter d’acheter du matériel coûteux avant d’avoir validé l’architecture.

---

## 2. Véhicule et système d’origine

### Système multimédia

Le véhicule est équipé du système Renault Carminat Navigation & Communication / CNC de génération Xanavi.

Éléments identifiés :

- écran GPS d’origine au tableau de bord ;
- unité navigation/GPS ancienne, destinée à être abandonnée fonctionnellement ;
- autoradio / système audio Renault d’origine ;
- commande centrale multimédia séparée ;
- commande multimédia au volant.

### Commande centrale

Référence achetée pour reverse engineering :

- Renault : `8200326970`
- modèle : `CSW-2000R`
- fabricant : Xanavi Informatics Corporation
- connecteur visible : 6 voies
- joystick central : famille ALPS, déjà démontée/réparée dans plusieurs publications en ligne.

Etat du reverse engineering :

- [CONFIRME] la pièce est démontable et réparée par des particuliers ;
- [CONFIRME] elle contient sa propre électronique ;
- [A VERIFIER] protocole de sortie exact ;
- [A VERIFIER] alimentation exacte ;
- [A VERIFIER] rôle de chacune des 6 broches ;
- [A VERIFIER] CAN, série, protocole propriétaire ou autre.

Ne pas supposer que le CSW-2000R est CAN tant que cela n’a pas été mesuré.

### Commande multimédia au volant

Référence achetée :

- Renault : `7701049643`
- marquage visible sur l’exemplaire acheté : `34442201AF`
- connecteur : 6 fils / 6 voies.

Etat des connaissances :

- [FORTEMENT PROBABLE] commande passive à contacts / matrice ;
- des adaptations existantes ont récupéré les boutons ;
- la molette n’est généralement pas récupérée dans les adaptations simples vers autoradio Android ;
- cela ne signifie pas qu’elle est inexploitable : elle semble demander un décodage de séquence de contacts plutôt qu’une simple valeur résistive.

Objectif : reverse engineering complet, molette comprise.

---

## 3. Pièces déjà achetées

Achat Ovoko effectué :

1. Commande multimédia au volant
   - `7701049643`
   - `34442201AF`

2. Commande centrale multimédia
   - `8200326970`
   - `CSW-2000R`

Ces pièces sont destinées au laboratoire afin de pouvoir les ouvrir, mesurer et modifier sans immobiliser ni risquer les pièces fonctionnelles du véhicule.

---

## 4. Écran

### Décision actuelle

Le nouvel écran sera :

- 7 pouces ;
- non tactile ;
- IPS de préférence ;
- HDMI de préférence ;
- luminosité élevée pour usage automobile ;
- piloté par le Raspberry Pi 4.

Le tactile n’est pas nécessaire : l’écran est trop éloigné du conducteur pour être utilisé confortablement à la main.

### Dimensions connues

Fenêtre visible actuelle approximative :

- largeur : `130 mm`
- hauteur : `70 mm`

Cette fenêtre ne constitue plus la limite définitive.

Décision : refaire le cache / entourage d’écran en impression 3D pour exploiter correctement toute la surface du nouvel écran 7".

L’espace disponible derrière l’écran d’origine est considéré comme important, mais les dimensions internes exactes seront relevées seulement au moment du démontage.

### Critères avant achat de l’écran

Ne pas commander uniquement sur la diagonale.

Vérifier :

- dimensions extérieures du module ;
- position des connecteurs HDMI / alimentation ;
- luminosité réelle ;
- angle de vision ;
- consommation ;
- température de fonctionnement ;
- possibilité de désactiver proprement le rétroéclairage ;
- résolution minimale cible : 1024×600 ;
- idéalement résolution supérieure si LIVI/CarPlay la gère proprement.

Objectif de luminosité : privilégier 500 nits ou plus si le prix reste raisonnable.

---

## 5. Apple CarPlay

### Contraintes utilisateur

- téléphone : iPhone ;
- navigation principale : Roole Map ;
- Roole Map est utilisé via Apple CarPlay ;
- Android Auto n’est donc pas l’objectif principal.

### Logiciel retenu

Base envisagée :

- Raspberry Pi 4 ;
- Linux ;
- LIVI comme head-unit CarPlay.

Le Raspberry Pi 4 est préféré car il est déjà disponible et son décodage vidéo H.264 est adapté à ce type de flux.

### Authentification Apple / MFi

Objectif final : ne pas utiliser de boîtier Carlinkit si cela n’apporte pas de valeur.

Composant retenu comme piste principale :

- Microchip `MFI343S00177-L`
- CarPlay Authentication Coprocessor 3.0
- boîtier DFN8 2×3 mm
- référence LCSC rencontrée : `C33770534`

L’idée est de monter directement ce composant sur notre PCB et de le raccorder au Raspberry Pi / LIVI par I²C.

Cela évite :

- le boîtier Carlinkit ;
- une couche USB supplémentaire ;
- un firmware propriétaire supplémentaire ;
- une boîte supplémentaire dans le véhicule.

[CONFIRME PAR LIVI]

Pour CarPlay natif, le coprocesseur MFi est une dépendance matérielle obligatoire.

LIVI peut être installé et son interface, son affichage et les entrées clavier/HID peuvent être testés sans MFi, mais la connexion iPhone et la validation réelle de CarPlay natif ne sont pas possibles sans coprocesseur MFi fonctionnel relié en I²C.

Source technique :
- documentation officielle LIVI — section MFi Authentication :
  `https://github.com/f-io/LIVI/blob/main/README.md#mfi-authentication`

Conséquence pour ce projet :

- le MFi de laboratoire doit être disponible avant les tests réels iPhone / CarPlay ;
- Roole Map, l’audio CarPlay, les appels, le microphone et Siri ne peuvent être considérés comme validables qu’après intégration du MFi ;
- ne jamais planifier « validation CarPlay native puis achat MFi ».

Cette correction est formalisée par la décision `D012` dans `docs/DECISIONS.md`.

[A VERIFIER AVANT PCB]
- disponibilité réelle du composant ;
- schéma d’application exact ;
- tension d’alimentation ;
- résistances de pull-up I²C ;
- GPIO éventuelle de power/reset demandée par LIVI ;
- contraintes MFi spécifiques.

---

## 6. Appels mains libres

Objectif : conserver les appels téléphoniques via CarPlay.

Architecture cible :

iPhone
→ CarPlay
→ LIVI / Raspberry Pi
→ sortie audio vers système Renault
→ haut-parleurs du véhicule

Microphone
→ interface audio
→ Raspberry Pi
→ CarPlay
→ iPhone

Priorité :

1. essayer de réutiliser le microphone Renault d’origine ;
2. si son câblage ou ses caractéristiques compliquent le projet, utiliser un microphone automobile externe discret.

La sélection des contacts peut être faite par :

- Siri ;
- interface Téléphone de CarPlay ;
- commandes physiques Renault une fois décodées.

---

## 7. Caméra de recul

Nouvelle exigence intégrée au projet.

Objectif :

- affichage automatique de la caméra lorsque la marche arrière est engagée ;
- retour automatique à CarPlay après sortie de marche arrière ;
- latence faible ;
- fonctionnement indépendant du téléphone.

Architecture générale :

caméra arrière
→ interface vidéo
→ Raspberry Pi
→ écran 7"

Signal marche arrière
→ entrée protégée
→ RP2040 ou entrée dédiée
→ application / service Linux
→ bascule automatique vers la caméra.

### Décisions encore ouvertes

Type de caméra :

- caméra analogique CVBS ;
- caméra AHD ;
- caméra USB UVC ;
- autre.

Pour une voiture, une caméra analogique/AHD avec câble long est probablement plus pratique qu’une caméra CSI Raspberry Pi, mais cela reste à arbitrer.

Critères :

- faible latence ;
- bonne vision nocturne ;
- étanchéité ;
- alimentation automobile ;
- disponibilité d’un convertisseur vidéo Linux stable ;
- comportement au démarrage ;
- absence d’écran bleu ou de délai long lors de l’enclenchement de la marche arrière.

Objectif cible de bascule : idéalement moins de 1 seconde.

---

## 8. Commande au volant — plan de reverse engineering

La commande achetée sera étudiée sur établi avant utilisation du RP2040.

### Etape 1 — inspection

- photographier recto / verso ;
- repérer les 6 broches ;
- relever les couleurs des fils si le faisceau est présent ;
- vérifier s’il existe réellement de l’électronique ou uniquement des pistes/contacts.

### Etape 2 — multimètre

Tester les 15 couples possibles entre 6 broches.

Pour chaque fonction :

- repos ;
- volume + ;
- volume - ;
- mute ;
- source ;
- boutons divers ;
- molette un cran dans un sens ;
- molette plusieurs crans ;
- molette dans l’autre sens.

Créer un tableau broche/broche/action.

### Etape 3 — molette

La molette doit être traitée comme un encodeur ou une séquence de contacts.

Rechercher des séquences du type :

- A ;
- A+B ;
- B ;
- repos ;

ou tout autre ordre répétitif.

Objectif : déterminer :

- sens ;
- nombre de crans ;
- rebond mécanique ;
- fréquence maximale réaliste.

### Etape 4 — RP2040

Une fois le comportement connu, le RP2040 pourra :

- scanner la matrice ;
- filtrer les rebonds ;
- décoder la molette ;
- produire des événements USB HID.

Exemples futurs :

- volume + → `KEY_VOLUMEUP`
- volume - → `KEY_VOLUMEDOWN`
- molette + → next / scroll +
- molette - → previous / scroll -
- mute → `KEY_MUTE`

---

## 9. Commande centrale CSW-2000R — plan de reverse engineering

Ne pas brancher directement les 6 broches au RP2040.

### Etape 1 — ouverture

- photos haute résolution des deux faces du PCB ;
- relever toutes les références de circuits intégrés ;
- identifier régulateurs, transistors, protections, éventuels transceivers CAN/UART.

### Etape 2 — continuité

Suivre les 6 broches du connecteur :

- alimentation ;
- masse ;
- éclairage ;
- réveil ;
- données éventuelles ;
- autre.

### Etape 3 — recherche documentaire

Pour chaque circuit intégré identifié :

- récupérer datasheet ;
- déterminer sa fonction ;
- comparer avec les pistes du PCB.

### Etape 4 — alimentation sur établi

Seulement après identification des alimentations.

Utiliser :

- alimentation de laboratoire ;
- limitation de courant ;
- montée progressive ;
- surveillance consommation.

### Etape 5 — protocole

Selon le résultat :

- contacts directs → RP2040 ;
- UART → analyseur logique / UART ;
- CAN → transceiver CAN ;
- autre série → oscilloscope/analyseur logique ;
- protocole propriétaire simple → décodage maison.

### Solution de secours

Si l’électronique d’origine est trop compliquée :

- garder toute la mécanique Renault ;
- lire directement le joystick ALPS et les boutons ;
- utiliser le RP2040 comme nouvelle électronique interne.

Cette solution garantit que la commande centrale restera exploitable même si son protocole d’origine n’est pas décodé.

---

## 10. Mapping fonctionnel visé

Mapping provisoire, à adapter après tests LIVI :

- `MAP 2D/3D` → CarPlay / Roole Map
- `INFO/ROUTE` → dashboard véhicule / CAN
- `MENU/SET` → menu système
- `BACK` → retour
- `DEST/HOME` → accueil / navigation
- `REPEAT/MUTE` → mute ou fonction média
- `LIGHT/DARK` → mode jour/nuit / luminosité
- joystick directionnel → navigation UI
- rotation molette centrale → déplacement / suivant-précédent
- clic molette → validation
- commande au volant → volume / média / navigation
- appui long sur une commande à choisir → Siri

---

## 11. RP2040

Le RP2040 n’est pas le Raspberry Pi.

Rôle prévu :

- lecture temps réel des commandes physiques ;
- gestion des rebonds ;
- décodage de la molette au volant ;
- éventuellement décodage direct du CSW si nécessaire ;
- lecture signal marche arrière ;
- éventuellement ACC / illumination ;
- présentation au Raspberry Pi comme périphérique USB HID.

Pour le développement, des modules RP2040-Zero peu coûteux sont suffisants.

Pour la version finale, le RP2040 pourra être intégré directement au PCB principal.

Le RP2040 ne sera pas utilisé comme contrôleur CAN principal.

---

## 12. CAN véhicule

### Objectif final

Pouvoir récupérer des données telles que :

- température liquide de refroidissement ;
- régime moteur ;
- vitesse ;
- pression admission ;
- pression turbo ;
- éventuellement pression turbo demandée/réelle ;
- pression rail ;
- FAP ;
- EGR ;
- autres paramètres Renault accessibles.

### Etat actuel

Un XTOOL A30M Bluetooth a été testé sous Linux.

Résultat :

- Bluetooth SPP présent ;
- mais protocole XTOOL propriétaire ;
- ce n’est pas un ELM327 exploitable directement par PyRen.

Un ancien ELM327 Wi-Fi a également été testé :

- IP : `192.168.0.10`
- TCP : `35000`
- `ATI` → `ELM327 v1.5`
- `ATDP` → `ISO 15765-4 (CAN 11/500)`
- requêtes `010C`, `0105`, `010B` → `CAN ERROR`

Conclusion : interface trop peu fiable pour poursuivre.

### ELS27

Une ELS27 V5/V5.2 Full a été étudiée.

Avantages :

- diagnostic Renault ;
- PyRen / DDT4All ;
- CAN principal ;
- possibilités de bus secondaires ;
- K-Line selon version.

Problème :

- prix environ 150 € en Europe.

Décision actuelle :

`[REPORTE]`

Ne pas acheter maintenant. Le budget doit d’abord aller au système multimédia fonctionnel.

---

## 13. Architecture CAN finale envisagée

Plutôt que laisser un gros adaptateur OBD dans le véhicule, fabriquer une petite interface intégrée.

Architecture :

Raspberry Pi 4
→ SPI
→ contrôleur CAN 1
→ transceiver CAN automobile
→ réseau véhicule

Raspberry Pi 4
→ SPI
→ contrôleur CAN 2
→ transceiver CAN automobile
→ réseau multimédia / secondaire

Candidat contrôleur :

- `MCP2518FD`

Candidat transceiver :

- `ATA6560` ou équivalent automobile.

Avantages :

- vrai contrôleur CAN matériel ;
- support Linux / SocketCAN ;
- faible coût ;
- taille réduite ;
- accès direct à `can0`, `can1`.

Des modules MCP2518FD + ATA6560 peu coûteux peuvent être utilisés pour les prototypes.

### Attention

Avant usage sur un bus véhicule existant :

- vérifier la fréquence du quartz ;
- vérifier la tension logique SPI ;
- vérifier la présence d’une terminaison 120 Ω ;
- supprimer/désactiver toute terminaison supplémentaire si nécessaire ;
- utiliser une dérivation courte ;
- utiliser une paire torsadée ;
- ajouter protections ESD/automobile sur le PCB final.

### Broches OBD / réseaux

CAN OBD standard :

- pin 6 : CAN-H
- pin 14 : CAN-L

L’existence et l’usage exact d’un CAN secondaire Renault sur d’autres broches, notamment 12/13, doivent être vérifiés sur CE véhicule avant câblage définitif.

Ne pas considérer 12/13 comme acquis tant que cela n’a pas été confirmé par schéma Renault ou mesure.

---

## 14. K-Line

Le PCB final pourra réserver l’emplacement d’un transceiver K-Line, par exemple :

- `L9637D`

Raison :

certains calculateurs de véhicules de cette génération peuvent encore utiliser ISO9141/KWP/K-Line.

Cette partie peut rester non peuplée si les tests montrent qu’elle n’est pas utile.

---

## 15. PCB final — vision actuelle

Objectif : une seule carte spécifique Espace IV plutôt qu’une collection de modules.

Bloc fonctionnel cible :

```text
+--------------------------------------------------+
|           Espace IV Interface Board              |
|                                                  |
| RP2040                                           |
|  - commande volant                               |
|  - CSW-2000R                                     |
|  - marche arrière                                |
|  - ACC / illumination                            |
|                                                  |
| MFI343S00177-L                                   |
|  - authentification Apple CarPlay                |
|                                                  |
| MCP2518FD #1 + transceiver CAN                   |
| MCP2518FD #2 + transceiver CAN                   |
|                                                  |
| emplacement K-Line / L9637D                      |
|                                                  |
| protections ESD / surtensions                    |
| entrées véhicule protégées                       |
| connecteurs détrompés                            |
|                                                  |
| liaison Raspberry Pi                             |
+--------------------------------------------------+
```

Le PCB ne doit être dessiné qu’après reverse engineering du CSW et de la commande au volant.

---

## 16. Alimentation automobile

Le Raspberry Pi ne doit pas être alimenté naïvement depuis un +12 V permanent de l’OBD.

Le système final devra gérer :

- +12 V automobile ;
- surtensions ;
- chute de tension au démarrage ;
- ACC / contact ;
- extinction propre Linux ;
- temporisation ;
- consommation à l’arrêt ;
- éventuel réveil rapide.

Architecture cible :

12 V véhicule
→ protection
→ buck automobile 5 V
→ Raspberry Pi

ACC/contact
→ entrée protégée
→ logique RP2040 / alimentation
→ shutdown propre
→ coupure après délai.

---

## 17. Audio

Objectif : conserver autant que possible l’audio Renault d’origine.

Piste principale :

Raspberry Pi
→ DAC / sortie ligne
→ entrée AUX du système Renault
→ amplification Renault
→ haut-parleurs d’origine.

Points à vérifier :

- type exact de système audio ;
- présence/activation AUX ;
- niveau ligne ;
- bruit de masse ;
- nécessité d’un isolateur audio ;
- comportement téléphone / média.

---

## 18. Logiciel

### Raspberry Pi

- Linux
- LIVI
- services système dédiés
- SocketCAN futur
- service commandes HID
- gestion caméra
- gestion ACC / shutdown
- dashboard véhicule futur.

### CarPlay

LIVI doit gérer :

- affichage CarPlay ;
- iPhone filaire/sans fil selon configuration ;
- audio ;
- microphone ;
- commandes physiques ;
- Siri ;
- Roole Map.

### Dashboard CAN

A développer après validation des données disponibles.

L’idée est de pouvoir basculer par exemple :

`INFO/ROUTE`
→ dashboard véhicule

et :

`MAP`
→ retour CarPlay / Roole Map.

---

## 19. Caméra de recul — intégration logicielle envisagée

Service Linux surveillant l’entrée marche arrière.

Pseudo-logique :

```text
si reverse == ON:
    mémoriser écran/application actuelle
    afficher caméra immédiatement
sinon:
    restaurer l’écran précédent
```

La caméra ne doit pas dépendre du fait que CarPlay soit connecté.

Prévoir un watchdog afin que la caméra reste disponible même si LIVI rencontre un problème.

---

## 20. Ordre de priorité budgétaire

### Priorité 1 — ergonomie et multimédia

- pièces de commandes Renault d’occasion : achetées ;
- écran 7" ;
- validation Raspberry Pi + LIVI hors CarPlay ;
- MFi de laboratoire ;
- validation CarPlay natif / Roole Map ;
- audio ;
- caméra de recul.

### Priorité 2 — intégration électronique

- RP2040 ;
- prototypes CAN ;
- alimentation automobile ;
- PCB maison.

### Priorité 3 — données moteur avancées

- reverse engineering CAN ;
- éventuelle interface de diagnostic professionnelle ;
- ELS27 seulement si elle devient réellement nécessaire.

Le projet ne doit pas dépenser 150 € dans un outil OBD avant d’avoir validé les fonctions utilisées tous les jours.

---

## 21. Matériel disponible / prévu

### Déjà disponible

- Raspberry Pi 4
- PC Linux pour développement
- imprimante 3D pour le cache écran
- XTOOL A30M pour diagnostic classique
- ELM327 Wi-Fi ancien, non retenu pour le projet.

### Acheté

- `8200326970 / CSW-2000R`
- `7701049643 / 34442201AF`

### A acheter à court terme

- écran 7" IPS non tactile suffisamment lumineux ;
- RP2040-Zero pour laboratoire ;
- MFi de laboratoire pour validation CarPlay native ;
- composants minimum nécessaires à son alimentation et à sa liaison I²C ;
- petit matériel de mesure / fils / crochets de test si nécessaire.

### A acheter après validation

- 2 interfaces MCP2518FD + transceiver pour prototype ;
- caméra de recul ;
- interface vidéo associée ;
- alimentation automobile ;
- DAC/audio si nécessaire.

---

## 22. Outils de laboratoire utiles

- multimètre ;
- alimentation de laboratoire avec limitation de courant ;
- oscilloscope ;
- analyseur logique ;
- micro-grabbers / crochets de test ;
- fils Dupont uniquement pour établi basse tension ;
- RP2040 ;
- Raspberry Pi 4 ;
- modules CAN.

Pour le CSW, ne jamais injecter 12 V au hasard avant d’avoir identifié masse et alimentation.

---

## 23. Roadmap

### Phase A — commandes Renault

1. réception CSW-2000R ;
2. réception commande au volant ;
3. photos détaillées ;
4. continuité commande au volant ;
5. décodage molette ;
6. inspection PCB CSW ;
7. identification brochage CSW ;
8. choix du mode d’interfaçage.

Livrable :
`docs/CONTROLS_REVERSE_ENGINEERING.md`

### Phase B — CarPlay sur établi

1. Pi 4 ;
2. LIVI ;
3. écran HDMI temporaire ;
4. validation démarrage / affichage / navigation clavier-HID hors CarPlay ;
5. intégrer un MFi de laboratoire ;
6. valider alimentation, liaison I²C et détection MFi par LIVI ;
7. connecter l’iPhone ;
8. valider CarPlay natif ;
9. valider Roole Map ;
10. valider commandes HID dans CarPlay ;
11. valider audio ;
12. valider microphone ;
13. valider Siri ;
14. valider reconnexion après reboot.

Livrable :
`docs/LIVI_CARPLAY_SETUP.md`

### Phase C — nouvel écran

1. choisir 7" ;
2. relever toutes dimensions ;
3. modéliser nouveau cache ;
4. impression prototype ;
5. validation visibilité ;
6. validation soleil / nuit.

Livrable :
`cad/display_bezel/`

### Phase D — caméra de recul

1. choisir technologie caméra ;
2. choisir interface vidéo ;
3. tester latence ;
4. récupérer signal marche arrière ;
5. écrire bascule automatique.

Livrable :
`docs/REVERSE_CAMERA.md`

### Phase E — PCB V1

Inclure :

- RP2040 ;
- MFi ;
- double CAN ;
- K-Line optionnelle ;
- entrées commandes ;
- reverse ;
- ACC ;
- protections.

Livrable :
`hardware/espace_iv_interface_v1/`

### Phase F — CAN Renault

1. acquisition passive ;
2. identifier bus ;
3. enregistrer trames ;
4. corréler actions véhicule ;
5. identifier données utiles ;
6. seulement ensuite envisager requêtes diagnostic actives.

Livrable :
`docs/CAN_RESEARCH.md`

---

## 24. Principes à respecter

1. Ne pas supposer un protocole Renault sans mesure.
2. Ne pas envoyer de commandes CAN actives avant d’avoir compris le bus.
3. Favoriser l’écoute passive.
4. Garder la prise OBD libre dans l’installation finale.
5. Ne pas couper le faisceau d’origine si un adaptateur/intermédiaire est possible.
6. Ne pas acheter un composant coûteux avant de savoir qu’il est nécessaire.
7. Conserver les pièces Renault visibles pour obtenir un résultat OEM+.
8. La caméra de recul doit fonctionner même sans iPhone.
9. Les commandes physiques restent prioritaires sur le tactile.
10. Documenter chaque découverte dans GitHub immédiatement.

---

## 25. Questions ouvertes

### Commande centrale

- protocole exact CSW-2000R ?
- alimentation ?
- brochage ?
- communication directement exploitable ou bypass interne nécessaire ?

### Commande au volant

- matrice exacte ?
- codage exact de la molette ?
- toutes les positions sont-elles détectables indépendamment ?

### Ecran

- modèle 7" final ?
- luminosité suffisante ?
- résolution optimale pour LIVI ?
- dimensions PCB réelles ?

### Audio

- comment reprendre proprement l’AUX ?
- micro Renault réutilisable ?

### Caméra

- CVBS, AHD ou USB ?
- meilleure interface faible latence Linux ?
- emplacement physique ?
- récupération signal marche arrière ?

### CAN

- topologie exacte des bus de cet Espace IV ?
- présence/utilité d’un bus multimédia accessible sans démonter ?
- quelles valeurs sont diffusées passivement ?
- quelles valeurs demandent une session diagnostic ?

### Alimentation

- emplacement +12 V / ACC définitif ?
- délai d’arrêt idéal ?
- besoin d’un supercondensateur / UPS ou simple shutdown contrôlé ?

---

## 26. Structure GitHub recommandée

```text
espace_iv_carplay/
├── README.md
├── PROJECT_STATE.md
├── BOM.md
├── docs/
│   ├── ARCHITECTURE.md
│   ├── CONTROLS_REVERSE_ENGINEERING.md
│   ├── CSW2000R.md
│   ├── STEERING_REMOTE.md
│   ├── LIVI_CARPLAY_SETUP.md
│   ├── DISPLAY.md
│   ├── AUDIO_MIC.md
│   ├── REVERSE_CAMERA.md
│   ├── POWER.md
│   ├── CAN_RESEARCH.md
│   └── TEST_LOG.md
├── firmware/
│   └── rp2040/
├── software/
│   ├── controls_bridge/
│   ├── camera_service/
│   └── dashboard/
├── hardware/
│   ├── prototypes/
│   └── espace_iv_interface_v1/
├── cad/
│   └── display_bezel/
├── captures/
│   ├── csw2000r/
│   ├── steering_remote/
│   └── can/
└── references/
```

---

## 27. Source de vérité pour les futurs chats

Lors d’une reprise dans un nouveau chat :

1. lire `PROJECT_STATE.md` ;
2. lire le dernier `docs/TEST_LOG.md` ;
3. lire le document correspondant à la partie en cours ;
4. ne pas se baser sur les anciennes hypothèses si elles sont contredites par des mesures plus récentes.

Les mesures sur le véhicule et sur les pièces physiques priment sur les informations de forums.

Ordre de confiance :

1. mesures réalisées sur notre Espace / nos pièces ;
2. schémas constructeur Renault ;
3. datasheets des composants ;
4. code / documentation des projets utilisés ;
5. reverse engineering public sérieux ;
6. forums ;
7. hypothèses.

---

## 28. Etat du projet au 01/09/2026

- architecture générale : définie ;
- Raspberry Pi 4 : disponible ;
- choix iPhone / CarPlay / Roole Map : défini ;
- LIVI : piste retenue ;
- MFi direct : piste retenue ;
- MFi de laboratoire : requis avant validation iPhone / CarPlay natif ;
- tests LIVI sans MFi : limités à l’application, l’affichage et les entrées locales/clavier/HID ;
- commande centrale de laboratoire : achetée ;
- commande au volant de laboratoire : achetée ;
- écran 7" : décision prise, modèle à choisir ;
- tactile : non nécessaire ;
- nouveau cache écran imprimé 3D : prévu ;
- caméra de recul : ajoutée au cahier des charges ;
- RP2040 : retenu pour les commandes physiques ;
- double CAN matériel : retenu pour la future carte ;
- ELS27 à ~150 € : reportée ;
- reverse engineering commande centrale : à démarrer à réception ;
- reverse engineering commande volant/molette : à démarrer à réception ;
- CAN véhicule : phase ultérieure ;
- PCB final : ne pas dessiner avant les mesures des commandes.
