
#ifndef __PARTICLE__
#define __PARTICLE__

#include "tools.cpp"

extern const int SCREEN_WIDTH;
extern const int SCREEN_HEIGHT;

class Particle {
  private:

    int offsetX;
    int offsetY;
    int velocityX;
    int velocityY;

    SDL_Surface *object;
    SDL_Rect tile;

  public:
    Particle( int, int );
    ~Particle( );

    SDL_Rect getRect( );
    int getX( );
    int getY( );
    bool offScreen( );

    void setPart( );
    void setVelocity( int x, int y );
    void setParticleSurface( SDL_Surface * );

    void handleMovement( );

    void render( SDL_Surface * );

};

Particle::Particle( int x, int y ) {

  offsetX = x;
  offsetY = y;
}

Particle::~Particle( ) {
}

SDL_Rect Particle::getRect( ) {
  return tile;
}

int Particle::getX( ) {
  return offsetX;
}

int Particle::getY( ) {
  return offsetY;
}

void Particle::setParticleSurface( SDL_Surface *sur ) {
  object = sur;
}

bool Particle::offScreen( ) {


  if( offsetY > SCREEN_HEIGHT ) return true;
  if( offsetX+tile.w < 0 ) return true;
  if( offsetX > SCREEN_WIDTH ) return true;
  if( offsetY+tile.h < 0 ) return true;

  return false;
}

void Particle::setPart( ) {
  do {
    tile.x = rand() % 500-24;
  } while ( tile.x < 0 );

  do {
    tile.y = rand() % 500-24;
  } while( tile.y < 0 );

  tile.w = 24;
  tile.h = 24;
}

void Particle::setVelocity( int x, int y ) {
  velocityX = x;
  velocityY = y;
}

void Particle::handleMovement( ) {
  offsetX += velocityX;
  offsetY += velocityY;
}

void Particle::render( SDL_Surface *screen ) {

  apply_surface( offsetX, offsetY, object, screen, &tile );
}


#endif
