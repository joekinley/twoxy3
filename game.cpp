#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sstream>

#include "timer.cpp"
#include "tools.cpp"
#include "enemy.cpp"
#include "missile.cpp"

#ifndef __GAME__
#define __GAME__

const int AWESOME_LIMIT = 50;
const int PARALAX_SPEED = 10;
const int BG_PARALAX_SPEED = 2;

const int SPRITE_IMAGE_WIDTH = 487;
const int SPRITE_IMAGE_HEIGHT = 445;

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;
extern const int ENEMY_WIDTH;
extern const int ENEMY_HEIGHT;

enum EnemyState;
enum OverlayType;

const int OVERLAY_COUNT = 3;
const int OVERLAY_LEVEL_COUNT = 5;
const int MISSILE_COUNT = 4;

const int SHIP_WIDTH = 46;
const int SHIP_HEIGHT = 74;
const int SHIP_SPEED = 10;
const int FIRING_SPEED = 150;

const int MAX_GRAV = 40;
const int GRAV_DIVIDE = 32767 / MAX_GRAV;

extern bool pauseGame;
extern bool quit;
extern bool runningGame;
extern bool firstRun;

struct HitScore;

const int SD_COUNT = 3;
enum ShipDirection {
  SD_LEFT,
  SD_CENTER,
  SD_RIGHT
};

enum OverlayLevel {
  OL_ONE,
  OL_TWO,
  OL_THREE,
  OL_FOUR,
  OL_FIVE
};

class Game {
  private:
    SDL_Surface *background;
    SDL_Surface *backgroundMove;
    SDL_Rect staticBgRect;
    SDL_Rect bgRect;
    SDL_Rect bgMoveRect;
    SDL_Surface *sprites;
    SDL_Surface *particles;

    SDL_Surface *intro;

    TTF_Font *font;

    // own player
    ShipDirection currentDirection;
    SDL_Rect ship[ SD_COUNT ];
    int offsetX;
    int offsetY;
    int velocityX;
    int velocityY;

    int score; // collected items
    int health; // health of player
    int introState;
    bool mayAdd;

    // back burner
    SDL_Rect backburner[ 3 ];
    int currentBackburner;

    // current weapon mode overlays
    SDL_Rect overlays[ OVERLAY_COUNT ][ OVERLAY_LEVEL_COUNT ];

    // ship missile sprites
    SDL_Rect missileSprites[ MISSILE_COUNT ];
    std::vector<Missile> missiles;

    OverlayType currentOverlay;
    OverlayLevel levels[ OVERLAY_COUNT ];

    // enemies
    Timer enemyTimer;
    int enemyInterval;
    std::vector<Enemy> enemies;

    SDL_Rect lostMessage;
    SDL_Rect awesomeMessage;

    Timer awesomeTimer;

    Timer firingTimer;
    bool isFiring;

  public:
    Game( );
    ~Game( );

    void initialize( );

    int getOverlayX( );
    int getOverlayY( );
    int getOverlayW( );
    int getOverlayH( );

    void addEnemy( );

    void handleInput( SDL_Event &event, SDL_Joystick *accelerometer );
    void shoot( );

    void switchOverlay( );
    void addLevel( OverlayType );
    void handleMovement( );
    void handleEnemies( );
    void handleFiring( );

    bool checkState( ); // checks wether player is dead or not

    void render( SDL_Surface *screen );
    void renderShipOverlay( SDL_Surface *screen );
    void renderMissiles( SDL_Surface *screen );
    void renderEnemies( SDL_Surface *screen );
    void renderStats( SDL_Surface *screen );

    void showGameOverScreen( SDL_Surface *screen );
    void showIntroScreens( SDL_Surface *screen );
    void nextIntro( );
};


Game::Game( ) {

  // random seed
  srand( time( NULL ) );

  // load background
  background = load_image( "background_2.png" );
  sprites = load_image( "sprites3.png" );
  particles = load_image( "particles.png" );
  backgroundMove = load_image( "background_move.png" );
  font = TTF_OpenFont( "font.ttf", 12 );

  intro = load_image( "title_screen.png" );

  // color keying sprite image
  Uint32 colorkey = SDL_MapRGBA( sprites->format, 0xFF, 0xFF, 0xFF, 0 );
  SDL_SetColorKey( sprites, SDL_SRCCOLORKEY, colorkey );

  // and particle image
  SDL_SetColorKey( particles, SDL_SRCCOLORKEY, colorkey );

  // and paralax background
  colorkey = SDL_MapRGBA( sprites->format, 0xFF, 0xFF, 0xFF, 0 );
  SDL_SetColorKey( backgroundMove, SDL_SRCCOLORKEY, colorkey );

  // static backgrounds
  staticBgRect.x = 0;
  staticBgRect.y = 0;
  staticBgRect.w = SCREEN_WIDTH;
  staticBgRect.h = SCREEN_HEIGHT;

  // image rects
  bgRect.x = 0;
  bgRect.y = 1000;
  bgRect.w = SCREEN_WIDTH;
  bgRect.h = SCREEN_HEIGHT;

  // moving background rect (paralax scrolling)
  bgMoveRect.x = 0;
  bgMoveRect.y = 1000;
  bgMoveRect.w = SCREEN_WIDTH;
  bgMoveRect.h = SCREEN_HEIGHT;

  // backburners
  backburner[ 0 ].x = 75;
  backburner[ 0 ].y = SPRITE_IMAGE_HEIGHT-38-18;
  backburner[ 0 ].w = 8;
  backburner[ 0 ].h = 18;
  backburner[ 1 ].x = 90;
  backburner[ 1 ].y = SPRITE_IMAGE_HEIGHT-38-18;
  backburner[ 1 ].w = 8;
  backburner[ 1 ].h = 18;
  backburner[ 2 ].x = 105;
  backburner[ 2 ].y = SPRITE_IMAGE_HEIGHT-38-18;
  backburner[ 2 ].w = 8;
  backburner[ 2 ].h = 18;
  currentBackburner = -1;

  // ship
  ship[ SD_CENTER ].x = 0;
  ship[ SD_CENTER ].y = SPRITE_IMAGE_HEIGHT-SHIP_HEIGHT;
  ship[ SD_CENTER ].w = SHIP_WIDTH;
  ship[ SD_CENTER ].h = SHIP_HEIGHT;
  ship[ SD_RIGHT ].x = 192;
  ship[ SD_RIGHT ].y = SPRITE_IMAGE_HEIGHT-SHIP_HEIGHT;
  ship[ SD_RIGHT ].w = SHIP_WIDTH;
  ship[ SD_RIGHT ].h = SHIP_HEIGHT;
  ship[ SD_LEFT ].x = 238;
  ship[ SD_LEFT ].y = SPRITE_IMAGE_HEIGHT-SHIP_HEIGHT;
  ship[ SD_LEFT ].w = SHIP_WIDTH;
  ship[ SD_LEFT ].h = SHIP_HEIGHT;

  // overlays
  overlays[ OT_CAMERA ][ OL_ONE ].x = 0;
  overlays[ OT_CAMERA ][ OL_ONE ].y = SPRITE_IMAGE_HEIGHT-(158+35);
  overlays[ OT_CAMERA ][ OL_ONE ].w = 46;
  overlays[ OT_CAMERA ][ OL_ONE ].h = 35;
  overlays[ OT_CAMERA ][ OL_TWO ].x = 46;
  overlays[ OT_CAMERA ][ OL_TWO ].y = SPRITE_IMAGE_HEIGHT-(158+48);
  overlays[ OT_CAMERA ][ OL_TWO ].w = 63;
  overlays[ OT_CAMERA ][ OL_TWO ].h = 48;
  overlays[ OT_CAMERA ][ OL_THREE ].x = 109;
  overlays[ OT_CAMERA ][ OL_THREE ].y = SPRITE_IMAGE_HEIGHT-(158+60);
  overlays[ OT_CAMERA ][ OL_THREE ].w = 80;
  overlays[ OT_CAMERA ][ OL_THREE ].h = 60;
  overlays[ OT_CAMERA ][ OL_FOUR ].x = 189;
  overlays[ OT_CAMERA ][ OL_FOUR ].y = SPRITE_IMAGE_HEIGHT-(158+73);
  overlays[ OT_CAMERA ][ OL_FOUR ].w = 97;
  overlays[ OT_CAMERA ][ OL_FOUR ].h = 73;
  overlays[ OT_CAMERA ][ OL_FIVE ].x = 286;
  overlays[ OT_CAMERA ][ OL_FIVE ].y = SPRITE_IMAGE_HEIGHT-(158+86);
  overlays[ OT_CAMERA ][ OL_FIVE ].w = 114;
  overlays[ OT_CAMERA ][ OL_FIVE ].h = 86;
  overlays[ OT_GRABBER ][ OL_ONE ].x = 0;
  overlays[ OT_GRABBER ][ OL_ONE ].y = SPRITE_IMAGE_HEIGHT-(74+32);
  overlays[ OT_GRABBER ][ OL_ONE ].w = 46;
  overlays[ OT_GRABBER ][ OL_ONE ].h = 32;
  overlays[ OT_GRABBER ][ OL_TWO ].x = 46;
  overlays[ OT_GRABBER ][ OL_TWO ].y = SPRITE_IMAGE_HEIGHT-(74+44);
  overlays[ OT_GRABBER ][ OL_TWO ].w = 63;
  overlays[ OT_GRABBER ][ OL_TWO ].h = 44;
  overlays[ OT_GRABBER ][ OL_THREE ].x = 109;
  overlays[ OT_GRABBER ][ OL_THREE ].y = SPRITE_IMAGE_HEIGHT-(74+56);
  overlays[ OT_GRABBER ][ OL_THREE ].w = 80;
  overlays[ OT_GRABBER ][ OL_THREE ].h = 56;
  overlays[ OT_GRABBER ][ OL_FOUR ].x = 189;
  overlays[ OT_GRABBER ][ OL_FOUR ].y = SPRITE_IMAGE_HEIGHT-(74+68);
  overlays[ OT_GRABBER ][ OL_FOUR ].w = 97;
  overlays[ OT_GRABBER ][ OL_FOUR ].h = 68;
  overlays[ OT_GRABBER ][ OL_FIVE ].x = 286;
  overlays[ OT_GRABBER ][ OL_FIVE ].y = SPRITE_IMAGE_HEIGHT-(74+80);
  overlays[ OT_GRABBER ][ OL_FIVE ].w = 114;
  overlays[ OT_GRABBER ][ OL_FIVE ].h = 80;
  overlays[ OT_WEAPON ][ OL_ONE ].x = 46;
  overlays[ OT_WEAPON ][ OL_ONE ].y = SPRITE_IMAGE_HEIGHT-SHIP_HEIGHT;
  overlays[ OT_WEAPON ][ OL_ONE ].w = SHIP_WIDTH;
  overlays[ OT_WEAPON ][ OL_ONE ].h = SHIP_HEIGHT;
  overlays[ OT_WEAPON ][ OL_TWO ] = overlays[ OT_WEAPON ][ OL_ONE ];
  overlays[ OT_WEAPON ][ OL_THREE ] = overlays[ OT_WEAPON ][ OL_ONE ];
  overlays[ OT_WEAPON ][ OL_FOUR ].x = 92;
  overlays[ OT_WEAPON ][ OL_FOUR ].y = SPRITE_IMAGE_HEIGHT-SHIP_HEIGHT;
  overlays[ OT_WEAPON ][ OL_FOUR ].w = SHIP_WIDTH;
  overlays[ OT_WEAPON ][ OL_FOUR ].h = SHIP_HEIGHT;
  overlays[ OT_WEAPON ][ OL_FIVE ] = overlays[ OT_WEAPON ][ OL_FOUR ];

  // missiles
  missileSprites[ MT_1 ].x = 145;
  missileSprites[ MT_1 ].y = SPRITE_IMAGE_HEIGHT-7;
  missileSprites[ MT_1 ].w = 3;
  missileSprites[ MT_1 ].h = 7;
  missileSprites[ MT_2 ].x = 155;
  missileSprites[ MT_2 ].y = SPRITE_IMAGE_HEIGHT-10;
  missileSprites[ MT_2 ].w = 4;
  missileSprites[ MT_2 ].h = 10;
  missileSprites[ MT_3 ].x = 165;
  missileSprites[ MT_3 ].y = SPRITE_IMAGE_HEIGHT-16;
  missileSprites[ MT_3 ].w = 7;
  missileSprites[ MT_3 ].h = 16;
  missileSprites[ MT_4 ].x = 175;
  missileSprites[ MT_4 ].y = SPRITE_IMAGE_HEIGHT-23;
  missileSprites[ MT_4 ].w = 10;
  missileSprites[ MT_4 ].h = 23;

  lostMessage.x = 250;
  lostMessage.y = SPRITE_IMAGE_HEIGHT-(405+40);
  lostMessage.w = 213;
  lostMessage.h = 75;

  awesomeMessage.x = 245;
  awesomeMessage.y = SPRITE_IMAGE_HEIGHT-(328+40);
  awesomeMessage.w = 266;
  awesomeMessage.h = 40;

  // start values
  currentOverlay = OT_WEAPON;
  levels[ OT_CAMERA ] = OL_ONE;
  levels[ OT_GRABBER ] = OL_ONE;
  levels[ OT_WEAPON ] = OL_ONE;

  introState = 0;

  initialize( );
}

Game::~Game( ) {
  SDL_FreeSurface( background );
  SDL_FreeSurface( sprites );
  SDL_FreeSurface( particles );
  SDL_FreeSurface( backgroundMove );
  // intro will be freed right after intro
  TTF_CloseFont( font );

  // no need to destroy enemies, are local type
}

void Game::initialize( ) {
  // start values
  currentOverlay = OT_WEAPON;
  levels[ OT_CAMERA ] = OL_ONE;
  levels[ OT_GRABBER ] = OL_ONE;
  levels[ OT_WEAPON ] = OL_ONE;

  // starting point
  offsetX = SCREEN_WIDTH/2-SHIP_WIDTH/2;
  offsetY = SCREEN_HEIGHT-SHIP_HEIGHT;
  velocityX = 0;
  velocityY = 0;

  score = 0;
  enemyTimer.start( );
  enemyInterval = 0;
  health = 10;
  mayAdd = false;

  missiles.clear( );
  enemies.clear( );
  currentDirection = SD_CENTER;

  isFiring = false;
  firingTimer.stop( );
}

void Game::addEnemy( ) {

  int startX, startY;

  do {
    startX = rand()%SCREEN_WIDTH-ENEMY_WIDTH;
  } while ( startX < 0 );
  startY = -40;

  Enemy newEnemy( startX, startY, 0, sprites );
  newEnemy.setParticleSurface( particles );
  enemies.push_back( newEnemy );
}

void Game::handleInput( SDL_Event &event, SDL_Joystick *accelerometer ) {

  velocityX = SDL_JoystickGetAxis( accelerometer, 0 ) / GRAV_DIVIDE;
  velocityY = (SDL_JoystickGetAxis( accelerometer, 1 )-23000) / GRAV_DIVIDE;

  if( event.type == SDL_KEYDOWN ) { // key is pressed
    switch( event.key.keysym.sym ) {

      case SDLK_UP:
        velocityY -= SHIP_SPEED;
        break;

      case SDLK_DOWN:
        velocityY += SHIP_SPEED;
        break;

      case SDLK_LEFT:
        velocityX -= SHIP_SPEED;
        break;

      case SDLK_RIGHT:
        velocityX += SHIP_SPEED;
        break;

      case SDLK_s:
        //switchOverlay( );
        break;

      case SDLK_a:
        shoot( );
        break;

      case SDLK_p:
        pauseGame = !pauseGame;
        break;

      case SDLK_ESCAPE:
        quit = true;
        break;

      // debugging test, remove me
      case SDLK_y:
        //addLevel( currentOverlay );
        break;

      case SDLK_x:
        //addEnemy( );
        break;
    }
  } else if( event.type == SDL_KEYUP ) {
    switch( event.key.keysym.sym ) {

      case SDLK_UP:
        velocityY += SHIP_SPEED;
        break;

      case SDLK_DOWN:
        velocityY -= SHIP_SPEED;
        break;

      case SDLK_LEFT:
        velocityX += SHIP_SPEED;
        break;

      case SDLK_RIGHT:
        velocityX -= SHIP_SPEED;
        break;
    }
  } else if( event.type == SDL_MOUSEBUTTONDOWN ) {
    isFiring = true;
    firingTimer.start( );
    shoot( ); // first shoot immediately
  } else if( event.type == SDL_MOUSEBUTTONUP ) {
    isFiring = false;
    firingTimer.stop( );
  }
}

void Game::handleEnemies( ) {

  if( enemyTimer.getTicks( ) >= enemyInterval ) {
    addEnemy( );
    enemyTimer.start( );

    do {
      enemyInterval = rand( ) % 3000; // TODO: make this level dependent
    } while( enemyInterval < 500 );
  }
}

void Game::handleFiring( ) {

  if( isFiring ) {
    if( firingTimer.getTicks( ) >= FIRING_SPEED ) {
      shoot( );
      firingTimer.start( );
    }
  }
}

void Game::handleMovement( ) {

  handleFiring( );

  // backburner animation
  currentBackburner++;
  if( currentBackburner > 2 ) {
    currentBackburner = 0;
  }

  // parallax scrolling
  bgMoveRect.y-=PARALAX_SPEED;
  if( bgMoveRect.y < 0 ) {
    bgMoveRect.y = 1000;
  }
  bgRect.y -= BG_PARALAX_SPEED;
  if( bgRect.y < 0 ) {
    bgRect.y = 1000;
  }

  if( score > 0 && awesomeTimer.getTicks( ) == 0 && score%AWESOME_LIMIT == 0 ) {
    awesomeTimer.start( );
    health+=3;
  }

  // get 2 health every 10 points
  if( score > 0 && score % 10 == 0 && mayAdd ) {
    health+=2;
    mayAdd = false;
  }

  // ship movement
  offsetX += velocityX;
  offsetY += velocityY;

  // stay within the level
  if( offsetX+SHIP_WIDTH > SCREEN_WIDTH ) offsetX = SCREEN_WIDTH-SHIP_WIDTH;
  else if( offsetX < 0 ) offsetX = 0;
  if( offsetY+SHIP_HEIGHT > SCREEN_HEIGHT ) offsetY = SCREEN_HEIGHT-SHIP_HEIGHT;
  else if( offsetY < 0 ) offsetY = 0;

  // handles enemy creation
  handleEnemies( );

  // missile movement
  for( int i = 0; i < (int)missiles.size( ); i++ ) {
    // basic move
    missiles[ i ].handleMovement( );

    // collision with level exits
    if( missiles[ i ].offsetY < 0 - missileSprites[ missiles[ i ].type ].h ) {
      missiles.erase( missiles.begin( )+i );
    }
  }

  // enemy movement (or particles of enemies)
  for( int i = 0; i < (int)enemies.size( ); i++ ) {

    // basic movement
    // give him the state and the measures of the grabber/photo
    HitScore hitScore = enemies[ i ].handleMovement( currentOverlay, getOverlayX( ), getOverlayY( ), getOverlayW( ), getOverlayH( ) );
    score += hitScore.score;
    health -= hitScore.hit;

    if( hitScore.score > 0 ) {
      mayAdd = true;
    }

    // collision of enemies with own missiles
    enemies[ i ].handleCollision( missiles, missileSprites );
    if( enemies[ i ].isDestroyed( )
        || enemies[ i ].outOfScreen( )
        || enemies[ i ].getState( ) == ES_DESTROY
    ) {
      enemies.erase( enemies.begin( )+i );
    }
  }
}

bool Game::checkState( ) {
  return ( health <= 0 );
}

void Game::shoot( ) {
  // TODO: add time in between shoots with timer depending on level
  // TODO: add damage modifier
  // OL_ONE   - smallest missile, slow
  // OL_TWO   - smallest missile, faster
  // OL_THREE - larger missile, fast as before
  // OL_FOUR  - yet larger missile, yet faster
  // OL_FIVE  - yet larger missile, yet faster

  if( currentOverlay != OT_WEAPON ) return;

  MissileType shootType = MT_1;
  int shootPosX1 = 0;
  int shootPosX2 = 0;
  int vel = 10;
  int dam = 5;

  if( levels[ OT_WEAPON ] <= OL_TWO ) {
    shootType = MT_1;
    shootPosX1 = offsetX+9;
    shootPosX2 = offsetX+SHIP_WIDTH-9;
    vel = 12;
  } else if( levels[ OT_WEAPON ] == OL_THREE ) {
    shootType = MT_2;
    shootPosX1 = offsetX+9;
    shootPosX2 = offsetX+SHIP_WIDTH-9;
    vel = 15;
  } else if( levels[ OT_WEAPON ] == OL_FOUR ) {
    shootType = MT_3;
    shootPosX1 = offsetX+4;
    shootPosX2 = offsetX+SHIP_WIDTH-4;
    vel = 17;
  } else {
    shootType = MT_4;
    shootPosX1 = offsetX+2;
    shootPosX2 = offsetX+SHIP_WIDTH-2;
    vel = 20;
  }

  missiles.push_back( Missile( shootPosX1, offsetY+40, shootType, vel, dam ) );
  missiles.push_back( Missile( shootPosX2, offsetY+40, shootType, vel, dam ) );
}

void Game::switchOverlay( ) {

  currentOverlay = (OverlayType)((int)currentOverlay+1);

  if( currentOverlay >= OVERLAY_COUNT ) {
    currentOverlay = (OverlayType)0;
  }
}

void Game::addLevel( OverlayType current ) {

  if( levels[ current ] < OL_FIVE ) {
    levels[ current ] = (OverlayLevel)((int)levels[ current ]+1);
  }
}

void Game::render( SDL_Surface *screen ) {

  if( velocityX > 10 ) {
    currentDirection = SD_RIGHT;
  } else if( velocityX < -10 ) {
    currentDirection = SD_LEFT;
  } else {
    currentDirection = SD_CENTER;
  }

  // apply background
  apply_surface( 0, 0, background, screen, &bgRect );

  // apply moving background
  apply_surface( 0, 0, backgroundMove, screen, &bgMoveRect );

  // render missiles
  renderMissiles( screen );

  // render enemies
  renderEnemies( screen );

  // draw player
  apply_surface( offsetX, offsetY, sprites, screen, &ship[ currentDirection ] );

  // draw overlay
  renderShipOverlay( screen );

  // render stats
  renderStats( screen );

  if( awesomeTimer.getTicks( ) > 0 ) {
    apply_surface( 50, 200, sprites, screen, &awesomeMessage );

    if( awesomeTimer.getTicks( ) > 3000 ) {
      awesomeTimer.stop( );
    }
  }
}

void Game::renderShipOverlay( SDL_Surface *screen ) {

  // determine where to draw current overlay
  //int drawX = getOverlayX( );
  //int drawY = getOverlayY( );

  // render it
  //apply_surface( drawX, drawY, sprites, screen, &overlays[ currentOverlay ][ levels[ currentOverlay ] ] );

  // render backburner
  if( velocityY < 0 ) {
    apply_surface( offsetX+5, offsetY+SHIP_HEIGHT, sprites, screen, &backburner[ currentBackburner ] );
    apply_surface( offsetX+SHIP_WIDTH-12, offsetY+SHIP_HEIGHT, sprites, screen, &backburner[ currentBackburner ] );
  }
}

int Game::getOverlayX( ) {

  if( currentOverlay == OT_WEAPON ) return offsetX;
  return offsetX - ( ( overlays[ currentOverlay ][ levels[ currentOverlay ] ].w - SHIP_WIDTH ) / 2 );
}

int Game::getOverlayY( ) {

  if( currentOverlay == OT_WEAPON ) return offsetY;
  return offsetY - ( overlays[ currentOverlay ][ levels[ currentOverlay ] ].h - 5 );
}

int Game::getOverlayW( ) {
  return overlays[ currentOverlay ][ levels[ currentOverlay ] ].w;
}

int Game::getOverlayH( ) {
  return overlays[ currentOverlay ][ levels[ currentOverlay ] ].h;
}

void Game::renderMissiles( SDL_Surface *screen ) {

  for( int i = 0; i < (int)missiles.size( ); i++ ) {
    apply_surface( missiles[ i ].offsetX, missiles[ i ].offsetY, sprites, screen, &missileSprites[ missiles[ i ].type ] );
  }
}

void Game::renderEnemies( SDL_Surface *screen ) {

  for( int i = 0; i < (int)enemies.size( ); i++ ) {
    enemies[ i ].render( screen );
  }
}

void Game::renderStats( SDL_Surface *screen ) {

  SDL_Surface *message;

  std::stringstream ss;
  ss << score;
  SDL_Color textColor = { 0, 255, 0 };
  std::string statString( "Score: " );
  statString += ss.str( );
  message = TTF_RenderText_Solid( font, statString.c_str( ), textColor );
  apply_surface( 2, 2, message, screen );
  SDL_FreeSurface( message );

  std::stringstream ss2;
  ss2 << health;
  std::string healthString( "Health: " );
  healthString += ss2.str( );
  message = TTF_RenderText_Solid( font, healthString.c_str( ), textColor );
  apply_surface( SCREEN_WIDTH-70, 2, message, screen );
  SDL_FreeSurface( message );
}

void Game::showGameOverScreen( SDL_Surface *screen ) {
  apply_surface( 70, 200, sprites, screen, &lostMessage );
}

void Game::showIntroScreens( SDL_Surface *screen ) {

  if( introState == 0 ) {
    apply_surface( 0, 0, intro, screen, &staticBgRect );
  } else if( introState == 1 ) {
    SDL_FreeSurface( intro );
    intro = load_image( "title_screen_2.png" );
    apply_surface( 0, 0, intro, screen, &staticBgRect );
  } /*else if( introState == 2 ) {
    SDL_FreeSurface( intro );
    intro = load_image( "title_3.png" );
    apply_surface( 0, 0, intro, screen, &staticBgRect );
  } */else {
    SDL_FreeSurface( intro );
    firstRun = false;
    runningGame = true;
  }
}

void Game::nextIntro( ) {
  introState++;
}


#endif