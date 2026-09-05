# Analyseur logique — protocole de mesure CSW-2000R

## Matériel

Analyseur logique USB 8 voies / 24 MHz, détecté sous Linux comme :

```text
0925:3881 Lakeview Research Saleae Logic
```

Logiciel : PulseView / sigrok avec driver `fx2lafw`.

Réglage de travail actuel :

```text
sample rate : 8 MHz
D0          : PCA82C250 pin 1 = TXD
D1          : PCA82C250 pin 4 = RXD
GND         : masse commune CSW / analyseur
```

Les entrées TXD/RXD du PCA82C250 sont observées directement, sans pont diviseur.

## Observation importante — canal flottant / fil Dupont

Essai du `2026-09-05` :

- lorsqu'un fil Dupont est branché sur une entrée de l'analyseur mais laissé libre côté circuit, le canal peut afficher une activité ressemblant au signal voisin ;
- lorsqu'aucun fil n'est raccordé à ce canal, il reste plat ;
- lorsqu'un seul canal est réellement relié au CSW, seul ce canal présente l'activité correspondante.

Conclusion : le phénomène observé précédemment sur un canal non relié au PCB provenait d'un **fil flottant agissant comme antenne / couplage capacitif**, pas d'une duplication interne entre les voies de l'analyseur.

Statut : **MEASURED / USER CONFIRMED — 2026-09-05**.

Conséquence : les anciennes captures où un canal flottant muni d'un fil Dupont semblait reproduire TXD/RXD ne doivent pas être utilisées pour comparer les deux signaux.

## Règles de mesure retenues

Pour les prochaines captures :

1. ne laisser aucun fil de mesure inutilisé flottant ;
2. n'interpréter que les voies réellement connectées ;
3. utiliser des fils aussi courts que possible ;
4. conserver une masse commune fiable ;
5. idéalement rapprocher chaque signal d'un conducteur de masse pour réduire le couplage ;
6. désactiver les voies inutilisées dans PulseView ;
7. valider d'abord la forme brute TXD/RXD avant d'interpréter le décodeur CAN.

## État du décodage CAN

Observations acquises :

- aucune activité au repos lorsque aucun bouton n'est actionné ;
- une pression sur un bouton provoque immédiatement une activité sur la voie réellement connectée ;
- une largeur d'impulsion de l'ordre de `2 µs` a été mesurée sur TXD, ce qui rend `500 kbit/s` plausible ;
- ce débit n'est pas encore considéré comme définitivement validé par une trame CAN correctement décodée ;
- les sorties PulseView précédentes de type `DLC 15`, `Data byte 32`, `CRC-21`, etc. sont considérées comme faux décodage / perte de cadrage et ne doivent pas être utilisées comme protocole réel du CSW.

## Prochaine mesure propre

Reconnecter simultanément :

```text
D0 -> PCA82C250 pin 1 TXD
D1 -> PCA82C250 pin 4 RXD
GND -> GND CSW
```

avec fils courts et aucun autre fil flottant.

Comparer TXD et RXD sur la même pression de bouton. Si une différence de durée reste visible entre TXD et RXD dans ces conditions propres, elle sera alors considérée comme réelle et on pourra tester l'influence d'une charge / terminaison CAN de banc.
