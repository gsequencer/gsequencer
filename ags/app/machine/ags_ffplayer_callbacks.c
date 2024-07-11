/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#include <ags/app/machine/ags_ffplayer_callbacks.h>
#include <ags/app/ags_machine_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#ifdef AGS_WITH_LIBINSTPATCH
#include <libinstpatch/libinstpatch.h>
#endif

#include <ags/i18n.h>

#include <math.h>

#include <ags/config.h>

void ags_ffplayer_open_dialog_response_callback(AgsFileDialog *file_dialog, gint response,
						AgsMachine *machine);

void
ags_ffplayer_destroy_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  if(ffplayer->open_dialog != NULL){
    gtk_window_destroy((GtkWindow *) ffplayer->open_dialog);
  }
}

void
ags_ffplayer_draw_callback(GtkWidget *drawing_area,
			   cairo_t *cr,
			   int width, int height,
			   AgsFFPlayer *ffplayer)
{
  GtkStyleContext *style_context;
  GtkSettings *settings;

  GtkAllocation allocation;
  
  GdkRGBA fg_color;
  GdkRGBA bg_color;

  double semi_key_height;
  guint bitmap;
  guint x[2];
  guint i, i_stop, j, j0;
  gboolean dark_theme;
  gboolean fg_success;
  gboolean bg_success;
  
  GValue value = G_VALUE_INIT;
  
  gtk_widget_get_allocation((GtkWidget *) ffplayer->drawing_area,
			    &allocation);

  style_context = gtk_widget_get_style_context((GtkWidget *) ffplayer);

  settings = gtk_settings_get_default();

  dark_theme = TRUE;
  
  g_object_get(settings,
	       "gtk-application-prefer-dark-theme", &dark_theme,
	       NULL);

  /* colors */
  fg_success = gtk_style_context_lookup_color(style_context,
					      "theme_fg_color",
					      &fg_color);
    
  bg_success = gtk_style_context_lookup_color(style_context,
					      "theme_bg_color",
					      &bg_color);

  if(!fg_success ||
     !bg_success){
    gdk_rgba_parse(&fg_color,
		   "#101010");

    gdk_rgba_parse(&bg_color,
		   "#cbd5d9");
  }
  
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
			bg_color.red,
			bg_color.green,
			bg_color.blue,
			bg_color.alpha);
  cairo_rectangle(cr, 0.0, 0.0, (double) allocation.width, (double) allocation.height);
  cairo_fill(cr);

  /* draw piano */
  cairo_set_line_width(cr, 1.0);

  cairo_set_source_rgba(cr,
			fg_color.red,
			fg_color.green,
			fg_color.blue,
			fg_color.alpha);

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

//  cairo_surface_mark_dirty(cairo_get_target(cr));
}

void
ags_ffplayer_open_clicked_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  AgsWindow *window;
  AgsFileDialog *file_dialog;
  AgsFileWidget *file_widget;

  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *str;

  gchar *sf2_bookmark_filename;

  sf2_bookmark_filename = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  sf2_bookmark_filename = NULL;
#endif
  
#if defined(AGS_FLATPAK_SANDBOX)
  sf2_bookmark_filename = g_strdup("/usr/share/sounds/sf2");
#endif

#if defined(AGS_SNAP_SANDBOX)
  sf2_bookmark_filename = g_strdup_printf("%s/usr/share/sounds/sf2",
					  getenv("AGS_SNAP_PATH"));
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  sf2_bookmark_filename = g_strdup("/usr/share/sounds/sf2");
#endif

  /* get application context */  
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
  
  file_dialog = (AgsFileDialog *) ags_file_dialog_new((GtkWidget *) window,
						      i18n("open Soundfont2 file"));

  file_widget = ags_file_dialog_get_file_widget(file_dialog);

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;

#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/Containers/%s/Data",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_sf2_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_sf2_bookmark.xml",
				      home_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

  /* recently-used */
  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

  /* bookmark */
  ags_file_widget_set_bookmark_filename(file_widget,
					bookmark_filename);

  ags_file_widget_read_bookmark(file_widget);

#if defined(AGS_MACOS_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_current_path(file_widget,
				   home_path);
#endif

  ags_file_widget_refresh(file_widget);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);

  if(g_file_test(sf2_bookmark_filename,
		 (G_FILE_TEST_EXISTS | G_FILE_TEST_IS_DIR))){
    ags_file_widget_add_bookmark(file_widget,
				 sf2_bookmark_filename);
  }
  
  ffplayer->open_dialog = (GtkWidget *) file_dialog;

  gtk_widget_set_visible((GtkWidget *) file_dialog,
			 TRUE);

  g_signal_connect((GObject *) file_dialog, "response",
		   G_CALLBACK(ags_ffplayer_open_dialog_response_callback), AGS_MACHINE(ffplayer));

  g_free(sf2_bookmark_filename);
}

void
ags_ffplayer_open_dialog_response_callback(AgsFileDialog *file_dialog, gint response,
					   AgsMachine *machine)
{
  AgsFFPlayer *ffplayer;

  ffplayer = AGS_FFPLAYER(machine);

  if(response == GTK_RESPONSE_ACCEPT){
    AgsFileWidget *file_widget;
    
    gchar *filename;

    gint strv_length;

    file_widget = ags_file_dialog_get_file_widget(file_dialog);

    filename = ags_file_widget_get_filename(file_widget);

    if(!g_strv_contains((const gchar * const *) file_widget->recently_used, filename)){
      strv_length = g_strv_length(file_widget->recently_used);

      file_widget->recently_used = g_realloc(file_widget->recently_used,
					     (strv_length + 2) * sizeof(gchar *));

      file_widget->recently_used[strv_length] = g_strdup(filename);
      file_widget->recently_used[strv_length + 1] = NULL; 
    
      ags_file_widget_write_recently_used(file_widget);
    }

    ags_ffplayer_open_filename(ffplayer,
			       filename);
  }

  ffplayer->open_dialog = NULL;

  gtk_window_destroy((GtkWindow *) file_dialog);

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
  AgsAudioContainer *audio_container;
  
  gint position;
  
  if((AGS_FFPLAYER_NO_LOAD & (ffplayer->flags)) != 0 ||
     ffplayer->audio_container == NULL||
     ffplayer->audio_container->sound_container == NULL){
    return;
  }
  
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

void
ags_ffplayer_hscrollbar_value_changed(GtkAdjustment *adjustment, AgsFFPlayer *ffplayer)
{
  gtk_widget_queue_draw((GtkWidget *) ffplayer);
}

void
ags_ffplayer_update_callback(GtkWidget *widget, AgsFFPlayer *ffplayer)
{
  ags_ffplayer_update(ffplayer);
}

void
ags_ffplayer_enable_aliase_callback(GtkCheckButton *check_button, AgsFFPlayer *ffplayer)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat enabled;

  enabled = gtk_check_button_get_active(check_button) ? 1.0: 0.0;
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(ffplayer)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;

    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "enabled", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  enabled);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_aliase_a_amount_callback(AgsDial *dial, AgsFFPlayer *ffplayer)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat amount;

  amount = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(ffplayer)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "a-amount", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  amount);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_aliase_a_phase_callback(AgsDial *dial, AgsFFPlayer *ffplayer)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat phase;

  phase = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(ffplayer)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "a-phase", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  phase);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_aliase_b_amount_callback(AgsDial *dial, AgsFFPlayer *ffplayer)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat amount;

  amount = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(ffplayer)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "b-amount", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  amount);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_aliase_b_phase_callback(AgsDial *dial, AgsFFPlayer *ffplayer)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat phase;

  phase = gtk_adjustment_get_value(ags_dial_get_adjustment(dial));
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(ffplayer)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_TWO_PASS_ALIASE_CHANNEL) ) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "b-phase", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  phase);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}

void
ags_ffplayer_volume_callback(GtkRange *range, AgsFFPlayer *ffplayer)
{
  AgsChannel *start_input;
  AgsChannel *channel;
  
  GList *start_play, *start_recall, *recall;

  gfloat volume;

  volume = (gfloat) gtk_range_get_value(range);
  
  start_input = NULL;
  
  g_object_get(AGS_MACHINE(ffplayer)->audio,
	       "input", &start_input,
	       NULL);

  channel = start_input;

  if(channel != NULL){
    g_object_ref(channel);
  }

  while(channel != NULL){
    AgsChannel *next;
    
    start_play = ags_channel_get_play(channel);
    start_recall = ags_channel_get_recall(channel);
    
    recall =
      start_recall = g_list_concat(start_play, start_recall);

    while((recall = ags_recall_find_type(recall, AGS_TYPE_FX_VOLUME_CHANNEL)) != NULL){
      AgsPort *port;

      port = NULL;
      
      g_object_get(recall->data,
		   "volume", &port,
		   NULL);

      if(port != NULL){
	GValue value = G_VALUE_INIT;

	g_value_init(&value,
		     G_TYPE_FLOAT);

	g_value_set_float(&value,
			  volume);

	ags_port_safe_write(port,
			    &value);

	g_object_unref(port);
      }
      
      /* iterate */
      recall = recall->next;
    }

    g_list_free_full(start_recall,
		     (GDestroyNotify) g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  if(start_input != NULL){
    g_object_unref(start_input);
  }
}
