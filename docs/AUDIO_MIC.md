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

Les commandes utilisateur ne doivent plus piloter l'ancien autoradio :

- `CSW-2000R` isolé sur un CAN privé vers le Raspberry Pi ;
- commande au volant lue directement par RP2040 ;
- aucun pont transparent vers le CAN OEM.

Piste de fonctionnement audio à tester :

```text
autoradio laissé sur AUX
niveau/gain OEM réglé à une valeur fixe sûre
volume utilisateur géré côté Raspberry Pi / DAC
```

Cette stratégie permet de conserver uniquement la partie audio/amplification de l'autoradio sans lui laisser le contrôle des boutons.

## A vérifier sur véhicule

- entrée AUX réellement accessible ;
- l'autoradio reste-t-il sur AUX après redémarrage ?
- a-t-il besoin du réseau OEM pour sélectionner AUX ou rester réveillé ?
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
