// RP2040-Zero — test commande au volant Renault 7701049643
// Validé sur banc le 2026-09-08.
//
// Commande pin 1..6 -> RP2040-Zero GP0..GP5.
// Commande passive et déconnectée du faisceau OEM pendant ce test.

const uint8_t lignes[6] = {0, 1, 2, 3, 4, 5};

enum EtatVolume {
  VOL_REPOS,
  ATTENTE_PLUS,
  ATTENTE_MOINS,
  PLUS_EMIS,
  MOINS_EMIS,
  MUTE_VERROUILLE
};

EtatVolume etatVolume = VOL_REPOS;
unsigned long debutVolume = 0;

int valeurMolette = 0;
int anciennePositionMolette = -1;

uint16_t lireContacts() {
  uint16_t resultat = 0;
  uint8_t bit = 0;

  for (uint8_t source = 0; source < 6; source++) {
    for (uint8_t i = 0; i < 6; i++) {
      pinMode(lignes[i], INPUT_PULLUP);
    }

    digitalWrite(lignes[source], LOW);
    pinMode(lignes[source], OUTPUT);
    delayMicroseconds(10);

    for (uint8_t destination = source + 1;
         destination < 6;
         destination++) {
      if (digitalRead(lignes[destination]) == LOW) {
        resultat |= (1U << bit);
      }
      bit++;
    }

    pinMode(lignes[source], INPUT_PULLUP);
  }

  return resultat;
}

bool contactFerme(uint16_t etat, uint8_t a, uint8_t b) {
  if (a > b) {
    uint8_t temporaire = a;
    a = b;
    b = temporaire;
  }

  uint8_t bit = 0;

  for (uint8_t ligneA = 1; ligneA <= 6; ligneA++) {
    for (uint8_t ligneB = ligneA + 1;
         ligneB <= 6;
         ligneB++) {
      if (ligneA == a && ligneB == b) {
        return etat & (1U << bit);
      }
      bit++;
    }
  }

  return false;
}

void traiterVolume(uint16_t etat) {
  bool plus  = contactFerme(etat, 1, 4);
  bool moins = contactFerme(etat, 4, 6);

  if (etatVolume == MUTE_VERROUILLE) {
    if (!plus && !moins) {
      etatVolume = VOL_REPOS;
    }
    return;
  }

  if (plus && moins) {
    Serial.println("Bouton : MUTE");
    etatVolume = MUTE_VERROUILLE;
    return;
  }

  if (plus) {
    if (etatVolume != ATTENTE_PLUS && etatVolume != PLUS_EMIS) {
      etatVolume = ATTENTE_PLUS;
      debutVolume = millis();
    }

    if (etatVolume == ATTENTE_PLUS &&
        millis() - debutVolume >= 60) {
      Serial.println("Bouton : VOLUME +");
      etatVolume = PLUS_EMIS;
    }
    return;
  }

  if (moins) {
    if (etatVolume != ATTENTE_MOINS && etatVolume != MOINS_EMIS) {
      etatVolume = ATTENTE_MOINS;
      debutVolume = millis();
    }

    if (etatVolume == ATTENTE_MOINS &&
        millis() - debutVolume >= 60) {
      Serial.println("Bouton : VOLUME -");
      etatVolume = MOINS_EMIS;
    }
    return;
  }

  etatVolume = VOL_REPOS;
}

void traiterBoutons(uint16_t etat) {
  static bool ancienInferieur = false;
  static bool ancienneGauche = false;
  static bool ancienneDroite = false;

  bool inferieur = contactFerme(etat, 3, 4);
  bool gauche    = contactFerme(etat, 5, 6);
  bool droite    = contactFerme(etat, 3, 5);

  if (inferieur && !ancienInferieur) {
    Serial.println("Bouton : INFERIEUR");
  }
  if (gauche && !ancienneGauche) {
    Serial.println("Bouton : SOURCE GAUCHE");
  }
  if (droite && !ancienneDroite) {
    Serial.println("Bouton : SOURCE DROITE");
  }

  ancienInferieur = inferieur;
  ancienneGauche = gauche;
  ancienneDroite = droite;
}

int lirePositionMolette(uint16_t etat) {
  bool position6 = contactFerme(etat, 2, 6);
  bool position3 = contactFerme(etat, 2, 3);
  bool position1 = contactFerme(etat, 2, 1);

  uint8_t nombreContacts = position6 + position3 + position1;

  if (nombreContacts != 1) {
    return -1;
  }

  if (position6) return 0;
  if (position3) return 1;
  return 2;
}

void traiterMolette(uint16_t etat) {
  int position = lirePositionMolette(etat);

  if (position < 0) return;

  if (anciennePositionMolette < 0) {
    anciennePositionMolette = position;
    return;
  }

  if (position == anciennePositionMolette) return;

  int mouvement = (position - anciennePositionMolette + 3) % 3;

  if (mouvement == 1) {
    valeurMolette++;
    Serial.print("Molette + : ");
    Serial.println(valeurMolette);
  } else if (mouvement == 2) {
    valeurMolette--;
    Serial.print("Molette - : ");
    Serial.println(valeurMolette);
  }

  anciennePositionMolette = position;
}

void setup() {
  Serial.begin(115200);

  for (uint8_t i = 0; i < 6; i++) {
    pinMode(lignes[i], INPUT_PULLUP);
  }

  delay(1000);
  Serial.println("Test commande au volant pret");
}

void loop() {
  static uint16_t dernierBrut = 0;
  static uint16_t etatStable = 0;
  static unsigned long changement = 0;

  uint16_t etatBrut = lireContacts();

  if (etatBrut != dernierBrut) {
    dernierBrut = etatBrut;
    changement = millis();
  }

  if (etatBrut != etatStable &&
      millis() - changement >= 4) {
    etatStable = etatBrut;
  }

  traiterVolume(etatStable);
  traiterBoutons(etatStable);
  traiterMolette(etatStable);
}
