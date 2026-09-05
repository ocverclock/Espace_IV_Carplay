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

Première lecture utilisateur obtenue sur une capture initialement faite pendant un essai de commande :

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

Cette analyse confirme indépendamment le décodage PulseView de la première trame, mais **ne prouve pas qu'elle correspond à un bouton**.

Statut : **CAPTURE FILE ANALYZED — 2026-09-05**.

## Baseline au repos confirmé

Capture dédiée sans aucune action utilisateur :

```text
csw_idle.sr
sample rate = 8 MHz
50 000 000 échantillons
 durée = 6,25 s
D0 = TXD
D5 = RXD
```

Décodage automatique de toute la capture :

```text
Identifier = 0x681
DLC        = 8
Payload    = F0 0A 0A 01 FF FF FF FF
```

Résultats :

- `2339` trames classiques valides ont été décodées ;
- **100 %** de ces trames ont le même ID et le même payload ;
- aucune autre trame / aucun autre payload n'a été trouvé dans cette capture idle ;
- l'émission se produit en salves d'environ `49 à 50 ms` ;
- `13` salves ont été observées sur `6,25 s` ;
- les débuts de salve sont espacés d'environ `500 ms` ;
- chaque salve contient typiquement environ `179` retransmissions de la même trame ;
- l'intervalle entre deux tentatives successives dans une salve est d'environ `274 µs`.

Conclusion :

```text
0x681  F0 0A 0A 01 FF FF FF FF
```

est **la baseline / trame périodique au repos sur notre banc**, et non une commande bouton identifiable à elle seule.

La répétition très dense pendant ~50 ms reste compatible avec le fait qu'aucun autre nœud CAN n'acquitte la trame sur le banc ; ce comportement devra être vérifié avec un second nœud actif.

Statut : **MEASURED FROM CAPTURE FILE — 2026-09-05**.

## Conséquence pour le reverse engineering des commandes

À partir de maintenant, une capture de commande doit être comparée explicitement à la baseline :

```text
IDLE = 0x681 / F0 0A 0A 01 FF FF FF FF
```

Une commande sera considérée identifiée uniquement si l'on observe au moins un des changements suivants :

- nouvel identifiant CAN ;
- modification d'un ou plusieurs octets du payload `0x681` ;
- apparition/disparition d'une trame ;
- modification reproductible de la périodicité ;
- séquence différente entre appui, maintien et relâchement.

Le fichier `csw_bouton_test_01.sr` étant identique à la baseline, il ne contient pas de commande distinguable dans la fenêtre capturée.

## Méthode de reverse engineering à partir de maintenant

Pour chaque commande du CSW :

1. garder le montage et le décodage actuels ;
2. faire une capture de plusieurs secondes ;
3. ne toucher à rien pendant environ 1 s ;
4. effectuer une seule commande et la maintenir environ 0,5 à 1 s ;
5. relâcher puis laisser encore environ 1 s ;
6. sauvegarder la capture complète en `.sr` avec un nom explicite ;
7. comparer automatiquement la capture à la baseline `idle` ;
8. construire ensuite une table fonction → ID + payload + transition ;
9. ajouter un second nœud CAN actif pour fournir l'ACK et valider le comportement nominal.

Ne pas retenir les anciens décodages incohérents (`DLC 15`, CRC-21, octets > 8) : ils provenaient d'une capture sans terminaison correcte et/ou d'entrées flottantes.
