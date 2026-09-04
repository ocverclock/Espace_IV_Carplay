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

Conséquence importante : dans notre projet, deux stratégies sont possibles :

1. lire directement les 6 fils avec le RP2040, ce qui est simple et totalement maîtrisé ;
2. écouter ultérieurement le réseau multimédia pour identifier les trames générées par le tableau de bord lorsqu'on utilise la commande au volant.

La stratégie 1 reste la plus simple pour l'intégration CarPlay.

## Conséquence pour l'interface RP2040

La commande peut être lue comme un ensemble de **contacts secs** :

- balayage numérique de matrice pour les cinq boutons ;
- suivi d'état de la molette sur `2↔6`, `2↔3`, `2↔1` ;
- détection du sens par ordre des transitions ;
- debounce logiciel nécessaire.

Aucune conversion analogique n'est requise à ce stade.

## À confirmer ultérieurement

- fonction OEM exacte du **bouton inférieur** ;
- sens fonctionnel à associer à la séquence `6 → 3 → 1` (molette haut/bas ou précédent/suivant selon l'usage Renault) ;
- comportement aux transitions rapides pour dimensionner le debounce ;
- trame multimédia correspondante émise par le tableau de bord pour chaque action, si l'on décide de documenter aussi le chemin OEM complet.
