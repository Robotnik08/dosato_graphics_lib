#include "main.h"

#define MAX_WINDOWS 1024


DosatoFunctionMapList functions;

SDL_Window* instance_windows[MAX_WINDOWS];
SDL_Renderer* instance_renderers[MAX_WINDOWS];
int window_count;

bool key_pressed[SDL_NUM_SCANCODES];
bool key_down[SDL_NUM_SCANCODES];
bool key_released[SDL_NUM_SCANCODES];

bool quit = false;


void init() {
    // SDL2 initialization
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init Error: %s\n", SDL_GetError());
        exit(1);
    }

    window_count = 0;

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
}

Value createWindow(ValueArray args, bool debug) {
    if (args.count != 3) {
        return BUILD_EXCEPTION(E_WRONG_NUMBER_OF_ARGUMENTS);
    }

    if (window_count >= MAX_WINDOWS) {
        printf("ERROR:\n Maximum number of windows reached.\n");
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
        printf("ERROR:\n Could not create window.\n");
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
        printf("ERROR:\n Invalid window id.\n");
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
        printf("ERROR:\n Invalid key id.\n");
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
        printf("ERROR:\n Invalid key id.\n");
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
        printf("ERROR:\n Invalid key id.\n");
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
        printf("ERROR:\n Invalid window id.\n");
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
        printf("ERROR:\n Invalid window id.\n");
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
        printf("ERROR:\n Invalid window id.\n");
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
        printf("ERROR:\n Invalid window id.\n");
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