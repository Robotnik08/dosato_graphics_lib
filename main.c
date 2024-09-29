#include "main.h"

#define MAX_WINDOWS 1024
#define MAX_TEXTURES 2048


DosatoFunctionMapList functions;

SDL_Window* instance_windows[MAX_WINDOWS];
SDL_Renderer* instance_renderers[MAX_WINDOWS];
SDL_Texture* textures[MAX_TEXTURES];
int window_count;
int texture_count;

bool key_pressed[SDL_NUM_SCANCODES];
bool key_down[SDL_NUM_SCANCODES];
bool key_released[SDL_NUM_SCANCODES];

#define SDL_MOUSEBUTTONS 6

bool mouse_pressed[SDL_MOUSEBUTTONS];
bool mouse_down[SDL_MOUSEBUTTONS];
bool mouse_released[SDL_MOUSEBUTTONS];

bool quit = false;


void init() {
    // SDL2 initialization
    SDL_Init(SDL_INIT_EVERYTHING);

    // Initialize counts
    window_count = 0;
    texture_count = 0;

    // Initialize the functions list
    init_DosatoFunctionMapList(&functions);

    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "createWindow",
        .function = createWindow
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "closeWindow",
        .function = closeWindow
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "updateState",
        .function = updateState
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getKey",
        .function = getKey
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getKeyDown",
        .function = getKeyDown
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getKeyReleased",
        .function = getKeyReleased
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "ifQuit",
        .function = ifQuit
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "setColor",
        .function = setColor
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "clearWindow",
        .function = clearScreen
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "render",
        .function = renderWindow
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "drawSquare",
        .function = drawSquare
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "createSprite",
        .function = createSprite
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getSpriteSize",
        .function = getSpriteDimensions
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "drawSprite",
        .function = drawSprite
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "drawLine",
        .function = drawLine
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getMousePosition",
        .function = getMousePosition
    }); 
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getMouse",
        .function = getMouseHeld
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getMouseDown",
        .function = getMousePressed
    });
    write_DosatoFunctionMapList(&functions, (DosatoFunctionMap) {
        .name = "getMouseReleased",
        .function = getMouseReleased
    });
}

Value createWindow(ValueArray args, bool debug) {
    if (args.count != 3) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    if (window_count >= MAX_WINDOWS) {
        printf("ERROR:\nMaximum number of windows reached.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value title = GET_ARG_COPY(args, 0);
    Value width = GET_ARG_COPY(args, 1);
    Value height = GET_ARG_COPY(args, 2);

    CAST_SAFE(title, TYPE_STRING);
    CAST_SAFE(width, TYPE_INT);
    CAST_SAFE(height, TYPE_INT);

    instance_windows[window_count] = SDL_CreateWindow(
        title.as.stringValue,
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width.as.intValue,
        height.as.intValue,
        SDL_WINDOW_SHOWN
    );

    if (instance_windows[window_count] == NULL) {
        printf("ERROR:\nCould not create window.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    instance_renderers[window_count] = SDL_CreateRenderer(instance_windows[window_count], -1, SDL_RENDERER_ACCELERATED);

    window_count++;

    destroyValue(&title);

    return BUILD_VALUE(TYPE_INT, intValue, window_count - 1);
}

Value closeWindow(ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    SDL_DestroyWindow(instance_windows[window_id.as.intValue]);
    SDL_DestroyRenderer(instance_renderers[window_id.as.intValue]);

    for (int i = window_id.as.intValue; i < window_count - 1; i++) {
        instance_windows[i] = instance_windows[i + 1];
        instance_renderers[i] = instance_renderers[i + 1];
    }

    window_count--;

    return UNDEFINED_VALUE;
}

Value updateState (ValueArray args, bool debug) {
    if (args.count != 0) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    SDL_Event event;

    for (int i = 0; i < SDL_NUM_SCANCODES; i++) {
        key_pressed[i] = false;
        key_released[i] = false;
    }

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYDOWN:
                // check if key is repeat (holding down)
                if (event.key.repeat) {
                    break;
                }
                key_pressed[event.key.keysym.scancode] = true;
                key_released[event.key.keysym.scancode] = false;
                key_down[event.key.keysym.scancode] = true;
                break;
            case SDL_KEYUP:
                key_pressed[event.key.keysym.scancode] = false;
                key_released[event.key.keysym.scancode] = true;
                key_down[event.key.keysym.scancode] = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                mouse_pressed[event.button.button] = true;
                mouse_released[event.button.button] = false;
                mouse_down[event.button.button] = true;
                break;
            case SDL_MOUSEBUTTONUP:
                mouse_pressed[event.button.button] = false;
                mouse_released[event.button.button] = true;
                mouse_down[event.button.button] = false;
                break;
            
        }
    }

    return UNDEFINED_VALUE;
}

Value getKey (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value key = GET_ARG_COPY(args, 0);
    CAST_SAFE(key, TYPE_INT);

    if (key.as.intValue < 0 || key.as.intValue >= SDL_NUM_SCANCODES) {
        printf("ERROR:\nInvalid key id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, key_down[key.as.intValue]);
}

Value getKeyDown (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value key = GET_ARG_COPY(args, 0);
    CAST_SAFE(key, TYPE_INT);

    if (key.as.intValue < 0 || key.as.intValue >= SDL_NUM_SCANCODES) {
        printf("ERROR:\nInvalid key id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, key_pressed[key.as.intValue]);
}

Value getKeyReleased (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value key = GET_ARG_COPY(args, 0);
    CAST_SAFE(key, TYPE_INT);

    if (key.as.intValue < 0 || key.as.intValue >= SDL_NUM_SCANCODES) {
        printf("ERROR:\nInvalid key id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, key_released[key.as.intValue]);
}

Value ifQuit(ValueArray args, bool debug) {
    if (args.count != 0) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, quit);
}

Value setColor(ValueArray args, bool debug) {
    if (args.count != 5) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value r = GET_ARG_COPY(args, 1);
    Value g = GET_ARG_COPY(args, 2);
    Value b = GET_ARG_COPY(args, 3);
    Value a = GET_ARG_COPY(args, 4);

    CAST_SAFE(r, TYPE_INT);
    CAST_SAFE(g, TYPE_INT);
    CAST_SAFE(b, TYPE_INT);
    CAST_SAFE(a, TYPE_INT);

    SDL_SetRenderDrawColor(instance_renderers[window_id.as.intValue], r.as.intValue, g.as.intValue, b.as.intValue, a.as.intValue);

    return UNDEFINED_VALUE;
}

Value clearScreen (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    SDL_RenderClear(instance_renderers[window_id.as.intValue]);

    return UNDEFINED_VALUE;
}

Value renderWindow (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    SDL_RenderPresent(instance_renderers[window_id.as.intValue]);

    return UNDEFINED_VALUE;
}


Value drawSquare (ValueArray args, bool debug) {
    if (args.count != 5) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value x = GET_ARG_COPY(args, 1);
    Value y = GET_ARG_COPY(args, 2);
    Value w = GET_ARG_COPY(args, 3);
    Value h = GET_ARG_COPY(args, 4);

    CAST_SAFE(x, TYPE_INT);
    CAST_SAFE(y, TYPE_INT);
    CAST_SAFE(w, TYPE_INT);
    CAST_SAFE(h, TYPE_INT);

    SDL_Rect rect = {x.as.intValue, y.as.intValue, w.as.intValue, h.as.intValue};

    SDL_RenderFillRect(instance_renderers[window_id.as.intValue], &rect);

    return UNDEFINED_VALUE;
}

Value createSprite (ValueArray args, bool debug) {
    if (args.count != 2) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value path = GET_ARG_COPY(args, 1);
    CAST_SAFE(path, TYPE_STRING);

    SDL_Texture* texture = IMG_LoadTexture(instance_renderers[window_id.as.intValue], path.as.stringValue);

    if (texture == NULL) {
        printf("ERROR:\nCould not create texture.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    textures[texture_count] = texture;

    return BUILD_VALUE(TYPE_INT, intValue, texture_count++);
}

Value getSpriteDimensions (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value texture_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(texture_id, TYPE_INT);

    if (texture_id.as.intValue < 0 || texture_id.as.intValue >= texture_count) {
        printf("ERROR:\nInvalid texture id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    int w, h;
    SDL_QueryTexture(textures[texture_id.as.intValue], NULL, NULL, &w, &h);

    return buildObject(
        2,
        "width", BUILD_VALUE(TYPE_INT, intValue, w),
        "height", BUILD_VALUE(TYPE_INT, intValue, h)
    );
}


Value drawSprite (ValueArray args, bool debug) {
    if (args.count != 6) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\n Invalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value texture_id = GET_ARG_COPY(args, 1);
    CAST_SAFE(texture_id, TYPE_INT);

    if (texture_id.as.intValue < 0 || texture_id.as.intValue >= texture_count) {
        printf("ERROR:\n Invalid texture id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value x = GET_ARG_COPY(args, 2);
    Value y = GET_ARG_COPY(args, 3);
    Value w = GET_ARG_COPY(args, 4);
    Value h = GET_ARG_COPY(args, 5);

    CAST_SAFE(x, TYPE_INT);
    CAST_SAFE(y, TYPE_INT);
    CAST_SAFE(w, TYPE_INT);
    CAST_SAFE(h, TYPE_INT);

    SDL_Rect rect = {x.as.intValue, y.as.intValue, w.as.intValue, h.as.intValue};

    SDL_RenderCopy(instance_renderers[window_id.as.intValue], textures[texture_id.as.intValue], NULL, &rect);

    return UNDEFINED_VALUE;
}

Value drawLine (ValueArray args, bool debug) {
    if (args.count != 5) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value window_id = GET_ARG_COPY(args, 0);
    CAST_SAFE(window_id, TYPE_INT);

    if (window_id.as.intValue < 0 || window_id.as.intValue >= window_count) {
        printf("ERROR:\nInvalid window id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    Value x1 = GET_ARG_COPY(args, 1);
    Value y1 = GET_ARG_COPY(args, 2);
    Value x2 = GET_ARG_COPY(args, 3);
    Value y2 = GET_ARG_COPY(args, 4);

    CAST_SAFE(x1, TYPE_INT);
    CAST_SAFE(y1, TYPE_INT);
    CAST_SAFE(x2, TYPE_INT);
    CAST_SAFE(y2, TYPE_INT);

    SDL_RenderDrawLine(instance_renderers[window_id.as.intValue], x1.as.intValue, y1.as.intValue, x2.as.intValue, y2.as.intValue);

    return UNDEFINED_VALUE;
}


Value getMousePosition (ValueArray args, bool debug) {
    if (args.count != 0) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    int x, y;
    SDL_GetMouseState(&x, &y);

    return buildObject(
        2,
        "x", BUILD_VALUE(TYPE_INT, intValue, x),
        "y", BUILD_VALUE(TYPE_INT, intValue, y)
    );
}

Value getMouseHeld (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value button = GET_ARG_COPY(args, 0);
    CAST_SAFE(button, TYPE_INT);

    if (button.as.intValue < 0 || button.as.intValue >= SDL_MOUSEBUTTONS) {
        printf("ERROR:\nInvalid mouse button id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, mouse_down[button.as.intValue]);
}

Value getMousePressed (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value button = GET_ARG_COPY(args, 0);
    CAST_SAFE(button, TYPE_INT);

    if (button.as.intValue < 0 || button.as.intValue >= SDL_MOUSEBUTTONS) {
        printf("ERROR:\nInvalid mouse button id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, mouse_pressed[button.as.intValue]);
}

Value getMouseReleased (ValueArray args, bool debug) {
    if (args.count != 1) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    Value button = GET_ARG_COPY(args, 0);
    CAST_SAFE(button, TYPE_INT);

    if (button.as.intValue < 0 || button.as.intValue >= SDL_MOUSEBUTTONS) {
        printf("ERROR:\nInvalid mouse button id.\n");
        return BUILD_EXCEPTION(E_EMPTY_MESSAGE);
    }

    return BUILD_VALUE(TYPE_BOOL, boolValue, mouse_released[button.as.intValue]);
}