# LIVI — personnalisations UI futures

Note de travail à conserver pour les prochaines évolutions de l'interface.

## Compteurs / jauges Dash1 et Dash2

Les compteurs natifs LIVI (`GaugeArc`) sont très paramétrables : rayon, ouverture, nombre de graduations, largeur / longueur des traits, taille des chiffres, bras haut / bas, traînée, couleurs, zone rouge, ombre et aiguille.

## Direction retenue

Le checkpoint UI/télémétrie validé reste conservé. Les évolutions ci-dessous sont une couche supplémentaire et doivent être validées séparément sur le Raspberry avant d'être intégrées au checkpoint principal.

L'objectif est que l'esthétique puisse être modifiée sans rebuild après une adaptation initiale de LIVI.

### Fichiers runtime

```text
~/.config/LIVI/custom/espace-ui.css
~/.config/LIVI/custom/espace-theme.css
~/.config/LIVI/custom/espace-gauges.json
```

- `espace-ui.css` : style de base commun ;
- `espace-theme.css` : palette / couleurs du thème actif ;
- `espace-gauges.json` : géométrie et famille visuelle des compteurs et de l'aiguille.

## Hot reload CSS

Patch :

```text
patches/livi/v8.3.0-espace-theme-hot-reload.patch
```

Le renderer principal relit le CSS externe toutes les secondes et remplace le bloc de style uniquement si le contenu a changé.

**État : validé sur Raspberry Pi le 2026-09-11.** Le changement OEM / Sport / Retro a été observé à chaud sur Dash4, sans redémarrage de LIVI.

## Objectif compteur : vrais styles, pas seulement des dimensions

Le besoin retenu est de pouvoir obtenir des compteurs réellement différents sans rebuild, pas seulement changer le rayon ou l'épaisseur des traits.

Le moteur de jauges devra donc supporter plusieurs familles de rendu sélectionnables depuis `espace-gauges.json`, par exemple :

- `runner` : pointeur court qui se déplace sur l'arc, proche du rendu LIVI actuel ;
- `needle` : vraie aiguille radiale partant d'un pivot central ;
- `sport` : aiguille radiale fine, graduations denses, accent rouge et traînée plus marquée ;
- `classic` : aiguille plus large, moyeu central visible, graduations plus épaisses et moins nombreuses ;
- `minimal` : arc épuré, très peu de graduations, priorité au grand affichage numérique.

Le JSON devra pouvoir sélectionner et paramétrer au minimum :

```text
renderer / pointerMode
tickShape
showRing / ringWidth
showHub / hubRadius
radius / gapDeg
armTicks / ticks / majorCount
trailMax
tickW / tickH / majorH / labelSize
pointerW / pointerH / pointerRadius
pointerTail / pointerInset
minorOpacity / majorOpacity
```

Les couleurs restent pilotées par CSS afin de pouvoir changer séparément palette et géométrie.

Cela doit permettre de passer, par exemple, d'un compteur OEM minimal à un compteur Sport à aiguille rouge puis à un compteur classique/retro avec moyeu central, sans reconstruire LIVI.

## Hot reload des compteurs

Patch de travail actuel :

```text
patches/livi/v8.3.0-espace-gauges-hot-reload.patch
```

Ce patch met déjà en place la lecture à chaud de :

```text
~/.config/LIVI/custom/espace-gauges.json
```

et externalise une première série de paramètres géométriques. **Cependant, dans son état actuel il reste basé sur un seul renderer `GaugeArc` et n'est pas encore suffisant pour l'objectif final de styles réellement différents. Il ne doit pas être considéré comme validé ni définitif.**

Il doit être étendu avant validation Raspberry pour introduire les familles de rendu ci-dessus et notamment une vraie aiguille radiale / un moyeu central en plus du pointeur d'arc actuel.

Les couleurs des compteurs passent par les variables CSS :

```text
--gauge-scale
--gauge-major
--gauge-pointer
--gauge-redline
--gauge-shadow
```

Les valeurs mécaniques de la voiture, comme la redline RPM réelle, restent dans la logique véhicule : un thème ne doit pas modifier une information de sécurité ou de fonctionnement.

## Presets

Chaque thème peut avoir une paire CSS + JSON :

```text
config/livi/themes/espace-oem.css
config/livi/themes/espace-oem.json
config/livi/themes/espace-sport.css
config/livi/themes/espace-sport.json
config/livi/themes/espace-retro.css
config/livi/themes/espace-retro.json
```

Le script :

```text
scripts/livi/theme-switch.sh
```

copie les deux fichiers runtime de manière atomique lorsqu'un preset possède un JSON :

```bash
theme-switch.sh espace-oem
theme-switch.sh espace-sport
theme-switch.sh espace-retro
theme-switch.sh off
```

Après validation du moteur de styles compteurs, un changement de thème devra modifier palette + famille de rendu + forme des graduations + aiguille + géométrie en environ une seconde, sans redémarrage de LIVI.

## Règle

La lisibilité automobile prime sur l'esthétique. Toute évolution des jauges doit être testée sur l'écran 7 pouces réel avant validation définitive.

**État du moteur compteurs : en cours de préparation, pas encore validé sur le Raspberry.** Ne pas intégrer le patch compteur actuel au checkpoint stable avant extension aux vrais styles, puis `git apply --check`, `typecheck`, `build:app`, build ARM64 et test visuel réel.
