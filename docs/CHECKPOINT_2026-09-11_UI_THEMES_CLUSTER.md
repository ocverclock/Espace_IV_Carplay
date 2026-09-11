# Checkpoint — UI LIVI, thèmes à chaud et piste remplacement combiné — 2026-09-11

## Statut

Ce checkpoint complète sans remplacer le checkpoint validé du 2026-09-10 :

```text
docs/LIVI_UI_TELEMETRY_CHECKPOINT_2026-09-10.md
```

Les éléments ci-dessous sont séparés en **validé**, **préparé mais non validé**, et **à vérifier sur véhicule** afin d'éviter de mélanger faits et hypothèses.

## Validé sur Raspberry Pi

### Accès distant

- `labwc` démarre correctement ;
- `wayvnc` démarre correctement ;
- WayVNC écoute sur `0.0.0.0:5900` ;
- après reboot, le tunnel SSH doit être recréé côté PC pour Remmina si la connexion utilise `127.0.0.1:15900`.

Commande de tunnel utilisée :

```bash
ssh -N -L 15900:127.0.0.1:5900 pi@raspberry-carplay.local
```

### Télémétrie simulée

Le script :

```text
scripts/livi/telemetry-espace-demo.sh
```

se connecte bien à LIVI sur :

```text
http://127.0.0.1:4000
```

et pousse les données toutes les 500 ms. Les valeurs sont visibles dans les dashboards.

### Hot reload CSS

Le changement de thème CSS sans redémarrer LIVI est **validé** sur le Raspberry.

Thèmes testés :

```text
espace-oem
espace-sport
espace-retro
```

Commande :

```bash
~/.local/bin/theme-switch.sh <theme>
```

Le changement visuel apparaît en environ une seconde sur Dash4.

Le thème Sport a été ajusté afin que le rouge serve d'accent plutôt que de teinter toute l'interface.

### LIVI — témoins déjà pris en charge

Le code LIVI v8.3.0 prévoit déjà dans `TelemetryPayload` et dans le dashboard les états suivants :

```text
lights        -> feux de croisement / position
highBeam      -> pleins phares
hazards       -> warnings
turn          -> clignotant gauche / droite
parkingBrake  -> frein de parking
```

Le composant `TelltaleBar` affiche déjà :

- feux de croisement en vert ;
- pleins phares en bleu ;
- warnings en rouge ;
- frein de parking en rouge ;
- clignotants gauche et droite.

Ces capacités sont confirmées dans le code LIVI. Elles ne prouvent pas encore comment les mêmes informations arrivent au combiné Renault d'origine.

## Préparé mais pas encore validé sur Raspberry

### Nouveau moteur de styles de compteurs

Le patch :

```text
patches/livi/v8.3.0-espace-gauges-hot-reload.patch
```

est une évolution expérimentale destinée à permettre des styles réellement différents, pas seulement des couleurs ou un changement de rayon.

Objectif du moteur :

- pointeur d'arc ;
- vraie aiguille radiale ;
- mode sans aiguille ;
- graduations lignes ou points ;
- anneau ;
- moyeu central ;
- géométrie des arcs ;
- densité des graduations ;
- taille des chiffres ;
- dimensions et forme de l'aiguille ;
- traînée ;
- palette CSS séparée ;
- rechargement à chaud via `espace-gauges.json`.

Le patch n'est **pas encore appliqué ni validé** sur le Raspberry dans ce checkpoint. Il doit passer :

```text
git apply --check
pnpm run typecheck
pnpm run build:app
pnpm run build:linux:arm64
test visuel réel
```

avant d'être considéré comme stable.

La version stable d'origine reste conservée et ne doit pas être écrasée :

```text
/home/pi/LIVI/LIVI.AppImage
```

## Remplacement éventuel du combiné Renault Espace IV

### Ce qui est établi

LIVI sait recevoir et afficher une partie importante des informations nécessaires à un combiné moderne : vitesse, régime, rapport, marche arrière, clignotants, warnings, feux, pleins phares, frein de parking, carburant, autonomie, températures, etc.

### Ce qui n'est PAS encore établi

Il n'est **pas établi** à ce stade que les états `feux de croisement`, `pleins phares`, `clignotants`, `porte ouverte`, etc. arrivent au combiné Renault par CAN ou par liaison filaire directe.

Une affirmation précédente indiquant que certains de ces témoins étaient forcément filaires jusqu'au combiné est considérée comme non validée et ne doit plus être utilisée comme fait.

Il faut distinguer :

```text
commande au commodo / UCH
        !=
mode de transmission de l'état vers le combiné
```

Le fait qu'une commande soit filaire vers l'UCH ne prouve pas que l'information soit ensuite filaire jusqu'au tableau de bord.

### Décision actuelle

Ne pas supprimer ni condamner le combiné Renault d'origine tant que son connecteur et les signaux réellement utilisés ne sont pas cartographiés.

La piste de remplacement complet reste ouverte, car si la majorité des informations utiles sont disponibles sur CAN, l'intégration pourrait être nettement plus simple que prévu.

## Prochaine étape physique verrouillée

Avant toute conclusion sur le remplacement du combiné :

1. accéder à l'arrière du tableau de bord d'origine ;
2. photographier clairement tous les connecteurs ;
3. relever nombre de connecteurs, couleurs, détrompeurs, nombre de voies et références ;
4. relever toute numérotation visible des broches ;
5. croiser avec la documentation Renault ;
6. seulement ensuite mesurer / sniffer pour séparer alimentation, masse, CAN et éventuels signaux discrets.

Aucune modification du faisceau n'est nécessaire à cette étape.

## Règles de continuité du projet

- conserver le combiné Renault fonctionnel pendant le reverse engineering ;
- ne pas supposer qu'un témoin est CAN ou filaire sans preuve ;
- conserver le contrat télémétrie LIVI comme couche d'abstraction : le futur décodeur CAN doit produire les mêmes champs que le simulateur ;
- ne pas lancer le simulateur et un producteur CAN réel simultanément ;
- conserver l'AppImage stable intacte ;
- Dash5 reste reporté ;
- les sujets Wi-Fi restent reportés à la mise en service.
