/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <ags/GSequencer/ags_osc_server_preferences.h>
#include <ags/GSequencer/ags_osc_server_preferences_callbacks.h>

#include <ags/GSequencer/ags_ui_provider.h>
#include <ags/GSequencer/ags_window.h>
#include <ags/GSequencer/ags_preferences.h>

#include <ags/i18n.h>

void ags_osc_server_preferences_class_init(AgsOscServerPreferencesClass *osc_server_preferences);
void ags_osc_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_osc_server_preferences_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_osc_server_preferences_init(AgsOscServerPreferences *osc_server_preferences);
void ags_osc_server_preferences_finalize(GObject *gobject);

void ags_osc_server_preferences_connect(AgsConnectable *connectable);
void ags_osc_server_preferences_disconnect(AgsConnectable *connectable);

void ags_osc_server_preferences_set_update(AgsApplicable *applicable, gboolean update);
void ags_osc_server_preferences_apply(AgsApplicable *applicable);
void ags_osc_server_preferences_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_osc_server_preferences
 * @short_description: A composite widget to do OSC server related preferences
 * @title: AgsOscServerPreferences
 * @section_id: 
 * @include: ags/X/ags_osc_server_preferences.h
 *
 * #AgsOscServerPreferences enables you to make OSC server related preferences.
 */

static gpointer ags_osc_server_preferences_parent_class = NULL;

GType
ags_osc_server_preferences_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_server_preferences = 0;

    static const GTypeInfo ags_osc_server_preferences_info = {
      sizeof (AgsOscServerPreferencesClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_server_preferences_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscServerPreferences),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_server_preferences_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_osc_server_preferences_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };
    
    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_osc_server_preferences_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_osc_server_preferences = g_type_register_static(GTK_TYPE_BOX,
							     "AgsOscServerPreferences", &ags_osc_server_preferences_info,
							     0);
    
    g_type_add_interface_static(ags_type_osc_server_preferences,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_osc_server_preferences,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_server_preferences);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_server_preferences_class_init(AgsOscServerPreferencesClass *osc_server_preferences)
{
  GObjectClass *gobject;

  ags_osc_server_preferences_parent_class = g_type_class_peek_parent(osc_server_preferences);

  /* GtkObjectClass */
  gobject = (GObjectClass *) osc_server_preferences;

  gobject->finalize = ags_osc_server_preferences_finalize;
}

void
ags_osc_server_preferences_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_osc_server_preferences_connect;
  connectable->disconnect = ags_osc_server_preferences_disconnect;
}

void
ags_osc_server_preferences_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_osc_server_preferences_set_update;
  applicable->apply = ags_osc_server_preferences_apply;
  applicable->reset = ags_osc_server_preferences_reset;
}

void
ags_osc_server_preferences_init(AgsOscServerPreferences *osc_server_preferences)
{
  GtkGrid *grid;
  GtkBox *hbox;
  GtkLabel *label;

  gchar *str;

  gtk_orientable_set_orientation(GTK_ORIENTABLE(osc_server_preferences),
				 GTK_ORIENTATION_VERTICAL);
  
  osc_server_preferences->flags = 0;

  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("Note: the fields below are applied immediately"),
				    "xalign", 0.0,
				    NULL);
  gtk_box_pack_start((GtkBox *) osc_server_preferences,
		     (GtkWidget *) label,
		     FALSE, FALSE,
		     0);

  /* grid */
  grid = (GtkGrid *) gtk_grid_new();
  gtk_box_pack_start((GtkBox *) osc_server_preferences,
		     (GtkWidget *) grid,
		     FALSE, FALSE,
		     0);

  /* auto-start */
  osc_server_preferences->auto_start = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("auto-start OSC server"));

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->auto_start,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->auto_start,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->auto_start,
		  0, 0,
		  2, 1);

  /* start/stop */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				0);

  gtk_widget_set_valign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) hbox,
		  0, 1,
		  2, 1);
  
  osc_server_preferences->start = (GtkButton *) gtk_button_new_with_label(i18n("start OSC server"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) osc_server_preferences->start,
		     FALSE, FALSE,
		     0);

  osc_server_preferences->stop = (GtkButton *) gtk_button_new_with_label(i18n("stop OSC server"));
  gtk_box_pack_start(hbox,
		     (GtkWidget *) osc_server_preferences->stop,
		     FALSE, FALSE,
		     0);
  
  /* listen any address */
  osc_server_preferences->any_address = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("listen on any address"));

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->any_address,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->any_address,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->any_address,
		  0, 2,
		  2, 1);

  /* enable IP4 address */
  osc_server_preferences->enable_ip4 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enable IPv4"));

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->enable_ip4,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->enable_ip4,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->enable_ip4,
		  0, 3,
		  2, 1);

  /* IP4 address */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("IPv4 address"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 4,
		  1, 1);

  osc_server_preferences->ip4_address = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(osc_server_preferences->ip4_address,
		     AGS_OSC_SERVER_DEFAULT_INET4_ADDRESS);

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->ip4_address,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->ip4_address,
			GTK_ALIGN_FILL);
  
  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->ip4_address,
		  1, 4,
		  1, 1);

  /* enable IP6 address */
  osc_server_preferences->enable_ip6 = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enable IPv6"));

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->enable_ip6,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->enable_ip6,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  GTK_WIDGET(osc_server_preferences->enable_ip6),
		  0, 5,
		  2, 1);

  /* IP6 address */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("IPv6 address"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  (GtkWidget *) label,
		  0, 6,
		  1, 1);

  osc_server_preferences->ip6_address = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(osc_server_preferences->ip6_address,
		     AGS_OSC_SERVER_DEFAULT_INET6_ADDRESS);

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->ip6_address,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->ip6_address,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->ip6_address,
		  1, 6,
		  1, 1);
  
  /* port */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("port"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 7,
		  1, 1);

  str = g_strdup_printf("%d",
			AGS_OSC_SERVER_DEFAULT_SERVER_PORT);
  osc_server_preferences->port = (GtkEntry *) gtk_entry_new();
  gtk_entry_set_text(osc_server_preferences->port,
		     str);

  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->port,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->port,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->port,
		  1, 7,
		  1, 1);

  g_free(str);

  /* monitor timeout */
  label = (GtkLabel *) g_object_new(GTK_TYPE_LABEL,
				    "label", i18n("monitor timeout"),
				    "xalign", 0.0,
				    NULL);

  gtk_widget_set_valign((GtkWidget *) label,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) label,
			GTK_ALIGN_FILL);

  gtk_widget_set_margin_end((GtkWidget *) label,
			    AGS_UI_PROVIDER_DEFAULT_MARGIN_END);

  gtk_grid_attach(grid,
		  GTK_WIDGET(label),
		  0, 8,
		  1, 1);

  osc_server_preferences->monitor_timeout = (GtkSpinButton *) gtk_spin_button_new_with_range(0.000001, 10.0, 0.0001);
  gtk_spin_button_set_digits(osc_server_preferences->monitor_timeout,
			     9);
  gtk_spin_button_set_value(osc_server_preferences->monitor_timeout,
			    AGS_OSC_METER_CONTROLLER_DEFAULT_MONITOR_TIMEOUT);


  gtk_widget_set_valign((GtkWidget *) osc_server_preferences->monitor_timeout,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) osc_server_preferences->monitor_timeout,
			GTK_ALIGN_FILL);

  gtk_grid_attach(grid,
		  (GtkWidget *) osc_server_preferences->monitor_timeout,
		  1, 8,
		  1, 1);
}

void
ags_osc_server_preferences_finalize(GObject *gobject)
{
  //TODO:JK: implement me
  
  /* call parent */
  G_OBJECT_CLASS(ags_osc_server_preferences_parent_class)->finalize(gobject);
}

void
ags_osc_server_preferences_connect(AgsConnectable *connectable)
{
  AgsOscServerPreferences *osc_server_preferences;

  osc_server_preferences = AGS_OSC_SERVER_PREFERENCES(connectable);

  if((AGS_OSC_SERVER_PREFERENCES_CONNECTED & (osc_server_preferences->flags)) != 0){
    return;
  }

  osc_server_preferences->flags |= AGS_OSC_SERVER_PREFERENCES_CONNECTED;

  /* start/stop */
  g_signal_connect(G_OBJECT(osc_server_preferences->start), "clicked",
		   G_CALLBACK(ags_osc_server_preferences_start_callback), osc_server_preferences);

  g_signal_connect(G_OBJECT(osc_server_preferences->stop), "clicked",
		   G_CALLBACK(ags_osc_server_preferences_stop_callback), osc_server_preferences);

  /* listen any address */
  g_signal_connect_after(G_OBJECT(osc_server_preferences->any_address), "clicked",
			 G_CALLBACK(ags_osc_server_preferences_any_address_callback), osc_server_preferences);

  /* IPv4 */
  g_signal_connect_after(G_OBJECT(osc_server_preferences->enable_ip4), "clicked",
			 G_CALLBACK(ags_osc_server_preferences_enable_ip4_callback), osc_server_preferences);

  g_signal_connect_after(G_OBJECT(osc_server_preferences->ip4_address), "changed",
			 G_CALLBACK(ags_osc_server_preferences_ip4_address_callback), osc_server_preferences);

  /* IPv6 */
  g_signal_connect_after(G_OBJECT(osc_server_preferences->enable_ip6), "clicked",
			 G_CALLBACK(ags_osc_server_preferences_enable_ip6_callback), osc_server_preferences);

  g_signal_connect_after(G_OBJECT(osc_server_preferences->ip6_address), "changed",
			 G_CALLBACK(ags_osc_server_preferences_ip6_address_callback), osc_server_preferences);

  /* port */
  g_signal_connect_after(G_OBJECT(osc_server_preferences->port), "changed",
			 G_CALLBACK(ags_osc_server_preferences_port_callback), osc_server_preferences);
}

void
ags_osc_server_preferences_disconnect(AgsConnectable *connectable)
{
  AgsOscServerPreferences *osc_server_preferences;

  osc_server_preferences = AGS_OSC_SERVER_PREFERENCES(connectable);

  if((AGS_OSC_SERVER_PREFERENCES_CONNECTED & (osc_server_preferences->flags)) == 0){
    return;
  }

  osc_server_preferences->flags &= (~AGS_OSC_SERVER_PREFERENCES_CONNECTED);

  /* start/stop */
  g_object_disconnect(G_OBJECT(osc_server_preferences->start),
		      "any_signal::clicked",
		      G_CALLBACK(ags_osc_server_preferences_start_callback),
		      osc_server_preferences,
		      NULL);

  g_object_disconnect(G_OBJECT(osc_server_preferences->stop),
		      "any_signal::clicked",
		      G_CALLBACK(ags_osc_server_preferences_stop_callback),
		      osc_server_preferences,
		      NULL);

  /* listen any address */
  g_object_disconnect(G_OBJECT(osc_server_preferences->any_address),
		      "any_signal::clicked",
		      G_CALLBACK(ags_osc_server_preferences_any_address_callback),
		      osc_server_preferences,
		      NULL);

  /* IPv4 */
  g_object_disconnect(G_OBJECT(osc_server_preferences->enable_ip4),
		      "any_signal::clicked",
		      G_CALLBACK(ags_osc_server_preferences_enable_ip6_callback),
		      osc_server_preferences,
		      NULL);

  g_object_disconnect(G_OBJECT(osc_server_preferences->ip4_address),
		      "any_signal::changed",
		      G_CALLBACK(ags_osc_server_preferences_ip4_address_callback),
		      osc_server_preferences,
		      NULL);

  /* IPv6 */
  g_object_disconnect(G_OBJECT(osc_server_preferences->enable_ip6),
		      "any_signal::clicked",
		      G_CALLBACK(ags_osc_server_preferences_enable_ip6_callback),
		      osc_server_preferences,
		      NULL);

  g_object_disconnect(G_OBJECT(osc_server_preferences->ip6_address),
		      "any_signal::changed",
		      G_CALLBACK(ags_osc_server_preferences_ip6_address_callback),
		      osc_server_preferences,
		      NULL);
  
  /* port */
  g_object_disconnect(G_OBJECT(osc_server_preferences->port),
		      "any_signal::changed",
		      G_CALLBACK(ags_osc_server_preferences_port_callback),
		      osc_server_preferences,
		      NULL);
}

void
ags_osc_server_preferences_set_update(AgsApplicable *applicable, gboolean update)
{
  //TODO:JK: implement me
}

void
ags_osc_server_preferences_apply(AgsApplicable *applicable)
{
  AgsOscServerPreferences *osc_server_preferences;

  AgsConfig *config;

  gchar *str;
  
  osc_server_preferences = AGS_OSC_SERVER_PREFERENCES(applicable);
  
  config = ags_config_get_instance();

  /* auto-start */
  if(gtk_toggle_button_get_active((GtkToggleButton *) osc_server_preferences->auto_start)){
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "auto-start",
			 "true");
  }else{
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "auto-start",
			 "false");
  }

  /* any address */
  if(gtk_toggle_button_get_active((GtkToggleButton *) osc_server_preferences->any_address)){
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "any-address",
			 "true");
  }else{
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "any-address",
			 "false");
  }

  /* IPv4 */
  if(gtk_toggle_button_get_active((GtkToggleButton *) osc_server_preferences->enable_ip4)){
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "enable-ip4",
			 "true");
  }else{
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "enable-ip4",
			 "false");
  }

  str = gtk_entry_get_text(osc_server_preferences->ip4_address);

  if(str == NULL){
    str = AGS_OSC_SERVER_DEFAULT_INET4_ADDRESS;
  }
  
  ags_config_set_value(config,
		       AGS_CONFIG_OSC_SERVER,
		       "ip4-address",
		       str);
  
  /* IPv6 */
  if(gtk_toggle_button_get_active((GtkToggleButton *) osc_server_preferences->enable_ip6)){
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "enable-ip6",
			 "true");
  }else{
    ags_config_set_value(config,
			 AGS_CONFIG_OSC_SERVER,
			 "enable-ip6",
			 "false");
  }

  str = gtk_entry_get_text(osc_server_preferences->ip6_address);

  if(str == NULL){
    str = AGS_OSC_SERVER_DEFAULT_INET6_ADDRESS;
  }
    
  ags_config_set_value(config,
		       AGS_CONFIG_OSC_SERVER,
		       "ip6-address",
		       str);

  /* port */
  str = gtk_entry_get_text(osc_server_preferences->port);

  if(str == NULL){
    str = g_strdup_printf("%d",
			  AGS_OSC_SERVER_DEFAULT_SERVER_PORT);
  }
  
  ags_config_set_value(config,
		       AGS_CONFIG_OSC_SERVER,
		       "server-port",
		       str);

  /* monitor timeout */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(osc_server_preferences->monitor_timeout));
  
  ags_config_set_value(config,
		       AGS_CONFIG_OSC_SERVER,
		       "monitor-timeout",
		       str);
}

void
ags_osc_server_preferences_reset(AgsApplicable *applicable)
{
  AgsOscServerPreferences *osc_server_preferences;

  AgsConfig *config;
   
  gchar *str;

  osc_server_preferences = AGS_OSC_SERVER_PREFERENCES(applicable);

  config = ags_config_get_instance();

  /* block update */
  osc_server_preferences->flags |= AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE;
  
  /* auto-start */
  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "auto-start");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "true",
			  5)){
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->auto_start,
				 TRUE);
  }else{
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->auto_start,
				 FALSE);
  }

  /* any address */
  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "any-address");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "true",
			  5)){
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->any_address,
				 TRUE);
  }else{
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->any_address,
				 FALSE);
  }

  /* IPv4 */
  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "enable-ip4");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "true",
			  5)){
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->enable_ip4,
				 TRUE);
  }else{
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->enable_ip4,
				 FALSE);
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "ip4-address");

  if(str != NULL){
    gtk_entry_set_text(osc_server_preferences->ip4_address,
		       str);
  }
  
  /* IPv6 */
  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "enable-ip6");

  if(str != NULL &&
     !g_ascii_strncasecmp(str,
			  "true",
			  5)){
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->enable_ip6,
				 TRUE);
  }else{
    gtk_toggle_button_set_active((GtkToggleButton *) osc_server_preferences->enable_ip6,
				 FALSE);
  }

  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "ip6-address");

  if(str != NULL){
    gtk_entry_set_text(osc_server_preferences->ip6_address,
		       str);
  }
  
  /* port */
  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "server-port");

  if(str != NULL){
    gtk_entry_set_text(osc_server_preferences->port,
		       str);
  }

  /* monitor timeout */
  str = ags_config_get_value(config,
			     AGS_CONFIG_OSC_SERVER,
			     "monitor-timeout");

  if(str != NULL){
    gtk_spin_button_set_value(osc_server_preferences->monitor_timeout,
			      g_ascii_strtod(str,
					     NULL));
  }
  
  /* unblock update */
  osc_server_preferences->flags &= (~AGS_OSC_SERVER_PREFERENCES_BLOCK_UPDATE);
}

/**
 * ags_osc_server_preferences_new:
 *
 * Create a new instance of #AgsOscServerPreferences
 *
 * Returns: the new #AgsOscServerPreferences
 *
 * Since: 3.0.0
 */
AgsOscServerPreferences*
ags_osc_server_preferences_new()
{
  AgsOscServerPreferences *osc_server_preferences;

  osc_server_preferences = (AgsOscServerPreferences *) g_object_new(AGS_TYPE_OSC_SERVER_PREFERENCES,
								    NULL);
  
  return(osc_server_preferences);
}
