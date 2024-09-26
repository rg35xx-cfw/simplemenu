#include <stddef.h>
#include <SDL/SDL_events.h>
#include <SDL/SDL_keyboard.h>

#include "../headers/globals.h"

SDL_Event event;

int pollEvent() {
	return SDL_WaitEvent(&event);
}

int getEventType() {
	return event.type;
}

int isLeftOrRight() {
	return event.jaxis.axis == 0;
}

int isUp() {
	const int JOYSTICK_DEAD_ZONE = 0;
	return (event.jaxis.axis == 1&&event.jaxis.value < JOYSTICK_DEAD_ZONE);
}

int isDown() {
	const int JOYSTICK_DEAD_ZONE = 0;
	return (event.jaxis.axis == 1&&event.jaxis.value > JOYSTICK_DEAD_ZONE);
}

int initializeJoystick() {
    // Initialize joystick
    if (SDL_NumJoysticks() > 0) {
        joystick = SDL_JoystickOpen(0);
        if (joystick == NULL) {
            printf("Failed to open joystick: %s\n", SDL_GetError());
            return 1;
        }

        printf("Joystick Name: %s\n", SDL_JoystickName(0));
        printf("Number of Axes: %d\n", SDL_JoystickNumAxes(joystick));
        printf("Number of Buttons: %d\n", SDL_JoystickNumButtons(joystick));
    }

    // SDL_Event event;
    // while (SDL_PollEvent(&event)) {
    //     // We are just polling all events to remove the initial joystick events from the queue.
    // }
    // printf("Finished setting up joystick\n");

    return 0;
}

int getPressedKey() {
	return event.key.keysym.sym;
}

int getJoystickPressedDirection() {
	return event.jhat.value;
}

int getKeyDown() {
	return SDL_KEYDOWN;
}

int getKeyUp() {
	return SDL_KEYUP;
}

int getJoystickMotion() {
	return SDL_JOYAXISMOTION;
}

void enableKeyRepeat() {
	SDL_EnableKeyRepeat(250,80);
}

void initializeKeys() {
	keys = SDL_GetKeyState(NULL);
}

void pumpEvents() {
	SDL_PumpEvents();
}

