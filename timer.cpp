#ifndef __TIMER__
#define __TIMER__

#include <SDL/SDL.h>

class Timer {

  private:

    int startTicks; // start time
    int pausedTicks; // save pause time

    bool started;
    bool paused;

  public:

    Timer( );

    void start( );
    void stop( );
    void pause( );
    void unpause( );

    int getTicks( );

    bool isStarted( );
    bool isPaused( );
};

Timer::Timer( ) {

  startTicks  = 0;
  pausedTicks = 0;
  started     = false;
  paused      = false;
}

void Timer::start( ) {

  started = true;
  paused  = false;

  startTicks = SDL_GetTicks( );
}

void Timer::stop( ) {

  started = false;
  paused = false;
}

void Timer::pause( ) {

  if( started && !paused ) {

    paused = true;
    pausedTicks = SDL_GetTicks( ) - startTicks;
  }
}

void Timer::unpause( ) {

  if( paused ) {

    paused = false;
    startTicks = SDL_GetTicks( ) - pausedTicks;
    pausedTicks = 0;
  }
}

int Timer::getTicks( ) {

  if( started ) {

    if( paused ) {
      return pausedTicks;
    } else {
      return SDL_GetTicks( ) - startTicks;
    }
  }

  return 0;
}

bool Timer::isStarted( ) {
  return started;
}

bool Timer::isPaused( ) {
  return paused;
}

#endif