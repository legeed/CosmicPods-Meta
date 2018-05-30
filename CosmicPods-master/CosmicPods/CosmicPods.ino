/*
  CosmicPods Meta
  
  CosmicPods Meta is a small shmup for GB-Meta 
  It's a port of CosmicPods for Arduboy created by cubic9com 
  <https://twitter.com/cubic9com>
  <https://github.com/ArduboyCollection/CosmicPods<
  Thanks to him !
  
  This code is licensed under the BSD 3-Clause license.
*/

#include <Gamebuino-Meta.h>

#define CHAR_WIDTH 3
#define CHAR_HEIGHT 5

#define X_MAX (WIDTH - (CHAR_WIDTH * 3) + 1)
#define Y_MAX (HEIGHT - CHAR_HEIGHT)

#define NUM_PLAYER_BULLETS 4
#define MAX_NUM_ENEMIES 8
#define NUM_STARS 32

#define ENEMY_RECT {0, 0, 12, 12}
#define PLAYER_RECT {0, 1, 16, 6}
uint8_t STAR_COLORS [] = {1, 6, 7, 12, 13};

struct Rect {
  int16_t x;      
  int16_t y;      
  uint8_t w;      
  uint8_t h;      
};

struct Point {
  int16_t x;      
  int16_t y;      
};

struct Vector2 {
  float x;
  float y;
};

struct FloatPoint {
  float x;
  float y;
};

struct PlayerBullet {
  Point point;
  boolean enabled;
};

struct Player {
  Rect rect;
  PlayerBullet bullets[NUM_PLAYER_BULLETS];
};

struct EnemyBullet {
  FloatPoint point;
  Vector2 delta;
  boolean enabled;
};

struct Enemy {
  Rect rect;
  EnemyBullet bullet;
};

struct Star {
  Point point;
  byte speed;
  byte color;
};

struct Player player;
struct Star stars[NUM_STARS];
struct Enemy enemies[MAX_NUM_ENEMIES];

boolean last_a_button_val;
boolean last_b_button_val;
boolean is_gameover;
boolean is_title;
unsigned int score;
unsigned int best_score;
boolean is_highscore;
byte num_enemies;
byte bullet_speed_factor;
byte level;
byte WIDTH;
byte HEIGHT;

const uint8_t cosmic_podData[] = {
12,12,
2,0,8,
14,
1,

0xEE,0xEE,0x11,0x11,0x1E,0xEE,
0xEE,0xE1,0x1C,0xCC,0x11,0xE1,
0xEE,0x11,0x00,0x0C,0xD1,0x18,
0xEE,0x1C,0x77,0x0C,0xDD,0x1A,
0xEE,0x1C,0x77,0xCD,0xDD,0x18,
0x11,0x1C,0x07,0xDD,0xDD,0x1A,
0x1C,0xDC,0xCC,0xDD,0xD1,0x18,
0x11,0x1D,0xDD,0xDD,0x11,0xE1,
0xEE,0xE1,0x11,0x11,0x11,0xEE,
0xEE,0xE1,0xEE,0xDE,0xE1,0xEE,
0xEE,0xED,0xEE,0xED,0xED,0xEE,
0xEE,0xEE,0xDE,0xED,0xEE,0xDE,
0xEE,0xEE,0x11,0x11,0x1E,0xEE,
0xEE,0xE1,0x1C,0xCC,0x11,0xE1,
0xEE,0x11,0x00,0x0C,0xD1,0x1A,
0xEE,0x1C,0x77,0x0C,0xDD,0x18,
0xEE,0x1C,0x77,0xCD,0xDD,0x1A,
0x11,0x1C,0x07,0xDD,0xDD,0x18,
0x1C,0xDC,0xCC,0xDD,0xD1,0x1A,
0x11,0x1D,0xDD,0xDD,0x11,0xE1,
0xEE,0xE1,0x11,0x11,0x11,0xEE,
0xEE,0xE1,0xEE,0xDE,0xE1,0xEE,
0xEE,0xED,0xEE,0xDE,0xED,0xEE,
0xEE,0xED,0xEE,0xED,0xED,0xEE
};
Image cosmic_pod(cosmic_podData);

const uint8_t cosmic_shipData[] = {
16,6,
2,0,8,
14,
1,

0xE7,0x7E,0xE8,0x77,0x7E,0xEE,0xEE,0xEE,
0x88,0x77,0x87,0x7E,0xE7,0x77,0xEE,0xEE,
0x9A,0x87,0x77,0x77,0x71,0xDC,0x78,0x88,
0xA9,0x87,0x77,0x77,0x71,0xDC,0x78,0x88,
0x88,0x77,0x87,0x7E,0xE7,0x77,0xEE,0xEE,
0xE7,0x7E,0xE8,0x77,0x7E,0xEE,0xEE,0xEE,
0xE7,0x7E,0xE8,0x77,0x7E,0xEE,0xEE,0xEE,
0x88,0x77,0x87,0x7E,0xE7,0x77,0xEE,0xEE,
0xA9,0x87,0x77,0x77,0x71,0xDC,0x78,0x88,
0x9A,0x87,0x77,0x77,0x71,0xDC,0x78,0x88,
0x88,0x77,0x87,0x7E,0xE7,0x77,0xEE,0xEE,
0xE7,0x7E,0xE8,0x77,0x7E,0xEE,0xEE,0xEE
};
Image cosmic_ship(cosmic_shipData);


void setup() {
  
  gb.begin();

  best_score = gb.save.get(0);

  WIDTH = gb.display.width();
  HEIGHT = gb.display.height();

  for (byte i = 0; i < MAX_NUM_ENEMIES; i++) {
    enemies[i].rect = ENEMY_RECT;
  }
  player.rect = PLAYER_RECT;

  beginGame();
}

void loop() {
  while(!gb.update());

  gb.display.clear();
  gb.lights.clear();

  if (is_title) {
    moveStars();
    drawStars();
    displayTitle();
    if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B)) {
      beginGame();
      is_title = false;
    }
    return;
  }
  
  if (is_gameover) {
    displayGameover();
    if (gb.buttons.pressed(BUTTON_A) || gb.buttons.pressed(BUTTON_B)) {
      beginGame();
    }
    return;
  }

  movePlayer();
  moveStars();
  moveEnemies();
  movePlayerBullets();
  moveEnemiesBullet();

  drawScore();
  drawPlayer();
  drawStars();
  drawEnemies();
  drawPlayerBullets();
  drawEnemiesBullet();

  checkEnemyCollision();
  checkPlayerCollision();
  
}

void beginGame() {
  initialize();
}

void initialize() {
  
  score = 0;

  player.rect.x = (WIDTH / 4) - (CHAR_WIDTH * 3 / 2);
  player.rect.y = (HEIGHT / 2) - (CHAR_HEIGHT / 2);

  for (byte i = 0; i < NUM_STARS; i++) {
    spawnStar(i);
  }

  for (byte i = 0; i < MAX_NUM_ENEMIES; i++) {
    spawnEnemy(i);
  }

  for (byte i = 0; i < NUM_PLAYER_BULLETS; i++) {
    player.bullets[i].enabled = false;
    player.bullets[i].point.x = 0;
    player.bullets[i].point.y = 0;
  }

  for (byte i = 0; i < MAX_NUM_ENEMIES; i++) {
    enemies[i].bullet.enabled = false;
    enemies[i].bullet.point.x = 0;
    enemies[i].bullet.point.y = 0;
  }

  is_gameover = false;
  is_title = true;
  is_highscore = false;

  shiftLevel();
}

void shiftLevel() {
  switch (score) {
    case 0:
      setLevel(1, 1, 1.4);
      break;
    case 1:
      setLevel(2, 2, 1.4);
      break;
    case 3:
      setLevel(3, 2, 1.4);
      break;
    case 7:
      setLevel(4, 3, 1.5);
      break;
    case 16:
      setLevel(5, 3, 1.6);
      break;
    case 25:
      setLevel(6, 4, 1.8);
      break;
    case 37:
      setLevel(7, 4, 2.0);
      break;
    case 49:
      setLevel(8, 5, 2.3);
      break;
    case 64:
      setLevel(9, 5, 2.4);
      break;
    case 79:
      setLevel(10, 6, 2.6);
      break;
    case 97:
      setLevel(11, 6, 2.8);
      break;
    case 115:
      setLevel(12, 7, 3.1);
      break;
    case 136:
      setLevel(13, 7, 3.4);
      break;
    case 157:
      setLevel(14, MAX_NUM_ENEMIES, 3.7);
      break;
    case 181:
      setLevel(15, MAX_NUM_ENEMIES, 4.1);
      break;
    default:
      break;
  }
}

void setLevel(byte lv, byte ne, float spd) {
  level = lv;
  num_enemies = ne;
  bullet_speed_factor = spd;
}

void spawnEnemy(byte i) {
  enemies[i].rect.x = random(WIDTH + 20, WIDTH * 2);
  enemies[i].rect.y = random(0, HEIGHT - 12);
}

void spawnStar(byte i) {
  stars[i].point.x = random(WIDTH, WIDTH * 2);
  stars[i].point.y = random(0, HEIGHT);
  stars[i].speed = random(1, 5);
  stars[i].color = STAR_COLORS[random(0,4)];
}

void spawnPlayerBullet(byte i) {
  player.bullets[i].enabled = true;
  player.bullets[i].point.x = player.rect.x + 16;
  player.bullets[i].point.y = player.rect.y + 3;
}

void spawnEnemyBullet(byte i) {
  enemies[i].bullet.enabled = true;
  enemies[i].bullet.point.x = enemies[i].rect.x - 4;
  enemies[i].bullet.point.y = enemies[i].rect.y + 4;

  Vector2 orig_delta;
  orig_delta.x = player.rect.x - enemies[i].rect.x;
  orig_delta.y = player.rect.y - enemies[i].rect.y;

  Vector2 new_delta = calcDelta(orig_delta);

  enemies[i].bullet.delta.x = new_delta.x;
  enemies[i].bullet.delta.y = new_delta.y;
}

Vector2 calcDelta(Vector2 v) {
  float mag = bullet_speed_factor / sqrt(v.x * v.x + v.y * v.y);

  Vector2 v2;
  v2.x = v.x * mag;
  v2.y = v.y * mag;

  return v2;
}

void movePlayer() {
  if (gb.buttons.repeat(BUTTON_RIGHT,1) && (player.rect.x < X_MAX)) {
    player.rect.x++;
  }

  if (gb.buttons.repeat(BUTTON_LEFT,1) && (player.rect.x > 0)) {
    player.rect.x--;
  }

  if (gb.buttons.repeat(BUTTON_UP,1) && (player.rect.y > 0)) {
    player.rect.y--;
  }

  if (gb.buttons.repeat(BUTTON_DOWN,1) && (player.rect.y < Y_MAX)) {
    player.rect.y++;
  }

  boolean is_just_press_a_button = gb.buttons.repeat(BUTTON_A,5) && !last_a_button_val;
  boolean is_just_press_b_button = gb.buttons.repeat(BUTTON_B,5) && !last_b_button_val;
  if (is_just_press_a_button || is_just_press_b_button) {
    for (byte i = 0; i < NUM_PLAYER_BULLETS; i++) {
      if (!player.bullets[i].enabled) {
        spawnPlayerBullet(i);
        break;
      }
    }
  }
  last_a_button_val = gb.buttons.repeat(BUTTON_A,5);
  last_b_button_val = gb.buttons.repeat(BUTTON_B,5);
}

void moveStars() {
  for (byte i = 0; i < NUM_STARS; i++) {
    stars[i].point.x -= stars[i].speed;
    if (stars[i].point.x < 0) {
      spawnStar(i);
    }
  }
}

void moveEnemies() {
  for (byte i = 0; i < num_enemies; i++) {
    enemies[i].rect.x--;

    if (
      (level > 2)
      && (enemies[i].rect.x > player.rect.x + 10)
      && (enemies[i].rect.x < WIDTH - 10)
      && (!enemies[i].bullet.enabled)
      && (random(0, 50) == 10)
    ) {
      spawnEnemyBullet(i);
    }

    if (enemies[i].rect.x <= 0) {
      spawnEnemy(i);
    }
  }
}

void movePlayerBullets() {
  for (byte i = 0; i < NUM_PLAYER_BULLETS; i++) {
    if (player.bullets[i].enabled) {
      player.bullets[i].point.x += 2;
      if (player.bullets[i].point.x > WIDTH) {
        player.bullets[i].enabled = false;
      }
    }
  }
}

void moveEnemiesBullet() {
  for (byte i = 0; i < num_enemies; i++) {
    if (enemies[i].bullet.enabled) {
      enemies[i].bullet.point.x += enemies[i].bullet.delta.x;
      enemies[i].bullet.point.y += enemies[i].bullet.delta.y;
      if (
        (enemies[i].bullet.point.x <= 0)
        || (enemies[i].bullet.point.y <= 0)
        || (enemies[i].bullet.point.y >= HEIGHT)
      ) {
        enemies[i].bullet.enabled = false;
      }
    }
  }
}

boolean collidePointRect(Point test_point, Rect test_rect){
  if ( (test_point.x < test_rect.x) || (test_point.x > (test_rect.x + test_rect.w - 1)) || (test_point.y < test_rect.y) || (test_point.y > (test_rect.y + test_rect.h -1)) ) {
    return false;
  } else {
    return true;
  }
}

boolean collideRectRect(Rect test_rect_A, Rect test_rect_B){
     if((test_rect_B.x >= test_rect_A.x + test_rect_A.w)
        || (test_rect_B.x + test_rect_B.w <= test_rect_A.x) 
        || (test_rect_B.y >= test_rect_A.y + test_rect_A.h)
        || (test_rect_B.y + test_rect_B.h <= test_rect_A.y)) {
          return false; 
        } else {
          return true; 
        }
}

void checkEnemyCollision() {
  for (byte j = 0; j < num_enemies; j++) {
    boolean is_col = false;
    for (byte i = 0; i < NUM_PLAYER_BULLETS; i++) {
      if (player.bullets[i].enabled) {
        is_col = collidePointRect(player.bullets[i].point, enemies[j].rect);
        if (is_col) {
          player.bullets[i].enabled = false;
          break;
        }
      }
    }
    if (is_col) {
      score++;
      shiftLevel();
      gb.display.setColor(random(8,11));
      gb.display.fillCircle(enemies[j].rect.x + 3, enemies[j].rect.y + 6, 6);
      gb.display.setColor(random(8,11));
      gb.display.fillCircle(enemies[j].rect.x + 7, enemies[j].rect.y + 2, 3);
      gb.lights.drawPixel(0, 2, ORANGE);
      gb.lights.drawPixel(1, 2, ORANGE);
      gb.lights.drawPixel(0, 3, YELLOW);
      gb.lights.drawPixel(1, 3, YELLOW);
      spawnEnemy(j);
    }
  }
}

void checkPlayerCollision() {
  for (byte i = 0; i < num_enemies; i++) {
    Point tmp_point;
    tmp_point.x = enemies[i].bullet.point.x;
    tmp_point.y = enemies[i].bullet.point.y;
    boolean is_enemy_col = collideRectRect(enemies[i].rect, player.rect);
    boolean is_bullet_col = (enemies[i].bullet.enabled) && (collidePointRect(tmp_point, player.rect));
    if ((is_enemy_col) || (is_bullet_col)) {
      if (score > best_score) { 
        gb.save.set(0, score);
        best_score = score;
        is_highscore = true;
        } else {
          is_highscore = false;
        }
      displayGameover();
      return;
    }
  }
}

void displayGameover() {
  for (byte i = 0; i <= 3; i++) {
    gb.lights.drawPixel(0, i, RED);
    gb.lights.drawPixel(1, i, RED);
  }
  drawScore();
  gb.display.setColor(WHITE);
  gb.display.setCursor((WIDTH / 2) - (CHAR_WIDTH * 12 / 2), (HEIGHT / 2) - (CHAR_HEIGHT / 2));
  gb.display.print("GAME OVER");
  if (is_highscore) {
     gb.display.setColor(RED);
     gb.display.setCursor((WIDTH / 2) - (CHAR_WIDTH * 18 / 2), (HEIGHT / 2) - (CHAR_HEIGHT / 2) + (CHAR_HEIGHT + 1));
     gb.display.print("NEW HIGH SCORE");
  }
  is_gameover = true;
}

void displayTitle() {
  gb.display.setColor(WHITE);
  gb.display.setCursor((WIDTH / 2) - (CHAR_WIDTH * 14 / 2), (HEIGHT / 2) - (CHAR_HEIGHT / 2) - (CHAR_HEIGHT + 1) );
  gb.display.println("Cosmic Pods");
  gb.display.println("                  ");
  gb.display.print("   Highscore : ");
  gb.display.println(best_score);
  gb.display.drawImage(10, 12, cosmic_ship);
  gb.display.drawImage(64, 45, cosmic_pod);
}

void drawScore() {
 gb.display.setColor(GRAY);
 gb.display.setCursor(1, HEIGHT - 8);
 gb.display.print("SCORE:");
 gb.display.setCursor(30, HEIGHT - 8);
 gb.display.print(score);
}

void drawPlayer() {
  byte x = player.rect.x;
  byte y = player.rect.y;
  byte w = player.rect.w;
  byte h = player.rect.h;
  /* HITBOX
  gb.display.setColor(WHITE); 
  gb.display.drawRect(x, y, w, h);*/
  gb.display.drawImage(x, y, cosmic_ship);
}

void drawStars() {
  for (byte i = 0; i < NUM_STARS; i++) {
    byte x = stars[i].point.x;
    byte y = stars[i].point.y;
    gb.display.setColor(stars[i].color);
    gb.display.drawPixel(x, y);
  }
}

void drawEnemies() {
  for (byte i = 0; i < num_enemies; i++) {
    byte x = enemies[i].rect.x;
    byte y = enemies[i].rect.y;
    byte w = enemies[i].rect.w;
    byte h = enemies[i].rect.h;
    /* HITBOX
    gb.display.setColor(RED);
    gb.display.drawRect(x, y, w, h); */
    gb.display.drawImage(x, y, cosmic_pod);
  }
}

void drawPlayerBullets() {
  for (byte i = 0; i < NUM_PLAYER_BULLETS; i++) {
    if (player.bullets[i].enabled) {
      byte x = player.bullets[i].point.x;
      byte y = player.bullets[i].point.y;
      gb.display.setColor(YELLOW);
      gb.display.drawRect(x, y, 2, 2);
    }
  }
}

void drawEnemiesBullet() {
  byte i;
  int x, y;

  for (i = 0; i < num_enemies; i++) {
    if (enemies[i].bullet.enabled) {
      x = enemies[i].bullet.point.x;
      y = enemies[i].bullet.point.y;
      gb.display.setColor(PINK);
      gb.display.drawCircle(x, y, 2);
    }
  }
}
