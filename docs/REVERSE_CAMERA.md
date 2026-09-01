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

## Mesures obligatoires

- temps caméra ON → première image ;
- temps reverse ON → affichage ;
- image nuit ;
- perte de signal ;
- comportement après suspend/reboot.

## Signal de marche arrière

Ne jamais envoyer le 12 V du feu de recul directement dans un GPIO.

Utiliser :
- optocoupleur, ou
- diviseur/protection automobile correctement dimensionné.
