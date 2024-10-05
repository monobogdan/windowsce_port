/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

/*
 *	Mouse vector
 *
 *	Patrice Mandin
 */

#ifndef _SDL_XBIOSINTERRUPT_S_H_
#define _SDL_XBIOSINTERRUPT_S_H_

#include "SDL_stdinc.h"

/* Variables */

extern Uint16 SDL_AtariXbios_installmousevector;	/* flag for SDL_AtariXbios_Install() */
extern volatile Uint16 SDL_AtariXbios_mouselock;	/* mouse lock position */
extern volatile Uint16 SDL_AtariXbios_mouseb;	/* buttons */
extern volatile Sint16 SDL_AtariXbios_mousex;	/* X relative motion */
extern volatile Sint16 SDL_AtariXbios_mousey;	/* Y relative motion */
extern Uint16 SDL_AtariXbios_installjoystickvector;	/* flag for SDL_AtariXbios_Install() */
extern volatile Uint16 SDL_AtariXbios_joystick;	/* Joystick */
extern Uint16 SDL_AtariXbios_installkeyboardvector;	/* flag for SDL_AtariXbios_Install() */
extern volatile Uint8 SDL_AtariXbios_keyboard[128];	/* Keyboard table */

/* Functions */ 

extern void SDL_AtariXbios_Install(void);
extern void SDL_AtariXbios_Restore(void);
extern void SDL_AtariXbios_MouseVector(void *buf);
extern void SDL_AtariXbios_JoystickVector(void *buf);
extern void SDL_AtariXbios_KeyboardVector(void *buf);

#endif /* _SDL_XBIOSINTERRUPT_S_H_ */
