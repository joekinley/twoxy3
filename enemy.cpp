#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "timer.cpp"
#include "tools.cpp"
#include "missile.cpp"
#include "particle.cpp"

#include <iostream>
#include <cstdlib>
#include <vector>
#include <ctime>

#ifndef __ENEMY__
#define __ENEMY__

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int SPRITE_IMAGE_HEIGHT;
const int TYPE_COUNT = 5;
extern const int ENEMY_MISSILE_COUNT;

const int ENEMY_WIDTH = 48;
const int ENEMY_HEIGHT = 48;

enum BodyType { // health
  BT_ONE,   // 2
  BT_TWO,   // 5
  BT_THREE, // 7
  BT_FOUR,  // 10
  BT_FIVE   // 15
};

enum ShieldType { // shield
  ST_ONE,   // 2
  ST_TWO,   // 5
  ST_THREE, // 7
  ST_FOUR,  // 10
  ST_FIVE   // 15
};

enum WeaponType { // damage
  WT_ONE,   // 2
  WT_TWO,   // 5
  WT_THREE, // 5
  WT_FOUR,  // 5
  WT_FIVE   // 7
};

enum ThrottleType { // speed
  TT_ONE,   // +5Y
  TT_TWO,   // +5-10Y
  TT_THREE, // +5-10Y +5X -5X
  TT_FOUR,  // +5Y+X -5Y+X
  TT_FIVE   // +10Y+X -10Y+X
};

enum EnemyState {
  ES_LIVING, // showing ship and moving
  ES_DEAD, // showing particles
  ES_DESTROY // destroy this object
};

struct HitScore {
  int hit;
  int score;

  HitScore( int h, int s ) {
    hit = h;
    score = s;
  }
};

class Enemy {

  private:
    std::vector<Particle> particles;
    std::vector<EnemyMissile> missiles;

    SDL_Surface *sprites;
    SDL_Surface *particleSurface;

    SDL_Rect bodyRect[ TYPE_COUNT ];
    SDL_Rect shieldRect[ TYPE_COUNT ];
    SDL_Rect weaponRect[ TYPE_COUNT ];
    SDL_Rect throttleRect[ TYPE_COUNT ];
    SDL_Rect enemyMissilesRect[ ENEMY_MISSILE_COUNT ];

    int offsetX;
    int offsetY;

    int velocityX;
    int velocityY;

    BodyType body;
    ShieldType shield;
    WeaponType weapon;
    ThrottleType throttle;

    int shieldStat;
    int healthStat;
    int damageStat;

    EnemyState currentState;

    Timer shootTimer;
    int shootInterval;

  public:
    Enemy( int, int, int, SDL_Surface * );
    ~Enemy( );

    EnemyState getState( );
    void setParticleSurface( SDL_Surface * );

    bool collidesAt( int, int );
    bool outOfScreen( );
    bool isDestroyed( );

    void addDamage( int );
    void shoot( );

    void handleCollision( std::vector<Missile> &, SDL_Rect [] );
    HitScore handleMovement( OverlayType, int, int, int, int );
    int handleMissiles( int, int, int, int );
    void handleShoot( );

    void createParticles( ); // creates up to 4 particles

    void render( SDL_Surface * );
    void renderMissiles( SDL_Surface * );

};

// TODO: difficulty should determine the max difficulty of the enemy (best weapons and stuff)
Enemy::Enemy( int x, int y, int difficulty, SDL_Surface *spriteSurface ) {

  // build up rect information
  // CLEANUP: in time make this a loop over [250 + ENEMY_HEIGHT...] (you get the idea)
  bodyRect[ BT_ONE ].x = 0*ENEMY_WIDTH;
  bodyRect[ BT_ONE ].y = SPRITE_IMAGE_HEIGHT-(250+ENEMY_HEIGHT);
  bodyRect[ BT_ONE ].w = ENEMY_WIDTH;
  bodyRect[ BT_ONE ].h = ENEMY_HEIGHT;
  bodyRect[ BT_TWO ].x = 1*ENEMY_WIDTH;
  bodyRect[ BT_TWO ].y = SPRITE_IMAGE_HEIGHT-(250+ENEMY_HEIGHT);
  bodyRect[ BT_TWO ].w = ENEMY_WIDTH;
  bodyRect[ BT_TWO ].h = ENEMY_HEIGHT;
  bodyRect[ BT_THREE ].x = 2*ENEMY_WIDTH;
  bodyRect[ BT_THREE ].y = SPRITE_IMAGE_HEIGHT-(250+ENEMY_HEIGHT);
  bodyRect[ BT_THREE ].w = ENEMY_WIDTH;
  bodyRect[ BT_THREE ].h = ENEMY_HEIGHT;
  bodyRect[ BT_FOUR ].x = 3*ENEMY_WIDTH;
  bodyRect[ BT_FOUR ].y = SPRITE_IMAGE_HEIGHT-(250+ENEMY_HEIGHT);
  bodyRect[ BT_FOUR ].w = ENEMY_WIDTH;
  bodyRect[ BT_FOUR ].h = ENEMY_HEIGHT;
  bodyRect[ BT_FIVE ].x = 4*ENEMY_WIDTH;
  bodyRect[ BT_FIVE ].y = SPRITE_IMAGE_HEIGHT-(250+ENEMY_HEIGHT);
  bodyRect[ BT_FIVE ].w = ENEMY_WIDTH;
  bodyRect[ BT_FIVE ].h = ENEMY_HEIGHT;
  shieldRect[ ST_ONE ].x = 0*ENEMY_WIDTH;
  shieldRect[ ST_ONE ].y = SPRITE_IMAGE_HEIGHT-(298+ENEMY_HEIGHT);
  shieldRect[ ST_ONE ].w = ENEMY_WIDTH;
  shieldRect[ ST_ONE ].h = ENEMY_HEIGHT;
  shieldRect[ ST_TWO ].x = 1*ENEMY_WIDTH;
  shieldRect[ ST_TWO ].y = SPRITE_IMAGE_HEIGHT-(298+ENEMY_HEIGHT);
  shieldRect[ ST_TWO ].w = ENEMY_WIDTH;
  shieldRect[ ST_TWO ].h = ENEMY_HEIGHT;
  shieldRect[ ST_THREE ].x = 2*ENEMY_WIDTH;
  shieldRect[ ST_THREE ].y = SPRITE_IMAGE_HEIGHT-(298+ENEMY_HEIGHT);
  shieldRect[ ST_THREE ].w = ENEMY_WIDTH;
  shieldRect[ ST_THREE ].h = ENEMY_HEIGHT;
  shieldRect[ ST_FOUR ].x = 3*ENEMY_WIDTH;
  shieldRect[ ST_FOUR ].y = SPRITE_IMAGE_HEIGHT-(298+ENEMY_HEIGHT);
  shieldRect[ ST_FOUR ].w = ENEMY_WIDTH;
  shieldRect[ ST_FOUR ].h = ENEMY_HEIGHT;
  shieldRect[ ST_FIVE ].x = 4*ENEMY_WIDTH;
  shieldRect[ ST_FIVE ].y = SPRITE_IMAGE_HEIGHT-(298+ENEMY_HEIGHT);
  shieldRect[ ST_FIVE ].w = ENEMY_WIDTH;
  shieldRect[ ST_FIVE ].h = ENEMY_HEIGHT;
  weaponRect[ WT_ONE ].x = 0*ENEMY_WIDTH;
  weaponRect[ WT_ONE ].y = SPRITE_IMAGE_HEIGHT-(346+ENEMY_HEIGHT);
  weaponRect[ WT_ONE ].w = ENEMY_WIDTH;
  weaponRect[ WT_ONE ].h = ENEMY_HEIGHT;
  weaponRect[ WT_TWO ].x = 1*ENEMY_WIDTH;
  weaponRect[ WT_TWO ].y = SPRITE_IMAGE_HEIGHT-(346+ENEMY_HEIGHT);
  weaponRect[ WT_TWO ].w = ENEMY_WIDTH;
  weaponRect[ WT_TWO ].h = ENEMY_HEIGHT;
  weaponRect[ WT_THREE ].x = 2*ENEMY_WIDTH;
  weaponRect[ WT_THREE ].y = SPRITE_IMAGE_HEIGHT-(346+ENEMY_HEIGHT);
  weaponRect[ WT_THREE ].w = ENEMY_WIDTH;
  weaponRect[ WT_THREE ].h = ENEMY_HEIGHT;
  weaponRect[ WT_FOUR ].x = 3*ENEMY_WIDTH;
  weaponRect[ WT_FOUR ].y = SPRITE_IMAGE_HEIGHT-(346+ENEMY_HEIGHT);
  weaponRect[ WT_FOUR ].w = ENEMY_WIDTH;
  weaponRect[ WT_FOUR ].h = ENEMY_HEIGHT;
  weaponRect[ WT_FIVE ].x = 4*ENEMY_WIDTH;
  weaponRect[ WT_FIVE ].y = SPRITE_IMAGE_HEIGHT-(346+ENEMY_HEIGHT);
  weaponRect[ WT_FIVE ].w = ENEMY_WIDTH;
  weaponRect[ WT_FIVE ].h = ENEMY_HEIGHT;
  throttleRect[ TT_ONE ].x = 0*ENEMY_WIDTH;
  throttleRect[ TT_ONE ].y = SPRITE_IMAGE_HEIGHT-(394+ENEMY_HEIGHT);
  throttleRect[ TT_ONE ].w = ENEMY_WIDTH;
  throttleRect[ TT_ONE ].h = ENEMY_HEIGHT;
  throttleRect[ TT_TWO ].x = 1*ENEMY_WIDTH;
  throttleRect[ TT_TWO ].y = SPRITE_IMAGE_HEIGHT-(394+ENEMY_HEIGHT);
  throttleRect[ TT_TWO ].w = ENEMY_WIDTH;
  throttleRect[ TT_TWO ].h = ENEMY_HEIGHT;
  throttleRect[ TT_THREE ].x = 2*ENEMY_WIDTH;
  throttleRect[ TT_THREE ].y = SPRITE_IMAGE_HEIGHT-(394+ENEMY_HEIGHT);
  throttleRect[ TT_THREE ].w = ENEMY_WIDTH;
  throttleRect[ TT_THREE ].h = ENEMY_HEIGHT;
  throttleRect[ TT_FOUR ].x = 3*ENEMY_WIDTH;
  throttleRect[ TT_FOUR ].y = SPRITE_IMAGE_HEIGHT-(394+ENEMY_HEIGHT);
  throttleRect[ TT_FOUR ].w = ENEMY_WIDTH;
  throttleRect[ TT_FOUR ].h = ENEMY_HEIGHT;
  throttleRect[ TT_FIVE ].x = 4*ENEMY_WIDTH;
  throttleRect[ TT_FIVE ].y = SPRITE_IMAGE_HEIGHT-(394+ENEMY_HEIGHT);
  throttleRect[ TT_FIVE ].w = ENEMY_WIDTH;
  throttleRect[ TT_FIVE ].h = ENEMY_HEIGHT;
  enemyMissilesRect[ EMT_1 ].x = 250;
  enemyMissilesRect[ EMT_1 ].y = SPRITE_IMAGE_HEIGHT-(298+4);
  enemyMissilesRect[ EMT_1 ].w = 4;
  enemyMissilesRect[ EMT_1 ].h = 4;
  enemyMissilesRect[ EMT_2 ].x = 260;
  enemyMissilesRect[ EMT_2 ].y = SPRITE_IMAGE_HEIGHT-(298+10);
  enemyMissilesRect[ EMT_2 ].w = 4;
  enemyMissilesRect[ EMT_2 ].h = 10;
  enemyMissilesRect[ EMT_3 ].x = 268;
  enemyMissilesRect[ EMT_3 ].y = SPRITE_IMAGE_HEIGHT-(298+15);
  enemyMissilesRect[ EMT_3 ].w = 2;
  enemyMissilesRect[ EMT_3 ].h = 15;
  enemyMissilesRect[ EMT_4_1 ].x = 275;
  enemyMissilesRect[ EMT_4_1 ].y = SPRITE_IMAGE_HEIGHT-(298+12);
  enemyMissilesRect[ EMT_4_1 ].w = 12;
  enemyMissilesRect[ EMT_4_1 ].h = 12;
  enemyMissilesRect[ EMT_4_2 ].x = 292;
  enemyMissilesRect[ EMT_4_2 ].y = SPRITE_IMAGE_HEIGHT-(298+12);
  enemyMissilesRect[ EMT_4_2 ].w = 12;
  enemyMissilesRect[ EMT_4_2 ].h = 12;
  enemyMissilesRect[ EMT_5 ].x = 310;
  enemyMissilesRect[ EMT_5 ].y = SPRITE_IMAGE_HEIGHT-(298+4);
  enemyMissilesRect[ EMT_5 ].w = 4;
  enemyMissilesRect[ EMT_5 ].h = 4;

  sprites = spriteSurface;
  offsetX = x;
  offsetY = y;
  // TODO: difficulty

  // just add random types
  body = (BodyType)(rand()%TYPE_COUNT);
  shield = (ShieldType)(rand()%TYPE_COUNT);
  weapon = (WeaponType)(rand()%TYPE_COUNT);
  throttle = (ThrottleType)(rand()%TYPE_COUNT);

  // TODO: velocities should be determined by throttle
  velocityX = 0;
  velocityY = 5;

  // TODO: shield and health should be determined by body and shield
  // also weapon
  shieldStat = 10;
  healthStat = 10;
  damageStat = 5;

  currentState = ES_LIVING;
  shootTimer.start( );

  shootInterval = 0;
}

Enemy::~Enemy( ) {
  particles.empty( );
  missiles.empty( );
}

EnemyState Enemy::getState( ) {
  return currentState;
}

void Enemy::setParticleSurface( SDL_Surface *one ) {
  particleSurface = one;
}

// TODO: maybe make it more clever with probabilities and ship proximity
void Enemy::handleShoot( ) {

  if( shootTimer.getTicks( ) >= shootInterval ) {
    shoot( );
    shootTimer.start( );

    do {
      shootInterval = rand()%3000;
    } while ( shootInterval < 500 );
  }
}

// returns no of collected particles, if negative, no of hits taken by missiles
HitScore Enemy::handleMovement( OverlayType currentType, int x, int y, int w, int h ) {

  int collected = 0;
  int hit = 0;

  if( currentState == ES_LIVING ) {

    offsetX += velocityX;
    offsetY += velocityY;

    // shoot
    handleShoot( );

    // handle missiles off screen
    hit += handleMissiles( x, y, w, h );

    // TODO: shooting algorithm, maybe with ship approximation or stuff
  } else if( currentState == ES_DEAD ) {

    if( (int)particles.size( ) > 0 ) {

      for( int i = 0; i < particles.size( ); i++ ) {

        particles[ i ].handleMovement( );
        int particleX = particles[ i ].getX( );
        int particleY = particles[ i ].getY( );
        int particleW = particles[ i ].getRect( ).w;
        int particleH = particles[ i ].getRect( ).h;

        // check collision with grabber
        if( x < particleX+particleW && x+w > particleX ) {

          if( y < particleY+particleH && y+h > particleY ) {
            particles.erase( particles.begin() + i );
            collected++;
            break;
          }
        }

        if( particles[ i ].offScreen( ) ) {
          particles.erase( particles.begin( )+i );
          break;
        }
      }
    }

    // handle missiles off screen
    hit += handleMissiles( x, y, w, h );
  }

  HitScore myReturn( hit, collected );

  return myReturn;
}

// returns no of being hit
int Enemy::handleMissiles( int x, int y, int w, int h ) {

  int hit = 0;

  for( int i = 0; i < (int)missiles.size( ); i++ ) {

    missiles[ i ].handleMovement( );

    // collision with player
    int missileX = missiles[ i ].offsetX;
    int missileY = missiles[ i ].offsetY;
    int missileW = enemyMissilesRect[ missiles[ i ].type ].w;
    int missileH = enemyMissilesRect[ missiles[ i ].type ].h;

    if( x < missileX+missileW && x+w > missileX ) {
      if( y < missileY+missileH && y+h > missileY ) {
        hit++;
        missiles.erase( missiles.begin( )+i ); // when hit, remove missile
      }
    }

    // off screen
    if( missiles[ i ].offScreen( enemyMissilesRect[ missiles[ i ].type ] ) ) {
      missiles.erase( missiles.begin( )+i );
    }
  }

  return hit;
}

void Enemy::handleCollision( std::vector<Missile> &missiles, SDL_Rect sprites[] ) {

  for( int i = 0; i < (int)missiles.size( ); i++ ) {

    if( offsetX < missiles[ i ].offsetX+sprites[ missiles[ i ].type ].w && offsetX+ENEMY_WIDTH > missiles[ i ].offsetX ) {
      if( offsetY < missiles[ i ].offsetY+sprites[ missiles[ i ].type ].h && offsetY+ENEMY_HEIGHT > missiles[ i ].offsetY ) {
        addDamage( missiles[ i ].damage );
        if( currentState == ES_LIVING ) {
          missiles.erase( missiles.begin( )+i );
        }
      }
    }
  }
}

void Enemy::addDamage( int d ) {

  shieldStat -= d;

  if( shieldStat < 0 ) {
    shieldStat = 0;
    healthStat -= d;
  }
}

void Enemy::shoot( ) {

  EnemyMissileType newType;
  EnemyMissile newMissile;
  switch( weapon ) {
    case WT_ONE:
      // left missile
      newMissile.setOffset( offsetX, offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_1 );
      newMissile.setVelocity( 10 );
      newMissile.setDamage( 2 );
      missiles.push_back( newMissile );

      // right missile
      newMissile.setOffset( offsetX+ENEMY_WIDTH-3, offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_1 );
      newMissile.setVelocity( 10 );
      newMissile.setDamage( 2 );
      missiles.push_back( newMissile );
      break;

    case WT_TWO:
      newMissile.setOffset( offsetX+(ENEMY_WIDTH/2), offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_2 );
      newMissile.setVelocity( 7 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );
      break;

    case WT_THREE:
      newMissile.setOffset( offsetX+(ENEMY_WIDTH/2), offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_3 );
      newMissile.setVelocity( 13 );
      newMissile.setDamage( 7 );
      missiles.push_back( newMissile );
      break;

    case WT_FOUR:
      // left missile
      newMissile.setOffset( offsetX, offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_4_1 );
      newMissile.setVelocity( -10, 10 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );

      // right missile
      newMissile.setOffset( offsetX+ENEMY_WIDTH, offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_4_2 );
      newMissile.setVelocity( 10, 10 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );
      break;

    case WT_FIVE:
      // left missile
      newMissile.setOffset( offsetX, offsetY+(ENEMY_HEIGHT/2) );
      newMissile.setType( EMT_5 );
      newMissile.setVelocity( -3, 0 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );

      // top missile
      newMissile.setOffset( offsetX+(ENEMY_WIDTH/2), offsetY );
      newMissile.setType( EMT_5 );
      newMissile.setVelocity( 0, -3 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );

      // right missile
      newMissile.setOffset( offsetX+ENEMY_WIDTH, offsetY+(ENEMY_HEIGHT/2) );
      newMissile.setType( EMT_5 );
      newMissile.setVelocity( 3, 0 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );

      // down missile
      newMissile.setOffset( offsetX+ENEMY_WIDTH/2, offsetY+ENEMY_HEIGHT );
      newMissile.setType( EMT_5 );
      newMissile.setVelocity( 0, 7 );
      newMissile.setDamage( 5 );
      missiles.push_back( newMissile );
      break;
  }


}

// returns wether the given point is within the enemy
bool Enemy::collidesAt( int x, int y ) {

  if( offsetX < x ) return false;
  if( offsetX > x+ENEMY_WIDTH ) return false;
  if( offsetY < y ) return false;
  if( offsetY > y+ENEMY_HEIGHT ) return false;

  return true;
}

// returns wether the enemy is out of a given area
bool Enemy::outOfScreen( ) {

  if( offsetY > SCREEN_HEIGHT ) return true;
  if( offsetX+ENEMY_WIDTH < 0 ) return true;
  if( offsetX > SCREEN_WIDTH ) return true;
  if( offsetY+ENEMY_HEIGHT < 0 ) return true;

  return false;
}

bool Enemy::isDestroyed( ) {

  if( healthStat > 0 ) return false;
  else if( currentState == ES_LIVING ) {
    currentState = ES_DEAD;
    createParticles( );
  } else if( currentState == ES_DESTROY || ( currentState == ES_DEAD && (int)particles.size( ) <= 0 ) ) {
    return true;
  }

  return false;
}

void Enemy::createParticles( ) {

  int amount;

  do {
    amount = rand( ) % 4;
  } while( amount < 0);

  int velX[] = {-1, 1, -1, 1};
  int velY[] = {-1, 1, 1, -1};

  for( int i = 0; i <= amount; i++ ) {

    Particle newParticle( offsetX+(ENEMY_WIDTH/2), offsetY+(ENEMY_HEIGHT/2));
    newParticle.setPart( );
    newParticle.setVelocity( velX[ i ], velY[ i ] );
    newParticle.setParticleSurface( particleSurface );

    particles.push_back( newParticle );
  }

}

void Enemy::render( SDL_Surface *screen ) {

  if( currentState == ES_LIVING ) {

    // throttle
    apply_surface( offsetX, offsetY, sprites, screen, &throttleRect[ throttle ] );

    // weapon
    apply_surface( offsetX, offsetY, sprites, screen, &weaponRect[ weapon ] );

    // body
    apply_surface( offsetX, offsetY, sprites, screen, &bodyRect[ body  ] );

    // shield
    apply_surface( offsetX, offsetY, sprites, screen, &shieldRect[ shield ] );

  } else {
    if( (int)particles.size( ) > 0 ) {
      for( int i = 0; i < (int)particles.size( ); i++ ) {
        particles[ i ].render( screen );
      }
    } else {
      currentState = ES_DESTROY;
    }
  }

  // render missiles
  renderMissiles( screen );
}

void Enemy::renderMissiles( SDL_Surface *screen ) {

  for( int i = 0; i < (int)missiles.size( ); i++ ) {
    missiles[ i ].render( screen, enemyMissilesRect, sprites );
  }
}

#endif