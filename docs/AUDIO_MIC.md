# Audio & Microphone

## Objectif

Conserver l’amplification et les haut-parleurs Renault autant que possible, tout en faisant du Raspberry Pi la source audio et l'arbitre principal des commandes utilisateur.

## Piste principale

```text
Pi / LIVI / autres sources audio
          ↓
    mixeur logiciel
          ↓
 niveau master fixe / calibré
          ↓
     DAC / sortie ligne
          ↓
      AUX Renault
          ↓
autoradio / étage ampli Renault
          ↓
 haut-parleurs d'origine
```

## Commandes utilisateur

Les commandes physiques ne restent pas reliées directement au système OEM :

- `CSW-2000R` isolé sur un CAN privé vers le Raspberry Pi ;
- commande au volant lue directement par RP2040 ;
- le RP2040 décide quelles actions restent internes au Raspberry et lesquelles sont reproduites vers l'électronique Renault.

## Volume — stratégie retenue

Observation utilisateur : **le volume de l'autoradio Renault n'est pas conservé au démarrage**.

Le volume général doit donc rester géré par l'électronique Renault. Le Raspberry ne sert pas de second potentiomètre de volume général.

Architecture :

```text
VOL+ / VOL- physiques
       ↓
     RP2040
       ↓
contacts électroniques flottants
       ↓
décodeur Renault d'origine
       ↓
volume OEM / ampli Renault
```

Contacts mesurés :

```text
VOL+ = 4 ↔ 1
VOL- = 4 ↔ 6
```

Le RP2040 devient un **proxy filtrant** :

- la commande physique est exclusive à notre électronique ;
- le vieux système ne voit jamais directement le bouton ;
- le RP2040 reproduit volontairement `VOL+` / `VOL-` vers le décodeur OEM ;
- les autres commandes peuvent rester exclusivement utilisées par LIVI/CarPlay.

### Progression du volume

La stratégie la plus naturelle est de reproduire la **durée réelle de fermeture** :

```text
appui court VOL+  → fermeture 4↔1 pendant la durée de l'appui
appui maintenu    → 4↔1 reste fermé tant que le bouton est maintenu
relâchement       → ouverture immédiate
```

Même principe pour `VOL-` avec `4↔6`.

Si le décodeur Renault gère lui-même l'auto-répétition lors d'un appui maintenu, on récupère ainsi exactement la progression OEM sans avoir à fabriquer une cadence artificielle. Cela doit être confirmé sur véhicule.

## Contact électronique final — PhotoMOS / OptoMOS

Un PhotoMOS/OptoMOS n'est pas un optocoupleur qui pilote un relais mécanique. C'est un **relais statique optiquement commandé** :

```text
GPIO RP2040
   ↓
résistance LED
   ↓
LED interne
   ║ isolation optique
   ↓
MOSFET(s) de sortie
   ↓
contact flottant électronique
```

Les versions adaptées à notre usage utilisent typiquement deux MOSFETs montés tête-bêche, ce qui donne un contact :

- sans clic ;
- sans pièce mécanique ;
- flottant par rapport au RP2040 ;
- bidirectionnel ;
- normalement ouvert ;
- proche électriquement d'un petit bouton mécanique.

Pour le PCB final, cette famille est préférée aux relais mécaniques si les mesures OEM confirment que tension, courant et résistance ON sont compatibles.

Ne pas choisir la référence exacte avant mesure du balayage OEM.

## Mixage logiciel Raspberry

Le **master de sortie du Raspberry/DAC reste fixe et calibré**. Le mixeur logiciel sert uniquement à équilibrer les sources générées par le Raspberry entre elles.

Exemple logique :

```text
CarPlay musique ──────┐
Navigation ───────────┤
Siri / appels ────────┤→ PipeWire / ALSA mixer → master fixe → DAC
sons système ─────────┤
alertes locales ──────┘
```

Le mixeur peut gérer :

- gain relatif par source ;
- ducking de la musique pendant navigation / Siri ;
- priorité appels ;
- mute par source ;
- limiteur / marge de sécurité pour éviter le clipping.

Le **volume utilisateur global** reste ensuite appliqué par l'électronique Renault.

## A vérifier sur véhicule

- entrée AUX réellement accessible ;
- tension entre les six lignes de commande volant côté décodeur OEM au repos ;
- courant lorsqu'un contact VOL+/VOL- est fermé ;
- fréquence éventuelle de balayage ;
- durée d'impulsion reconnue pour un appui court ;
- comportement d'un appui long : fermeture continue ou autre séquence ;
- auto-répétition réellement gérée par le décodeur OEM ;
- comportement AUX/wake de l'autoradio ;
- niveau ligne fixe optimal du DAC ;
- headroom logiciel nécessaire ;
- masse ;
- boucle de masse ;
- mute téléphone ;
- comportement CarPlay ;
- pops au boot/shutdown ;
- besoin éventuel d'une isolation audio 1:1.

## Micro

Priorité :
1. micro Renault d’origine ;
2. micro automobile externe discret.

Mesurer la nature du micro d’origine avant connexion à une carte son.
