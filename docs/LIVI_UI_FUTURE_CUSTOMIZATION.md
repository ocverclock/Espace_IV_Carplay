# LIVI — personnalisation UI / compteurs à chaud

## Objectif

Après une dernière adaptation de LIVI, pouvoir modifier l'esthétique sans rebuild : palette, forme des compteurs, type de graduations, aiguille, anneau, moyeu central, tailles et position des readouts.

Le checkpoint UI/télémétrie validé reste conservé. Le moteur de compteurs ci-dessous est une couche supplémentaire et doit être validé séparément sur le Raspberry avant intégration au checkpoint principal.

## Fichiers runtime

```text
~/.config/LIVI/custom/espace-ui.css
~/.config/LIVI/custom/espace-theme.css
~/.config/LIVI/custom/espace-gauges.json
```

- `espace-ui.css` : style de base commun ;
- `espace-theme.css` : palette / couleurs / effets CSS du thème actif ;
- `espace-gauges.json` : renderer et géométrie des compteurs.

## Hot reload CSS

Patch :

```text
patches/livi/v8.3.0-espace-theme-hot-reload.patch
```

Le renderer principal relit le CSS externe toutes les secondes et remplace le style uniquement si son contenu a changé.

**État : validé sur Raspberry Pi le 2026-09-11.** Les changements OEM / Sport / Retro ont été observés à chaud sur Dash4 sans redémarrage de LIVI.

## Moteur de compteurs multi-style

Patch :

```text
patches/livi/v8.3.0-espace-gauges-hot-reload.patch
```

Le patch ne se contente plus de changer le rayon d'un même compteur. Il ajoute un renderer Espace dédié et configurable à chaud pour Dash1 / Dash2 / Dash3.

### Modes de pointeur

```text
arc      pointeur tangentiel sur l'arc, proche du LIVI actuel
needle   vraie aiguille radiale
none     pas d'aiguille, style digital/minimal
```

### Modes de graduations

```text
line     traits classiques
dot      points pour les graduations mineures, traits pour les majeures
```

### Paramètres externalisés

Le JSON peut piloter notamment :

```text
style
pointerMode
tickMode
radius
gapDeg
armTicks
ticks
majorCount
trailMax
tickW
tickH
majorH
labelSize
tickRadius
dotRadius
arcPointerW
arcPointerH
arcPointerRadius
needleInner
needleOuter
needleWidth
needleCap
hubRadius
hubOpacity
ringWidth
ringOpacity
readoutDx
readoutSize
```

Les paramètres sont bornés et sanitisés côté renderer afin qu'un JSON incorrect ne puisse pas produire des valeurs absurdes.

### Couleurs et effets CSS

Les couleurs passent par :

```text
--gauge-background
--gauge-scale
--gauge-major
--gauge-pointer
--gauge-redline
--gauge-shadow
--gauge-readout
--gauge-readout-label
```

Les éléments SVG ont aussi des classes dédiées :

```text
.espace-gauge-ring
.espace-gauge-tick
.espace-gauge-major
.espace-gauge-trail
.espace-gauge-pointer
.espace-gauge-pointer-arc
.espace-gauge-pointer-needle
.espace-gauge-hub
.espace-gauge-label
```

Cela permet de modifier glow, opacité, filtres ou typographie directement en CSS, sans rebuild.

## Presets

Chaque thème est une paire CSS + JSON :

```text
espace-oem
espace-sport
espace-retro
espace-minimal
```

### OEM

- pointeur sur l'arc ;
- graduations linéaires ;
- géométrie proche du compteur LIVI actuel ;
- palette blanche / grise.

### Sport

- vraie aiguille radiale rouge ;
- arc plus fermé ;
- 53 graduations ;
- grandes graduations plus longues ;
- anneau léger ;
- glow rouge sur l'aiguille ;
- readout plus compact.

### Retro

- aiguille radiale ambre ;
- moyeu central visible ;
- graduations mineures en points ;
- traits majeurs plus épais ;
- anneau plus présent ;
- arc plus ouvert ;
- readout plus petit et décalé.

### Minimal

- aucune aiguille ;
- peu de graduations en points ;
- grand readout numérique ;
- anneau très discret ;
- palette blanche / grise.

## Changement de thème

Le script :

```text
scripts/livi/theme-switch.sh
```

copie atomiquement le CSS et le JSON du preset. Il valide aussi le JSON avec `python3 -m json.tool` lorsqu'il est disponible.

Exemples :

```bash
theme-switch.sh espace-oem
theme-switch.sh espace-sport
theme-switch.sh espace-retro
theme-switch.sh espace-minimal
theme-switch.sh off
```

Après validation du patch compteurs, le changement doit modifier la palette **et la famille visuelle du compteur** en environ une seconde, sans redémarrage de LIVI.

## Limites volontaires

Les informations mécaniques réelles de la voiture restent dans la logique véhicule. Un thème ne modifie donc pas la redline RPM réelle, les échelles de vitesse ou les données télémétriques.

La lisibilité automobile prime sur l'esthétique et chaque preset devra être validé sur l'écran 7 pouces réel.

**État du moteur multi-style : préparé dans le dépôt mais pas encore validé sur le Raspberry.** Procédure : `git apply --check`, `typecheck`, `build:app`, build ARM64, puis test visuel réel OEM / Sport / Retro / Minimal.
