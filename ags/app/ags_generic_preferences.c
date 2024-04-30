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

#include <ags/app/ags_generic_preferences.h>
#include <ags/app/ags_generic_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>

#include <ags/i18n.h>

void ags_generic_preferences_class_init(AgsGenericPreferencesClass *generic_preferences);
void ags_generic_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_generic_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_generic_preferences_init(AgsGenericPreferences *generic_preferences);
static void ags_generic_preferences_finalize(GObject *gobject);

void ags_generic_preferences_connect(AgsConnectable *connectable);
void ags_generic_preferences_disconnect(AgsConnectable *connectable);

void ags_generic_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_generic_preferences_apply(AgsApplicable *applicable);
void ags_generic_preferences_reset(AgsApplicable *applicable);

GtkListStore* ags_generic_preferences_create_segmentation();

/**
 * SECTION:ags_generic_preferences
 * @short_description: A composite widget to do generic related preferences
 * @title: AgsGenericPreferences
 * @section_id: 
 * @include: ags/app/ags_generic_preferences.h
 *
 * #AgsGenericPreferences enables you to make generic related preferences.
 */

static gpointer ags_generic_preferences_parent_class = NULL;

GType
ags_generic_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_generic_preferences = 0;

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

    ags_type_generic_preferences = g_type_register_static(GTK_TYPE_BOX,
							  "AgsGenericPreferences", &ags_generic_preferences_info,
							  0);
    
    g_type_add_interface_static(ags_type_generic_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_generic_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_generic_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_generic_preferences_class_init(AgsGenericPreferencesClass *generic_preferences)
{
  GObjectClass *gobject;

  ags_generic_preferences_parent_class = g_type_class_peek_parent(generic_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) generic_preferences;

  gobject->finalize = ags_generic_preferences_finalize;
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
  GtkBox *hbox;
  GtkLabel *label;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(generic_preferences),
				 GTK_ORIENTATION_VERTICAL);

  gtk_box_set_spacing((GtkBox *) generic_preferences,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  generic_preferences->autosave_thread = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("autosave thread"));
  gtk_box_append((GtkBox *) generic_preferences,
		 (GtkWidget *) generic_preferences->autosave_thread);
		     
  /* segmentation */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) generic_preferences,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("segmentation"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);
  
  generic_preferences->segmentation = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_combo_box_set_model(generic_preferences->segmentation,
			  (GtkTreeModel *) ags_generic_preferences_create_segmentation());
  gtk_combo_box_set_active(GTK_COMBO_BOX(generic_preferences->segmentation),
			   2);
  gtk_box_append(hbox,
		 (GtkWidget *) generic_preferences->segmentation);

  gtk_widget_set_visible(hbox,
			 FALSE);
  
  /* engine mode */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) generic_preferences,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("engine mode"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);
  
  generic_preferences->engine_mode = (GtkComboBox *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(generic_preferences->engine_mode),
				 "deterministic");
  gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(generic_preferences->engine_mode),
				 "performance");
  gtk_combo_box_set_active(generic_preferences->engine_mode,
			   0);
  
  gtk_box_append(hbox,
		 (GtkWidget *) generic_preferences->engine_mode);

  gtk_widget_set_visible(hbox,
			 FALSE);

  /* rt-safe */
  generic_preferences->rt_safe = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("rt-safe"));

  gtk_box_append((GtkBox *) generic_preferences,
		 (GtkWidget *) generic_preferences->rt_safe);
  
  /* GUI scale factor */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) generic_preferences,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("GUI scale factor"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  generic_preferences->gui_scale = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(generic_preferences->gui_scale,
				 "1.0");
  gtk_combo_box_text_append_text(generic_preferences->gui_scale,
				 "1.25");
  gtk_combo_box_text_append_text(generic_preferences->gui_scale,
				 "2.0");
  gtk_combo_box_set_active(GTK_COMBO_BOX(generic_preferences->gui_scale),
			   0);
  
  gtk_box_append(hbox,
		 (GtkWidget *) generic_preferences->gui_scale);
}

static void
ags_generic_preferences_finalize(GObject *gobject)
{
  /* call parent */
  G_OBJECT_CLASS(ags_generic_preferences_parent_class)->finalize(gobject);
}

void
ags_generic_preferences_connect(AgsConnectable *connectable)
{
  AgsGenericPreferences *generic_preferences;
  
  generic_preferences = AGS_GENERIC_PREFERENCES(connectable);

  if((AGS_GENERIC_PREFERENCES_CONNECTED & (generic_preferences->flags)) != 0){
    return;
  }

  generic_preferences->flags |= AGS_GENERIC_PREFERENCES_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(generic_preferences->rt_safe), "toggled",
			 G_CALLBACK(ags_generic_preferences_rt_safe_callback), generic_preferences);
}

void
ags_generic_preferences_disconnect(AgsConnectable *connectable)
{
  AgsGenericPreferences *generic_preferences;
  
  generic_preferences = AGS_GENERIC_PREFERENCES(connectable);

  if((AGS_GENERIC_PREFERENCES_CONNECTED & (generic_preferences->flags)) == 0){
    return;
  }

  generic_preferences->flags &= (~AGS_GENERIC_PREFERENCES_CONNECTED);
  
  g_object_disconnect(G_OBJECT(generic_preferences->rt_safe),
		      "any_signal::toggled",
		      G_CALLBACK(ags_generic_preferences_rt_safe_callback),
		      generic_preferences,
		      NULL);
}

void
ags_generic_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_generic_preferences_apply(AgsApplicable *applicable)
{
  AgsGenericPreferences *generic_preferences; 

  AgsConfig *config;

  gchar *str;
  
  generic_preferences = AGS_GENERIC_PREFERENCES(applicable);

  config = ags_config_get_instance();

  ags_config_set_value(config,
		       AGS_CONFIG_GENERIC,
		       "disable-feature",
		       "experimental");

  if(gtk_check_button_get_active(generic_preferences->autosave_thread)){
    ags_config_set_value(config,
			 AGS_CONFIG_GENERIC,
			 "autosave-thread",
			 "true");
  }else{
    ags_config_set_value(config,
			 AGS_CONFIG_GENERIC,
			 "autosave-thread",
			 "false");
  }

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(generic_preferences->segmentation));
  ags_config_set_value(config,
		       AGS_CONFIG_GENERIC,
		       "segmentation",
		       str);
  
  g_free(str);

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(generic_preferences->engine_mode));
  ags_config_set_value(config,
		       AGS_CONFIG_GENERIC,
		       "engine-mode",
		       str);
  
  g_free(str);

  ags_config_set_value(config,
		       AGS_CONFIG_GENERIC,
		       "rt-safe",
		       (gtk_check_button_get_active(generic_preferences->rt_safe) ? "true": "false"));

  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(generic_preferences->gui_scale));
  ags_config_set_value(config,
		       AGS_CONFIG_GENERIC,
		       "gui-scale",
		       str);
  
  g_free(str);
}

void
ags_generic_preferences_reset(AgsApplicable *applicable)
{
  AgsGenericPreferences *generic_preferences; 

  AgsConfig *config;

  gchar *str;
  
  generic_preferences = AGS_GENERIC_PREFERENCES(applicable);

  config = ags_config_get_instance();

  /* auto-save thread */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "autosave-thread");

  if(str != NULL){
    gtk_check_button_set_active(generic_preferences->autosave_thread,
				((!g_ascii_strncasecmp(str,
						       "true",
						       5)) ? TRUE: FALSE));
  }else{
    gtk_check_button_set_active(generic_preferences->autosave_thread,
				FALSE);
  }
  
  g_free(str);

  /* engine mode */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "engine-mode");

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "performance",
			    12)){
      gtk_combo_box_set_active((GtkComboBox *) generic_preferences->engine_mode,
			       1);
    }else{
      gtk_combo_box_set_active((GtkComboBox *) generic_preferences->engine_mode,
			       0);
    }
  }
  
  g_free(str);
  
  /* rt-safe */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "rt-safe");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "true",
			  5)){
    gtk_check_button_set_active(generic_preferences->rt_safe,
				TRUE);
  }

  g_free(str);

  /* GUI scale factor */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "gui-scale");

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "1.0",
			    4)){
      gtk_combo_box_set_active((GtkComboBox *) generic_preferences->gui_scale,
			       0);
    }else if(!g_ascii_strncasecmp(str,
				  "1.25",
				  5)){
      gtk_combo_box_set_active((GtkComboBox *) generic_preferences->gui_scale,
			       1);
    }else if(!g_ascii_strncasecmp(str,
				  "2.0",
				  4)){
      gtk_combo_box_set_active((GtkComboBox *) generic_preferences->gui_scale,
			       2);
    }
  }
  
  g_free(str);
}

GtkListStore*
ags_generic_preferences_create_segmentation()
{
  GtkListStore *model;
  GtkTreeIter iter;

  model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_UINT, G_TYPE_UINT);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "1/1",
		     1, 1,
		     2, 1,
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "3/4",
		     1, 4,
		     2, 4,
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "4/4",
		     1, 4,
		     2, 4,
		     -1);

  gtk_list_store_append(model, &iter);
  gtk_list_store_set(model, &iter,
		     0, "8/8",
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
 * Since: 3.0.0
 */
AgsGenericPreferences*
ags_generic_preferences_new()
{
  AgsGenericPreferences *generic_preferences;

  generic_preferences = (AgsGenericPreferences *) g_object_new(AGS_TYPE_GENERIC_PREFERENCES,
							       NULL);
  
  return(generic_preferences);
}
