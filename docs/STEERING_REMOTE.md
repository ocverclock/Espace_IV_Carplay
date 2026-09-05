# Steering Remote — Renault 7701049643

## Identification

- Renault : `7701049643`
- exemplaire acheté : `34442201AF`
- connecteur : **6 voies** ;
- reverse engineering réalisé sur l'exemplaire laboratoire le `2026-09-04`.

Objectif : réutiliser **toutes** les commandes d'origine, notamment la molette généralement perdue dans les adaptations autoradio simples.

## Convention du connecteur

Convention issue du relevé utilisateur, détrompeur représenté à droite :

```text
1   2   3
4   5   6     [détrompeur]
```

Couleurs relevées :

| Pin | Couleur |
|---:|---|
| 1 | blanc |
| 2 | pourpre / violet |
| 3 | beige |
| 4 | marron |
| 5 | orange |
| 6 | gris |

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

## Boutons — matrice de contacts

Les commandes sont passives et ferment les paires suivantes :

| Repère | Fonction | Contacts |
|---|---|---|
| A | volume − | `4 + 6` |
| B | volume + | `4 + 1` |
| C | source − | `3 + 5` |
| D | source + | `6 + 5` |
| E | bouton inférieur — fonction OEM encore inconnue | `2 + 4` |

Correction importante : `D = source +` est **`6 + 5`**. L'ancien relevé `2 + 5` est invalidé.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

## Molette

La molette :

- **n'est pas cliquable** ;
- utilise `pin 2` comme **contact commun permanent** ;
- commute successivement ce commun vers les pins `6`, `3` et `1` selon la rotation.

Séquence mesurée dans un sens :

```text
2 + 6
  ↓
2 + 3
  ↓
2 + 1
```

Dans le sens inverse, la séquence est parcourue à l'envers :

```text
2 + 1
  ↓
2 + 3
  ↓
2 + 6
```

Cela montre que la direction de rotation peut être déterminée à partir de l'ordre des états. Ce n'est donc pas nécessaire de convertir la molette en une simple information analogique/résistive.

Statut : **MEASURED / USER CONFIRMED — 2026-09-04**.

## Carte fonctionnelle actuelle

```text
pin 1 ─┬─ volume + avec pin 4
       └─ état molette avec commun pin 2

pin 2 ─┬─ commun molette
       └─ bouton inférieur avec pin 4

pin 3 ─┬─ source − avec pin 5
       └─ état molette avec commun pin 2

pin 4 ─┬─ volume − avec pin 6
       ├─ volume + avec pin 1
       └─ bouton inférieur avec pin 2

pin 5 ─┬─ source − avec pin 3
       └─ source + avec pin 6

pin 6 ─┬─ volume − avec pin 4
       ├─ source + avec pin 5
       └─ état molette avec commun pin 2
```

## Architecture d'origine Espace IV

La documentation / les schémas de câblage Espace IV indiquent que la commande au volant (`repère 325`) ne semble pas être décodée directement par l'autoradio.

Les **6 conducteurs** de la commande arrivent au **tableau de bord / afficheur déporté**, notamment sur le connecteur gris 30 voies. L'électronique du tableau de bord interprète les états de la commande puis transmet l'ordre sous forme numérique vers le système audio.

Architecture de travail :

```text
commande au volant 7701049643
6 fils / contacts secs
        │
        ▼
tableau de bord / afficheur
        │
        │ décodage de la matrice
        ▼
liaison numérique multimédia
(CAN ou I²C selon génération / autoradio)
        │
        ▼
autoradio / système CNC
```

Pour l'Espace IV équipé du CNC, cette observation est cohérente avec le réseau multimédia CAN déjà identifié sur le projet.

Sources externes consultées : schémas Espace IV et retours techniques Planète Renault / GPS-Carminat. Ce chemin doit être considéré comme **DOCUMENTED / EXTERNAL CORROBORATION**, distinct des mesures directes ci-dessus.

## Architecture projet : RP2040 comme proxy de commande

La commande physique au volant sera lue directement par le RP2040. Le RP2040 devient donc l'unique arbitre de l'action utilisateur.

Pour certaines fonctions qui doivent continuer à piloter le système OEM — en particulier le volume — le RP2040 peut **reproduire électriquement la même fermeture de contacts du côté du décodeur Renault**.

Principe :

```text
commande physique
      │
      ▼
    RP2040
      ├──► événement Raspberry / LIVI si souhaité
      │
      └──► contact électronique vers faisceau OEM
```

Pour le volume :

```text
VOL+ : fermer côté OEM les lignes 4 ↔ 1
VOL- : fermer côté OEM les lignes 4 ↔ 6
```

Le tableau de bord / afficheur voit alors la même fermeture que si la commande d'origine était encore branchée directement.

Cette approche permet de garder le bouton **physiquement exclusif au RP2040** tout en laissant le RP2040 décider de reproduire ou non certaines commandes OEM.

## Technologie d'émulation de contact

### Prototype

Deux petits relais `SPST-NO` peuvent être utilisés pour `VOL+` et `VOL-`.

Avantages :

- contact réellement flottant ;
- aucune hypothèse de polarité ;
- très proche électriquement du bouton mécanique d'origine.

Inconvénients : encombrement, bruit, usure, vitesse limitée.

### PCB final

Préférence : relais statiques à sortie MOSFET bidirectionnelle de type **PhotoMOS / OptoMOS** ou interrupteurs analogiques bilatéraux compatibles avec la tension réellement mesurée sur le faisceau OEM.

Un simple NPN/NMOS raccordé à la masse n'est **pas** retenu avant mesure du balayage, car il suppose une polarité et une référence communes qui ne sont pas encore établies.

Nombre de contacts électroniques nécessaires :

```text
2 canaux : VOL+ / VOL-
5 canaux : tous les boutons
8 canaux : tous les boutons + les 3 états de molette
```

## Mesures requises avant choix du composant final

Sur le faisceau côté décodeur Renault, commande au volant débranchée :

1. relever les tensions entre les six lignes et la masse ;
2. relever les tensions entre paires pertinentes ;
3. mesurer le courant lors d'une fermeture `4↔1` puis `4↔6` ;
4. observer si les lignes sont balayées périodiquement ;
5. mesurer la durée minimale d'appui reconnue ;
6. relever le comportement d'un appui long.

Ces mesures détermineront la référence exacte du PhotoMOS/interrupteur analogique.

## À confirmer ultérieurement

- fonction OEM exacte du **bouton inférieur** ;
- sens fonctionnel à associer à la séquence `6 → 3 → 1` ;
- comportement aux transitions rapides pour dimensionner le debounce ;
- trame multimédia correspondante émise par le tableau de bord pour chaque action, si l'on décide de documenter aussi le chemin OEM complet ;
- tension/courant de balayage côté décodeur OEM pour figer le composant d'émulation.
