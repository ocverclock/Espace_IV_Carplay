# ESP32 + MCP2518FD — banc CSW-2000R

Objectif : utiliser l'ESP32 comme hôte SPI du module MCP2518FD/ATA6563 pour :

- fournir l'ACK CAN au CSW-2000R ;
- recevoir le CAN classique à `500 kbit/s` ;
- afficher `timestamp / ID / DLC / data` sur le port série ;
- comparer automatiquement l'état idle et les commandes.

## Câblage proposé

```text
ESP32 GPIO18 SCK  -> SCK
ESP32 GPIO23 MOSI -> SDI
ESP32 GPIO19 MISO <- SDO
ESP32 GPIO5  CS   -> nCS
ESP32 GPIO27 INT  <- INT
ESP32 GND         -> GND
```

Côté CAN :

```text
H -> CSW CANH = CN1-5/6
L -> CSW CANL = CN1-11/12
G -> CSW GND  = CN1-8
```

La notice du module annonce une alimentation `3.3 V - 5 V` et une compatibilité directe ESP32. Vérifier toutefois la sérigraphie du module reçu avant première alimentation.

## Librairie de travail

Ébauche basée sur `ACAN2517` de Pierre Molinaro, qui supporte MCP2517FD/MCP2518FD en CAN 2.0B et ESP32.

Installer la librairie `ACAN2517` dans Arduino IDE / PlatformIO avant compilation.

## Point bloquant avant premier flash

La fréquence d'oscillateur du module doit être lue physiquement. Le sketch est préparé avec `40 MHz` comme hypothèse de travail, mais cette ligne doit être corrigée si le quartz reçu est `20 MHz`.

## Résultat attendu

Exemple de sortie :

```text
0001234567 ID=681 DLC=8 DATA=F0 0A 0A 01 FF FF FF FF
```

Une fois l'ACK fonctionnel, les retransmissions massives observées avec PulseView devraient fortement diminuer. Ensuite les boutons pourront être comparés directement à la baseline.
