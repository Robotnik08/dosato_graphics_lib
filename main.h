#ifndef MAIN_H
#define MAIN_H

#include "dosato_source/dosato.h" // Include the dosato library.
#include <SDL2/SDL.h> // Include the SDL2 library.

DosatoFunctionMapList functions;
void init();

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

#endif // MAIN_H