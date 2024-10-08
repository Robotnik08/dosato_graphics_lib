#ifndef MAIN_H
#define MAIN_H

#include "dosato_source/dosato.h" // Include the dosato library.
#include <SDL2/SDL.h> // Include the SDL2 library.
#include <SDL2/SDL_image.h> // Include the SDL2_image library.

DosatoFunctionMapList functions;
void init(VirtualMachine* vm);

Value createWindow (ValueArray args, bool debug);
Value closeWindow (ValueArray args, bool debug);
Value updateState (ValueArray args, bool debug);
Value getKey (ValueArray args, bool debug);
Value getKeyDown (ValueArray args, bool debug);
Value getKeyReleased (ValueArray args, bool debug);
Value ifQuit (ValueArray args, bool debug);
Value setColor (ValueArray args, bool debug);
Value clearScreen (ValueArray args, bool debug);
Value renderWindow (ValueArray args, bool debug);
Value drawSquare (ValueArray args, bool debug);
Value createSprite (ValueArray args, bool debug);
Value getSpriteDimensions (ValueArray args, bool debug);
Value drawSprite (ValueArray args, bool debug);
Value drawLine (ValueArray args, bool debug);
Value getMousePosition (ValueArray args, bool debug);
Value getMouseHeld (ValueArray args, bool debug);
Value getMousePressed (ValueArray args, bool debug);
Value getMouseReleased (ValueArray args, bool debug);


#endif // MAIN_H