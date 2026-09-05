# Audio & Microphone

## Objectif

Conserver l’amplification et les haut-parleurs Renault autant que possible, tout en faisant du Raspberry Pi la source audio et le contrôleur utilisateur principal.

## Piste principale

```text
Pi / LIVI
  ↓
DAC / sortie ligne
  ↓
AUX Renault
  ↓
autoradio / étage ampli Renault
  ↓
haut-parleurs d'origine
```

## Principe de commande

Les commandes utilisateur ne doivent plus piloter directement l'ancien autoradio :

- `CSW-2000R` isolé sur un CAN privé vers le Raspberry Pi ;
- commande au volant lue directement par RP2040 ;
- aucun pont transparent vers le CAN OEM.

## Volume — point d'architecture à valider

Le point critique est la façon dont sera réglé le niveau sonore final. À ce stade, le projet ne sait pas encore si le volume de l'autoradio/amplificateur Renault peut être commandé indépendamment, ni si son réglage est mémorisé de manière exploitable.

Trois stratégies sont retenues pour les essais, par ordre de simplicité :

### A — Gain OEM fixe + volume côté Raspberry/DAC

```text
VOL+ / VOL-
    ↓
Raspberry / RP2040
    ↓
volume numérique ou matériel du DAC
    ↓
AUX Renault
    ↓
autoradio/ampli réglé une fois à un niveau fixe
```

Avantage : aucune commande utilisateur n'est envoyée à l'ancien autoradio. C'est la solution préférée si l'autoradio peut rester sur AUX avec un niveau fixe et sûr.

À valider : mémorisation du niveau après redémarrage, plage dynamique, bruit de fond, volume maximal, comportement en appel/Siri et sécurité au boot.

### B — Raspberry maître, commande sélective du volume OEM

Si le gain fixe n'est pas satisfaisant mais que les messages de volume OEM sont identifiés, le Raspberry reste le seul destinataire des commandes physiques puis décide lui-même d'envoyer uniquement une commande de volume à l'autoradio/amplificateur via une passerelle CAN filtrée.

```text
commande physique
      ↓
 Raspberry
      ├──► LIVI / UI
      └──► CAN OEM : uniquement VOL+/VOL-/MUTE si nécessaire
```

Ce n'est pas un pont transparent : l'ancien système ne voit jamais directement les commandes Renault. Il reçoit seulement les messages explicitement générés par notre contrôleur.

### C — Contrôle analogique externe avant l'AUX

Si ni A ni B ne sont satisfaisants, ajouter un véritable étage de volume stéréo commandé entre le DAC et l'entrée AUX : contrôleur de volume audio / PGA / atténuateur analogique dédié. Éviter un simple potentiomètre numérique générique non prévu pour l'audio.

```text
DAC → volume audio commandé → AUX Renault → ampli
```

Cette solution donne un volume totalement indépendant du protocole OEM mais ajoute du matériel audio.

### Option de dernier recours — attaque directe de l'étage ampli

Bypasser le préampli / contrôle de volume de l'autoradio et injecter directement dans son étage de puissance n'est pas retenu tant que l'architecture interne n'a pas été reverse-engineerée. Risque de niveau, offset, mute/standby et pops.

## A vérifier sur véhicule

- entrée AUX réellement accessible ;
- l'autoradio reste-t-il sur AUX après redémarrage ?
- mémorise-t-il son volume ?
- a-t-il besoin du réseau OEM pour sélectionner AUX ou rester réveillé ?
- le réglage de volume OEM est-il piloté par CAN et identifiable ?
- niveau ligne ;
- masse ;
- boucle de masse ;
- gain fixe optimal ;
- volume de démarrage sûr ;
- mute téléphone ;
- comportement CarPlay ;
- pops au boot/shutdown ;
- besoin éventuel d'une isolation audio 1:1.

Si l'autoradio exige certains messages réseau pour fonctionner, utiliser au besoin une passerelle CAN à liste blanche limitée aux messages de service strictement nécessaires. Ne jamais relayer les commandes utilisateur par défaut.

## Micro

Priorité :
1. micro Renault d’origine ;
2. micro automobile externe discret.

Mesurer la nature du micro d’origine avant connexion à une carte son.
