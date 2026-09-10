# Power Management

## Contraintes

Une automobile n’est pas une alimentation 12 V de laboratoire.

Prévoir :
- surtensions ;
- transitoires ;
- chute pendant démarreur ;
- inversion ;
- courant de veille ;
- extinction Linux propre ;
- accès maintenance ponctuel contact coupé.

## Séquence cible normale

```text
ACC ON
→ contrôleur alimentation autorise le 5 V Pi
→ boot Linux
→ LIVI

ACC OFF
→ demande de shutdown au Pi
→ Linux termine proprement
→ confirmation Pi arrêté / temporisation de sécurité
→ coupure matérielle du 5 V
```

Commande Linux normale :

```bash
sudo systemctl poweroff
```

ou équivalent `shutdown -h now`.

Le simple arrêt Linux ne suffit pas à supprimer physiquement le 5 V. La coupure finale doit être assurée par le circuit d’alimentation, seulement après que Linux a terminé.

## Principe du contrôleur d'alimentation

Le Raspberry Pi ne doit pas ouvrir directement un relais qui coupe sa propre alimentation au moment où la commande `poweroff` est lancée : la coupure pourrait intervenir avant la fin des écritures disque.

Architecture cible :

```text
12 V batterie permanent
        │
        ├──> petit contrôleur de veille / logique power
        │       ├── entrée ACC
        │       ├── entrée maintenance
        │       ├── demande shutdown vers Pi
        │       └── commande POWER_EN
        │
        └──> protections automobile
                ↓
             buck 5 V
                ↓
          switch / relais puissance
                ↓
          Raspberry Pi + périphériques
```

La logique de veille reste alimentée avec une consommation à mesurer et minimiser. Le Pi et les périphériques lourds doivent pouvoir être réellement hors tension.

## Handshake d'arrêt recommandé

Une implémentation robuste pourra utiliser :

```text
ACC tombe
   ↓
contrôleur signale SHUTDOWN_REQ
   ↓
service systemd côté Pi lance poweroff
   ↓
Linux démonte les systèmes de fichiers et s'arrête
   ↓
PI_ALIVE / POWER_HOLD disparaît, ou timeout de sécurité expiré
   ↓
contrôleur coupe POWER_EN
```

Le signal exact et la temporisation restent à prototyper. Prévoir des points de test sur le PCB.

## Mode maintenance

Objectif : pouvoir utiliser SSH / terminal / WayVNC lorsque la voiture est arrêtée, sans laisser le Raspberry Pi allumé en permanence.

Deux niveaux sont distingués.

### Maintenance locale / préparée

Un bouton ou une entrée dédiée peut demander au contrôleur d'alimentation :

```text
MAINTENANCE ON
→ maintenir POWER_EN même si ACC = OFF
→ Pi démarre / reste allumé
→ connexion Wi-Fi connue
→ SSH / maintenance
```

Une commande côté Pi pourra ensuite demander la sortie du mode maintenance puis lancer un arrêt propre :

```text
maintenance-off
→ demande de fin maintenance au contrôleur
→ systemctl poweroff
→ contrôleur attend la fin Linux
→ coupure 5 V
```

Le détail du script et du protocole Pi ↔ contrôleur sera défini après prototype d'alimentation.

### Limite importante : réveil distant depuis un Pi totalement éteint

Si le Raspberry Pi est réellement hors tension, il n'a plus de Wi-Fi et ne peut donc pas recevoir une commande SSH.

Une commande SSH `maintenance-on` ne peut fonctionner que si le Pi est déjà alimenté.

Pour pouvoir réveiller le Pi depuis la maison ou un téléphone alors qu'il est totalement éteint, il faut un dispositif de veille séparé capable de recevoir la demande : par exemple un petit contrôleur réseau toujours alimenté, ou une autre source de wake externe. Cette fonction doit être étudiée séparément car elle ajoute consommation de veille, sécurité réseau et complexité.

Décision actuelle :

- prévoir le mode maintenance dans l'architecture d'alimentation ;
- conserver un moyen local simple et fiable ;
- ne pas figer encore la solution de réveil distant permanent ;
- mesurer sa consommation avant toute adoption définitive.

## RP2040

Le RP2040 déjà prévu peut participer à la logique ACC / shutdown / maintenance s'il est alimenté de manière adaptée, mais il n'offre pas à lui seul un réveil Wi-Fi distant dans sa version RP2040-Zero actuelle.

Ne pas lui confier une coupure brutale du Pi. Le handshake avec Linux doit être validé avant intégration finale.

## Cas panne / sécurité

Prévoir :

- timeout maximal si Linux ne répond pas à la demande de shutdown ;
- possibilité de forcer la coupure après un délai de sécurité en cas de crash ;
- comportement sûr si le contrôleur redémarre ;
- absence de boucle ON/OFF ;
- état par défaut défini après déconnexion/reconnexion batterie ;
- fusible 12 V au plus près du prélèvement ;
- protection inversion / transitoires / TVS selon conception finale.

## Interdictions

- Pi directement sur pin OBD 16 via petit buck générique non protégé ;
- GPIO directement sur ACC/reverse 12 V ;
- coupure brutale quotidienne sans stratégie filesystem ;
- compter sur SSH pour réveiller un Pi physiquement hors tension ;
- laisser le Pi complet allumé 24 h/24 uniquement pour permettre une maintenance occasionnelle sans avoir mesuré l'impact batterie.

## Validation avant véhicule

- mesurer boot ;
- mesurer consommation Pi en fonctionnement ;
- mesurer consommation du contrôleur de veille ;
- tester ACC ON / OFF au moins plusieurs dizaines de cycles ;
- vérifier extinction Linux sans erreur filesystem ;
- tester mode maintenance ACC OFF ;
- tester sortie maintenance + shutdown ;
- simuler crash Linux et vérifier timeout matériel ;
- tester déconnexion/reconnexion batterie.
