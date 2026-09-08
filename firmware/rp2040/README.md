# RP2040 Firmware

## État

Le prototype de lecture de la commande au volant est opérationnel sur RP2040-Zero.

Firmware validé :

- `steering_remote_test/steering_remote_test.ino`

Fonctions acquises :

- scan des six contacts secs ;
- debounce ;
- reconnaissance des cinq boutons ;
- combinaison `VOL+ + VOL− = MUTE` ;
- décodage bidirectionnel de la molette ;
- compteur série de diagnostic.

Prochaine étape : USB HID vers le Raspberry Pi, puis intégration de reverse et ACC/illumination.

## Responsabilités prévues

- commande au volant ;
- molette ;
- reverse ;
- ACC / illumination ;
- USB HID vers Raspberry Pi.

Le CSW-2000R reste traité séparément sur son bus CAN privé et n'est pas lu directement par le RP2040.

## Règle

Ne coder les tables de commandes qu’après mesures documentées dans `docs/`.
