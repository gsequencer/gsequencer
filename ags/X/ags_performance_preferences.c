/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#include <ags/X/ags_performance_preferences.h>
#include <ags/X/ags_performance_preferences_callbacks.h>

#include <ags/libags.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

#include <ags/i18n.h>

void ags_performance_preferences_class_init(AgsPerformancePreferencesClass *performance_preferences);
void ags_performance_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_performance_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_performance_preferences_init(AgsPerformancePreferences *performance_preferences);

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
 * @include: ags/X/ags_performance_preferences.h
 *
 * #AgsPerformancePreferences enables you to make performance related preferences.
 */

static gpointer ags_performance_preferences_parent_class = NULL;

GType
ags_performance_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
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

    ags_type_performance_preferences = g_type_register_static(GTK_TYPE_VBOX,
							      "AgsPerformancePreferences", &ags_performance_preferences_info,
							      0);
    
    g_type_add_interface_static(ags_type_performance_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_performance_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_performance_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_performance_preferences_class_init(AgsPerformancePreferencesClass *performance_preferences)
{
  ags_performance_preferences_parent_class = g_type_class_peek_parent(performance_preferences);
}

void
ags_performance_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_performance_preferences_connect;
  connectable->disconnect = ags_performance_preferences_disconnect;
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
  GtkHBox *hbox;
  GtkLabel *label;

  performance_preferences->flags = 0;
  
  /* auto-sense */
  performance_preferences->stream_auto_sense = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Auto-sense on stream"));
  gtk_box_pack_start(GTK_BOX(performance_preferences),
		     GTK_WIDGET(performance_preferences->stream_auto_sense),
		     FALSE, FALSE,
		     0);

  /* super threaded - audio */
  performance_preferences->super_threaded_audio = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Super threaded - audio"));
  gtk_box_pack_start(GTK_BOX(performance_preferences),
		     GTK_WIDGET(performance_preferences->super_threaded_audio),
		     FALSE, FALSE,
		     0);

  /* super threaded - channel */
  performance_preferences->super_threaded_channel = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("Super threaded - channel"));
  gtk_box_pack_start(GTK_BOX(performance_preferences),
		     GTK_WIDGET(performance_preferences->super_threaded_channel),
		     FALSE, FALSE,
		     0);

  /* thread-pool - max unused threads */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(performance_preferences),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("thread pool - max unused threads"));
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  performance_preferences->thread_pool_max_unused_threads = gtk_spin_button_new_with_range(1.0,
											   (gdouble) AGS_THREAD_POOL_DEFAULT_MAX_THREADS,
											   1.0);
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(performance_preferences->thread_pool_max_unused_threads),
		     FALSE, FALSE,
		     0);
  
  /* max precision */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(performance_preferences),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  label = (GtkLabel *) gtk_label_new(i18n("max precision of threads [Hz]"));
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(label),
		     FALSE, FALSE,
		     0);

  performance_preferences->max_precision = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(performance_preferences->max_precision,
				 "125");
  gtk_combo_box_text_append_text(performance_preferences->max_precision,
				 "250");
  gtk_combo_box_text_append_text(performance_preferences->max_precision,
				 "1000");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(performance_preferences->max_precision),
		     FALSE, FALSE,
		     0);
}

void
ags_performance_preferences_connect(AgsConnectable *connectable)
{
  AgsPerformancePreferences *performance_preferences;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(connectable);

  if((AGS_PERFORMANCE_PREFERENCES_CONNECTED & (performance_preferences->flags)) != 0){
    return;
  }

  performance_preferences->flags |= AGS_PERFORMANCE_PREFERENCES_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(performance_preferences->super_threaded_channel), "clicked",
			 G_CALLBACK(ags_performance_preferences_super_threaded_channel_callback), performance_preferences);
}

void
ags_performance_preferences_disconnect(AgsConnectable *connectable)
{
  AgsPerformancePreferences *performance_preferences;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(connectable);

  if((AGS_PERFORMANCE_PREFERENCES_CONNECTED & (performance_preferences->flags)) == 0){
    return;
  }

  performance_preferences->flags &= (~AGS_PERFORMANCE_PREFERENCES_CONNECTED);
  
  g_object_disconnect(G_OBJECT(performance_preferences->super_threaded_channel),
		      "any_signal::clicked",
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
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(applicable);

  config = ags_config_get_instance();

  /* auto-sense */
  str = g_strdup(((gtk_toggle_button_get_active((GtkToggleButton *) performance_preferences->stream_auto_sense)) ? "true": "false"));
  ags_config_set_value(config,
		       AGS_CONFIG_RECALL,
		       "auto-sense",
		       str);
  g_free(str);

  /* restore thread config */
  if(gtk_toggle_button_get_active(performance_preferences->super_threaded_audio) ||
     gtk_toggle_button_get_active(performance_preferences->super_threaded_channel)){
    ags_config_set_value(config,
			 AGS_CONFIG_THREAD,
			 "model",
			 "super-threaded");

    if(gtk_toggle_button_get_active(performance_preferences->super_threaded_channel)){
      ags_config_set_value(config,
			   AGS_CONFIG_THREAD,
			   "super-threaded-scope",
			   "channel");
    }else if(gtk_toggle_button_get_active(performance_preferences->super_threaded_audio)){
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
  str = gtk_combo_box_get_active_text(performance_preferences->max_precision);
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

  //TODO:JK: implement me
}

void
ags_performance_preferences_reset(AgsApplicable *applicable)
{
  AgsPerformancePreferences *performance_preferences;

  AgsConfig *config;

  gchar *str;

  guint max_precision;
  
  performance_preferences = AGS_PERFORMANCE_PREFERENCES(applicable);

  config = ags_config_get_instance();

  /* auto-sense */
  str = ags_config_get_value(config,
			     AGS_CONFIG_RECALL,
			     "auto-sense");
  gtk_toggle_button_set_active((GtkToggleButton *) performance_preferences->stream_auto_sense,
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
      gtk_toggle_button_set_active((GtkToggleButton *) performance_preferences->super_threaded_audio,
				   TRUE);
    }else if(!g_ascii_strncasecmp(str,
				  "channel",
				  8)){
      gtk_toggle_button_set_active((GtkToggleButton *) performance_preferences->super_threaded_channel,
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
	gtk_combo_box_set_active(performance_preferences->max_precision,
				 0);
      }
      break;
    case 250:
      {
	gtk_combo_box_set_active(performance_preferences->max_precision,
				 1);
      }
      break;
    case 1000:
      {
	gtk_combo_box_set_active(performance_preferences->max_precision,
				 2);
      }
      break;
    default:
      g_warning("unknown max-precision configuration");
    }
  }

  //TODO:JK: implement me
}

/**
 * ags_performance_preferences_new:
 *
 * Create a new instance of #AgsPerformancePreferences
 *
 * Returns: the new #AgsPerformancePreferences
 *
 * Since: 2.0.0
 */
AgsPerformancePreferences*
ags_performance_preferences_new()
{
  AgsPerformancePreferences *performance_preferences;

  performance_preferences = (AgsPerformancePreferences *) g_object_new(AGS_TYPE_PERFORMANCE_PREFERENCES,
								       NULL);
  
  return(performance_preferences);
}
