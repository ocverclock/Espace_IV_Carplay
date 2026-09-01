# LIVI / CarPlay Setup

## Cible

- Raspberry Pi 4
- Linux
- iPhone
- Apple CarPlay
- Roole Map
- commandes physiques
- écran HDMI 7" non tactile

## Dépendance critique : MFi

Pour la cible CarPlay native avec LIVI, un coprocesseur d’authentification Apple MFi physique est une dépendance du banc CarPlay.

Le README officiel de LIVI précise que :

- Apple CarPlay natif nécessite une authentification MFi ;
- le coprocesseur est matériel et ne peut pas être émulé en logiciel ;
- LIVI dialogue directement avec ce coprocesseur par I²C ;
- sans coprocesseur, CarPlay natif est indisponible.

Source :
- https://github.com/f-io/LIVI/blob/main/README.md#mfi-authentication

Conséquence importante :

- LIVI peut être installé et testé sans MFi pour l’affichage, le démarrage et la navigation locale ;
- la navigation clavier / D-Pad / HID peut également être préparée sans iPhone ;
- en revanche, la validation réelle de la connexion iPhone et de CarPlay natif ne doit pas être planifiée avant la présence d’un MFi fonctionnel ;
- Roole Map, l’audio CarPlay, les appels, le micro et Siri font donc partie de la phase de validation après intégration du MFi de laboratoire.

Cette règle corrige l’ancien ordre implicite « valider CarPlay puis acheter le MFi ».

## Ordre de validation sur établi

### Étape 1 — LIVI sans CarPlay

- [ ] LIVI s’installe et démarre ;
- [ ] affichage HDMI stable ;
- [ ] démarrage automatique ;
- [ ] navigation clavier fonctionne ;
- [ ] événements HID externes fonctionnent.

### Étape 2 — MFi de laboratoire

Piste retenue :

- `MFI343S00177-L`

À valider avant PCB final :

- [ ] composant réellement exploitable pour notre prototype ;
- [ ] alimentation correcte ;
- [ ] liaison I²C correcte ;
- [ ] résistances de pull-up adaptées ;
- [ ] éventuel reset / power control ;
- [ ] LIVI détecte et utilise correctement le coprocesseur.

### Étape 3 — CarPlay natif

Une fois le MFi fonctionnel :

- [ ] iPhone se connecte ;
- [ ] CarPlay apparaît ;
- [ ] Roole Map fonctionne ;
- [ ] navigation clavier/HID pilote correctement CarPlay ;
- [ ] audio média fonctionne ;
- [ ] appel entrant/sortant ;
- [ ] micro ;
- [ ] Siri ;
- [ ] reconnexion après reboot.

Documenter ici le schéma MFi validé et la configuration LIVI dès qu’ils sont testés.
