/* transitions.c

Copyright (C) 2025 Your Name Here.

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

#define TRANSITION_STEPS 5  /* Number of steps for transition effects */

/* Initialize a transition */
void feh_transition_init(winwidget winwid)
{
    if (!winwid || !winwid->im || !winwid->old_im) {
        D(("Invalid parameters for transition init\n"));
        return;
    }

    winwid->transition_step = 0;
    winwid->in_transition = 1;
    
    D(("Transition initialized: type=%d\n", winwid->transition_type));
}

/* Process a single step of a transition */
int feh_transition_process(winwidget winwid)
{
    Imlib_Image trans_im;
    int w, h;
    
    /* Validate parameters */
    if (!winwid || !winwid->im || !winwid->old_im || !winwid->in_transition) {
        D(("Invalid parameters for transition process\n"));
        feh_transition_cleanup(winwid);
        return 0;
    }
    
    w = winwid->w;
    h = winwid->h;
    
    /* If image dimensions are invalid, cancel */
    if (w <= 0 || h <= 0) {
        D(("Invalid dimensions for transition\n"));
        feh_transition_cleanup(winwid);
        return 0;
    }
    
    /* Create temporary image for transition */
    trans_im = imlib_create_image(w, h);
    if (!trans_im) {
        D(("Failed to create transition image\n"));
        feh_transition_cleanup(winwid);
        return 0;
    }
    
    /* Fill with background color to prevent artifacts */
    gib_imlib_image_fill_rectangle(trans_im, 0, 0, w, h, 0, 0, 0, 255);
    
    /* Process according to transition type */
    if (winwid->transition_type == TRANSITION_FADE) {
        /* Fade transition: blend old and new images with changing alpha */
        float blend_factor = (float)(winwid->transition_step + 1) / (TRANSITION_STEPS + 1);
        
        int old_w = gib_imlib_image_get_width(winwid->old_im);
        int old_h = gib_imlib_image_get_height(winwid->old_im);
        int new_w = gib_imlib_image_get_width(winwid->im);
        int new_h = gib_imlib_image_get_height(winwid->im);
        
        /* Calculate positions to center images */
        int old_x = (w - old_w) / 2;
        int old_y = (h - old_h) / 2;
        int new_x = (w - new_w) / 2;
        int new_y = (h - new_h) / 2;
        
        /* Blend old image with decreasing opacity */
        gib_imlib_blend_image_onto_image(trans_im, winwid->old_im, 0,
                0, 0, old_w, old_h,
                old_x, old_y, old_w, old_h,
                1.0 - blend_factor, 1, 0);
        
        /* Blend new image with increasing opacity */
        gib_imlib_blend_image_onto_image(trans_im, winwid->im, 0,
                0, 0, new_w, new_h,
                new_x, new_y, new_w, new_h,
                blend_factor, 1, 0);
        
    } else if (winwid->transition_type == TRANSITION_SLIDE) {
        /* Slide transition: move old image out and new image in */
        int offset = (w * (winwid->transition_step + 1)) / (TRANSITION_STEPS + 1);
        
        int old_w = gib_imlib_image_get_width(winwid->old_im);
        int old_h = gib_imlib_image_get_height(winwid->old_im);
        int new_w = gib_imlib_image_get_width(winwid->im);
        int new_h = gib_imlib_image_get_height(winwid->im);
        
        /* Calculate vertical centering */
        int old_y = (h - old_h) / 2;
        int new_y = (h - new_h) / 2;
        
        /* Draw old image sliding left */
        gib_imlib_blend_image_onto_image(trans_im, winwid->old_im, 0,
                0, 0, old_w, old_h,
                -offset, old_y, old_w, old_h,
                1, 1, 0);
        
        /* Draw new image sliding in from right */
        gib_imlib_blend_image_onto_image(trans_im, winwid->im, 0,
                0, 0, new_w, new_h,
                w - offset, new_y, new_w, new_h,
                1, 1, 0);
    } else {
        /* Unknown transition type */
        D(("Unknown transition type: %d\n", winwid->transition_type));
        gib_imlib_free_image_and_decache(trans_im);
        feh_transition_cleanup(winwid);
        return 0;
    }
    
    /* Display the transition frame */
    imlib_context_set_image(trans_im);
    gib_imlib_render_image_on_drawable(winwid->bg_pmap, trans_im, 0, 0, 1, 1, 0);
    XSetWindowBackgroundPixmap(disp, winwid->win, winwid->bg_pmap);
    XClearWindow(disp, winwid->win);
    
    /* Free temporary image */
    gib_imlib_free_image_and_decache(trans_im);
    
    /* Increment transition step */
    winwid->transition_step++;
    
    /* Check if transition is complete */
    if (winwid->transition_step >= TRANSITION_STEPS) {
        D(("Transition complete\n"));
        feh_transition_cleanup(winwid);
        return 0;
    }
    
    return 1;  /* Continue transition */
}

/* Clean up transition resources */
void feh_transition_cleanup(winwidget winwid)
{
    if (!winwid) {
        return;
    }
    
    if (winwid->old_im) {
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
    }
    
    winwid->in_transition = 0;
    winwid->transition_step = 0;
}