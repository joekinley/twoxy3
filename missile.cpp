
#ifndef __MISSILE__
#define __MISSILE__

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

enum MissileType {
  MT_1,
  MT_2,
  MT_3,
  MT_4
};

enum EnemyMissileType {
  EMT_1,
  EMT_2,
  EMT_3,
  EMT_4_1,
  EMT_4_2,
  EMT_5
};

const int ENEMY_MISSILE_COUNT = 6;

struct Missile {
  MissileType type;
  int offsetX;
  int offsetY;
  int velocity;
  int damage;

  Missile( ) {}

  Missile( int x, int y, MissileType t, int v, int d ) {
    offsetX = x;
    offsetY = y;
    type = t;
    velocity = v;
    damage = d;
  }

  void handleMovement( ) {
    offsetY -= velocity;
  }
};

struct EnemyMissile : public Missile {
  EnemyMissileType type;
  int velocityX;
  int velocityY;

  EnemyMissile( ) {}

  EnemyMissile( int x, int y, EnemyMissileType t, int v, int d ) {
    offsetX = x;
    offsetY = y;
    type = t;
    velocity = v;
    damage = d;
  }

  void setOffset( int x, int y ) {
    offsetX = x;
    offsetY = y;
  }

  void setType( EnemyMissileType t ) {
    type = t;
  }

  void setVelocity( int v ) {
    velocityY = v;
    velocityX = 0;
  }

  void setVelocity( int x, int y ) {
    velocityX = x;
    velocityY = y;
  }

  void setDamage( int d ) {
    damage = d;
  }

  void handleMovement( ) {
    offsetX += velocityX;
    offsetY += velocityY;
  }

  bool offScreen( SDL_Rect tile ) {

    if( offsetY > SCREEN_HEIGHT ) return true;
    if( offsetX+tile.w < 0 ) return true;
    if( offsetX > SCREEN_WIDTH ) return true;
    if( offsetY+tile.h < 0 ) return true;

    return false;
  }

  void render( SDL_Surface *screen, SDL_Rect *tiles, SDL_Surface *sprites ) {
    apply_surface( offsetX, offsetY, sprites, screen, &tiles[ type ] );
  }
};

#endif