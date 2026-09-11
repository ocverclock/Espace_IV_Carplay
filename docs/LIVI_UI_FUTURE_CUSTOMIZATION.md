# LIVI — personnalisations UI futures

Note de travail à conserver pour les prochaines évolutions de l'interface.

## Compteurs / jauges Dash1 et Dash2

Les compteurs natifs LIVI sont déjà très paramétrables dans le code (`GaugeArc`, constantes des dashboards) :

- rayon ;
- ouverture de l'arc ;
- nombre de graduations ;
- nombre de graduations majeures ;
- largeur / longueur des traits ;
- taille des chiffres ;
- bras haut / bas ;
- intensité de la traînée ;
- couleurs ;
- zone rouge ;
- ombre / glow ;
- animation de l'aiguille.

## Direction retenue

Le checkpoint UI/télémétrie validé reste conservé tel quel. Les évolutions ci-dessous sont une couche supplémentaire et doivent être validées séparément sur le Raspberry avant d'être intégrées au checkpoint principal.

L'objectif est de donner progressivement à Dash1 / Dash2 la même identité visuelle que Dash4 : police Inter, palette Espace IV, contraste automobile, hiérarchie visuelle cohérente.

Séparer autant que possible les réglages visuels du code compilé :

```text
~/.config/LIVI/custom/espace-ui.css
```

pour le style de base, et :

```text
~/.config/LIVI/custom/espace-theme.css
```

pour les surcharges de thème actives.

## Changement de thème à chaud

Un patch expérimental est maintenant préparé :

```text
patches/livi/v8.3.0-espace-theme-hot-reload.patch
```

Principe :

1. LIVI charge `espace-ui.css` puis, s'il existe, `espace-theme.css` ;
2. le renderer principal relit le CSS externe toutes les secondes ;
3. il compare le contenu avec la version déjà appliquée ;
4. si le fichier a changé, le bloc `<style id="espace-custom-ui">` est remplacé immédiatement ;
5. aucun redémarrage de LIVI n'est requis pour un changement purement CSS.

Le polling à une seconde est volontairement simple et robuste. Les fichiers CSS sont très petits et le coût est négligeable à l'échelle du Raspberry Pi 4. Une surveillance native du fichier pourra être envisagée plus tard si nécessaire.

### Thèmes de départ

Trois presets de travail sont conservés dans le dépôt :

```text
config/livi/themes/espace-oem.css
config/livi/themes/espace-sport.css
config/livi/themes/espace-retro.css
```

Ils ne constituent pas encore trois designs définitifs des compteurs. Ils servent à valider le mécanisme de changement de palette / contraste / typographie à chaud sur l'écran d'attente et Dash4.

Une fois copiés dans :

```text
~/.config/LIVI/custom/themes/
```

le script :

```text
scripts/livi/theme-switch.sh
```

permettra notamment :

```bash
theme-switch.sh list
theme-switch.sh espace-oem
theme-switch.sh espace-sport
theme-switch.sh espace-retro
theme-switch.sh off
```

Le script remplace atomiquement `espace-theme.css`. Le renderer doit alors appliquer le nouveau thème en environ une seconde sans redémarrage.

## Géométrie des compteurs — étape suivante

Les paramètres structurels de `GaugeArc` ne sont pas encore externalisés. Ils restent aujourd'hui compilés dans LIVI.

Une future étape pourra introduire par exemple :

```text
~/.config/LIVI/custom/espace-gauges.json
```

pour les paramètres comme :

- rayon ;
- ouverture ;
- nombre de graduations ;
- graduations majeures ;
- épaisseurs ;
- traînée ;
- redline ;
- paramètres d'animation.

Ce fichier devra lui aussi pouvoir être rechargé à chaud afin de comparer plusieurs géométries directement sur l'écran 7 pouces sans recompilation complète à chaque essai.

## Règle

La lisibilité automobile prime sur l'esthétique. Toute évolution des jauges doit être testée sur l'écran 7 pouces réel avant validation définitive.

Le patch de hot reload est pour l'instant **préparé mais non validé sur le Raspberry**. Ne pas remplacer le checkpoint stable tant que `git apply --check`, `typecheck`, `build:app`, le build ARM64 et le test visuel réel n'ont pas été réalisés.
