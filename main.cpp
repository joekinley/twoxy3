#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include "timer.cpp"
#include "tools.cpp"
#include "game.cpp"

const int SCREEN_WIDTH = 320;
const int SCREEN_HEIGHT = 480;
const int SCREEN_BPP = 32;

const int FRAMES_PER_SECOND = 30;

Timer fps;
int frame = 0;
bool quit = false;
bool pauseGame = false;
bool gameOver = false;
bool runningGame = false;
bool firstRun = true;

int main( int argc, char* args[] ) {

  SDL_Init( SDL_INIT_EVERYTHING );
  TTF_Init( );

  SDL_Surface *screen = NULL;
  SDL_Event event;

  SDL_WM_SetCaption( "Twoxy 3", NULL );
  screen = SDL_SetVideoMode( SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_SWSURFACE );

  // accelerometer input
  SDL_Joystick *accelerometer = SDL_JoystickOpen( 0 );

  // my game object
  Game myGame;

  // main loop
  while( !quit ) {

    if( !runningGame && firstRun ) {

      // show intro screens
      myGame.showIntroScreens( screen );
    }

    while( runningGame && !gameOver && !quit ) {
      // clear screen
      if( !pauseGame ) {
        SDL_FillRect( screen, &screen->clip_rect, SDL_MapRGB( screen->format, 0xFF, 0xFF, 0xFF ) );
      }
      fps.start( );

      // handle events
      while( SDL_PollEvent( &event ) ) {
        if( event.type == SDL_QUIT ) {
          quit = true;
          break;
        }

        myGame.handleInput( event, accelerometer );
      }

      if( !pauseGame ) {
        // handle logic
        myGame.handleMovement( );

        // handle rendering
        myGame.render( screen );

        gameOver = myGame.checkState( );

        // draw screen
        SDL_Flip( screen );
      }

      frame++;

      // frame limiter
      if( fps.getTicks( ) < 1000 / FRAMES_PER_SECOND ) {
        SDL_Delay( ( 1000 / FRAMES_PER_SECOND ) - fps.getTicks( ) );
      }
    }

    if( gameOver ) {
      runningGame = false;
      myGame.showGameOverScreen( screen );

    }

    // handle events
    while( SDL_PollEvent( &event ) ) {
      if( event.type == SDL_QUIT ) {
        quit = true;
        break;
      }
      if( event.type == SDL_KEYDOWN || event.type == SDL_MOUSEBUTTONDOWN ) {
        // here be dragons, that is some pretty messed up code here
        if( event.key.keysym.sym == SDLK_RETURN || event.type == SDL_MOUSEBUTTONDOWN ) {
          gameOver = false;
          myGame.initialize( );
        }

        if( firstRun ) {
          myGame.nextIntro( );
        } else {
          runningGame = true;
        }
        break;
      }
      if( event.key.keysym.sym == SDLK_ESCAPE ) {
        quit = true;
        break;
      }
    }

    // draw screen
    SDL_Flip( screen );
  }

  // free my stuff
  SDL_FreeSurface( screen );

  // cleanup sdl stuff
  TTF_Quit( );
  SDL_Quit( );


  return 0;
}