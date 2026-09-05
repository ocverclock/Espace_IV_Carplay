# Audio & Microphone

## Objectif

Conserver l’amplification et les haut-parleurs Renault autant que possible, tout en faisant du Raspberry Pi la source audio et l'arbitre principal des commandes utilisateur.

## Piste principale

```text
Pi / LIVI
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

## Volume — stratégie corrigée

Observation utilisateur : **le volume de l'autoradio Renault n'est pas conservé au démarrage**.

Conséquence : la stratégie précédente "autoradio laissé à un niveau fixe + volume quotidien uniquement côté Pi/DAC" n'est plus la stratégie principale.

Le chemin privilégié devient :

```text
VOL+ / VOL- physiques
       ↓
     RP2040
       ↓
fermeture électronique des mêmes paires que la commande d'origine
       ↓
décodeur Renault d'origine
       ↓
système audio / volume OEM
```

Contacts mesurés :

```text
VOL+ = 4 ↔ 1
VOL- = 4 ↔ 6
```

Le RP2040 devient donc un **proxy filtrant** :

- la commande physique est exclusive à notre électronique ;
- le vieux système ne voit jamais directement le bouton ;
- le RP2040 peut néanmoins simuler un appui OEM uniquement pour `VOL+` / `VOL-` ;
- les autres commandes peuvent rester exclusivement utilisées par LIVI/CarPlay.

Pour le prototype, deux relais SPST-NO conviennent. Pour le PCB final, privilégier des contacts électroniques flottants/bidirectionnels de type PhotoMOS/OptoMOS ou un interrupteur analogique adapté à la tension de balayage réellement mesurée.

Ne pas choisir un simple NPN/NMOS à la masse avant mesure du balayage OEM.

Le DAC du Pi sera maintenu à un niveau de ligne stable et sûr. Éviter une double variation simultanée du volume logiciel Pi et du volume OEM.

## A vérifier sur véhicule

- entrée AUX réellement accessible ;
- tension entre les six lignes de commande volant côté décodeur OEM au repos ;
- courant lorsqu'un contact VOL+/VOL- est fermé ;
- fréquence éventuelle de balayage ;
- durée d'impulsion reconnue pour un appui court ;
- comportement d'un appui long / auto-répétition ;
- comportement AUX/wake de l'autoradio ;
- niveau ligne ;
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
