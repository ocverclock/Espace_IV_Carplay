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

Ne rien modifier pour le moment : le checkpoint actuel est validé.

Lors d'une future phase UI, donner à Dash1 / Dash2 la même identité visuelle que Dash4 : police Inter, palette Espace IV, contraste automobile, hiérarchie visuelle cohérente.

Séparer autant que possible les réglages visuels du code compilé :

```text
~/.config/LIVI/custom/espace-ui.css
```

pour les couleurs, polices, opacités, tailles et espacements ; et envisager un fichier dédié de géométrie, par exemple :

```text
~/.config/LIVI/custom/espace-gauges.json
```

pour les paramètres comme rayon, ouverture, nombre de graduations, épaisseurs, traînée ou géométrie générale.

Objectif : après une adaptation initiale de LIVI, pouvoir retoucher l'esthétique des compteurs sans recompilation complète à chaque essai.

## Règle

La lisibilité automobile prime sur l'esthétique. Toute évolution des jauges doit être testée sur l'écran 7 pouces réel avant validation définitive.
