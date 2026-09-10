# GPS / navigation autonome de secours

## Objectif

Le système doit rester capable d'afficher une navigation routière même si aucun iPhone / CarPlay n'est disponible.

Deux modes sont donc prévus :

```text
Mode principal
GNSS Raspberry -> LIVI -> CarPlay -> application de navigation iPhone

Mode secours
GNSS Raspberry -> moteur de navigation Linux hors ligne -> écran 7 pouces
```

La navigation de secours doit fonctionner sans réseau mobile et sans iPhone, à partir de cartes téléchargées localement.

## Récepteur GNSS cible

LIVI accepte les récepteurs série NMEA-0183. Sa documentation indique un développement/test avec u-blox NEO-M9N et ajoute des informations matérielles UBX pour les modules u-blox.

Cible de travail recommandée :

- u-blox NEO-M9N ou module équivalent NMEA-0183 ;
- antenne GNSS active adaptée à l'automobile ;
- connexion USB-série ou UART selon le prototype ;
- montage définitif seulement après validation de la réception dans le véhicule.

Le module GNSS doit également pouvoir fournir l'heure système et aider à restaurer heure/date/fuseau après une coupure complète d'alimentation.

## Ce que LIVI sait faire

LIVI sait :

- lire un récepteur GNSS série ;
- publier latitude/longitude dans sa télémétrie ;
- transmettre la position à CarPlay / Android Auto ;
- utiliser l'horloge satellite comme source de temps ;
- dériver le fuseau horaire depuis la position.

LIVI v8.3.0 ne fournit pas à lui seul une carte routière autonome complète sur sa page Home.

## Navigation de secours

### Candidat principal à tester : Navit

Navit est un moteur de navigation automobile Linux autonome :

- cartes OpenStreetMap locales ;
- calcul d'itinéraire hors ligne ;
- guidage virage par virage ;
- GPS via gpsd ou NMEA direct ;
- interface adaptée historiquement aux car-PC et écrans tactiles/embarqués.

Avantage pour ce projet : fonctionnement réellement autonome sans téléphone ni Internet.

Inconvénient : interface plus ancienne ; il faudra vérifier lisibilité, résolution 7 pouces, commandes physiques et intégration visuelle avec LIVI.

### Candidat secondaire : Organic Maps Linux

Organic Maps possède une version Linux avec cartes et routage hors ligne, mais la version Linux n'a pas encore la même maturité d'interface/GPS que les versions mobiles. Elle reste intéressante à tester mais ne doit pas encore être considérée comme la solution de secours validée.

## Distribution des données GNSS

Ne pas ouvrir directement le même port série physique depuis LIVI et le navigateur de secours simultanément.

Architecture à définir lors du prototype :

```text
                  GNSS physique
                       |
              service de distribution
                 /             \
            LIVI            navigation
        CarPlay/temps       secours Linux
```

Options à évaluer : gpsd + pont NMEA/PTY pour LIVI, ou autre multiplexage propre.

Aucune solution de multiplexage n'est validée à ce stade.

## Comportement cible

```text
Démarrage véhicule
       |
       v
LIVI / Home
       |
       +-- CarPlay disponible -> navigation iPhone prioritaire
       |
       +-- CarPlay indisponible -> Navigation secours sélectionnable
                                   |
                                   v
                              cartes locales
                              + GNSS Raspberry
```

La caméra de recul reste prioritaire sur les deux modes :

```text
reverse ON -> caméra immédiatement
reverse OFF -> retour à l'écran précédent
```

## Critères de validation

- [ ] récepteur GNSS détecté après reboot ;
- [ ] fix à froid dans le véhicule ;
- [ ] position stable en déplacement ;
- [ ] heure correcte après coupure batterie sans réseau ;
- [ ] cartes France / zones utiles disponibles hors ligne ;
- [ ] calcul d'itinéraire sans Internet ;
- [ ] recalcul après sortie de route ;
- [ ] guidage vocal ou indication visuelle exploitable ;
- [ ] commandes physiques suffisantes sur écran non tactile ;
- [ ] bascule vers caméra < 1 s si possible ;
- [ ] retour correct après caméra ;
- [ ] coexistence correcte avec LIVI / CarPlay ;
- [ ] plusieurs cycles reboot / coupure totale validés.

## Statut

DESIGN / NON TESTE — 2026-09-10.

La navigation autonome est désormais une exigence du projet, mais ni le module GNSS ni le logiciel de secours ne sont encore validés sur le banc.
