# Test Log

Journal chronologique des mesures.

## 2026-08-31 — XTOOL A30M

- connexion Bluetooth Linux réussie ;
- périphérique : `A30M-WE86VF7`
- Bluetooth Serial Port Profile présent ;
- RFCOMM channel observé : 3 ;
- commandes ELM `ATI/ATDP` non exploitables ;
- conclusion : protocole propriétaire XTOOL, ne pas considérer comme ELM327.

## 2026-08-31 — ELM327 Wi-Fi ancien

- PC : `192.168.0.11`
- interface : `192.168.0.10`
- TCP : `35000`
- connexion TCP réussie ;
- `ATI` → `ELM327 v1.5`
- `ATDP` → `ISO 15765-4 (CAN 11/500)`
- `010C` → `CAN ERROR`
- `0105` → `CAN ERROR`
- `010B` → `CAN ERROR`

Conclusion : interface non fiable pour la suite.

## Prochains tests

### Commande volant
TBD à réception.

### CSW-2000R
TBD à réception.
