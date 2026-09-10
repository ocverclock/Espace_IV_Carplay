# Reverse Camera

## Exigence

La caméra doit fonctionner même si l’iPhone n’est pas connecté.

## Fonctionnement cible

```text
reverse ON
  → affichage caméra
  → latence cible < 1 s

reverse OFF
  → retour à l’écran précédent
```

## Technologies à comparer

### CVBS
+ très répandu
+ peu coûteux
- qualité limitée
- dépend du convertisseur USB

### AHD
+ meilleure image
- compatibilité capture Linux à vérifier précisément

### USB UVC
+ intégration Linux simple si vraie UVC
- longueur USB et démarrage de caméra à surveiller

## Capture vidéo

LIVI v8.3.0 ouvre la caméra comme une entrée vidéo système. La voie la plus simple à valider sur Raspberry Pi est donc un périphérique de capture USB reconnu par Linux comme caméra vidéo (idéalement UVC / `/dev/video*`).

Piste CVBS économique à tester : caméra RCA composite → grabber AV USB. Le produit AliExpress `1005010487260807` est décrit comme un grabber RCA AV → USB/USB-C ; il doit encore être validé sur Raspberry Pi (`lsusb`, `/dev/video*`, `v4l2-ctl`) avant d'être retenu. Son intitulé ne prouve pas la compatibilité AHD.

## Signal de marche arrière

### Voie prioritaire : information CAN

Chercher d'abord passivement si l'état marche arrière est déjà présent sur un des réseaux CAN accessibles de l'Espace IV.

Objectif :

```text
CAN véhicule
  → can0/can1
  → décodage état reverse
  → LIVI reverse ON/OFF
```

Avantages :

- aucun repiquage du +12 V du feu de recul si l'information est disponible ;
- intégration plus propre au faisceau OEM ;
- possibilité de réutiliser ultérieurement d'autres états véhicule.

Statut : **À IDENTIFIER / NON ENCORE MESURÉ**. Ne pas supposer que l'information est disponible sur le segment CAN choisi avant capture réelle.

Méthode : écoute passive d'abord, comparer des captures `candump` marche arrière désengagée / engagée et identifier l'ID/bit qui change de façon reproductible.

### Fallback : signal électrique feu de recul

Si l'état reverse n'est pas disponible ou suffisamment fiable sur le CAN retenu, utiliser le +12 V de commande du feu de recul comme signal de secours.

Ne jamais envoyer ce 12 V directement dans un GPIO.

Utiliser :
- optocoupleur, ou
- entrée automobile protégée / diviseur correctement dimensionné.

## Mesures obligatoires

- temps caméra ON → première image ;
- temps reverse ON → affichage ;
- image nuit ;
- perte de signal ;
- comportement après suspend/reboot ;
- délai CAN reverse → écran caméra si la voie CAN est retenue ;
- vérification qu'aucune perte de trames ne laisse la caméra dans un mauvais état.
