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
DLC        = 8
```

La valeur `CA` vue à proximité dans PulseView n'est pas un neuvième octet de payload : avec `DLC = 8`, les huit octets utiles sont ceux ci-dessus.

### Vérification directe du fichier PulseView

Capture fournie :

```text
csw_bouton_test_01.sr
```

Le fichier sigrok contient :

```text
sample rate = 8 MHz
5 000 000 échantillons
D0 = TXD
D5 = RXD
```

Analyse directe des échantillons :

- D0 et D5 contiennent le même trafic, avec un décalage d'environ 2 échantillons seulement entre TXD et RXD ;
- la première activité commence vers `417,59 ms` dans cette capture ;
- `184` occurrences valides de la même trame `0x681` ont été retrouvées ;
- payload identique sur ces occurrences : `F0 0A 0A 01 FF FF FF FF` ;
- la séquence d'émissions observée dure environ `50,15 ms` dans ce fichier ;
- aucune autre valeur de payload n'a été trouvée dans cette capture.

Cette analyse confirme indépendamment le décodage PulseView de la première trame.

Statut : **CAPTURE FILE ANALYZED — 2026-09-05**.

## Trame sans action utilisateur

Observation utilisateur supplémentaire : **le CSW émet également au moins une trame sans aucun appui**.

Conséquence importante : `0x681` et/ou un payload particulier ne doivent pas être assimilés directement à une commande avant comparaison avec l'état de repos. Il faut désormais distinguer :

```text
repos / heartbeat
appui
maintien
relâchement
rotation / déplacement
```

Le fichier `csw_bouton_test_01.sr` ne permet pas encore d'identifier la trame de repos annoncée : dans ce fichier, l'activité détectée est concentrée dans une fenêtre d'environ 50 ms. Une capture dédiée sans aucune action est nécessaire pour établir le baseline.

Statut : **USER OBSERVED — baseline exact à capturer**.

## Méthode de reverse engineering à partir de maintenant

Pour chaque commande du CSW :

1. faire d'abord une capture `idle` sans toucher au boîtier ;
2. sauvegarder la capture complète en `.sr` ;
3. faire ensuite une capture avec une seule commande isolée ;
4. comparer les IDs, payloads et périodicités ;
5. relever séparément appui, maintien et relâchement si le comportement change ;
6. construire une table fonction → ID + payload + transition ;
7. ajouter ensuite un second nœud CAN actif pour fournir l'ACK et valider le comportement nominal.

Ne pas retenir les anciens décodages incohérents (`DLC 15`, CRC-21, octets > 8) : ils provenaient d'une capture sans terminaison correcte et/ou d'entrées flottantes.
