/* transitions.c

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

#include "feh.h"
#include "winwidget.h"
#include "options.h"
#include "transitions.h"

/* Handle transition between frames */
int feh_transition_step(winwidget winwid) {
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

/* Empty placeholder implementations - we'll implement these in later steps */
int feh_transition_step_fade(winwidget winwid) {
    float blend_factor;
    Imlib_Image trans_im;
    
    /* Calculate dimensions */
    int old_w = gib_imlib_image_get_width(winwid->old_im);
    int old_h = gib_imlib_image_get_height(winwid->old_im);
    int new_w = gib_imlib_image_get_width(winwid->im);
    int new_h = gib_imlib_image_get_height(winwid->im);
    int w = winwid->w;
    int h = winwid->h;
    
    /* Validate dimensions */
    if (w <= 0 || h <= 0) {
        /* Invalid size, cancel transition */
        winwid->in_transition = 0;
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
        return 0;
    }
    
    /* Calculate blend factor */
    blend_factor = (winwid->transition_step + 1.0) / opt.transition_steps;
    
    /* Create temporary transition image */
    trans_im = imlib_create_image(w, h);
    if (!trans_im) {
        /* Failed to create image, cancel transition */
        winwid->in_transition = 0;
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
        return 0;
    }
    
    /* Clear image to prevent artifacts */
    gib_imlib_image_fill_rectangle(trans_im, 0, 0, w, h, 0, 0, 0, 255);
    
    /* Calculate positions to center images */
    int old_x = (w - old_w) / 2;
    int old_y = (h - old_h) / 2;
    int new_x = (w - new_w) / 2;
    int new_y = (h - new_h) / 2;
    
    /* Blend old image with decreasing opacity */
    gib_imlib_blend_image_onto_image(trans_im, winwid->old_im, 0, 
            0, 0, old_w, old_h, 
            old_x, old_y, old_w, old_h, 
            1, 1, 0);
    
    /* Blend new image with increasing opacity */
    gib_imlib_blend_image_onto_image(trans_im, winwid->im, 0, 
            0, 0, new_w, new_h, 
            new_x, new_y, new_w, new_h, 
            blend_factor, 1, 0);

    /* Draw a 20x20 colored square at top-right corner */
    /* Generate random RGB values */
    int r = random() % 256;
    int g = random() % 256;
    int b = random() % 256;
    
    /* Draw filled square at top-right (w-20, 0) */
    gib_imlib_image_fill_rectangle(trans_im, w - 20, 0, 20, 20, r, g, b, 255);

    /* Display the transition frame */
    imlib_context_set_image(trans_im);
    gib_imlib_render_image_on_drawable(winwid->bg_pmap, trans_im, 0, 0, 1, 1, 0);
    XSetWindowBackgroundPixmap(disp, winwid->win, winwid->bg_pmap);
    XClearWindow(disp, winwid->win);
    
    /* Free temporary image */
    gib_imlib_free_image_and_decache(trans_im);
    
    /* Increment step counter */
    winwid->transition_step++;
    
    /* Check if transition is complete */
    if (winwid->transition_step >= opt.transition_steps - 1) {
        /* Clean up */
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
        winwid->in_transition = 0;
        winwid->transition_step = 0;
        
        /* Render the final state */
        winwidget_render_image(winwid, 0, 0);
        return 0; /* Transition complete */
    }
    
    return 1; /* Continue transition */
}

int feh_transition_step_slide(winwidget winwid) {
    Imlib_Image trans_im;
    int offset;
    
    /* Calculate dimensions */
    int old_w = gib_imlib_image_get_width(winwid->old_im);
    int old_h = gib_imlib_image_get_height(winwid->old_im);
    int new_w = gib_imlib_image_get_width(winwid->im);
    int new_h = gib_imlib_image_get_height(winwid->im);
    int w = winwid->w;
    int h = winwid->h;
    
    /* Validate dimensions */
    if (w <= 0 || h <= 0) {
        /* Invalid size, cancel transition */
        winwid->in_transition = 0;
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
        return 0;
    }
    
    /* Calculate slide offset for current frame */
    offset = (w * (winwid->transition_step + 1)) / opt.transition_steps;
    
    /* Create temporary transition image */
    trans_im = imlib_create_image(w, h);
    if (!trans_im) {
        /* Failed to create image, cancel transition */
        winwid->in_transition = 0;
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
        return 0;
    }
    
    /* Clear image to prevent artifacts */
    gib_imlib_image_fill_rectangle(trans_im, 0, 0, w, h, 0, 0, 0, 255);
    
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

    /* Draw a 20x20 colored square at top-right corner */
    /* Generate random RGB values */
    int r = random() % 256;
    int g = random() % 256;
    int b = random() % 256;
    
    /* Draw filled square at top-right (w-20, 0) */
    gib_imlib_image_fill_rectangle(trans_im, w - 20, 0, 20, 20, r, g, b, 255);

    /* Display the transition frame */
    imlib_context_set_image(trans_im);
    gib_imlib_render_image_on_drawable(winwid->bg_pmap, trans_im, 0, 0, 1, 1, 0);
    XSetWindowBackgroundPixmap(disp, winwid->win, winwid->bg_pmap);
    XClearWindow(disp, winwid->win);
    
    /* Free temporary image */
    gib_imlib_free_image_and_decache(trans_im);
    
    /* Increment step counter */
    winwid->transition_step++;
    
    /* Check if transition is complete */
    if (winwid->transition_step >= opt.transition_steps - 1) {
        /* Clean up */
        gib_imlib_free_image_and_decache(winwid->old_im);
        winwid->old_im = NULL;
        winwid->in_transition = 0;
        winwid->transition_step = 0;
        
        /* Render the final state */
        winwidget_render_image(winwid, 0, 0);
        return 0; /* Transition complete */
    }
    
    return 1; /* Continue transition */
}
