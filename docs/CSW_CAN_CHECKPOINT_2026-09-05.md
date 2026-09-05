# CSW-2000R — checkpoint CAN dynamique 2026-09-05

## Montage de banc

Commande centrale : Renault `8200326970` / Xanavi `CSW-2000R`.

Alimentation / réveil déjà validés :

```text
CN1-7 = + batterie protégé, banc à 12,5 V
CN1-1 = wake 34HU
CN1-8 = GND
PCA82C250 VCC = 5,0 V
```

Analyseur logique USB 8 voies / 24 MHz sous PulseView :

```text
D0 = PCA82C250 pin 1 = TXD
D5 = PCA82C250 pin 4 = RXD
sample rate = 8 MHz
CAN decoder RX = D5
nominal bitrate = 500000 bit/s
```

Les fils flottants sur des voies non raccordées ont montré de la diaphonie / captation parasite. Les captures de voies non physiquement raccordées ne doivent donc pas être interprétées.

## Terminaison de banc

Le CSW seul mesure environ :

```text
R(CANH, CANL) = 37 kΩ
```

Il n'a donc pas de terminaison locale de 120 Ω.

Essai avec deux résistances de `220 Ω` en parallèle entre CANH et CANL :

```text
220 Ω // 220 Ω = 110 Ω
```

Cette valeur est utilisée comme terminaison de banc approchée de 120 Ω.

## Résultat décisif

Sans terminaison adaptée, TXD et RXD présentaient des formes très différentes et PulseView produisait des décodages incohérents.

Avec `110 Ω` entre CANH et CANL :

- TXD et RXD suivent désormais la même trame ;
- PulseView reconnaît des trames CAN classiques cohérentes ;
- le décodeur affiche un champ `ID`, huit blocs `DB` (8 octets de données), un `CRC-15` et la fin de trame ;
- le réglage `500000 bit/s` donne un décodage cohérent ;
- le débit CAN de travail du CSW est donc **500 kbit/s, confirmé sur banc par décodage cohérent**.

Statut : **MEASURED / USER CONFIRMED — 2026-09-05**.

Les trames se répètent sur le banc. En l'absence d'un second nœud CAN actif capable d'acquitter, une répétition liée à l'absence d'ACK est plausible ; cela reste à confirmer avec une interface CAN active.

## Première trame décodée

Première lecture utilisateur obtenue sur une pression de commande :

```text
Identifier = 0x681
Payload    = F0 0A 0A 01 FF FF FF FF
DLC        = 8 (cohérent avec les 8 octets visibles)
```

Un octet/valeur `CA` a aussi été vu à proximité dans PulseView, mais il n'est **pas encore classé comme donnée utile** : avec `DLC = 8`, les huit octets de payload sont ceux listés ci-dessus. `CA` peut appartenir à un autre champ (CRC/annotation/élément voisin) et doit être identifié avant d'être retenu.

Statut : **MEASURED / USER REPORTED — fonction de la commande capturée à préciser**.

## Prochaine étape

Pour chaque commande du CSW :

1. capturer une pression / rotation isolée ;
2. relever l'identifiant CAN exact ;
3. relever les 8 octets de données ;
4. comparer repos / appui / maintien / relâchement ;
5. construire une table fonction → ID + payload ;
6. exporter les captures / annotations PulseView pour éviter les relevés manuels ;
7. ajouter ensuite un second nœud CAN actif pour fournir l'ACK et valider le comportement nominal.

Ne pas retenir les anciens décodages incohérents (`DLC 15`, CRC-21, octets > 8) : ils provenaient d'une capture sans terminaison correcte et/ou d'entrées flottantes.
