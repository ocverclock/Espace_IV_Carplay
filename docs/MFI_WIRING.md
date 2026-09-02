# MFI343S00177-L — Câblage Raspberry Pi 4 / LIVI

Projet : **Renault Espace IV — CarPlay OEM+**  
Cible : **Raspberry Pi 4 + LIVI + Apple CarPlay natif**  
Composant : **Microchip `MFI343S00177-L` — Authentication Coprocessor 3.0**

> **Statut**
>
> Ce document s'appuie sur la documentation publique actuelle de LIVI, plusieurs schémas publics indépendants utilisant le `MFI343S00177`, la référence fournisseur LCSC `C33770534` et WACResearch.
>
> La documentation électrique Apple/Microchip complète du composant n'est pas publique. Les éléments non mesurés sur notre propre prototype restent donc explicitement à valider avant PCB final.
>
> **Ne jamais appliquer 5 V au MFi.**

---

## 1. Ce qui est confirmé par LIVI

LIVI exige un coprocesseur MFi physique pour CarPlay natif et communique directement avec lui par I²C.

Configuration par défaut actuelle :

```text
carPlayMfiI2cBus = 2
carPlayMfiPowerGpio = 21
```

Le script d'installation LIVI configure sur Raspberry Pi :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
```

Donc, pour notre projet :

```text
SDA  = GPIO19
SCL  = GPIO26
BUS  = /dev/i2c-2
PWR_EN = GPIO21
```

**Important :** `carPlayMfiPowerGpio` signifie que LIVI pilote un GPIO destiné à contrôler l'alimentation du MFi. Ce document ne suppose plus que ce GPIO doit alimenter directement la puce.

---

## 2. Architecture électrique retenue

Architecture de référence pour le projet :

```text
Raspberry Pi 4

GPIO19 ---------------------------- SDA
GPIO26 ---------------------------- SCL
GPIO21 ---------------------------- EN load-switch
GND -------------------------------- GND
3V3 ----> load-switch ----> MFI_VCC
```

Puis :

```text
MFI_VCC → alimentation MFi
MFI_VCC → pull-up SDA
MFI_VCC → pull-up SCL
```

Cette architecture évite :

- de faire porter directement le courant du MFi à GPIO21 ;
- de réalimenter le MFi par SDA/SCL lorsqu'il est coupé ;
- de figer une hypothèse électrique non documentée publiquement.

Le load-switch exact sera choisi après mesure du courant du prototype.

---

## 3. Brochage retenu du MFI343S00177-L

Plusieurs schémas publics indépendants concordent sur :

| Broche | Fonction | Connexion projet |
|---:|---|---|
| 1 | GND | masse |
| 2 | NC | ne pas connecter |
| 3 | NC | ne pas connecter |
| 4 | GND | masse |
| 5 | SDA | I²C SDA |
| 6 | SCL | I²C SCL |
| 7 | GND | masse |
| 8 | VCC | `MFI_VCC` |
| 9 | PAD / GND | masse, pad central |

Brochage retenu :

```text
1 = GND
2 = NC
3 = NC
4 = GND
5 = SDA
6 = SCL
7 = GND
8 = VCC
9 = PAD / GND
```

### Niveau de confiance

`CONFIRMED_FROM_MULTIPLE_PUBLIC_SCHEMATICS`, mais pas encore `MEASURED` sur notre carte.

Avant fabrication :

1. vérifier une dernière fois l'orientation du boîtier et le repère pin 1 ;
2. comparer le footprint KiCad avec la vue pinout/land pattern de la référence fournisseur `C33770534` ;
3. effectuer un contrôle de continuité sur la première carte assemblée avant alimentation.

---

## 4. Connexions Raspberry Pi 4

| Fonction | GPIO BCM | Broche physique Pi | Destination |
|---|---:|---:|---|
| SDA | GPIO19 | 35 | MFi pin 5 via résistance série |
| SCL | GPIO26 | 37 | MFi pin 6 via résistance série |
| MFi power enable | GPIO21 | 40 | entrée `EN` du load-switch |
| 3.3 V | — | 1 ou 17 | entrée du load-switch |
| GND | — | 39 par exemple | MFi pins 1, 4, 7 et pad 9 |

**GPIO21 ne doit pas être relié à la pin 8 dans le schéma final.**

---

## 5. Schéma recommandé

```text
                          Raspberry Pi 4

 +3V3 -------------------------+
                               |
                               v
                       +---------------+
 GPIO21 -------------->| EN LOAD SWITCH|   actif haut
                       +-------+-------+
                               |
                               +---------------- MFI_VCC
                               |                    |
                               |                   === C1 100 nF
                               |                    |
                               |                   === C2 1 µF
                               |                    |
                               |                   GND
                               |
                               +----[RPU1 4.7k]----+
                               |                    |
 GPIO19 ----[RS1 33R]----------+--------------------+---- pin 5 SDA
                               |
                               +----[RPU2 4.7k]----+
                               |                    |
 GPIO26 ----[RS2 33R]----------+--------------------+---- pin 6 SCL

 MFI_VCC ------------------------------------------------ pin 8 VCC
 GND ---------------------------------------------------- pin 1 GND
 GND ---------------------------------------------------- pin 4 GND
 GND ---------------------------------------------------- pin 7 GND
 GND ---------------------------------------------------- pad 9 GND

 pin 2 -------------------------------------------------- NC
 pin 3 -------------------------------------------------- NC
```

---

## 6. Valeurs de départ

### Pull-up I²C

```text
RPU1 SDA = 4.7 kΩ
RPU2 SCL = 4.7 kΩ
```

Ces pull-up vont vers `MFI_VCC`, pas vers un 3.3 V permanent.

Prévoir des footprints permettant de passer à `2.2 kΩ` si les mesures montrent que cela est préférable.

### Résistances série

```text
RS1 SDA = 33 Ω
RS2 SCL = 33 Ω
```

Elles servent principalement à amortir les fronts et peuvent être remplacées par `0 Ω` si nécessaire.

### Découplage

Au plus près de la pin 8 :

```text
C1 = 100 nF céramique
C2 = 1 µF céramique
```

---

## 7. Tension d'alimentation

### Etat des connaissances

Des implémentations publiques du `MFI343S00177` existent en 3.3 V et certaines en 1.8 V. La plage complète n'est pas confirmée par une datasheet fabricant publique accessible.

Le Raspberry Pi utilise des GPIO 3.3 V.

### Décision prototype

Pour notre banc Raspberry Pi :

```text
MFI_VCC = 3.3 V
I²C pull-up = 3.3 V commuté
```

Statut : `PROTOTYPE_ASSUMPTION_SUPPORTED_BY_PUBLIC_IMPLEMENTATIONS`.

Cette valeur doit être validée sur le matériel réel avant d'être promue en `MEASURED`.

**5 V interdit.**

---

## 8. Adresse I²C attendue

WACResearch documente :

```text
MFI343S00177 / CP3.0 = 0x10
```

Adresse attendue sur notre bus :

```text
/dev/i2c-2 → 0x10
```

Cette information est une référence publique externe ; la validation projet sera faite par mesure avec notre propre composant.

---

## 9. Configuration Raspberry Pi / LIVI

LIVI requiert actuellement Raspberry Pi 4/5 sous Trixie / Debian 13 pour OpenGL ES 3.x.

Overlay attendu :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
```

Le script LIVI gère normalement cette configuration. En installation manuelle, vérifier le fichier de boot réellement utilisé par la distribution avant modification.

Configuration LIVI :

```json
{
  "carPlayMfiI2cBus": 2,
  "carPlayMfiPowerGpio": 21
}
```

---

## 10. Procédure de validation du prototype

### V0 — inspection

Avant alimentation :

- orientation du MFi confirmée ;
- soudures inspectées à la loupe/microscope ;
- pin 2 et 3 non connectées ;
- pins 1/4/7/pad 9 à la masse ;
- pin 8 reliée uniquement à `MFI_VCC` ;
- absence de court-circuit VCC/GND.

### V1 — alimentation sans MFi si possible

Tester d'abord le load-switch :

```text
GPIO21 LOW  → MFI_VCC ≈ 0 V
GPIO21 HIGH → MFI_VCC ≈ 3.3 V
```

Vérifier qu'il n'existe pas de tension significative sur `MFI_VCC` lorsque le switch est coupé.

### V2 — MFi monté, sans iPhone

Mesurer :

- `MFI_VCC` ;
- courant consommé ;
- SDA au repos ;
- SCL au repos.

Attendu lorsque activé :

```text
MFI_VCC ≈ 3.3 V
SDA ≈ MFI_VCC
SCL ≈ MFI_VCC
```

Inscrire les valeurs dans `docs/TEST_LOG.md`.

### V3 — détection I²C

Installer `i2c-tools` si nécessaire puis :

```bash
ls /dev/i2c-*
sudo i2cdetect -y 2
```

Résultat attendu d'après WACResearch :

```text
0x10
```

Ne pas considérer le MFi validé tant que `0x10` n'a pas été mesuré sur notre prototype.

### V4 — LIVI

Démarrer LIVI et vérifier :

- commande d'alimentation GPIO21 ;
- présence du MFi ;
- absence d'erreur MFi dans les logs ;
- comportement après redémarrage.

### V5 — CarPlay

Seulement après V0 à V4 :

- connexion iPhone ;
- CarPlay filaire ;
- CarPlay sans fil ensuite ;
- Roole Map ;
- audio ;
- microphone ;
- Siri ;
- reconnexion après reboot.

---

## 11. Points de test à prévoir sur PCB

Obligatoires :

```text
TP_MFI_VCC
TP_MFI_EN
TP_MFI_SDA
TP_MFI_SCL
TP_GND
```

Option utile :

```text
TP_3V3_IN
```

Les points doivent être accessibles même lorsque le module est intégré dans son boîtier de laboratoire.

---

## 12. Critères du load-switch

Référence non encore choisie.

Exigences :

- alimentation entrée 3.3 V ;
- sortie 3.3 V ;
- entrée `EN` compatible logique Raspberry Pi 3.3 V ;
- actif haut pour correspondre naturellement au GPIO LIVI ;
- très faible courant au repos ;
- faible chute de tension ;
- courant largement supérieur au courant MFi qui sera mesuré ;
- boîtier facile à assembler sur PCB prototype si possible.

Le choix définitif fera l'objet d'une entrée dans `docs/DECISIONS.md` après mesure.

---

## 13. Règles de routage PCB

- `C1 100 nF` au plus près de pin 8 ;
- retour GND du condensateur court vers plan de masse/pad central ;
- SDA/SCL courtes et éloignées des convertisseurs à découpage ;
- résistances série accessibles ;
- pull-up vers la sortie commutée `MFI_VCC` ;
- ne pas router sous le pad exposé sans raison ;
- prévoir un vrai land pattern compatible assemblage du DFN/XDFN ;
- assemblage professionnel recommandé pour le PCB final.

---

## 14. Niveaux de preuve

### Confirmé par LIVI

- MFi physique requis pour CarPlay natif ;
- communication I²C ;
- bus par défaut `2` ;
- power GPIO par défaut `21` ;
- overlay Raspberry Pi : GPIO19 SDA / GPIO26 SCL ;
- Pi 4 nécessite Trixie / Debian 13 pour LIVI actuel.

### Confirmé par plusieurs schémas publics

- pinout retenu 1/4/7/9 GND, 5 SDA, 6 SCL, 8 VCC ;
- usages 3.3 V observés ;
- pull-up et découplage local.

### Confirmé par WACResearch

- CP3.0 `MFI343S00177` attendu à `0x10`.

### A mesurer sur notre prototype

- tension réellement acceptée par notre lot de composants ;
- courant consommé ;
- adresse `0x10` ;
- valeurs optimales de pull-up ;
- timing de GPIO21 ;
- authentification CarPlay réelle.

---

## 15. Sources

### LIVI

- dépôt : `https://github.com/f-io/LIVI`
- README : section `MFi Authentication`
- installation : `scripts/install/common.sh`

### WACResearch

- `https://github.com/BertoldVdb/WACResearch`

### Fournisseur

- Microchip `MFI343S00177-L`
- LCSC `C33770534`

Les schémas publics tiers utilisés pour recouper le pinout doivent être archivés ou référencés dans `references/` avant le gel du PCB V1 afin que la décision reste reproductible même si les pages externes disparaissent.

---

## 16. Décision actuelle du projet

```text
MFI343S00177-L

pin 5 SDA      → GPIO19 via 33 Ω
pin 6 SCL      → GPIO26 via 33 Ω
pin 8 VCC      → MFI_VCC commuté
pins 1/4/7/9   → GND
pins 2/3       → NC

3.3 V → load-switch → MFI_VCC
GPIO21 → load-switch EN

SDA pull-up → MFI_VCC via 4.7 kΩ
SCL pull-up → MFI_VCC via 4.7 kΩ

C1 = 100 nF
C2 = 1 µF

LIVI :
carPlayMfiI2cBus = 2
carPlayMfiPowerGpio = 21
```

Le PCB V1 ne doit pas être envoyé en fabrication tant que l'orientation/footprint du composant n'a pas été revérifié et que le banc n'a pas confirmé la détection I²C et l'authentification CarPlay.
