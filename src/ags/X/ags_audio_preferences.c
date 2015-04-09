/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/ags_audio_preferences.h>
#include <ags/X/ags_audio_preferences_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_devout.h>
#include <ags/audio/ags_config.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences);
void ags_audio_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_audio_preferences_init(AgsAudioPreferences *audio_preferences);
void ags_audio_preferences_connect(AgsConnectable *connectable);
void ags_audio_preferences_disconnect(AgsConnectable *connectable);
void ags_audio_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_audio_preferences_apply(AgsApplicable *applicable);
void ags_audio_preferences_reset(AgsApplicable *applicable);
static void ags_audio_preferences_finalize(GObject *gobject);
void ags_audio_preferences_show(GtkWidget *widget);

/**
 * SECTION:ags_audio_preferences
 * @short_description: A composite widget to do audio related preferences
 * @title: AgsAudioPreferences
 * @section_id: 
 * @include: ags/X/ags_audio_preferences.h
 *
 * #AgsAudioPreferences enables you to make preferences of soundcard, audio channels,
 * samplerate and buffer size.
 */

static gpointer ags_audio_preferences_parent_class = NULL;

GType
ags_audio_preferences_get_type(void)
{
  static GType ags_type_audio_preferences = 0;

  if(!ags_type_audio_preferences){
    static const GTypeInfo ags_audio_preferences_info = {
      sizeof (AgsAudioPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_audio_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsAudioPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_audio_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_audio_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_audio_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    ags_type_audio_preferences = g_type_register_static(GTK_TYPE_VBOX,
							"AgsAudioPreferences\0", &ags_audio_preferences_info,
							0);
    
    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_audio_preferences);
}

void
ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_audio_preferences_parent_class = g_type_class_peek_parent(audio_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) audio_preferences;

  gobject->finalize = ags_audio_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) audio_preferences;

  widget->show = ags_audio_preferences_show;
}

void
ags_audio_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_audio_preferences_connect;
  connectable->disconnect = ags_audio_preferences_disconnect;
}

void
ags_audio_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_audio_preferences_set_update;
  applicable->apply = ags_audio_preferences_apply;
  applicable->reset = ags_audio_preferences_reset;
}

void
ags_audio_preferences_init(AgsAudioPreferences *audio_preferences)
{
  GtkTable *table;
  GtkLabel *label;
  GtkCellRenderer *cell_renderer;

  table = (GtkTable *) gtk_table_new(2, 4, FALSE);
  gtk_box_pack_start(GTK_BOX(audio_preferences),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     2);

  /* sound card */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "sound card\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  audio_preferences->card = (GtkComboBox *) gtk_combo_box_new();
  gtk_table_attach(table,
		   GTK_WIDGET(audio_preferences->card),
		   1, 2,
		   0, 1,
		   GTK_FILL, GTK_FILL,
		   0, 0);
  
  cell_renderer = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(audio_preferences->card),
			     cell_renderer,
			     FALSE); 
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(audio_preferences->card),
				 cell_renderer,
				 "text\0", 0,
				 "text\0", 1,
				 NULL);
  gtk_combo_box_set_active(audio_preferences->card, 0);
  
  /* audio channels */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "audio channels\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  audio_preferences->audio_channels = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 24.0, 1.0);
  gtk_spin_button_set_value(audio_preferences->audio_channels, 2);
  gtk_table_attach(table,
		   GTK_WIDGET(audio_preferences->audio_channels),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* samplerate */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "samplerate\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  audio_preferences->samplerate = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 192000.0, 1.0);
  gtk_spin_button_set_value(audio_preferences->samplerate, 44100);
  gtk_table_attach(table,
		   GTK_WIDGET(audio_preferences->samplerate),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* buffer size */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label\0", "buffer size\0",
				    "xalign\0", 0.0,
				    NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  audio_preferences->buffer_size = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0, 65535.0, 1.0);
  gtk_spin_button_set_value(audio_preferences->buffer_size, 512);
  gtk_table_attach(table,
		   GTK_WIDGET(audio_preferences->buffer_size),
		   1, 2,
		   3, 4,
		   GTK_FILL, GTK_FILL,
		   0, 0);
}

void
ags_audio_preferences_connect(AgsConnectable *connectable)
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = AGS_AUDIO_PREFERENCES(connectable);

  g_signal_connect_after(G_OBJECT(audio_preferences->card), "changed\0",
			 G_CALLBACK(ags_audio_preferences_card_changed_callback), audio_preferences);
}

void
ags_audio_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_audio_preferences_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_audio_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_audio_preferences_apply(AgsApplicable *applicable)
{
  AgsPreferences *preferences;
  AgsAudioPreferences *audio_preferences; 
  AgsConfig *config;
  GList *card_id, *card_name;
  char *device;
  gchar *str;
  int card_num;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							   AGS_TYPE_PREFERENCES);
  config = AGS_CONFIG(AGS_MAIN(AGS_WINDOW(preferences->window)->ags_main)->config);

  /* samplerate */
  str = g_strdup_printf("%u\0",
			(guint) gtk_spin_button_get_value(audio_preferences->samplerate));
  ags_config_set(config,
		 AGS_CONFIG_DEVOUT,
		 "samplerate\0",
		 str);
  g_free(str);

  /* buffer size */
  str = g_strdup_printf("%u\0",
			(guint) gtk_spin_button_get_value(audio_preferences->buffer_size));
  ags_config_set(config,
		 AGS_CONFIG_DEVOUT,
		 "buffer-size\0",
		 str);
  g_free(str);

  /* dsp channels */
  str = g_strdup_printf("%u\0",
			(guint) gtk_spin_button_get_value(audio_preferences->audio_channels));
  ags_config_set(config,
		 AGS_CONFIG_DEVOUT,
		 "dsp-channels\0",
		 str);
  g_free(str);

  /* card */
  ags_config_set(config,
		 AGS_CONFIG_DEVOUT,
		 "alsa-handle\0",
		 gtk_combo_box_text_get_active_text(audio_preferences->card));
}

void
ags_audio_preferences_reset(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsPreferences *preferences;
  AgsAudioPreferences *audio_preferences;
  AgsDevout *devout;
  GtkListStore *model;
  GtkTreeIter iter;
  GList *card_id, *card_name;
  char *device;
  int card_num;
  guint channels, channels_min, channels_max;
  guint rate, rate_min, rate_max;
  guint buffer_size, buffer_size_min, buffer_size_max;
  GError *error;

  audio_preferences = AGS_AUDIO_PREFERENCES(applicable);

  /*  */
  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							   AGS_TYPE_PREFERENCES);
  window = AGS_WINDOW(preferences->window);

  /* refresh */
  ags_devout_list_cards(&card_id, &card_name);
  model = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_STRING);

  while(card_id != NULL){
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter,
		       0, card_id->data,
		       1, card_name->data,
		       -1);
      
    card_id = card_id->next;
    card_name = card_name->next;
  }

  g_list_free(card_id);
  g_list_free(card_name);

  gtk_combo_box_set_model(audio_preferences->card,
			  GTK_TREE_MODEL(model));

  /*  */
  devout = window->devout;
  g_object_get(G_OBJECT(devout),
	       "device\0", &device,
	       "pcm_channels\0", &channels,
	       "frequency\0", &rate,
	       "buffer_size\0", &buffer_size,
	       NULL);


  error = NULL;

  /*  */
  sscanf(device, "hw:%i\0", &card_num);

  //  gtk_combo_box_set_active(audio_preferences->card,
  //			   card_num);

  gtk_spin_button_set_value(audio_preferences->audio_channels,
			    (gdouble) channels);
  gtk_spin_button_set_value(audio_preferences->samplerate,
			    (gdouble) rate);
  gtk_spin_button_set_value(audio_preferences->buffer_size,
			    (gdouble) buffer_size);

  /*  */
  ags_devout_pcm_info(gtk_combo_box_get_active_text(audio_preferences->card),
  		      &channels_min, &channels_max,
  		      &rate_min, &rate_max,
  		      &buffer_size_min, &buffer_size_max,
  		      &error);

  if(error != NULL){
    GtkMessageDialog *dialog;

    dialog = (GtkMessageDialog *) gtk_message_dialog_new((GtkWindow *) gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
											       AGS_TYPE_PREFERENCES),
							 GTK_DIALOG_MODAL,
							 GTK_MESSAGE_ERROR,
							 GTK_BUTTONS_CLOSE,
							 error->message);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(GTK_WIDGET(dialog));

    gtk_spin_button_set_range(audio_preferences->audio_channels, 0.0, 24.0);
    gtk_spin_button_set_range(audio_preferences->samplerate, 1.0, 192000.0);
    gtk_spin_button_set_range(audio_preferences->buffer_size, 1.0, 65535.0);

    return;
  }

  gtk_spin_button_set_range(audio_preferences->audio_channels,
			    channels_min, channels_max);
  gtk_spin_button_set_range(audio_preferences->samplerate,
			    rate_min, rate_max);
  gtk_spin_button_set_range(audio_preferences->buffer_size,
			    buffer_size_min, buffer_size_max);
}

void
ags_audio_preferences_show(GtkWidget *widget)
{
  AgsAudioPreferences *audio_preferences;
  pthread_t thread;

  audio_preferences = AGS_AUDIO_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_audio_preferences_parent_class)->show(widget);
}

/**
 * ags_audio_preferences_new:
 *
 * Creates an #AgsAudioPreferences
 *
 * Returns: a new #AgsAudioPreferences
 *
 * Since: 0.4
 */
AgsAudioPreferences*
ags_audio_preferences_new()
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = (AgsAudioPreferences *) g_object_new(AGS_TYPE_AUDIO_PREFERENCES,
							   NULL);
  
  return(audio_preferences);
}
