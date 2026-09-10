# Checkpoint LIVI UI / télémétrie — 2026-09-10

## Statut

Checkpoint validé sur Raspberry Pi 4 avec LIVI v8.3.0.

Résultat obtenu :

- écran d'attente LIVI personnalisé avec heure + date, sans l'icône téléphone d'origine ;
- logique native de projection conservée : l'overlay d'attente reste piloté par le `show` de `StatusOverlay` ;
- Dash4 réaffecté à une page télémétrie Espace IV ;
- télémétrie simulée reçue par LIVI via Socket.IO et affichée dans les dashboards ;
- Dash4 validé visuellement avec une mise en page 1280×720 adaptée à l'écran ;
- style Dash4 et écran d'attente externalisé dans un CSS modifiable sans recompilation ;
- police Inter installée sur le Raspberry et utilisée par le thème Espace ;
- build applicatif et build Linux ARM64 réussis ;
- l'AppImage personnalisée a été lancée et testée sur le banc.

Ce checkpoint ne valide pas encore CarPlay réel, MFi, caméra physique, audio, micro, Siri ni CAN véhicule réel.

## Architecture retenue

### Écran d'attente

Le premier essai consistait à modifier `Home.tsx`. Cette approche est abandonnée.

LIVI v8.3.0 utilise `Home` comme hôte vide tandis que `Projection` gère l'overlay et le flux de projection. Le bon point d'intégration est donc :

```text
src/renderer/src/components/pages/projection/Projection.tsx
```

Le `CropPortraitOutlinedIcon` d'origine a été supprimé et remplacé par :

```text
heure
jour + date
```

Le comportement attendu reste :

```text
pas de CarPlay
    ↓
heure + date
    ↓
CarPlay démarre
    ↓
overlay natif masqué
    ↓
projection plein écran
```

La disparition réelle de l'overlay lors d'une session CarPlay doit encore être validée avec le MFi et l'iPhone.

## Dash4 Espace IV

Le Dash4 d'origine de LIVI était un hôte `cluster stream only`. Il a été réaffecté à la télémétrie Espace IV.

Cela ne remplace pas la caméra de recul : la caméra utilise sa propre route `ROUTES.CAMERA` et la logique `autoSwitchOnReverse` reste indépendante.

Le mode cluster vidéo n'est pas perdu dans le projet : Dash3 conserve le mode `clusterFull`.

### Mise en page validée

```text
EXTÉRIEUR   16.5 °C                                      15:53

MOTEUR                                  FAP
Eau                    89 °C            Charge                  42 %
Huile                  96 °C            Régénération            NON
Admission              31 °C            Pression diff.        1.8 kPa
                                        Temp. FAP              310 °C

TURBO                                   ÉLECTRIQUE
Pression             0.75 bar           Batterie               14.2 V

CARBURANT
Niveau                  58 %            Autonomie               610 km
Conso instant.      6.7 L/100           Moyenne             7.1 L/100
```

Règles :

- pas de titre `VÉHICULE` ;
- heure en haut à droite ;
- température extérieure en haut à gauche ;
- groupes MOTEUR / FAP / TURBO / ÉLECTRIQUE / CARBURANT ;
- valeur absente = `--`, jamais `0` par défaut ;
- régénération FAP active prévue comme état visuellement accentué ;
- scène fixe 1280×720 redimensionnée proportionnellement, comme les dashboards natifs LIVI ;
- interface destinée à rester lisible rapidement dans un véhicule.

## Champs télémétrie utilisés

Champs LIVI existants :

```text
ambientC
coolantC
oilC
iatC
boostKpa
batteryV
fuelPct
rangeKm
consumptionLPer100Km
consumptionAvgLPer100Km
```

Champs Espace IV expérimentaux utilisés pour le prototype :

```text
dpfLoadPct
dpfRegenActive
dpfDifferentialKpa
dpfTempC
```

LIVI autorise actuellement les extensions de payload via son point d'extension `[key: string]: unknown`, ce qui permet le prototypage. Lors de l'intégration CAN définitive, ces champs devront idéalement être ajoutés proprement au contrat `TelemetryPayload` et au routage télémétrie.

## Chaîne télémétrie validée

```text
scripts/tools/telemetry-sim.ts
        ↓ Socket.IO telemetry:push
LIVI : http://127.0.0.1:4000
        ↓
TelemetryStore
        ↓
useVehicleTelemetry
        ↓
Dash1 / Dash2 / Dash4
```

La chaîne a été validée avec des valeurs simulées : vitesse 87 km/h, régime 2200 rpm, rapport 6, température extérieure 16.5 °C, huile 96 °C, carburant 58 %, etc.

Le profil de test reproductible est sauvegardé dans :

```text
scripts/livi/telemetry-espace-demo.sh
```

Le but est qu'un futur service CAN remplace simplement le simulateur comme producteur des mêmes champs, sans modifier Dash4.

Ne jamais laisser le simulateur et le futur service CAN réel publier simultanément les mêmes valeurs en production.

## CSS externe

Le style est chargé depuis le dossier de données utilisateur LIVI :

```text
~/.config/LIVI/custom/espace-ui.css
```

Une copie de référence est sauvegardée dans le dépôt :

```text
config/livi/espace-ui.css
```

Le renderer demande le CSS via un IPC limité :

```text
renderer -> preload -> main process -> lecture du fichier CSS -> injection <style>
```

Le renderer n'obtient pas un accès générique au système de fichiers.

Les paramètres modifiables sans recompilation incluent notamment :

- famille de police ;
- taille de l'heure et de la date d'attente ;
- position de l'overlay ;
- tailles des valeurs Dash4 ;
- tailles des libellés et unités ;
- espacements ;
- contraste ;
- lignes de séparation ;
- couleur d'alerte.

Après une modification du CSS, un redémarrage de LIVI suffit.

## Police

Inter n'était pas initialement installée : `fc-match Inter` retombait sur DejaVu Sans.

Après installation :

```text
Inter-Regular.otf: "Inter" "Regular"
```

Le CSS utilise :

```css
--espace-font-family: "Inter", "Roboto", sans-serif;
```

## Fichiers LIVI modifiés dans le checkout de travail

Checkout :

```text
/home/pi/LIVI-dev
```

Fichiers source concernés :

```text
src/renderer/src/components/pages/projection/Projection.tsx
src/renderer/src/components/pages/telemetry/dashboards/dash4/Dash4.tsx
src/main/ipc/settings.ts
src/preload/index.ts
src/renderer/src/env.d.ts
src/renderer/src/main.tsx
```

Fichier externe :

```text
~/.config/LIVI/custom/espace-ui.css
```

Les fichiers `.wrap` générés sous `native/livi-compositor/subprojects/` sont des artefacts de build et ne font pas partie de la modification à archiver.

## Validations réalisées

Les commandes suivantes ont réussi après les modifications :

```bash
git diff --check
pnpm run typecheck
pnpm run build:app
pnpm run build:linux:arm64
```

`pnpm run typecheck` :

```text
typecheck:node OK
typecheck:web  OK
```

`pnpm run build:app` :

```text
renderer : OK
main     : OK
preload  : OK
```

L'avertissement Vite concernant le futur `configLoader: native` est non bloquant et n'a pas empêché le build.

L'AppImage de test est :

```text
/home/pi/LIVI-dev/dist/LIVI-8.3.0-linux-arm64.AppImage
```

La version stable d'origine doit rester conservée tant que toute l'intégration n'est pas terminée :

```text
/home/pi/LIVI/LIVI.AppImage
```

## Autostart pendant la phase de test

Pendant les tests de la build personnalisée, l'autostart stable avait été neutralisé en renommant :

```text
~/.config/autostart/LIVI.desktop
```

en :

```text
~/.config/autostart/LIVI.desktop.disabled
```

À la mise en service, il faudra soit restaurer l'autostart stable, soit le faire pointer volontairement vers la build personnalisée validée.

## Dash5

Une page diagnostic CAN supplémentaire a été envisagée, mais la décision actuelle est :

```text
NE PAS ajouter Dash5 pour le moment.
```

LIVI v8.3.0 est actuellement structuré autour de dash1..dash4. On conserve donc les quatre pages et on valide d'abord le résultat actuel.

## Mise en service Wi-Fi — note pour plus tard

À traiter seulement en phase de mise en service :

- mémoriser plusieurs réseaux Wi-Fi avec NetworkManager ;
- définir des priorités d'autoconnexion ;
- privilégier un SSID dédié au Raspberry CarPlay plutôt que le Wi-Fi principal ;
- isoler ce SSID dans un VLAN limité ;
- conserver les profils NetworkManager avec permissions root-only ;
- ne pas compter sur ces permissions comme protection contre un attaquant possédant physiquement la microSD non chiffrée.

Ce point est volontairement reporté.

## Restant avant archivage final de la build

- exporter mécaniquement le `git diff` exact du checkout `/home/pi/LIVI-dev` contre v8.3.0 ;
- remplacer l'ancien patch `patches/livi/v8.3.0-home-clock.patch`, désormais obsolète ;
- vérifier l'application du nouveau patch sur un worktree propre v8.3.0 ;
- enregistrer taille et SHA256 de l'AppImage personnalisée finale ;
- valider avec MFi+iPhone que l'overlay heure/date disparaît bien à l'apparition de CarPlay.

