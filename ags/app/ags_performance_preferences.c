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

#include <ags/app/ags_performance_preferences.h>
#include <ags/app/ags_performance_preferences_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_preferences.h>

#include <ags/i18n.h>

void ags_performance_preferences_class_init(AgsPerformancePreferencesClass *performance_preferences);
void ags_performance_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_performance_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_performance_preferences_init(AgsPerformancePreferences *performance_preferences);

gboolean ags_performance_preferences_is_connected(AgsConnectable *connectable);
void ags_performance_preferences_connect(AgsConnectable *connectable);
void ags_performance_preferences_disconnect(AgsConnectable *connectable);

void ags_performance_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_performance_preferences_apply(AgsApplicable *applicable);
void ags_performance_preferences_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_performance_preferences
 * @short_description: A composite widget to do performance related preferences
 * @title: AgsPerformancePreferences
 * @section_id: 
 * @include: ags/app/ags_performance_preferences.h
 *
 * #AgsPerformancePreferences enables you to make performance related preferences.
 */

static gpointer ags_performance_preferences_parent_class = NULL;

GType
ags_performance_preferences_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_performance_preferences = 0;

    static const GTypeInfo ags_performance_preferences_info = {
      sizeof (AgsPerformancePreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_performance_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPerformancePreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_performance_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_performance_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_performance_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_performance_preferences = g_type_register_static(GTK_TYPE_BOX,
							      "AgsPerformancePreferences", &ags_performance_preferences_info,
							      0);
    
    g_type_add_interface_static(ags_type_performance_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_performance_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_performance_preferences);
  }

  return g_define_type_id__static;
}

void
ags_performance_preferences_class_init(AgsPerformancePreferencesClass *performance_preferences)
{
  ags_performance_preferences_parent_class = g_type_class_peek_parent(performance_preferences);
}

void
ags_performance_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;

  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_performance_preferences_is_connected;  
  connectable->connect = ags_performance_preferences_connect;
  connectable->disconnect = ags_performance_preferences_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_performance_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_performance_preferences_set_update;
  applicable->apply = ags_performance_preferences_apply;
  applicable->reset = ags_performance_preferences_reset;
}

void
ags_performance_preferences_init(AgsPerformancePreferences *performance_preferences)
{
  GtkBox *hbox;
  GtkLabel *label;
  
  performance_preferences->flags = 0;
  performance_preferences->connectable_flags = 0;  

  gtk_orientable_set_orientation(GTK_ORIENTABLE(performance_preferences),
				 GTK_ORIENTATION_VERTICAL);  

  gtk_box_set_spacing((GtkBox *) performance_preferences,
		      AGS_UI_PROVIDER_DEFAULT_SPACING);

  /* auto-sense */
  performance_preferences->stream_auto_sense = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Auto-sense on stream"));
  gtk_box_append(GTK_BOX(performance_preferences),
		 GTK_WIDGET(performance_preferences->stream_auto_sense));

  /* super threaded - audio */
  performance_preferences->super_threaded_audio = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Super threaded - audio"));
  gtk_box_append(GTK_BOX(performance_preferences),
		 GTK_WIDGET(performance_preferences->super_threaded_audio));

  /* super threaded - channel */
  performance_preferences->super_threaded_channel = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Super threaded - channel"));
  gtk_box_append(GTK_BOX(performance_preferences),
		 GTK_WIDGET(performance_preferences->super_threaded_channel));

  /* thread-pool - max unused threads */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(GTK_BOX(performance_preferences),
		 GTK_WIDGET(hbox));

  label = (GtkLabel *) gtk_label_new(i18n("thread pool - max unused threads"));
  gtk_box_append(GTK_BOX(hbox),
		 GTK_WIDGET(label));

  performance_preferences->thread_pool_max_unused_threads = (GtkSpinButton *) gtk_spin_button_new_with_range(1.0,
													     (gdouble) AGS_THREAD_POOL_DEFAULT_MAX_THREADS,
													     1.0);
  gtk_box_append(GTK_BOX(hbox),
		 GTK_WIDGET(performance_preferences->thread_pool_max_unused_threads));
  
  /* max precision */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(GTK_BOX(performance_preferences),
		 GTK_WIDGET(hbox));

  label = (GtkLabel *) gtk_label_new(i18n("max precision of threads [Hz]"));
  gtk_box_append(GTK_BOX(hbox),
		 GTK_WIDGET(label));

  performance_preferences->max_precision = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(performance_preferences->max_precision,
				 "125");
  gtk_combo_box_text_append_text(performance_preferences->max_precision,
				 "250");
  gtk_combo_box_text_append_text(performance_preferences->max_precision,
				 "1000");
  gtk_box_append(GTK_BOX(hbox),
		 GTK_WIDGET(performance_preferences->max_precision));

  /* update-ui */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append((GtkBox *) performance_preferences,
		 (GtkWidget *) hbox);

  label = (GtkLabel *) gtk_label_new(i18n("update UI timeout[s]"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  performance_preferences->update_ui_timeout = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0333,
											    1.0,
											    0.0625);
  gtk_spin_button_set_value(performance_preferences->update_ui_timeout,
			    AGS_UI_PROVIDER_UPDATE_UI_TIMEOUT);
  gtk_spin_button_set_digits(performance_preferences->update_ui_timeout,
			     4);
  gtk_box_append(hbox,
		 (GtkWidget *) performance_preferences->update_ui_timeout);
}

gboolean
ags_performance_preferences_is_connected(AgsConnectable *connectable)
{
  AgsPerformancePreferences *performance_preferences;
  
  gboolean is_connected;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (performance_preferences->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_performance_preferences_connect(AgsConnectable *connectable)
{
  AgsPerformancePreferences *performance_preferences;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  performance_preferences->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(performance_preferences->super_threaded_channel), "toggled",
			 G_CALLBACK(ags_performance_preferences_super_threaded_channel_callback), performance_preferences);
}

void
ags_performance_preferences_disconnect(AgsConnectable *connectable)
{
  AgsPerformancePreferences *performance_preferences;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  performance_preferences->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
  
  g_object_disconnect(G_OBJECT(performance_preferences->super_threaded_channel),
		      "any_signal::toggled",
		      G_CALLBACK(ags_performance_preferences_super_threaded_channel_callback),
		      performance_preferences,
		      NULL);
}

void
ags_performance_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_performance_preferences_apply(AgsApplicable *applicable)
{
  AgsPerformancePreferences *performance_preferences; 

  AgsConfig *config;

  gchar *str;

  guint max_precision;
  gdouble update_ui_timeout;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(applicable);

  config = ags_config_get_instance();

  /* auto-sense */
  str = g_strdup(((gtk_check_button_get_active(performance_preferences->stream_auto_sense)) ? "true": "false"));
  ags_config_set_value(config,
		       AGS_CONFIG_RECALL,
		       "auto-sense",
		       str);
  g_free(str);

  /* restore thread config */
  if(gtk_check_button_get_active(performance_preferences->super_threaded_audio) ||
     gtk_check_button_get_active(performance_preferences->super_threaded_channel)){
    ags_config_set_value(config,
			 AGS_CONFIG_THREAD,
			 "model",
			 "super-threaded");

    if(gtk_check_button_get_active(performance_preferences->super_threaded_channel)){
      ags_config_set_value(config,
			   AGS_CONFIG_THREAD,
			   "super-threaded-scope",
			   "channel");
    }else if(gtk_check_button_get_active(performance_preferences->super_threaded_audio)){
      ags_config_set_value(config,
			   AGS_CONFIG_THREAD,
			   "super-threaded-scope",
			   "audio");
    }
  }else{
    ags_config_set_value(config,
			 AGS_CONFIG_THREAD,
			 "model",
			 "multi-threaded");
  }
  
  ags_config_set_value(config,
		       AGS_CONFIG_THREAD,
		       "lock-global",
		       "ags-thread");
  
  ags_config_set_value(config,
		       AGS_CONFIG_THREAD,
		       "lock-parent",
		       "ags-recycling-thread");

  /* thread pool - max unused threads */
  str = g_strdup_printf("%d",
			gtk_spin_button_get_value_as_int(performance_preferences->thread_pool_max_unused_threads));
  ags_config_set_value(config,
		       AGS_CONFIG_THREAD,
		       "thread-pool-max-unused-threads",
		       str);

  g_free(str);
  
  /* max-precision */
  str = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(performance_preferences->max_precision));
  max_precision = 0;
  
  if(str != NULL){
    max_precision = g_ascii_strtoull(str,
				     NULL,
				     10);
  }
    
  switch(max_precision){
  case 0:
    {
      g_critical("invalid max-precision configuration");

      max_precision = AGS_THREAD_DEFAULT_MAX_PRECISION;
    }
  case 125:
  case 250:
  case 1000:
  default:
    str = g_strdup_printf("%d",
			  max_precision);
      
    ags_config_set_value(config,
			 AGS_CONFIG_THREAD,
			 "max-precision",
			 str);

    g_free(str);
  }

  /* update UI */
  update_ui_timeout = gtk_spin_button_get_value(performance_preferences->update_ui_timeout);
  
  str = g_strdup_printf("%.4f",
			update_ui_timeout);
  ags_config_set_value(config,
		       AGS_CONFIG_GENERIC,
		       "update-ui-timeout",
		       str);
  
  g_free(str);

  //TODO:JK: implement me
}

void
ags_performance_preferences_reset(AgsApplicable *applicable)
{
  AgsPerformancePreferences *performance_preferences;

  AgsConfig *config;

  gchar *str;

  guint max_precision;
  gdouble update_ui_timeout;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(applicable);

  config = ags_config_get_instance();

  /* auto-sense */
  str = ags_config_get_value(config,
			     AGS_CONFIG_RECALL,
			     "auto-sense");
  gtk_check_button_set_active(performance_preferences->stream_auto_sense,
			      !g_strcmp0("true",
					 str));
  
  /*  */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "super-threaded-scope");

  if(str != NULL){
    if(!g_ascii_strncasecmp(str,
			    "audio",
			    6)){
      gtk_check_button_set_active(performance_preferences->super_threaded_audio,
				  TRUE);
    }else if(!g_ascii_strncasecmp(str,
				  "channel",
				  8)){
      gtk_check_button_set_active(performance_preferences->super_threaded_channel,
				  TRUE);
    }
  }
  
  /*  */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "thread-pool-max-unused-threads");

  if(str != NULL){
    gtk_spin_button_set_value(performance_preferences->thread_pool_max_unused_threads,
			      g_ascii_strtod(str,
					     NULL));
  }else{
    gtk_spin_button_set_value(performance_preferences->thread_pool_max_unused_threads,
			      AGS_THREAD_POOL_DEFAULT_MAX_UNUSED_THREADS);
  }
  
  /* max-precision */
  str = ags_config_get_value(config,
			     AGS_CONFIG_THREAD,
			     "max-precision");

  if(str != NULL){
    max_precision = g_ascii_strtoull(str,
				     NULL,
				     10);

    switch(max_precision){
    case 125:
      {
	gtk_combo_box_set_active((GtkComboBox *) performance_preferences->max_precision,
				 0);
      }
      break;
    case 250:
      {
	gtk_combo_box_set_active((GtkComboBox *) performance_preferences->max_precision,
				 1);
      }
      break;
    case 1000:
      {
	gtk_combo_box_set_active((GtkComboBox *) performance_preferences->max_precision,
				 2);
      }
      break;
    default:
      g_warning("unknown max-precision configuration");
    }
  }

  /* update UI timeout */
  str = ags_config_get_value(config,
			     AGS_CONFIG_GENERIC,
			     "update-ui-timeout");

  if(str != NULL){
    gtk_spin_button_set_value(performance_preferences->update_ui_timeout,
			      g_strtod(str,
				       NULL));
  }
  
  g_free(str);

  //TODO:JK: implement me
}

/**
 * ags_performance_preferences_new:
 *
 * Create a new instance of #AgsPerformancePreferences
 *
 * Returns: the new #AgsPerformancePreferences
 *
 * Since: 3.0.0
 */
AgsPerformancePreferences*
ags_performance_preferences_new()
{
  AgsPerformancePreferences *performance_preferences;

  performance_preferences = (AgsPerformancePreferences *) g_object_new(AGS_TYPE_PERFORMANCE_PREFERENCES,
								       NULL);
  
  return(performance_preferences);
}
