/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/machine/ags_ffplayer_callbacks.h>
#include <ags/X/ags_machine_callbacks.h>

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <math.h>

#include <ags/config.h>

void ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
						AgsMachine *machine);

void
ags_ffplayer_parent_set_callback(GtkWidget *widget, GtkWidget *old_parent, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;

  gchar *str;

  if(old_parent != NULL){
    return;
  }

  window = (AgsWindow *) gtk_widget_get_toplevel(widget);
  
  str = g_strdup_printf("Default %d",
			ags_window_find_machine_counter(window, AGS_TYPE_FFPLAYER)->counter);

  g_object_set(AGS_MACHINE(ffplayer),
	       "machine-name", str,
	       NULL);

  ags_window_increment_machine_counter(window,
				       AGS_TYPE_FFPLAYER);

  g_free(str);
}

void
ags_ffplayer_destroy_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  if(ffplayer->open_dialog != NULL){
    gtk_widget_destroy(ffplayer->open_dialog);
  }
}

gboolean
ags_ffplayer_draw_callback(GtkWidget *drawing_area, cairo_t *cr,
			   AgsFFPlayer *ffplayer)
{
  GtkWidget *widget;

  GtkStyleContext *ffplayer_style_context;

  GtkAllocation allocation;
  
  GdkRGBA *fg_color;
  GdkRGBA *bg_color;

  double semi_key_height;
  guint bitmap;
  guint x[2];
  guint i, i_stop, j, j0;
  
  GValue value = {0,};
    
  widget = (GtkWidget *) ffplayer->drawing_area;

  gtk_widget_get_allocation(ffplayer->drawing_area, &allocation);

  /* style context */
  ffplayer_style_context = gtk_widget_get_style_context(GTK_WIDGET(ffplayer->drawing_area));

  gtk_style_context_get_property(ffplayer_style_context,
				 "color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  fg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);

  gtk_style_context_get_property(ffplayer_style_context,
				 "background-color",
				 GTK_STATE_FLAG_NORMAL,
				 &value);

  bg_color = g_value_dup_boxed(&value);
  g_value_unset(&value);
  
  semi_key_height = 2.0 / 3.0 * (double) ffplayer->control_height;
  bitmap = 0x52a52a; // description of the keyboard

  j = (guint) ceil(gtk_adjustment_get_value(ffplayer->hadjustment) / (double) ffplayer->control_width);
  j = j % 12;

  x[0] = (guint) round(gtk_adjustment_get_value(ffplayer->hadjustment)) % ffplayer->control_width;

  if(x[0] != 0){
    x[0] = ffplayer->control_width - x[0];
  }

  x[1] = ((guint) allocation.width - x[0]) % ffplayer->control_width;
  i_stop = (allocation.width - x[0] - x[1]) / ffplayer->control_width;

  /* clear with background color */
  cairo_set_source_rgba(cr,
			bg_color->red,
			bg_color->green,
			bg_color->blue,
			bg_color->alpha);
  cairo_rectangle(cr, 0.0, 0.0, (double) allocation.width, (double) allocation.height);
  cairo_fill(cr);

  /* draw piano */
  cairo_set_line_width(cr, 1.0);

  cairo_set_source_rgba(cr,
			fg_color->red,
			fg_color->green,
			fg_color->blue,
			fg_color->alpha);

  if(x[0] != 0){
    j0 = (j != 0) ? j -1: 11;

    if(((1 << j0) & bitmap) != 0){
      cairo_rectangle(cr, 0.0, 0.0, x[0], (double) semi_key_height);
      cairo_fill(cr); 	

      if(x[0] > ffplayer->control_width / 2){
	cairo_move_to(cr, (double) (x[0] - ffplayer->control_width / 2),  semi_key_height);
	cairo_line_to(cr, (double) (x[0] - ffplayer->control_width / 2), (double) ffplayer->control_height);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, 0.0, ffplayer->control_height);
      cairo_line_to(cr, (double) x[0], ffplayer->control_height);
      cairo_stroke(cr);
    }else{
      if(((1 << (j0 + 1)) & bitmap) == 0){
	cairo_move_to(cr, (double) x[0], 0.0);
	cairo_line_to(cr, (double) x[0], ffplayer->control_height);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, 0.0, ffplayer->control_height);
      cairo_line_to(cr, (double) x[0], ffplayer->control_height);
      cairo_stroke(cr);
    }
  }

  for(i = 0; i < i_stop; i++){
    if(((1 << j) & bitmap) != 0){
      // draw semi tone key
      cairo_rectangle(cr, (double) (i * ffplayer->control_width + x[0]), 0.0, (double) ffplayer->control_width, semi_key_height);
      cairo_fill(cr); 	

      cairo_move_to(cr, (double) (i * ffplayer->control_width + x[0] + ffplayer->control_width / 2), semi_key_height);
      cairo_line_to(cr, (double) (i * ffplayer->control_width + x[0] + ffplayer->control_width / 2), ffplayer->control_height);
      cairo_stroke(cr);

      cairo_move_to(cr, (double) (i * ffplayer->control_width + x[0]), ffplayer->control_height);
      cairo_line_to(cr, (double) (i * ffplayer->control_width + x[0] + ffplayer->control_width), ffplayer->control_height);
      cairo_stroke(cr);
    }else{
      // no semi tone key
      if(((1 << (j + 1)) & bitmap) == 0){
	cairo_move_to(cr, (double) (i * ffplayer->control_width + x[0] + ffplayer->control_width), 0.0);
	cairo_line_to(cr, (double) (i * ffplayer->control_width + x[0] + ffplayer->control_width), ffplayer->control_height);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) (i * ffplayer->control_width + x[0]), ffplayer->control_height);
      cairo_line_to(cr, (double) (i * ffplayer->control_width + x[0] + ffplayer->control_width), ffplayer->control_height);
      cairo_stroke(cr);
    }

    if(j == 11)
      j = 0;
    else
      j++;
  }

  if(x[1] != 0){
    j0 = j;

    if(((1 << j0) & bitmap) != 0){
      cairo_rectangle(cr, (double) (allocation.width - x[1]), 0.0, (double) x[1], semi_key_height);
      cairo_fill(cr); 	

      if(x[1] > ffplayer->control_width / 2){
	cairo_move_to(cr, (double) (allocation.width - x[1] + ffplayer->control_width / 2), semi_key_height);
	cairo_line_to(cr, (double) (allocation.width - x[1] + ffplayer->control_width / 2), ffplayer->control_height);
	cairo_stroke(cr);
      }

      cairo_move_to(cr, (double) (allocation.width - x[1]), ffplayer->control_height);
      cairo_line_to(cr, (double) allocation.width, ffplayer->control_height);
      cairo_stroke(cr);
    }else{
      cairo_move_to(cr, (double) (allocation.width - x[1]), ffplayer->control_height);
      cairo_line_to(cr, (double) allocation.width, ffplayer->control_height);
      cairo_stroke(cr);
    }
  }

  g_boxed_free(GDK_TYPE_RGBA, fg_color);
  g_boxed_free(GDK_TYPE_RGBA, bg_color);
//  cairo_surface_mark_dirty(cairo_get_target(cr));

  return(FALSE);
}

void
ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  GtkFileChooserDialog *file_chooser;

  file_chooser = ags_machine_file_chooser_dialog_new(AGS_MACHINE(ffplayer));
  gtk_file_chooser_add_shortcut_folder_uri(GTK_FILE_CHOOSER(file_chooser),
					   "file:///usr/share/sounds/sf2",
					   NULL);
  ffplayer->open_dialog = (GtkWidget *) file_chooser;
  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser),
				       FALSE);

  g_signal_connect((GObject *) file_chooser, "response",
		   G_CALLBACK(ags_ffplayer_open_dialog_response_callback), AGS_MACHINE(ffplayer));

  gtk_widget_show_all((GtkWidget *) file_chooser);
}

void
ags_ffplayer_open_dialog_response_callback(GtkWidget *widget, gint response,
					   AgsMachine *machine)
{
  AgsFFPlayer *ffplayer;

  ffplayer = AGS_FFPLAYER(machine);

  if(response == GTK_RESPONSE_ACCEPT){
    gchar *filename;

    filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget));
    ags_ffplayer_open_filename(ffplayer,
			       filename);
  }

  ffplayer->open_dialog = NULL;
  gtk_widget_destroy(widget);

  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->preset), 0);
}

void
ags_ffplayer_preset_changed_callback(GtkComboBox *preset, AgsFFPlayer *ffplayer)
{
  AgsAudioContainer *audio_container;
  
  gint position;
  
  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0 ||
     ffplayer->audio_container == NULL||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }

  audio_container = ffplayer->audio_container;

  /* reset */
  ags_sound_container_level_up(AGS_SOUND_CONTAINER(audio_container->sound_container),
			       3);

  /* load presets */
  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->preset));
  
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
					    position);
  AGS_IPATCH(audio_container->sound_container)->nesting_level += 1;
  
  /* select first instrument */
  ags_ffplayer_load_instrument(ffplayer);

  gtk_combo_box_set_active(GTK_COMBO_BOX(ffplayer->instrument), 0);
}

void
ags_ffplayer_instrument_changed_callback(GtkComboBox *instrument, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;

  AgsAudio *audio;
  AgsChannel *start_input;
  
  AgsAudioContainer *audio_container;
  
  gint position;
  
  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0 ||
     ffplayer->audio_container == NULL||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }
  
  window = (AgsWindow *) gtk_widget_get_toplevel((GtkWidget *) ffplayer);

  audio = AGS_MACHINE(ffplayer)->audio;
  
  /*  */
  audio_container = ffplayer->audio_container;

  /* reset */
  ags_sound_container_level_up(AGS_SOUND_CONTAINER(audio_container->sound_container),
			       3);

  /* load presets */
  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->preset));
  
  if(position == -1){
    position = 0;
  }

  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
					    position);
  AGS_IPATCH(audio_container->sound_container)->nesting_level += 1;

  /* load instrument */
  position = gtk_combo_box_get_active(GTK_COMBO_BOX(ffplayer->instrument));

  if(position == -1){
    position = 0;
  }
  
  ags_sound_container_select_level_by_index(AGS_SOUND_CONTAINER(audio_container->sound_container),
  					    position);

  AGS_IPATCH(audio_container->sound_container)->nesting_level += 1;

  /* update */
  ags_ffplayer_update(ffplayer);
}

gboolean
ags_ffplayer_drawing_area_button_press_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{

  return(FALSE);
}

void
ags_ffplayer_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsFFPlayer *ffplayer)
{
  gtk_widget_queue_draw(ffplayer);
}

void
ags_ffplayer_update_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  ags_ffplayer_update(ffplayer);
}
