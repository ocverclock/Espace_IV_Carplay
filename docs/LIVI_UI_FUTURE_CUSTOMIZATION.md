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
- `espace-gauges.json` : géométrie des compteurs et de l'aiguille.

## Hot reload CSS

Patch :

```text
patches/livi/v8.3.0-espace-theme-hot-reload.patch
```

Le renderer principal relit le CSS externe toutes les secondes et remplace le bloc de style uniquement si le contenu a changé.

**État : validé sur Raspberry Pi le 2026-09-11.** Le changement OEM / Sport / Retro a été observé à chaud sur Dash4, sans redémarrage de LIVI.

## Hot reload des compteurs

Patch préparé :

```text
patches/livi/v8.3.0-espace-gauges-hot-reload.patch
```

Ce patch ajoute la lecture à chaud de :

```text
~/.config/LIVI/custom/espace-gauges.json
```

Dash1 / Dash2 / Dash3 doivent relire ce fichier environ une fois par seconde.

Paramètres externalisés :

```json
{
  "radius": 110,
  "gapDeg": 180,
  "armTicks": 3,
  "ticks": 41,
  "majorCount": 6,
  "trailMax": 0.5,
  "tickW": 3,
  "tickH": 14,
  "majorH": 26,
  "labelSize": 15,
  "pointerW": 4,
  "pointerH": 38,
  "pointerRadius": 1.5
}
```

Les couleurs des compteurs passent par les variables CSS :

```text
--gauge-scale
--gauge-major
--gauge-pointer
--gauge-redline
--gauge-shadow
```

Ainsi la couleur peut changer instantanément par CSS, tandis que la géométrie est rechargée depuis le JSON sans rebuild.

Les valeurs mécaniques de la voiture, comme la redline RPM réelle, restent dans la logique véhicule : un thème ne doit pas modifier une information de sécurité ou de fonctionnement.

## Presets

Chaque thème peut maintenant avoir une paire CSS + JSON :

```text
config/livi/themes/espace-oem.css
config/livi/themes/espace-oem.json
config/livi/themes/espace-sport.css
config/livi/themes/espace-sport.json
config/livi/themes/espace-retro.css
config/livi/themes/espace-retro.json
```

Le thème Sport utilise volontairement le rouge comme accent : aiguille et redline rouges, textes principaux blancs, graduations gris clair.

Le script :

```text
scripts/livi/theme-switch.sh
```

copie maintenant les deux fichiers runtime de manière atomique lorsqu'un preset possède un JSON :

```bash
theme-switch.sh espace-oem
theme-switch.sh espace-sport
theme-switch.sh espace-retro
theme-switch.sh off
```

Après validation du patch compteurs, un changement de thème doit donc modifier palette + forme des compteurs + aiguille en environ une seconde, sans redémarrage de LIVI.

## Règle

La lisibilité automobile prime sur l'esthétique. Toute évolution des jauges doit être testée sur l'écran 7 pouces réel avant validation définitive.

**État du patch compteurs : préparé mais pas encore validé sur le Raspberry.** Il faut faire `git apply --check`, `typecheck`, `build:app`, build ARM64 et test visuel réel avant intégration au checkpoint stable.
