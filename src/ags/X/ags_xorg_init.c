/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_xorg_init.h>

#include <ags/audio/ags_audio_init.h>

#include <ags/widget/ags_gui_init.h>

void ags_xorg_colors_alloc();

extern GtkStyle *matrix_style;
extern GtkStyle *ffplayer_style;
extern GtkStyle *editor_style;
extern GtkStyle *notebook_style;
extern GtkStyle *meter_style;
extern GtkStyle *note_edit_style;

void
ags_xorg_colors_alloc()
{
  static gboolean allocated = FALSE;

  if(!allocated){
    allocated = TRUE;

    matrix_style = gtk_style_new();
    matrix_style->fg[0].red = 120 * (65535/255);
    matrix_style->fg[0].green = 220 * (65535/255);
    matrix_style->fg[0].blue = 120 * (65535/255);
    matrix_style->fg[0].pixel = (gulong)(120*65536 + 220*256 + 120);

    matrix_style->light[0].red = 100 * (65535/255);
    matrix_style->light[0].green = 200 * (65535/255);
    matrix_style->light[0].blue = 100 * (65535/255);
    matrix_style->light[0].pixel = (gulong)(100*65536 + 200*256 + 100);

    matrix_style->bg[0].red = 0 * (65535/255);
    matrix_style->bg[0].green = 80 * (65535/255);
    matrix_style->bg[0].blue = 0 * (65535/255);
    matrix_style->bg[0].pixel = (gulong)(0*65536 + 80*256 + 0);

    matrix_style->bg[1].red = 120 * (65535/255);
    matrix_style->bg[1].green = 220 * (65535/255);
    matrix_style->bg[1].blue = 120 * (65535/255);
    matrix_style->bg[1].pixel = (gulong)(120*65536 + 220*256 + 120);

    matrix_style->bg[2].red = 120 * (65535/255);
    matrix_style->bg[2].green = 220 * (65535/255);
    matrix_style->bg[2].blue = 120 * (65535/255);
    matrix_style->bg[2].pixel = (gulong)(120*65536 + 220*256 + 120);

    matrix_style->base[0].red = 120 * (65535/255);
    matrix_style->base[0].green = 220 * (65535/255);
    matrix_style->base[0].blue = 120 * (65535/255);
    matrix_style->base[0].pixel = (gulong)(120*65536 + 220*256 + 120);


    /*  matrix_style->fg[1] = matrix_green;
	matrix_style->fg[2] = matrix_green;
	matrix_style->fg[3] = matrix_green;
	matrix_style->fg[4] = matrix_green;

	matrix_style->bg[0] = matrix_led_bg;
	matrix_style->bg[1] = matrix_led_bg;
	matrix_style->bg[2] = matrix_led_bg;
	matrix_style->bg[2] = matrix_led_bg;
	matrix_style->bg[3] = matrix_led_bg;
	matrix_style->bg[4] = matrix_led_bg;
    */

    /* ffplayer style */
    ffplayer_style = gtk_style_new();
    ffplayer_style->fg[0].red = 180 * (65535/255);
    ffplayer_style->fg[0].green = 180 * (65535/255);
    ffplayer_style->fg[0].blue = 250 * (65535/255);
    ffplayer_style->fg[0].pixel = (gulong)(120*65536 + 120*256 + 220);

    ffplayer_style->bg[0].red = 4 * (65535/255);
    ffplayer_style->bg[0].green = 8 * (65535/255);
    ffplayer_style->bg[0].blue = 56 * (65535/255);
    ffplayer_style->bg[0].pixel = (gulong)(4*65536 + 8*256 + 56);

    ffplayer_style->bg[1].red = 120 * (65535/255);
    ffplayer_style->bg[1].green = 120 *(65535/255);
    ffplayer_style->bg[1].blue = 120 * (65535/255);
    ffplayer_style->bg[1].pixel = (gulong)(120*65536 + 120*256 + 120);

    ffplayer_style->bg[2].red = 120 * (65535/255);
    ffplayer_style->bg[2].green = 120 *(65535/255);
    ffplayer_style->bg[2].blue = 120 * (65535/255);
    ffplayer_style->bg[2].pixel = (gulong)(255*65536 + 255*256 + 255);

    ffplayer_style->mid[0].red = 4 * (65535/255);
    ffplayer_style->mid[0].green = 8 * (65535/255);
    ffplayer_style->mid[0].blue = 56 * (65535/255);
    ffplayer_style->mid[0].pixel = (gulong)(4*65536 + 8*256 + 56);

    ffplayer_style->light[0].red = 120 * (65535/255);
    ffplayer_style->light[0].green = 120 * (65535/255);
    ffplayer_style->light[0].blue = 220 * (65535/255);
    ffplayer_style->light[0].pixel = (gulong)(120*65536 + 120*256 + 220);

    ffplayer_style->base[0].red = 120 * (65535/255);
    ffplayer_style->base[0].green = 120 * (65535/255);
    ffplayer_style->base[0].blue = 220 * (65535/255);
    ffplayer_style->base[0].pixel = (gulong)(120*65536 + 120*256 + 220);


    /* editor style */
    editor_style = gtk_style_new();
    editor_style->fg[0].red = 255 * (65535/255);
    editor_style->fg[0].green = 240 *(65535/255);
    editor_style->fg[0].blue = 200 * (65535/255);
    editor_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    editor_style->bg[0].red = 255 * (65535/255);
    editor_style->bg[0].green = 255 *(65535/255);
    editor_style->bg[0].blue = 100 * (65535/255);
    editor_style->bg[0].pixel = (gulong)(255*65536 + 255*256 + 255);

    editor_style->mid[0].red = 250 * (65535/255);
    editor_style->mid[0].green = 0 *(65535/255);
    editor_style->mid[0].blue = 250 * (65535/255);
    editor_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);


    editor_style->base[0].red = 250 * (65535/255);
    editor_style->base[0].green = 250 *(65535/255);
    editor_style->base[0].blue = 250 * (65535/255);
    editor_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);


    /* notebook style */
    notebook_style = gtk_style_new();
    notebook_style->fg[0].red = 255 * (65535/255);
    notebook_style->fg[0].green = 255 *(65535/255);
    notebook_style->fg[0].blue = 0 * (65535/255);
    notebook_style->fg[0].pixel = (gulong)(255*65536 + 255*256 + 0);

    notebook_style->bg[0].red = 40 * (65535/255);
    notebook_style->bg[0].green = 40 *(65535/255);
    notebook_style->bg[0].blue = 40 * (65535/255);
    notebook_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    notebook_style->mid[0].red = 10 * (65535/255);
    notebook_style->mid[0].green = 20 *(65535/255);
    notebook_style->mid[0].blue = 230 * (65535/255);
    notebook_style->mid[0].pixel = (gulong)(10*65536 + 20*256 + 230);

    notebook_style->dark[0].red = 192 * (65535/255);
    notebook_style->dark[0].green = 192 *(65535/255);
    notebook_style->dark[0].blue = 0 * (65535/255);
    notebook_style->dark[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    notebook_style->light[0].red = 255 * (65535/255);
    notebook_style->light[0].green = 255 *(65535/255);
    notebook_style->light[0].blue = 63 * (65535/255);
    notebook_style->light[0].pixel = (gulong)(255*65536 + 255*256 + 63);

    notebook_style->base[0].red = 0 * (65535/255);
    notebook_style->base[0].green = 0 *(65535/255);
    notebook_style->base[0].blue = 0 * (65535/255);
    notebook_style->base[0].pixel = (gulong)(0*65536 + 0*256 + 0);


    /* ruler style */
    //TODO:JK: deprecated
    //    ruler_style = gtk_style_new();
    //    ruler_style->fg[0].red = 255 * (65535/255);
    //    ruler_style->fg[0].green = 240 *(65535/255);
    //    ruler_style->fg[0].blue = 200 * (65535/255);
    //    ruler_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    //    ruler_style->bg[0].red = 40 * (65535/255);
    //    ruler_style->bg[0].green = 40 *(65535/255);
    //    ruler_style->bg[0].blue = 40 * (65535/255);
    //    ruler_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    //    ruler_style->mid[0].red = 250 * (65535/255);
    //    ruler_style->mid[0].green = 0 *(65535/255);
    //    ruler_style->mid[0].blue = 250 * (65535/255);
    //    ruler_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    //    ruler_style->base[0].red = 250 * (65535/255);
    //    ruler_style->base[0].green = 250 *(65535/255);
    //    ruler_style->base[0].blue = 250 * (65535/255);
    //    ruler_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);


    /* meter style */
    meter_style = gtk_style_new();
    meter_style->fg[0].red = 100 * (65535/255);
    meter_style->fg[0].green = 200 *(65535/255);
    meter_style->fg[0].blue = 255 * (65535/255);
    meter_style->fg[0].pixel = (gulong)(100*65536 + 200*256 + 255);

    meter_style->bg[0].red = 40 * (65535/255);
    meter_style->bg[0].green = 40 *(65535/255);
    meter_style->bg[0].blue = 40 * (65535/255);
    meter_style->bg[0].pixel = (gulong)(40*65536 + 40*256 + 40);

    meter_style->mid[0].red = 10 * (65535/255);
    meter_style->mid[0].green = 20 *(65535/255);
    meter_style->mid[0].blue = 230 * (65535/255);
    meter_style->mid[0].pixel = (gulong)(10*65536 + 20*256 + 230);

    meter_style->light[0].red = 0 * (65535/255);
    meter_style->light[0].green = 150 *(65535/255);
    meter_style->light[0].blue = 255 * (65535/255);
    meter_style->light[0].pixel = (gulong)(150*65536 + 150*256 + 250);

    meter_style->base[0].red = 0 * (65535/255);
    meter_style->base[0].green = 0 *(65535/255);
    meter_style->base[0].blue = 0 * (65535/255);
    meter_style->base[0].pixel = (gulong)(0*65536 + 0*256 + 0);


    /* note_edit style */
    note_edit_style = gtk_style_new();
    note_edit_style->fg[0].red = 255 * (65535/255);
    note_edit_style->fg[0].green = 240 *(65535/255);
    note_edit_style->fg[0].blue = 200 * (65535/255);
    note_edit_style->fg[0].pixel = (gulong)(255*65536 + 240*256 + 200);

    note_edit_style->bg[0].red = 255 * (65535/255);
    note_edit_style->bg[0].green = 255 *(65535/255);
    note_edit_style->bg[0].blue = 100 * (65535/255);
    note_edit_style->bg[0].pixel = (gulong)(255*65536 + 255*256 + 255);

    note_edit_style->mid[0].red = 250 * (65535/255);
    note_edit_style->mid[0].green = 0 *(65535/255);
    note_edit_style->mid[0].blue = 250 * (65535/255);
    note_edit_style->mid[0].pixel = (gulong)(150*65536 + 150*256 + 250);


    note_edit_style->base[0].red = 250 * (65535/255);
    note_edit_style->base[0].green = 250 *(65535/255);
    note_edit_style->base[0].blue = 250 * (65535/255);
    note_edit_style->base[0].pixel = (gulong)(250*65536 + 250*256 + 250);
  }
}

void
ags_xorg_init_context(AgsApplicationContext *application_context)
{  
  /* map x11 collors */
  ags_xorg_colors_alloc();
}

