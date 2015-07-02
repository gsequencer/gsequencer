/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/X/ags_generic_preferences.h>
#include <ags/X/ags_generic_preferences_callbacks.h>

#include <ags/main.h>

#include <ags-lib/object/ags_connectable.h>

#include <ags/object/ags_applicable.h>

#include <ags/audio/ags_config.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void ags_generic_preferences_class_init(AgsGenericPreferencesClass *generic_preferences);
void ags_generic_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_generic_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_generic_preferences_init(AgsGenericPreferences *generic_preferences);
void ags_generic_preferences_connect(AgsConnectable *connectable);
void ags_generic_preferences_disconnect(AgsConnectable *connectable);
void ags_generic_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_generic_preferences_apply(AgsApplicable *applicable);
void ags_generic_preferences_reset(AgsApplicable *applicable);
static void ags_generic_preferences_finalize(GObject *gobject);
void ags_generic_preferences_show(GtkWidget *widget);

GtkListStore* ags_generic_preferences_create_segmentation();

/**
 * SECTION:ags_generic_preferences
 * @short_description: A composite widget to do generic related preferences
 * @title: AgsGenericPreferences
 * @section_id: 
 * @include: ags/X/ags_generic_preferences.h
 *
 * #AgsGenericPreferences enables you to make generic related preferences.
 */

static gpointer ags_generic_preferences_parent_class = NULL;

GType
ags_generic_preferences_get_type(void)
{
  static GType ags_type_generic_preferences = 0;

  if(!ags_type_generic_preferences){
    static const GTypeInfo ags_generic_preferences_info = {
      sizeof (AgsGenericPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_generic_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsGenericPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_generic_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_generic_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_generic_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_generic_preferences = g_type_register_static(GTK_TYPE_VBOX,
							  "AgsGenericPreferences\0", &ags_generic_preferences_info,
							  0);
    
    g_type_add_interface_static(ags_type_generic_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_generic_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_generic_preferences);
}

void
ags_generic_preferences_class_init(AgsGenericPreferencesClass *generic_preferences)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_generic_preferences_parent_class = g_type_class_peek_parent(generic_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) generic_preferences;

  gobject->finalize = ags_generic_preferences_finalize;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) generic_preferences;

  widget->show = ags_generic_preferences_show;
}

void
ags_generic_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_generic_preferences_connect;
  connectable->disconnect = ags_generic_preferences_disconnect;
}

void
ags_generic_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_generic_preferences_set_update;
  applicable->apply = ags_generic_preferences_apply;
  applicable->reset = ags_generic_preferences_reset;
}

void
ags_generic_preferences_init(AgsGenericPreferences *generic_preferences)
{
  GtkHBox *hbox;
  GtkLabel *label;

  generic_preferences->autosave_thread = (GtkCheckButton *) gtk_check_button_new_with_label("autosave thread\0");
  gtk_box_pack_start(GTK_BOX(generic_preferences),
		     GTK_WIDGET(generic_preferences->autosave_thread),
		     FALSE, FALSE,
		     0);

  hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(generic_preferences),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = gtk_label_new("segmentation\0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);
  
  generic_preferences->segmentation = gtk_combo_box_text_new();
  gtk_combo_box_set_model(generic_preferences->segmentation,
			  ags_generic_preferences_create_segmentation());
  gtk_combo_box_set_active(generic_preferences->segmentation,
			   2);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(generic_preferences->segmentation),
		     FALSE, FALSE,
		     0);
}

void
ags_generic_preferences_connect(AgsConnectable *connectable)
{
  AgsGenericPreferences *generic_preferences;

  generic_preferences = AGS_GENERIC_PREFERENCES(connectable);
  
  //  g_signal_connect_after(G_OBJECT(generic_preferences->autosave_thread), "clicked\0",
  //			 G_CALLBACK(ags_generic_preferences_autosave_thread_clicked_callback), generic_preferences);
}

void
ags_generic_preferences_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_generic_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_generic_preferences_apply(AgsApplicable *applicable)
{
  AgsPreferences *preferences;
  AgsGenericPreferences *generic_preferences; 
  AgsConfig *config;
 
  generic_preferences = AGS_GENERIC_PREFERENCES(applicable);

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(generic_preferences),
							   AGS_TYPE_PREFERENCES);
  config = AGS_CONFIG(AGS_MAIN(AGS_WINDOW(preferences->window)->ags_main)->config);
  
  if(gtk_toggle_button_get_active((GtkToggleButton *) generic_preferences->autosave_thread)){
    ags_config_set(config,
		   AGS_CONFIG_GENERIC,
		   "autosave-thread\0",
		   "true\0");
  }else{
    ags_config_set(config,
		   AGS_CONFIG_GENERIC,
		   "autosave-thread\0",
		   "false\0");
  }

  ags_config_set(config,
		 AGS_CONFIG_GENERIC,
		 "segmentation\0",
		 gtk_combo_box_text_get_active_text(generic_preferences->segmentation));
}

void
ags_generic_preferences_reset(AgsApplicable *applicable)
{
  AgsPreferences *preferences;
  AgsGenericPreferences *generic_preferences; 
  AgsConfig *config;
 
  generic_preferences = AGS_GENERIC_PREFERENCES(applicable);

  preferences = (AgsPreferences *) gtk_widget_get_ancestor(GTK_WIDGET(generic_preferences),
							   AGS_TYPE_PREFERENCES);
  config = AGS_CONFIG(AGS_MAIN(AGS_WINDOW(preferences->window)->ags_main)->config);
  
  gtk_toggle_button_set_active((GtkToggleButton *) generic_preferences->autosave_thread,
			       ((!strncmp(ags_config_get(config,
							 AGS_CONFIG_GENERIC,
							 "autosave-thread\0"),
					  "true\0",
					  5)) ? TRUE: FALSE));
}

static void
ags_generic_preferences_finalize(GObject *gobject)
{
  //TODO:JK: implement me
}

void
ags_generic_preferences_show(GtkWidget *widget)
{
  AgsGenericPreferences *generic_preferences;
  pthread_t thread;

  generic_preferences = AGS_GENERIC_PREFERENCES(widget);
  
  GTK_WIDGET_CLASS(ags_generic_preferences_parent_class)->show(widget);
}

GtkListStore*
ags_generic_preferences_create_segmentation()
{
  GtkListStore *model;
  GtkTreeIter iter;

  model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "1/1\0",
		     1, 1,
		     2, 1,
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "3/4\0",
		     1, 4,
		     2, 4,
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "4/4\0",
		     1, 4,
		     2, 4,
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "8/8\0",
		     1, 4,
		     2, 4,
		     -1);

  return(model);
}

/**
 * ags_generic_preferences_new:
 *
 * Creates an #AgsGenericPreferences
 *
 * Returns: a new #AgsGenericPreferences
 *
 * Since: 0.4
 */
AgsGenericPreferences*
ags_generic_preferences_new()
{
  AgsGenericPreferences *generic_preferences;

  generic_preferences = (AgsGenericPreferences *) g_object_new(AGS_TYPE_GENERIC_PREFERENCES,
							       NULL);
  
  return(generic_preferences);
}
