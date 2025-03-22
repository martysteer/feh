/* transitions.h

Copyright (C) 2023 Implementation Plan Author.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef TRANSITIONS_H
#define TRANSITIONS_H

enum transition_type {
    TRANSITION_NONE = 0,
    TRANSITION_FADE,
    TRANSITION_SLIDE,
    TRANSITION_GLITCH
};

/* Forward declare winwidget to avoid circular dependency */
struct __winwidget;
typedef struct __winwidget _winwidget;
typedef _winwidget *winwidget;

/* Process a transition frame */
int feh_transition_step(winwidget winwid);

/* Specific transition functions */
int feh_transition_step_fade(winwidget winwid);
int feh_transition_step_slide(winwidget winwid);

#endif /* TRANSITIONS_H */