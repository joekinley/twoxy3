#ifndef __TOOLS__
#define __TOOLS__

#include <string>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

enum OverlayType {
  OT_CAMERA,
  OT_GRABBER,
  OT_WEAPON
// TODO: overlay mode research
};

SDL_Surface *load_image( std::string filename ) {

  SDL_Surface *myImage = NULL;
  SDL_Surface *returnImage = NULL;

  myImage = IMG_Load( filename.c_str( ) );
  if( myImage != NULL ) {

    returnImage = SDL_DisplayFormat( myImage );
    SDL_FreeSurface( myImage );

    if( returnImage != NULL ) {
      SDL_SetColorKey( returnImage, SDL_SRCCOLORKEY, SDL_MapRGB( returnImage->format, 0, 0xFF, 0xFF ) );
    }
  }

  return returnImage;
}

void apply_surface( int x, int y, SDL_Surface *source, SDL_Surface *destination, SDL_Rect *clip = NULL ) {

  SDL_Rect offset;

  offset.x = x;
  offset.y = y;

  SDL_BlitSurface( source, clip, destination, &offset );
}

#endif