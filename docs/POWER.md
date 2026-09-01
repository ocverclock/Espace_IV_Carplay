# Power Management

## Contraintes

Une automobile n’est pas une alimentation 12 V de laboratoire.

Prévoir :
- surtensions ;
- transitoires ;
- chute pendant démarreur ;
- inversion ;
- courant de veille ;
- extinction Linux propre.

## Séquence cible

ACC ON
→ alimentation Pi
→ boot
→ LIVI

ACC OFF
→ signal RP2040/service
→ shutdown Linux
→ attente
→ coupure 5 V

## Interdictions

- Pi directement sur pin OBD 16 via petit buck générique non protégé ;
- GPIO directement sur ACC/reverse 12 V ;
- coupure brutale quotidienne sans stratégie filesystem.
