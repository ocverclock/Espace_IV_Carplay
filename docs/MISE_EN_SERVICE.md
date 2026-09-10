# Mise en service finale — Espace IV CarPlay

Ce document regroupe les opérations à effectuer **avant le montage définitif dans le véhicule**, quand l'accès direct au Raspberry Pi deviendra difficile ou impossible.

## 1. Démarrage / mode kiosk

- mesurer le temps de boot réel avec `systemd-analyze` ;
- identifier les services lents avec `systemd-analyze blame` et `systemd-analyze critical-chain graphical.target` ;
- supprimer les composants desktop inutiles au démarrage ;
- conserver uniquement la session graphique minimale nécessaire à LIVI et à l'accès de maintenance ;
- masquer complètement le bureau : fond noir, aucune barre, aucune icône, aucun panneau visible ;
- lancer LIVI automatiquement en plein écran ;
- objectif UX : écran noir -> overlay heure/date LIVI -> CarPlay dès qu'il est prêt ;
- vérifier plusieurs démarrages complets après coupure d'alimentation.

## 2. Réseaux Wi-Fi

À préparer avant montage définitif :

- enregistrer tous les réseaux Wi-Fi utiles avec NetworkManager ;
- définir les priorités d'autoconnexion ;
- inclure les réseaux maison / atelier nécessaires et, si retenu, un partage de connexion de secours ;
- privilégier un SSID dédié au Raspberry CarPlay avec VLAN limité ;
- vérifier les permissions root-only des profils NetworkManager ;
- documenter la limite : une microSD non chiffrée reste lisible par un attaquant disposant d'un accès physique complet ;
- tester la reconnexion automatique sur chaque réseau avant installation dans le véhicule.

## 3. Audio / mixage

Architecture cible :

```text
CarPlay / navigation / Siri / appels / sons système
                     ↓
             mixage logiciel LIVI
                     ↓
          niveau de sortie Pi calibré
                     ↓
                  DAC
                     ↓
              AUX Renault
                     ↓
         volume utilisateur OEM Renault
```

Le niveau général Raspberry/DAC doit être calibré puis laissé fixe. Le réglage quotidien du volume reste assuré par l'électronique Renault.

Avant montage définitif :

- sélectionner la bonne sortie audio dans LIVI ;
- sélectionner la bonne entrée micro ;
- calibrer le niveau `Head Unit` / niveau ligne sans saturation ;
- régler séparément Music, Navigation, Voice Assistant, Phone Calls et System Sounds ;
- vérifier le comportement réel du ducking entre musique, navigation, Siri et appels ;
- vérifier l'absence de clipping à niveau élevé ;
- vérifier souffle, boucle de masse et pops au boot/shutdown ;
- sauvegarder les valeurs finales de mixage dans la configuration LIVI ;
- vérifier qu'elles persistent après plusieurs redémarrages et une coupure complète.

## 4. Accès de maintenance et alimentation

Avant fermeture de l'installation :

- confirmer SSH ;
- confirmer WayVNC via tunnel SSH si cette méthode est conservée ;
- vérifier qu'un accès de maintenance reste possible lorsqu'un réseau Wi-Fi connu est disponible ;
- valider un mode maintenance ACC OFF permettant de maintenir ou démarrer le Pi volontairement ;
- valider la sortie du mode maintenance suivie d'un `systemctl poweroff` propre ;
- vérifier que la coupure matérielle du 5 V intervient seulement après arrêt Linux ou timeout de sécurité ;
- conserver un moyen local simple de maintenance même si le réveil distant est retenu ;
- ne pas supposer qu'un Pi totalement hors tension peut être réveillé par SSH : le réveil distant exige un contrôleur de veille séparé ;
- mesurer la consommation de toute logique toujours alimentée avant validation ;
- documenter une procédure de récupération locale en cas de perte réseau ou de configuration incorrecte.

Architecture détaillée : `docs/POWER.md`.

## 5. Budget I/O / USB

Avant fermeture :

- vérifier la matrice réelle des ports selon `docs/IO_PORTS.md` ;
- attribuer chaque périphérique à un port/interface physique ;
- réserver les interfaces CAN au SPI plutôt qu'à un pont USB par défaut ;
- privilégier une interface audio USB combinant sortie ligne + entrée micro ;
- ne retenir un SSD USB que si son besoin réel justifie un port supplémentaire ;
- si un hub devient nécessaire, valider alimentation, stabilité au boot et partage de bande passante sur banc ;
- documenter le routage physique des câbles avant fabrication du faisceau/boîtier final.

## 6. Validation finale avant fermeture

Ne fermer définitivement l'intégration qu'après validation de :

- boot autonome ;
- absence de bureau visible ;
- reconnexion Wi-Fi automatique ;
- LIVI plein écran ;
- CarPlay ;
- commandes physiques ;
- audio / mixage / micro ;
- caméra de recul ;
- arrêt propre et redémarrage après coupure complète ;
- mode maintenance ;
- budget USB/I/O final ;
- accès de récupération documenté.
