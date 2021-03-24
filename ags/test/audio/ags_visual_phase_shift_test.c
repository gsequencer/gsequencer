/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
 *
 * This file is part of GSequencer.
 *
 * GSequencer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * GSequencer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GSequencer.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <cairo.h>

#include <math.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

void ags_visual_phase_shift_putpixel(unsigned char *data, int x, int y, unsigned long int pixel);

void ags_visual_phase_shift_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
gboolean ags_visual_phase_shift_orig_wave_draw(GtkWidget *widget,
					       cairo_t *cr,
					       gpointer user_data);
gboolean ags_visual_phase_shift_phase_shifted_wave_draw(GtkWidget *widget,
							cairo_t *cr,
							gpointer user_data);


GtkWindow *window;
GtkDrawingArea *orig_wave;
GtkDrawingArea *phase_shifted_wave;

cairo_surface_t *orig_surface = NULL;
cairo_surface_t *phase_shifted_surface = NULL;

guint WHITE_PIXEL = 0xffffff;
guint BLACK_PIXEL = 0x0;

gint STRIDE;

gdouble orig_buffer[1920];
gdouble *phase_shifted_buffer;

void
ags_visual_phase_shift_putpixel(unsigned char *data, int x, int y, unsigned long int pixel)
{
  int bpp = 4; // STRIDE / 400;
  /* Here p is the address to the pixel we want to set */
  unsigned char *p = data + y * STRIDE + x * bpp;

  if(x < 0 ||
     y < 0 ||
     x >= 1920 ||
     y >= 200){
    return;
  }
  
  switch(bpp) {
  case 1:
    *p = pixel;
    break;

  case 2:
    *(unsigned short int *)p = pixel;
    break;

  case 3:
      p[0] = pixel & 0xff;
      p[1] = (pixel >> 8) & 0xff;
      p[2] = (pixel >> 16) & 0xff;
    break;

  case 4:
    *(unsigned long int *)p = pixel;
    break;
  }
}


void
ags_visual_phase_shift_window_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
  /* leave main loop */
  gtk_main_quit();
}

gboolean
ags_visual_phase_shift_orig_wave_draw(GtkWidget *widget,
				      cairo_t *cr,
				      gpointer user_data)
{
  unsigned char *data;

  double value;
  int i;

  data = cairo_image_surface_get_data(orig_surface);

  memset(data, 0xff, 4 * 1920 * 200 * sizeof(guchar));

  for(i = 0; i < 1920; i++){
    value = orig_buffer[i] * 100.0;    

    ags_visual_phase_shift_putpixel(data, i, 100 + (int) floor(value), BLACK_PIXEL);
  }

  cairo_surface_flush(orig_surface);

  /* do painting */
  cairo_set_source_surface(cr, orig_surface, 0, 0);
  cairo_surface_mark_dirty(orig_surface);

  cairo_paint(cr);
    
  return(FALSE);
}

gboolean
ags_visual_phase_shift_phase_shifted_wave_draw(GtkWidget *widget,
					       cairo_t *cr,
					       gpointer user_data)
{
  guchar *data;

  double value;
  int i;

  data = cairo_image_surface_get_data(phase_shifted_surface);

  memset(data, 0xff, 4 * 1920 * 200 * sizeof(guchar));
  
  for(i = 0; i < 1920; i++){
    value = phase_shifted_buffer[i] * 100.0;    

    ags_visual_phase_shift_putpixel(data, i, 100 + (int) floor(value), BLACK_PIXEL);
  }

  cairo_surface_flush(phase_shifted_surface);

  /* do painting */
  cairo_set_source_surface(cr, phase_shifted_surface, 0, 0);
  cairo_surface_mark_dirty(phase_shifted_surface);

  cairo_paint(cr);

  return(FALSE);
}

int
main(int argc, char* argv[])
{
  GtkBox *vbox;

  guint i;
  
  gtk_init(&argc, &argv);
  
  for(i = 0; i < 1920; i++){
    orig_buffer[i] = sin(((double) i / (1920.0 / 8.0)) * 2.0 * M_PI);
  }

  phase_shifted_buffer = NULL;
  
  ags_phase_shift_util_compute_double(orig_buffer,
				      1920,
				      1920,
				      8.0,
				      2 * M_PI,
				      &phase_shifted_buffer);
  
  window = (GtkWindow *) gtk_window_new(GTK_WINDOW_TOPLEVEL);

  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				0);
  gtk_container_add((GtkContainer *) window,
		    (GtkWidget *) vbox);

  orig_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
					    1920, 200);
  phase_shifted_surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24,
						     1920, 200);

  STRIDE = cairo_image_surface_get_stride(orig_surface);
  
  orig_wave = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_size_request((GtkWidget *) orig_wave,
			      1920, 200);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) orig_wave,
		     FALSE, FALSE,
		     0);
  
  phase_shifted_wave = (GtkDrawingArea *) gtk_drawing_area_new();
  gtk_widget_set_size_request((GtkWidget *) phase_shifted_wave,
			      1920, 200);
  gtk_box_pack_start(vbox,
		     (GtkWidget *) phase_shifted_wave,
		     FALSE, FALSE,
		     0);

  g_signal_connect(window, "delete-event",
		   G_CALLBACK(ags_visual_phase_shift_window_delete_event), NULL);
  
  g_signal_connect(orig_wave, "draw",
		   G_CALLBACK(ags_visual_phase_shift_orig_wave_draw), NULL);

  g_signal_connect(phase_shifted_wave, "draw",
		   G_CALLBACK(ags_visual_phase_shift_phase_shifted_wave_draw), NULL);

  gtk_widget_show_all((GtkWidget *) window);
  
  gtk_main();
  
  return(0);
}
