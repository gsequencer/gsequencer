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

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_devout.h>

#include <ags/X/ags_preferences.h>

void ags_audio_preferences_class_init(AgsAudioPreferencesClass *audio_preferences);
void ags_audio_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_audio_preferences_init(AgsAudioPreferences *audio_preferences);
void ags_audio_preferences_connect(AgsConnectable *connectable);
void ags_audio_preferences_disconnect(AgsConnectable *connectable);
static void ags_audio_preferences_finalize(GObject *gobject);
void ags_audio_preferences_show(GtkWidget *widget);

void ags_audio_preferences_reset(AgsAudioPreferences *audio_preferences);
void* ags_audio_preferences_refresh(void *ptr);

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
    
    ags_type_audio_preferences = g_type_register_static(GTK_TYPE_VBOX,
							"AgsAudioPreferences\0", &ags_audio_preferences_info,
							0);
    
    g_type_add_interface_static(ags_type_audio_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
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
  connectable->connect = ags_audio_preferences_connect;
  connectable->disconnect = ags_audio_preferences_disconnect;
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
}

void
ags_audio_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

static void
ags_audio_preferences_finalize(GObject *gobject)
{
}

void
ags_audio_preferences_show(GtkWidget *widget)
{
  AgsAudioPreferences *audio_preferences;
  pthread_t thread;

  audio_preferences = AGS_AUDIO_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_audio_preferences_parent_class)->show(widget);

  /* poll */
  ags_audio_preferences_refresh(audio_preferences);
}

void
ags_audio_preferences_reset(AgsAudioPreferences *audio_preferences)
{
}

void*
ags_audio_preferences_refresh(void *ptr)
{
  AgsPreferences *preferences;
  AgsAudioPreferences *audio_preferences;
  GtkListStore *model;
  GtkTreeIter iter;
  GList *list;

  audio_preferences = AGS_AUDIO_PREFERENCES(ptr);

  preferences = AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							AGS_TYPE_PREFERENCES));

  g_object_ref(preferences);

  list = ags_devout_list_cards();
  model = gtk_list_store_new(1, G_TYPE_STRING);
    
  while(list != NULL){
    gtk_list_store_append(model, &iter);
    gtk_list_store_set(model, &iter,
		       0, g_strdup_printf(list->data),
		       -1);
      
    list = list->next;
  }
    
  gtk_combo_box_set_model(audio_preferences->card,
			  GTK_TREE_MODEL(model));

  /* reset */
  ags_audio_preferences_reset(audio_preferences);

  g_object_unref(preferences);
}

AgsAudioPreferences*
ags_audio_preferences_new()
{
  AgsAudioPreferences *audio_preferences;

  audio_preferences = (AgsAudioPreferences *) g_object_new(AGS_TYPE_AUDIO_PREFERENCES,
							   NULL);
  
  return(audio_preferences);
}
