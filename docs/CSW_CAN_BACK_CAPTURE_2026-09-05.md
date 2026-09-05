# CSW-2000R — capture bouton BACK — 2026-09-05

Capture fournie :

```text
csw_bouton_back.sr
```

## Montage

```text
sample rate = 8 MHz
D0 = PCA82C250 pin 1 = TXD
D5 = PCA82C250 pin 4 = RXD
CAN nominal bitrate = 500000 bit/s
terminaison banc ≈ 110 Ω (2 × 220 Ω en parallèle)
```

Durée de capture : `6,25 s` (`50 000 000` échantillons).

## Décodage automatique

Nombre de trames classiques décodées :

```text
2188
```

Toutes les trames sont strictement identiques :

```text
ID      = 0x681
DLC     = 8
Payload = F0 0A 0A 01 FF FF FF FF
```

Aucun autre identifiant, aucun autre payload et aucune modification d'octet n'ont été trouvés dans la capture.

TXD et RXD correspondent : les débuts de trame observés sur D5 sont décalés d'environ `2` échantillons seulement par rapport à D0, soit environ `0,25 µs` à 8 MHz.

## Organisation temporelle

La capture contient `13` salves périodiques. Les débuts de salve sont espacés d'environ `500 ms`.

Les salves complètes contiennent typiquement environ `178 à 184` retransmissions et durent environ `48,5 à 50,1 ms`. La dernière salve est tronquée par la fin de capture.

Le comportement est pratiquement identique à la capture `csw_idle.sr` : même ID, même payload, même périodicité et même structure en salves.

## Conclusion

Dans les conditions actuelles du banc, l'action BACK ne produit **aucune trame CAN distinguable de la baseline idle**.

La baseline reste :

```text
0x681  F0 0A 0A 01 FF FF FF FF
```

Cela ne permet pas de conclure que le bouton BACK n'est pas traité par le CSW. Le banc ne comporte encore aucun second nœud CAN actif capable d'acquitter les trames. Le CSW peut donc être bloqué dans un comportement de retransmission / état réseau non nominal et ne pas atteindre le fonctionnement applicatif normal.

## Prochaine étape prioritaire

Ajouter un second contrôleur CAN actif configuré en mode normal à `500 kbit/s`, sans émission applicative volontaire dans un premier temps, afin qu'il fournisse l'ACK matériel aux trames valides.

Ensuite vérifier :

1. si les salves de retransmission disparaissent ;
2. quelle devient la périodicité normale de `0x681` ;
3. si l'appui BACK génère alors un nouvel ID, un changement de payload ou une trame supplémentaire ;
4. si aucun changement n'apparaît malgré l'ACK, rechercher alors une initialisation applicative attendue d'un autre nœud CNC/Xanavi.

Statut : **CAPTURE FILE ANALYZED — 2026-09-05**.
