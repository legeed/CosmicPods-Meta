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
  byte type;
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

const uint8_t cosmic_pod_1Data[] = {
16,16,
2,0,8,
14,
1,

0xEE,0xEE,0xE0,0x00,0x00,0x0E,0xEE,0xEE,
0xEE,0xE0,0x0B,0xBB,0xBB,0xB0,0x0E,0xEE,
0xEE,0x03,0x33,0xBB,0xBB,0x33,0x30,0xEE,
0xE0,0x33,0x30,0x0B,0xB0,0x03,0x33,0x0E,
0x03,0x33,0x77,0x3B,0xB3,0x77,0x33,0x30,
0x0B,0x37,0x07,0x7B,0xB7,0x07,0x73,0xB0,
0x0B,0x37,0x07,0x73,0x37,0x07,0x73,0xB0,
0x0B,0x33,0x77,0x30,0x03,0x77,0x33,0xB0,
0x0B,0x33,0x03,0x0B,0xB0,0x30,0x33,0xB0,
0xE0,0xB0,0x30,0xB0,0x0B,0x03,0x0B,0x0E,
0xEE,0x00,0x30,0xB0,0x0B,0x03,0x00,0xEE,
0xEE,0xE0,0x30,0x0B,0xB0,0x03,0x0E,0xEE,
0xE0,0x03,0x0B,0xB0,0x0B,0xB0,0x30,0x0E,
0x03,0x30,0x0B,0x0E,0xE0,0xB0,0x03,0x30,
0xE0,0x0B,0xBB,0x0E,0xE0,0xBB,0xB0,0x0E,
0xEE,0xE0,0x00,0xEE,0xEE,0x00,0x0E,0xEE,
0xEE,0xEE,0xE0,0x00,0x00,0x0E,0xEE,0xEE,
0xEE,0xE0,0x0B,0xBB,0xBB,0xB0,0x0E,0xEE,
0xEE,0x03,0x33,0xBB,0xBB,0x33,0x30,0xEE,
0xE0,0x33,0x30,0x3B,0xB3,0x03,0x33,0x0E,
0x03,0x33,0x77,0x0B,0xB0,0x77,0x33,0x30,
0x0B,0x37,0x07,0x7B,0xB7,0x07,0x73,0xB0,
0x0B,0x37,0x07,0x73,0x37,0x07,0x73,0xB0,
0x0B,0x33,0x77,0x30,0x03,0x77,0x33,0xB0,
0x0B,0x33,0x03,0x0B,0xB0,0x30,0x33,0xB0,
0xE0,0xB0,0x30,0xB0,0x0B,0x03,0x0B,0x0E,
0xEE,0x00,0x30,0xB0,0x0B,0x03,0x00,0xEE,
0xEE,0xE0,0x30,0x0B,0xB0,0x03,0x0E,0xEE,
0xEE,0x03,0x0B,0xB0,0x0B,0xB0,0x30,0xEE,
0xE0,0x33,0x00,0x0E,0xE0,0x00,0x33,0x0E,
0xEE,0x00,0x0E,0xEE,0xEE,0xE0,0x00,0xEE,
0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE
};
Image cosmic_pod_1(cosmic_pod_1Data);

const uint8_t cosmic_pod_2Data[] = {
16,16,
2,0,8,
14,
1,

0xEE,0xEE,0xE0,0x00,0x00,0x0E,0xEE,0xEE,
0xEE,0xE0,0x08,0x88,0x88,0x80,0x0E,0xEE,
0xEE,0x02,0x22,0x88,0x88,0x22,0x20,0xEE,
0xE0,0x22,0x20,0x08,0x80,0x02,0x22,0x0E,
0x02,0x22,0x77,0x28,0x82,0x77,0x22,0x20,
0x08,0x27,0x07,0x78,0x87,0x07,0x72,0x80,
0x08,0x27,0x07,0x72,0x27,0x07,0x72,0x80,
0x08,0x22,0x77,0x20,0x02,0x77,0x22,0x80,
0x08,0x22,0x02,0x08,0x80,0x20,0x22,0x80,
0xE0,0x80,0x20,0x80,0x08,0x02,0x08,0x0E,
0xEE,0x00,0x20,0x80,0x08,0x02,0x00,0xEE,
0xEE,0xE0,0x20,0x08,0x80,0x02,0x0E,0xEE,
0xE0,0x02,0x08,0x80,0x08,0x80,0x20,0x0E,
0x02,0x20,0x08,0x0E,0xE0,0x80,0x02,0x20,
0xE0,0x08,0x88,0x0E,0xE0,0x88,0x80,0x0E,
0xEE,0xE0,0x00,0xEE,0xEE,0x00,0x0E,0xEE,
0xEE,0xEE,0xE0,0x00,0x00,0x0E,0xEE,0xEE,
0xEE,0xE0,0x08,0x88,0x88,0x80,0x0E,0xEE,
0xEE,0x02,0x22,0x88,0x88,0x22,0x20,0xEE,
0xE0,0x22,0x20,0x28,0x82,0x02,0x22,0x0E,
0x02,0x22,0x77,0x08,0x80,0x77,0x22,0x20,
0x08,0x27,0x07,0x78,0x87,0x07,0x72,0x80,
0x08,0x27,0x07,0x72,0x27,0x07,0x72,0x80,
0x08,0x22,0x77,0x20,0x02,0x77,0x22,0x80,
0x08,0x22,0x02,0x08,0x80,0x20,0x22,0x80,
0xE0,0x80,0x20,0x80,0x08,0x02,0x08,0x0E,
0xEE,0x00,0x20,0x80,0x08,0x02,0x00,0xEE,
0xEE,0xE0,0x20,0x08,0x80,0x02,0x0E,0xEE,
0xEE,0x02,0x08,0x80,0x08,0x80,0x20,0xEE,
0xE0,0x22,0x00,0x0E,0xE0,0x00,0x22,0x0E,
0xEE,0x00,0x0E,0xEE,0xEE,0xE0,0x00,0xEE,
0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE,0xEE
};
Image cosmic_pod_2(cosmic_pod_2Data);

const uint8_t cosmic_squidData[] = {
24,12,
4,0,8,
14,
1,

0xE1,0xDD,0xDE,0xEE,0xEE,0xED,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0xEE,0x11,0x1D,0xDD,0xEE,0xEE,0x00,0xD1,0xEE,0xE6,0xDE,0xEE,
0xEE,0xEE,0xE1,0x11,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xD1,0x66,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0x11,0xE1,0x11,0xD1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xEE,0xEE,0xEE,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0xEE,0xEE,0xEE,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0x11,0xE1,0x11,0xD1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xD1,0x66,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0xEE,0xEE,0xE1,0x11,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xEE,0x11,0x1D,0xDD,0xEE,0xEE,0x00,0xC1,0xEE,0xE6,0xDE,0xEE,
0xE1,0xDD,0xDE,0xEE,0xEE,0xE1,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0x1D,0xDD,0xDE,0xEE,0xEE,0xE1,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0xE1,0x11,0xDD,0xDE,0xEE,0xEE,0x00,0xC1,0xEE,0xE6,0xDE,0xEE,
0xEE,0xEE,0x11,0x1D,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xD1,0x66,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0x11,0xD1,0x11,0xDD,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xEE,0xEE,0xEE,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0x11,0xE1,0x11,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0xD1,0x66,0xEE,0xD1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xEE,0xEE,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0xEE,0xEE,0xE1,0x11,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xEE,0x11,0x1D,0xDD,0xEE,0xEE,0x00,0xD1,0xEE,0xE6,0xDE,0xEE,
0xE1,0xDD,0xDE,0xEE,0xEE,0xED,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0xE1,0xDD,0xDE,0xEE,0xEE,0xE1,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0xEE,0x11,0x1D,0xDD,0xEE,0xEE,0x00,0xC1,0xEE,0xE6,0xDE,0xEE,
0xEE,0xEE,0xE1,0x11,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xD1,0x66,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0x11,0xE1,0x11,0xD1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xEE,0xEE,0xEE,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0xEE,0xEE,0xEE,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0x11,0xE1,0x11,0xD1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xD1,0x66,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0xEE,0xEE,0xE1,0x11,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xEE,0x11,0x1D,0xDD,0xEE,0xEE,0x00,0xD1,0xEE,0xE6,0xDE,0xEE,
0xE1,0xDD,0xDE,0xEE,0xEE,0xED,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0xEE,0xEE,0xEE,0xEE,0xEE,0xE1,0xDD,0xEE,0xEE,0xED,0xEE,0xEE,
0xEE,0x1D,0xDD,0xDD,0xEE,0xEE,0x00,0xC1,0xEE,0xE6,0xDE,0xEE,
0xEE,0x11,0x11,0x11,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xEE,0xEE,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0xDD,0x66,0xEE,0xE1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0x1D,0xE1,0x11,0xD1,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0xEE,0xEE,0xEE,0x11,0xC5,0xDD,0xDC,0xC1,0x66,0xCC,0xCC,0x16,
0x11,0xE1,0x11,0xD1,0x1D,0x11,0x00,0xC1,0x6C,0xCC,0x11,0x6D,
0xDD,0x66,0xEE,0xEE,0x11,0x10,0xA8,0x06,0xCC,0x11,0x6D,0xDE,
0xEE,0xEE,0x11,0x1D,0xDD,0xD0,0xA8,0x06,0x11,0xE6,0xDD,0xEE,
0xEE,0xE1,0xDD,0xDE,0xEE,0xEE,0x00,0xD1,0xEE,0xE6,0xDE,0xEE,
0xEE,0xED,0xDD,0xEE,0xEE,0xED,0xDD,0xEE,0xEE,0xED,0xEE,0xEE
};
Image cosmic_squid(cosmic_squidData);

const Gamebuino_Meta::Sound_FX sfx_shoot[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,255,-15,25,24,8},
};

const Gamebuino_Meta::Sound_FX sfx_explosion[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,1,180,25,35,100,2},
  {Gamebuino_Meta::Sound_FX_Wave::NOISE,1,200,0,19,112,9},
  {Gamebuino_Meta::Sound_FX_Wave::NOISE,0,90,0,20,0,16},
};

const Gamebuino_Meta::Sound_FX sfx_blop[] = {
  {Gamebuino_Meta::Sound_FX_Wave::SQUARE,0,255,30,30,100,13},
};

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
  enemies[i].type = random(1,3);
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
        gb.sound.fx(sfx_shoot);
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
      gb.sound.fx(sfx_blop);
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
      gb.display.fillCircle(enemies[j].rect.x + 3, enemies[j].rect.y + 6, 7);
      gb.display.setColor(random(8,11));
      gb.display.fillCircle(enemies[j].rect.x + 7, enemies[j].rect.y + 2, 4);
      gb.lights.drawPixel(0, 1, RED);
      gb.lights.drawPixel(1, 1, RED);
      gb.lights.drawPixel(0, 2, ORANGE);
      gb.lights.drawPixel(1, 2, ORANGE);
      gb.lights.drawPixel(0, 3, YELLOW);
      gb.lights.drawPixel(1, 3, YELLOW);
      gb.sound.fx(sfx_explosion);
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
  gb.display.drawImage(8, 8, cosmic_squid);
  gb.display.drawImage(64, 45, cosmic_pod_1);
  gb.display.drawImage(58, 6, cosmic_pod_2);
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
  gb.display.drawImage(x-4, y-3, cosmic_squid);
  /*gb.display.setColor(WHITE); 
  gb.display.drawRect(x, y, w, h);*/
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
    switch (enemies[i].type) {
      case 1:
        gb.display.drawImage(x-2, y-2, cosmic_pod_1);
        break;
      case 2:
        gb.display.drawImage(x-2, y-2, cosmic_pod_2);
        break;
    }
    /*gb.display.setColor(WHITE);
    gb.display.drawRect(x, y, w, h);*/
    
  }
}

void drawPlayerBullets() {
  for (byte i = 0; i < NUM_PLAYER_BULLETS; i++) {
    if (player.bullets[i].enabled) {
      byte x = player.bullets[i].point.x;
      byte y = player.bullets[i].point.y;
      gb.display.setColor(ORANGE);
      gb.display.fillCircle(x, y, 2);
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
      gb.display.fillCircle(x, y, 2);
      gb.display.setColor(WHITE);
      gb.display.drawCircle(x, y, 1);
    }
  }
}
