/* ------------------------------- */
/*    Imports des bibliothèques    */
/* ------------------------------- */
#include "Adafruit_ILI9341.h"
#include "Adafruit_GFX.h"
#include "SPI.h"
#include "math.h"

/* ------------------------------- */
/*  */
/* ------------------------------- */
#define TFT_DC 9
#define TFT_CS 10

/* ------------------------------- */
/* Constante contenant le code héxadécimal des couleurs */
/* Convertion ds couleurs en code RGB565 sur: http://www.barth-dev.de/online/rgb565-color-picker/ */
/* ------------------------------- */
#define BLACK     0x0000
#define BLUE      0x001F
#define RED       0xF800
#define GREEN     0x07E0
#define CYAN      0x07FF
#define MAGENTA   0xF81F
#define YELLOW    0xFFE0
#define WHITE     0xFFFF
#define BROWN     0x8200
#define GRAY      0x31A6
#define LIGHTGRAY 0xB596

/* ------------------------------- */
/* Different type d'orientation de l'écran */
/* ------------------------------- */
#define PORTRAIT 0
#define LANDSCAPE 1

/* ------------------------------- */
/* Calcule du nombre de ligne d'un tableau */
/* ------------------------------- */
#define ROW_COUNT(array)    (sizeof(array) / sizeof(*array))
/* Calcule du nombre de colonne d'un tableau */
#define COLUMN_COUNT(array) (sizeof(array) / (sizeof(**array) * ROW_COUNT(array)))

/* ------------------------------- */
/* Taille d'une case en pixel sur l'écran */
/* ------------------------------- */
#define BOX 20

/* ------------------------------- */
/* Longueur en pixel de l'écran */
/* ------------------------------- */
#define SCREENWIDTH 320 // Equivalent à tft.width()
/* Largeur en pixel de l'écran */
#define SCREENHEIGHT 240 // Equivalent à tft.height()

/* ------------------------------- */
/* Position en abscice (x) de départ du Pac-Man */
#define FIRSTX 3
/* Position en ordonnée (y) de départ du Pac-Man */
#define FIRSTY 1

/* ------------------------------- */
/* Position en abscice (x) de départ du Fantome */
#define GHOSTX 4
/* Position en ordonnée (y) de départ du Fantome */
#define GHOSTY 1

/* ------------------------------- */
/* Définition du Pac-Man */
/* ------------------------------- */
struct Entity {
  char type[10]; // pacman ou ghost
  double actuX; // Coordonnée X actuelles
  double actuY; // Coordonnée Y actuelles
  double prevX; // Coordonnées X anciennes
  double prevY; // Coordonnée Y anciennes
  char direc; // Direction HAUT = H, BAS = B, GAUCHE = G, Droite = D
};

/* ------------------------------- */
/* Nombre de vie du Pac-Man */
/* ------------------------------- */
int lifepoint = 3;

/* ------------------------------- */
/* Définition d'une partie */
/* ------------------------------- */
struct Party {
  int partyTime; // Temps depuis le début de la party
  int partyScore; // Score totale recolté
};

/* ------------------------------- */
/*  */
/* ------------------------------- */
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);


/* ------------------------------- */
/* Terrain de jeu */
/* ------------------------------- */
char ground[13][19] = {
  {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'x', 'o', 'x'},
  {'x', 'o', 'x', 'x', 'o', 'x', 'o', 'x', 'x', 'x', 'o', 'o', 'o', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'x', 'x', 'x', 'x', 'o', 'x', 'o', 'x'},
  {'x', 'x', 'o', 'x', 'o', 'o', 'o', 'x', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'x', 'x', 'o', 'x', 'o', 'x', 'o', 'x', 'o', 'o', 'x'},
  {'x', 'o', 'x', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'x', 'o', 'x', 'x', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'x', 'x', 'o', 'x', 'o', 'x', 'o', 'x', 'o', 'o', 'x'},
  {'x', 'x', 'o', 'x', 'o', 'o', 'o', 'x', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'x', 'x', 'x', 'x', 'o', 'x', 'o', 'x'},
  //{'x', 'o', 'x', 'x', 'o', 'x', 'o', 'x', 'x', 'x', 'o', 'o', 'o', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'o', 'o', 'o', 'x', 'o', 'o', 'x', 'o', 'x'},
  {'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x', 'x'},
};

/*char ground[5][5] = {
  {'x', 'x', 'x', 'x', 'x'},
  {'x', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'x'},
  {'x', 'o', 'o', 'o', 'x'},
  {'x', 'x', 'x', 'x', 'x'},
};*/

/* ------------------------------- */
/* Initialisation du Pac-Man */
/* ------------------------------- */
struct Entity pacman = {
  "pacman",
  FIRSTX, FIRSTY,
  // Position hors du terrain car il n'a pas encore bougé 
  -FIRSTX, -FIRSTY, 
  'D'
};

/* ------------------------------- */
/* Initialisation du Fantome */
/* ------------------------------- */
struct Entity ghost = {
  "ghost",
  GHOSTX, GHOSTY,
  // Position hors du terrain car il n'a pas encore bougé 
  -GHOSTX, -GHOSTY, 
  'D'
};

/* ------------------------------- */
/* Initialisation de la party */
/* ------------------------------- */
struct Party party = {
  0,
  0
};


/* ------------------------------- */
/* Coordonnées pour faire les designs graphiques */
/* ------------------------------- */
  
  /* ------------------------------- */
  /* Coordonnées du PacMan pour l'affichage */
  /* ------------------------------- */
  int r = 7 ;
  double a = PI / 4;
  int x1, x2, y1, y2, xR, yR;
  
  /* ------------------------------- */
  /* Coordonnées des Bricks pour l'affichage */
  /* ------------------------------- */
  int wightBrick1 = 9;
  int wightBrick2 = 5;
  int wightBrick3 = 1;
  int heightBrick = 4;

  /* ------------------------------- */
  /* Coordonnées pour les boules de score */
  /* ------------------------------- */
  int rScoring = 3;

/* ------------------------------- */
/* Affiche une brick du terrain */
/* ------------------------------- */
void display_brick(int x, int y, int w, int h, int color) {  
  tft.fillRect(x, y, w, h, BLACK);
  
  tft.fillRect(x, y, wightBrick1, heightBrick, color);
  tft.fillRect(x+10, y, wightBrick1, heightBrick, color);
  tft.fillRect(x+BOX, y, wightBrick3, heightBrick, color);
  tft.fillRect(x, y+5, wightBrick2, heightBrick, color);
  tft.fillRect(x+6, y+5, wightBrick1, heightBrick, color);
  tft.fillRect(x+16, y+5, wightBrick2, heightBrick, color);
  tft.fillRect(x, y+10, wightBrick1, heightBrick, color);
  tft.fillRect(x+10, y+10, wightBrick1, heightBrick, color);
  tft.fillRect(x+10, y+10, wightBrick3, heightBrick, color);
  tft.fillRect(x, y+15, wightBrick2, heightBrick, color);
  tft.fillRect(x+6, y+15, wightBrick1, heightBrick, color);
  tft.fillRect(x+16, y+15, wightBrick2, heightBrick, color);
}

/* ------------------------------- */
/* Affiche le menu en haut de l'écran */
/* ------------------------------- */
void display_topMenu() {
  tft.fillRect(0, 0, SCREENWIDTH, BOX, LIGHTGRAY);
}

/* ------------------------------- */
/* Affiche les différentes informations du menu en haut de l'écran */
/* ------------------------------- */
void display_topMenuLabel() {
  // Nombre de vies
  tft.setCursor(5, BOX/3);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("LIFE ");

  // Score 
  tft.setCursor((SCREENWIDTH/2)-BOX*3, BOX/3);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("SCORE ");

  // Temps 
  tft.setCursor(SCREENWIDTH-BOX*5, BOX/3);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print("TIME ");
}

/* ------------------------------- */
/* Affiche le nombre de vies restantes */
/* ------------------------------- */
void display_topMenuLife(Entity pacman) {
  pacman.actuX = 2;
  pacman.actuY = -0.5;
  display_pacman(pacman, LIGHTGRAY);
  pacman.actuX = 3;
  display_pacman(pacman, LIGHTGRAY);
  pacman.actuX = 4;
  display_pacman(pacman, LIGHTGRAY);
}

/* ------------------------------- */
/* Affiche le score de la party */
/* ------------------------------- */
void display_topMenuScore() {
  tft.setCursor(SCREENWIDTH/2, BOX/3);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print(party.partyScore);
}

/* ------------------------------- */
/* Affiche le temps de la party */
/* ------------------------------- */
void display_topMenuTime() {
  tft.setCursor(SCREENWIDTH-BOX*2, BOX/3);
  tft.setTextColor(BLACK);
  tft.setTextSize(1);
  tft.print(party.partyTime);
}

/* ------------------------------- */
/* Affiche le Pac-Man */
/* ------------------------------- */
void display_pacman(Entity &pacman, int backgroundColor) {
  int x = (pacman.actuX*BOX);
  int y = (pacman.actuY*BOX)+BOX;
  int x1 = x + r * cos(a);
  int x2 = x + r * cos(a);
  int y1 = y - r * sin(a);
  int y2 = y + r * sin(a);
  int w = (x + r) - x1;
  int h = y2 - y1 + 2;
  if (pacman.direc == 'D') {
    x1 = x + r;
    x2 = x + r;
    y1 = y - r;
    y2 = y + r;
  } else if (pacman.direc == 'G') {
    x1 = x - r;
    x2 = x - r;
    y1 = y - r;
    y2 = y + r;
  } else if (pacman.direc == 'H') {
    x1 = x + r;
    x2 = x - r;
    y1 = y - r;
    y2 = y - r;
  } else if (pacman.direc == 'B') {
    x1 = x - r;
    x2 = x + r;
    y1 = y + r;
    y2 = y + r;
  }
  tft.fillCircle(x, y, r, YELLOW);
  tft.fillTriangle(x, y, x1, y1, x2, y2, backgroundColor);
}

/* ------------------------------- */
/* Affiche un Fantome */
/* ------------------------------- */
void display_ghost(int x, int y) {
  tft.fillCircle(x, y, 7, RED);
}

/* ------------------------------- */
/* Affiche un carré pour caché le Pac-Man */
/* ------------------------------- */
void display_brick(int x, int y, int w, int l) {
  tft.fillRect(x, y, w, l, GRAY);
}

/* ------------------------------- */
/* Affiche une boule de score */
/* ------------------------------- */
void display_scoringBalls(int x, int y, int r) {
  tft.fillCircle(x, y, r, BLUE);
}

/* ------------------------------- */
/* Affiche un boule d'invinsibilité */
/* ------------------------------- */
void display_invinsibilityBalls(int x, int y) {
  int rayon = BOX/2;
  
  int x1 = x + rayon*cos(PI/3);
  int x2 = x + rayon*cos(PI/4);
  int x3 = x + rayon*cos(PI/6);
  int x4 = x - rayon*cos(PI/3);
  int x5 = x - rayon*cos(PI/4);
  int x6 = x - rayon*cos(PI/6);

  int y1 = x + rayon*sin(PI/3);
  int y2 = x + rayon*sin(PI/4);
  int y3 = x + rayon*sin(PI/6);
  int y4 = x - rayon*sin(PI/3);
  int y5 = x - rayon*sin(PI/4);
  int y6 = x - rayon*sin(PI/6);
  
  tft.fillRect(x, y, BOX, BOX/4, MAGENTA);
  tft.fillRect(x, y+(BOX/4), BOX, BOX/4, RED);
  tft.fillRect(x, y+(BOX/2), BOX, BOX/4, GREEN);
  tft.fillRect(x, y+((BOX*3)/4), BOX, BOX/4, BLUE);
}


/* ------------------------------- */
/* Retourne vrai si il y a une collision avec le terrain */
/* ------------------------------- */
bool occuped(int x, int y) {
  if (ground[y][x] == 'x') {
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Retourne vrai si il y a une collision avec un collectible */
/* ------------------------------- */
bool occuped(int x, int y) {
  if (ground[y][x] == 'x') {
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Retourne vrai si il y a une collision entre deux Entité */
/* ------------------------------- */
bool entityCollision(Entity pacman, Entity ghost) {
  if ((pacman.prevX == ghost.prevX && pacman.prevY == ghost.prevY) || 
      (pacman.actuX == ghost.prevX && pacman.actuY == ghost.prevY)|| 
      (pacman.prevX == ghost.actuX && pacman.prevY == ghost.actuY)) {
    return true;
  } else {
    return false;
  }
}

/* ------------------------------- */
/* Affiche le Pac-Man à la bonne position suivant les collisions */
/* ------------------------------- */
// Affiche le PacMan à la bonne position s'il n'y a pas de collision
void applyOccuped(Entity &pacman, Entity &ghost) {

  // Si il y a une collision entre un Pac-Man et un Fantome, place le Pac-Man à sa position de départ
  if (entityCollision(pacman, ghost)) {
    pacman.actuX = FIRSTX;
    pacman.actuY = FIRSTY;
  } 
  
  // Si il y a une collision avec le terrain, le Pac-Man retoure à la position précédente
  if (occuped(pacman.actuX, pacman.actuY)) { 
    pacman.actuY = pacman.prevY;
    pacman.actuX = pacman.prevX;
  } else { 
    if ((pacman.actuX != pacman.prevX || pacman.actuY != pacman.prevY)) {
      // Suppression de l'affichage du pacman à son ancienne position
      display_brick(pacman.prevX*BOX-(BOX/2), pacman.prevY*BOX+(BOX/2), BOX, BOX);
      // Ajout de l'affichage du pacman à sa nouvelle position
      display_pacman(pacman, GRAY);
    } 
  }
  // Si il y a une collision avec le terrain, le Fantome retoure à la position précédente
  if (occuped(ghost.actuX, ghost.actuY)) { 
    ghost.actuY = ghost.prevY;
    ghost.actuX = ghost.prevX;
  } else { 
    if ((ghost.actuX != ghost.prevX || ghost.actuY != ghost.prevY)) {
      // Suppression de l'affichage du pacman à son ancienne position
      display_brick(ghost.prevX*BOX-(BOX/2), ghost.prevY*BOX+(BOX/2), BOX, BOX);
      // Ajout de l'affichage du pacman à sa nouvelle position
      display_ghost(ghost.actuX*BOX, ghost.actuY*BOX+BOX);
    } 
  }
}

/* ------------------------------- */
/* Modifie la position et l'orientation du Pac-Man suivant les directions */
/* ------------------------------- */
// Modifie la position du PacMan suivant la position du joystick
void testInput(Entity &pacman, Entity &ghost) {
  // Modifie la position et la direction du pacman suivant l'orientation du joystick
  pacman.prevY = pacman.actuY;
  pacman.prevX = pacman.actuX;
  if (digitalRead(31) || (digitalRead(31) && digitalRead(28)) || (digitalRead(31) && digitalRead(29))) {
    pacman.direc = 'G';
    pacman.actuX--;
    delay(100);
  } else if (digitalRead(30) || (digitalRead(30) && digitalRead(28)) || (digitalRead(30) && digitalRead(29))) {
    pacman.direc = 'D';
    pacman.actuX++;
    delay(100);
  } else if (digitalRead(29)) {
    pacman.direc = 'H';
    pacman.actuY--;
    delay(100);
  } else if (digitalRead(28)) {
    pacman.direc = 'B';
    pacman.actuY++;
    delay(100);
  }

  // Modifie la position et la direction du ghost suivant les bouton appuyés
  ghost.prevY = ghost.actuY;
  ghost.prevX = ghost.actuX;
  if (digitalRead(25)) {
    ghost.direc = 'G';
    ghost.actuX--;
    delay(100);
  } else if (digitalRead(22)) {
    ghost.direc = 'D';
    ghost.actuX++;
    delay(100);
  } else if (digitalRead(23)) {
    ghost.direc = 'H';
    ghost.actuY--;
    delay(100);
  } else if (digitalRead(24)) {
    ghost.direc = 'B';
    ghost.actuY++;
    delay(100);
  }
}

/* ------------------------------- */
/* Affiche le terrain suivant le tableau */
/* ------------------------------- */
void display_ground() {
  // Vrai si on sort par la droite
  bool exceedRight = false;
  // vrai si on sort par le bas
  bool exceedBottom = false;
  // Affiche le terrain sans sortir des limites de l'afficheur
  for (int i = 0; i < ROW_COUNT(ground) && exceedBottom == false; i++) {
    exceedRight = false;
    /*if (BOX*(i+1) >= SCREENHEIGHT)
      exceedBottom = true;*/
    for (int j = 0; j < COLUMN_COUNT(ground) && exceedRight == false; j++) {
      if (ground[i][j] == 'x') 
        display_brick(j*BOX-(BOX/2), i*BOX+(BOX/2), BOX, BOX, BROWN); 
      else if (ground[i][j] == 'o' && )
        display_scoringBalls(j*BOX, i*BOX, rScoring);
      /*if (BOX*(j+1) >= SCREENWIDTH)
        exceedRight = true;*/
    }
  }
}

/* ------------------------------- */
/* Setup */
/* ------------------------------- */
void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.fillScreen(GRAY);
  tft.setRotation(LANDSCAPE);
  pinMode(28, INPUT); pinMode(24, INPUT); // Bas
  pinMode(29, INPUT); pinMode(23, INPUT); // Haut
  pinMode(30, INPUT); pinMode(22, INPUT); // Droite
  pinMode(31, INPUT); pinMode(25, INPUT); // Gauche
  display_ground();
  display_topMenu();
  display_topMenuLabel();
  display_topMenuLife(pacman);
  //display_invinsibilityBalls();
}

/* ------------------------------- */
/* Loop */
/* ------------------------------- */
void loop() {
  display_topMenuTime();
  display_topMenuScore();
  applyOccuped(pacman, ghost);
  testInput(pacman, ghost);
}

// tft.dear(COLOR) efface l'écran avec la couleur voulue
// tft.RGB(r, g, b) retourne une couleur au format RGB
// tft.drawPixel(x, y, COLOR) affiche un pixel
