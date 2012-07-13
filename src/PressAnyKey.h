/* ---------------------------------------------------------------------------
 * PressAnyKey()
 * ---------------------------------------------------------------------------
 * Copyright 2008 Michael Thomas Greer
 * http://www.boost.org/LICENSE_1_0.txt
 *
 * function
 *   Optionally print a message and and wait for the user to press (and
 *   release) a single key.
 *
 * arguments
 *   The message to print. If NULL, uses a default message. Specify the empty
 *   string "" to not print anything.
 *
 * returns
 *   The keycode for the key that was pressed.
 *
 *   Extended key codes (like arrow keys) are properly handled, but their
 *   keycode is not understood; they are simply returned as the last code in
 *   the sequence, negated. For example, it is likely that the arrow keys are:
 *
 *     UP_ARROW    = -'A' = -65
 *     DOWN_ARROW  = -'B' = -66
 *     RIGHT_ARROW = -'C' = -67
 *     LEFT_ARROW  = -'D' = -68
 *
 *   Exactly identifying the values for these keys requires a foray into the
 *   terminfo database, which is a subject for later. For now we'll leave it
 *   at this.
 */
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

int PressAnyKey( const char* prompt )
  {
  #define MAGIC_MAX_CHARS 18
  struct termios initial_settings;
  struct termios settings;
  unsigned char  keycodes[ MAGIC_MAX_CHARS ];
  int            count;

  tcgetattr( STDIN_FILENO, &initial_settings );
  settings = initial_settings;

  /* Set the console mode to no-echo, raw input. */
  /* The exact meaning of all this jazz will be discussed later. */
  settings.c_cc[ VTIME ] = 1;
  settings.c_cc[ VMIN  ] = MAGIC_MAX_CHARS;
  settings.c_iflag &= ~(IXOFF);
  settings.c_lflag &= ~(ECHO | ICANON);
  tcsetattr( STDIN_FILENO, TCSANOW, &settings );

  printf( "%s", prompt ? prompt : "Press a key to continue..." );
  count = read( STDIN_FILENO, (void*)keycodes, MAGIC_MAX_CHARS );

  tcsetattr( STDIN_FILENO, TCSANOW, &initial_settings );

  return (count == 1)
       ? keycodes[ 0 ]
       : -(int)(keycodes[ count -1 ]);
  }
