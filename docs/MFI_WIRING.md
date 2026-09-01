# MFI343S00177-L — Câblage Raspberry Pi 4 / LIVI

Projet : **Renault Espace IV — CarPlay OEM+**  
Cible : **Raspberry Pi 4 + LIVI + Apple CarPlay natif**  
Composant : **Microchip MFI343S00177-L — Authentication Coprocessor 3.0**

> **Statut**
>
> Ce document s'appuie sur la documentation publique de LIVI, plusieurs schémas publics de produits commerciaux utilisant le `MFI343S00177`, et WACResearch.
> La fiche technique électrique complète Apple/Microchip n'étant pas publique, les points non garantis par le fabricant sont signalés.
>
> **Ne jamais appliquer 5 V au MFi.**

---

## 1. Architecture retenue

LIVI publie pour Raspberry Pi l'exemple suivant :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
```

Et utilise par défaut :

```text
carPlayMfiI2cBus = 2
carPlayMfiPowerGpio = 21
```

Architecture :

```text
Raspberry Pi 4
        │
        ├── GPIO19 ───── SDA ─────┐
        ├── GPIO26 ───── SCL ─────┤
        ├── GPIO21 ───── POWER ───┤
        └── GND ──────────────────┤
                                  ▼
                         MFI343S00177-L
```

---

## 2. Brochage MFI343S00177-L

Deux schémas publics indépendants de produits commerciaux concordent sur ce brochage :

| Broche | Fonction | Connexion |
|---:|---|---|
| 1 | GND | masse |
| 2 | NC | ne pas connecter |
| 3 | NC | ne pas connecter |
| 4 | GND | masse |
| 5 | SDA | I²C SDA |
| 6 | SCL | I²C SCL |
| 7 | GND | masse |
| 8 | VCC | alimentation |
| 9 | PAD / GND | masse, pad central |

**Brochage retenu pour le projet :**

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

Un commentaire communautaire GitHub donne une numérotation inversée tout en précisant que l'auteur n'est pas certain de l'orientation. Il ne doit donc pas servir de référence pour le PCB.

Avant fabrication, comparer une dernière fois le footprint avec la vue pinout/footprint LCSC du `C33770534`.

---

## 3. Connexions Raspberry Pi 4

| Fonction | GPIO BCM | Broche physique Pi | Broche MFi |
|---|---:|---:|---:|
| SDA | GPIO19 | 35 | 5 |
| SCL | GPIO26 | 37 | 6 |
| POWER | GPIO21 | 40 | 8 |
| GND | — | 39 par exemple | 1, 4, 7, 9 |

Schéma minimal :

```text
RASPBERRY PI 4                           MFI343S00177-L
===============                          =================

GPIO19 / pin 35 ----[22..33Ω]----------> pin 5 SDA
                          |
                          +--[4.7k]--+
                                     |
GPIO26 / pin 37 ----[22..33Ω]----------> pin 6 SCL
                          |          |
                          +--[4.7k]--+
                                     |
GPIO21 / pin 40 ------------------------+----> pin 8 VCC
                                     |
                                    === 100 nF
                                     |
                                    === 1 µF
                                     |
GND / pin 39 ---------------------------+----> pin 1 GND
                                     +----> pin 4 GND
                                     +----> pin 7 GND
                                     +----> pin 9 PAD/GND
```

Les pull-up SDA/SCL doivent être reliées à la **même alimentation commutée que le MFi**.

---

## 4. Valeurs recommandées

### Pull-up I²C

```text
SDA → VCC MFi : 4.7 kΩ
SCL → VCC MFi : 4.7 kΩ
```

Des schémas commerciaux montrent des valeurs de 4.7 kΩ ou 2.2 kΩ.  
`4.7 kΩ` est une bonne valeur de départ pour notre prototype court.

### Résistances série

```text
SDA : 22 à 33 Ω
SCL : 22 à 33 Ω
```

Elles sont facultatives sur le banc, mais recommandées comme footprints sur le PCB final.

### Découplage

Au plus près de la broche 8 :

```text
C1 = 100 nF céramique
C2 = 1 µF
```

entre VCC et GND.

---

## 5. Alimentation

LIVI indique explicitement que le coprocesseur MFi est alimenté depuis un GPIO sur Raspberry Pi.

### Prototype de laboratoire

```text
GPIO21 → VCC MFi
```

correspond au fonctionnement documenté par LIVI.

### PCB final recommandé

Pour la carte automobile définitive, prévoir plutôt :

```text
3.3 V
  │
  ▼
LOAD SWITCH actif à l'état haut
  │ EN
  └──── GPIO21
  │
  ▼
MFI_VCC
```

Puis :

```text
MFI_VCC → pin 8
MFI_VCC → pull-up SDA
MFI_VCC → pull-up SCL
```

Avantages :

- GPIO21 ne porte pas directement le courant du MFi ;
- LIVI conserve le contrôle d'alimentation ;
- les pull-up sont coupées en même temps que le coprocesseur ;
- moins de risque de réalimentation parasite par SDA/SCL.

La référence du load-switch sera choisie après mesure du courant du prototype.

---

## 6. Tension VCC

### Éléments publics observés

- le Raspberry Pi travaille en logique 3.3 V ;
- plusieurs schémas commerciaux alimentent ce MFi en 3.3 V ;
- au moins un autre schéma public montre une alimentation 1.8 V ;
- aucune fiche technique électrique publique complète n'a été trouvée pour confirmer officiellement toute la plage admissible.

### Décision pour notre prototype

```text
VCC = 3.3 V
I²C = 3.3 V
```

Cette décision s'appuie sur le Raspberry Pi et des implémentations commerciales publiques.

**Ne jamais connecter le MFi au 5 V du Raspberry Pi.**

---

## 7. Adresse I²C

WACResearch documente le `MFI343S00177` / CP3.0 à l'adresse :

```text
0x10
```

Une fois alimenté, le coprocesseur doit donc apparaître à l'adresse `10`.

---

## 8. Configuration Raspberry Pi

Ajouter dans `/boot/firmware/config.txt` :

```ini
dtoverlay=i2c-gpio,bus=2,i2c_gpio_sda=19,i2c_gpio_scl=26,i2c_gpio_delay_us=5
```

Puis redémarrer.

Vérifier :

```bash
ls /dev/i2c-*
```

On doit notamment obtenir :

```text
/dev/i2c-2
```

---

## 9. Configuration LIVI

Configuration correspondant à ce document :

```json
{
  "carPlayMfiI2cBus": 2,
  "carPlayMfiPowerGpio": 21
}
```

Récapitulatif :

```text
bus I²C = 2
SDA     = GPIO19
SCL     = GPIO26
POWER   = GPIO21
```

---

## 10. Procédure de test

### Test 1 — hors tension

Contrôler au multimètre :

```text
pin 1 → GND
pin 4 → GND
pin 7 → GND
pad 9 → GND
pin 5 → SDA
pin 6 → SCL
pin 8 → alimentation
```

Vérifier l'absence de court-circuit :

```text
VCC ↔ GND
SDA ↔ GND
SCL ↔ GND
```

### Test 2 — alimentation seule

Ne pas connecter l'iPhone.

Démarrer uniquement :

```text
Raspberry Pi + MFi
```

Mesurer lorsque le MFi est alimenté :

```text
VCC ≈ 3.3 V
SDA ≈ 3.3 V au repos
SCL ≈ 3.3 V au repos
```

### Test 3 — détection I²C

Une fois GPIO21 actif :

```bash
sudo i2cdetect -y 2
```

Adresse attendue :

```text
0x10
```

**Ne pas poursuivre vers CarPlay avant d'avoir validé cette étape.**

---

## 11. Implantation PCB

Le `MFI343S00177-L` est un DFN/XDFN 8 d'environ `2 × 3 mm` avec pad central.

Prévoir :

- `C1 100 nF` immédiatement à côté de VCC ;
- piste GND très courte vers le pad central ;
- SDA/SCL courtes ;
- footprints de résistances série ;
- pull-up vers `MFI_VCC`, pas vers un +3.3 V permanent ;
- points de test.

Points de test recommandés :

```text
TP_MFI_VCC
TP_MFI_SDA
TP_MFI_SCL
TP_GND
```

---

## 12. Schéma recommandé pour le PCB final

```text
                         +3V3
                          │
                          ▼
                +------------------+
GPIO21 -------->| EN   LOAD SWITCH |
                +--------+---------+
                         |
                         +--------- MFI_VCC
                         |             |
                         |            === 100nF
                         |             |
                         |            === 1µF
                         |             |
                         |            GND
                         |
                         +--[4.7k]--+
                         |          |
GPIO19 ----[33Ω]---------+----------+------ SDA / pin 5
                                    |
                         +--[4.7k]--+
                         |          |
GPIO26 ----[33Ω]---------+----------+------ SCL / pin 6

GND -------------------------------------- pin 1
GND -------------------------------------- pin 4
GND -------------------------------------- pin 7
GND -------------------------------------- pad 9

MFI_VCC ---------------------------------- pin 8
```

---

## 13. Points encore à confirmer avant PCB V1

À mesurer sur notre prototype :

- courant réel du `MFI343S00177-L` ;
- comportement exact de GPIO21 au démarrage de LIVI ;
- délai nécessaire entre alimentation et accès I²C ;
- détection réelle à `0x10` ;
- fonctionnement réel de l'authentification CarPlay ;
- nécessité réelle des résistances série ;
- valeur optimale des pull-up.

À ne pas figer avant ces tests :

- référence définitive du load-switch ;
- valeur finale des pull-up ;
- footprint définitif sans comparaison avec le pinout fournisseur.

---

## 14. Sources techniques

### LIVI

Documentation officielle publique :

`https://github.com/f-io/LIVI`

Informations utilisées :

- coprocesseur MFi obligatoire ;
- communication directe en I²C ;
- `carPlayMfiI2cBus = 2` ;
- `carPlayMfiPowerGpio = 21` ;
- exemple Pi : SDA GPIO19 / SCL GPIO26.

### WACResearch

`https://github.com/BertoldVdb/WACResearch`

Information utilisée :

```text
MFI343S00177 / CP3.0 → adresse I²C 0x10
```

### Schémas publics de produits commerciaux

Plusieurs schémas concordent sur :

```text
1 GND
2 NC
3 NC
4 GND
5 SDA
6 SCL
7 GND
8 VCC
9 PAD/GND
```

Ils montrent également des conceptions 3.3 V, des pull-up de 2.2 kΩ à 4.7 kΩ, des résistances série de l'ordre de 22–33 Ω et un découplage local.

### LCSC

Référence :

```text
MFI343S00177-L
LCSC C33770534
```

La vue pinout/footprint LCSC doit être contrôlée une dernière fois avant envoi du PCB.

---

## 15. Décision projet

Pour le prototype Espace IV :

```text
MFI343S00177-L

pin 5 SDA      → Raspberry Pi GPIO19
pin 6 SCL      → Raspberry Pi GPIO26
pin 8 VCC      → Raspberry Pi GPIO21
pins 1/4/7/9   → GND

SDA pull-up → VCC MFi via 4.7 kΩ
SCL pull-up → VCC MFi via 4.7 kΩ

C1 = 100 nF
C2 = 1 µF

LIVI :
carPlayMfiI2cBus = 2
carPlayMfiPowerGpio = 21
```

Avant le PCB final, le prototype doit impérativement confirmer `0x10` sur `/dev/i2c-2` et le fonctionnement réel de l'authentification CarPlay.
