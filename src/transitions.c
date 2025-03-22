/* transitions.c

Copyright (C) 1999-2003 Tom Gilbert.
Copyright (C) 2010-2025 Birte Kristina Friesel.

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

#include "feh.h"
#include "winwidget.h"
#include "options.h"
#include "transitions.h"
#include "debug.h"

/* Handle transition between frames */
int feh_transition_step(winwidget winwid)
{
    /* Validate inputs */
    if (!winwid || !winwid->im || !winwid->old_im || !winwid->in_transition) {
        /* Clean up and cancel transition if invalid */
        if (winwid && winwid->old_im) {
            gib_imlib_free_image_and_decache(winwid->old_im);
            winwid->old_im = NULL;
        }
        if (winwid) {
            winwid->in_transition = 0;
            winwid->transition_step = 0;
        }
        return 0;
    }
    
    /* Process specific transition type */
    if (winwid->transition_type == TRANSITION_FADE)
        return feh_transition_step_fade(winwid);
    else if (winwid->transition_type == TRANSITION_SLIDE)
        return feh_transition_step_slide(winwid);
    
    /* Unknown transition, cancel */
    winwid->in_transition = 0;
    gib_imlib_free_image_and_decache(winwid->old_im);
    winwid->old_im = NULL;
    return 0;
}

/* Placeholder functions - will be implemented in later steps */
int feh_transition_step_fade(winwidget winwid)
{
    /* Will be implemented in Step 3 */
    D(("Fade transition step %d\n", winwid->transition_step));
    return 0;
}

int feh_transition_step_slide(winwidget winwid)
{
    /* Will be implemented in Step 4 */
    D(("Slide transition step %d\n", winwid->transition_step));
    return 0;
}