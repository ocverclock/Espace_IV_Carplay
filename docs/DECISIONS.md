# Decision Log

## D001 — Raspberry Pi 4
**Décision :** utiliser le Pi 4 déjà disponible.  
**Raison :** coût nul, performances suffisantes, bon support vidéo.

## D002 — iPhone / Apple CarPlay
**Décision :** CarPlay est la cible principale.  
**Raison :** téléphone utilisé au quotidien ; Roole Map disponible via CarPlay.

## D003 — LIVI plutôt qu’OpenAuto
**Décision :** privilégier LIVI pour le prototype CarPlay.  
**Raison :** besoin Apple CarPlay, pas Android Auto.

## D004 — MFi direct
**Décision :** viser une puce MFi sur le PCB plutôt qu’un boîtier Carlinkit final.  
**Raison :** coût, compacité, contrôle de l’architecture.  
**Condition :** prototype à valider avant intégration finale.

## D005 — Écran 7" non tactile
**Décision :** écran non tactile.  
**Raison :** écran éloigné ; commandes physiques préférables.

## D006 — Nouvelle façade 3D
**Décision :** ne pas conserver la fenêtre 130×70 mm comme limite.  
**Raison :** impression 3D d’un nouveau cache permettant d’exploiter le 7" complet.

## D007 — Réutiliser les commandes Renault
**Décision :** reverse engineering des commandes plutôt que remplacement esthétique.  
**Raison :** résultat OEM+ et ergonomie existante.

## D008 — Molette au volant obligatoire
**Décision :** ne pas reproduire les adaptations qui abandonnent la molette.  
**Raison :** RP2040 permet un décodage de séquence plus complet.

## D009 — ELS27 reportée
**Décision :** ne pas dépenser ~150 € maintenant.  
**Raison :** CAN/télémétrie n’est pas la première valeur d’usage.

## D010 — Double CAN sur PCB
**Décision :** prévoir deux canaux matériels sur le PCB final.  
**Raison :** coût marginal faible et flexibilité de reverse engineering.

## D011 — Caméra de recul
**Décision :** fait partie du cahier des charges de base.  
**Raison :** forte valeur d’usage ; doit fonctionner sans téléphone.
