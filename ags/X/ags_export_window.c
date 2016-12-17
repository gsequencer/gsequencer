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

#include <ags/X/ags_export_window.h>
#include <ags/X/ags_export_window_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_soundcard.h>

#include <ags/audio/ags_notation.h>

#include <ags/X/ags_navigation.h>

#include <stdlib.h>

void ags_export_window_class_init(AgsExportWindowClass *export_window);
void ags_export_window_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_export_window_init(AgsExportWindow *export_window);
void ags_export_window_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec);
void ags_export_window_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec);
void ags_export_window_finalize(GObject *gobject);
void ags_export_window_connect(AgsConnectable *connectable);
void ags_export_window_disconnect(AgsConnectable *connectable);
void ags_export_window_show(GtkWidget *widget);
gboolean ags_export_window_delete_event(GtkWidget *widget, GdkEventAny *event);

/**
 * SECTION:ags_export_window
 * @short_description: The export dialog
 * @title: AgsExportWindow
 * @section_id:
 * @include: ags/X/ags_export_window.h
 *
 * #AgsExportWindow lets you export to audio files.
 */

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
};

static gpointer ags_export_window_parent_class = NULL;

GType
ags_export_window_get_type()
{
  static GType ags_type_export_window = 0;

  if(!ags_type_export_window){
    static const GTypeInfo ags_export_window_info = {
      sizeof (AgsExportWindowClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_export_window_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsExportWindow),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_export_window_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_export_window_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_export_window = g_type_register_static(GTK_TYPE_WINDOW,
						    "AgsExportWindow\0", &ags_export_window_info,
						    0);
    
    g_type_add_interface_static(ags_type_export_window,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_export_window);
}

void
ags_export_window_class_init(AgsExportWindowClass *export_window)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_export_window_parent_class = g_type_class_peek_parent(export_window);

  /* GObjectClass */
  gobject = (GObjectClass *) export_window;

  gobject->set_property = ags_export_window_set_property;
  gobject->get_property = ags_export_window_get_property;

  gobject->finalize = ags_export_window_finalize;

  /* properties */
  /**
   * AgsExportWindow:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 0.4
   */
  param_spec = g_param_spec_object("application-context\0",
				   "assigned application context\0",
				   "The AgsApplicationContext it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);


  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) export_window;

  widget->show = ags_export_window_show;
  widget->delete_event = ags_export_window_delete_event;
}

void
ags_export_window_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_export_window_connect;
  connectable->disconnect = ags_export_window_disconnect;
}

void
ags_export_window_init(AgsExportWindow *export_window)
{
  GtkVBox *vbox;
  GtkHBox *hbox;
  GtkTable *table;
  GtkLabel *label;

  gchar *str;
  
  export_window->flags = 0;

  g_object_set(export_window,
	       "title\0", "export to audio data\0",
	       NULL);

  vbox = (GtkVBox *) gtk_vbox_new(FALSE,
				  0);
  gtk_container_add(GTK_CONTAINER(export_window),
		    GTK_WIDGET(vbox));

  /* live export */
  str = ags_config_get_value(ags_config_get_instance(),
			     AGS_CONFIG_GENERIC,
			     "disable-feature\0");
  
  if(str == NULL ||
     !g_ascii_strncasecmp(str,
			  "experimental\0",
			  13)){
    export_window->live_export = NULL;
  }else{
    export_window->live_export = (GtkCheckButton *) gtk_check_button_new_with_label("live export\0");
    gtk_toggle_button_set_active((GtkToggleButton *) export_window->live_export,
				 TRUE);
    gtk_box_pack_start(GTK_BOX(vbox),
		       GTK_WIDGET(export_window->live_export),
		       FALSE, FALSE,
		       0);
  }

  g_free(str);
  
  /* exclude sequencer */
  export_window->exclude_sequencer = (GtkCheckButton *) gtk_check_button_new_with_label("exclude sequencers\0");
  gtk_toggle_button_set_active((GtkToggleButton *) export_window->exclude_sequencer,
			       TRUE);
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(export_window->exclude_sequencer),
		     FALSE, FALSE,
		     0);

  table = (GtkTable *) gtk_table_new(5, 2,
				     FALSE);
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(table),
		     FALSE, FALSE,
		     0);

  /* mode */
  label = (GtkLabel *) gtk_label_new("mode\0");
  g_object_set(G_OBJECT(label),
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   1, 2,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  export_window->mode = (GtkComboBoxText *) gtk_combo_box_text_new();
  gtk_combo_box_text_append_text(export_window->mode,
				 "tact\0");
  //TODO:JK: uncomment me
  //  gtk_combo_box_text_append_text(export_window->mode,
  //				 "time\0");
  gtk_combo_box_set_active((GtkComboBox *) export_window->mode,
			   0);
  gtk_table_attach(table,
		   GTK_WIDGET(export_window->mode),
		   1, 2,
		   1, 2,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* tact */
  label = (GtkLabel *) gtk_label_new("tact\0");
  g_object_set(G_OBJECT(label),
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   2, 3,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  export_window->tact = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, AGS_NOTATION_DEFAULT_LENGTH, 0.25);
  gtk_spin_button_set_digits(export_window->tact,
			     2);
  gtk_table_attach(table,
		   GTK_WIDGET(export_window->tact),
		   1, 2,
		   2, 3,
		   GTK_FILL, GTK_FILL,
		   0, 0);

  /* time */
  label = (GtkLabel *) gtk_label_new("time\0");
  g_object_set(G_OBJECT(label),
	       "xalign\0", 0.0,
	       NULL);
  gtk_table_attach(table,
		   GTK_WIDGET(label),
		   0, 1,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  /* duration */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_table_attach(table,
		   GTK_WIDGET(hbox),
		   1, 2,
		   3, 4,
		   GTK_FILL|GTK_EXPAND, GTK_FILL|GTK_EXPAND,
		   0, 0);

  export_window->duration = (GtkLabel *) gtk_label_new(ags_navigation_tact_to_time_string(0.0,
											  AGS_SOUNDCARD_DEFAULT_BPM,
											  1.0));
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(export_window->duration),
		     FALSE, FALSE,
		     0);

  /* export */
  hbox = (GtkHBox *) gtk_hbox_new(FALSE,
				  0);
  gtk_box_pack_start(GTK_BOX(vbox),
		     GTK_WIDGET(hbox),
		     FALSE, FALSE,
		     0);

  export_window->export = (GtkToggleButton *) gtk_toggle_button_new_with_label("export\0");
  gtk_box_pack_start(GTK_BOX(hbox),
		     GTK_WIDGET(export_window->export),
		     FALSE, FALSE,
		     0);

}

void
ags_export_window_set_property(GObject *gobject,
			       guint prop_id,
			       const GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) export_window->application_context == application_context)
	return;

      if(export_window->application_context != NULL){
	g_object_unref(export_window->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      export_window->application_context = (GObject *) application_context;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_window_get_property(GObject *gobject,
			       guint prop_id,
			       GValue *value,
			       GParamSpec *param_spec)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, export_window->application_context);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_export_window_connect(AgsConnectable *connectable)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(connectable);

  if((AGS_EXPORT_WINDOW_CONNECTED & (export_window->flags)) != 0){
    return;
  }

  export_window->flags |= AGS_EXPORT_WINDOW_CONNECTED;

  g_signal_connect_after(G_OBJECT(export_window->tact), "value-changed\0",
			 G_CALLBACK(ags_export_window_tact_callback), export_window);

  g_signal_connect_after(G_OBJECT(export_window->export), "clicked\0",
			 G_CALLBACK(ags_export_window_export_callback), export_window);
}

void
ags_export_window_disconnect(AgsConnectable *connectable)
{
  AgsExportWindow *export_window;

  export_window = AGS_EXPORT_WINDOW(connectable);

  if((AGS_EXPORT_WINDOW_CONNECTED & (export_window->flags)) == 0){
    return;
  }

  export_window->flags &= (~AGS_EXPORT_WINDOW_CONNECTED);
  
  g_object_disconnect(G_OBJECT(export_window->tact),
		      "value-changed\0",
		      G_CALLBACK(ags_export_window_tact_callback),
		      export_window,
		      NULL);

  g_object_disconnect(G_OBJECT(export_window->export),
		      "clicked\0",
		      G_CALLBACK(ags_export_window_export_callback),
		      export_window,
		      NULL);
}

void
ags_export_window_finalize(GObject *gobject)
{
  AgsExportWindow *export_window;

  export_window = (AgsExportWindow *) gobject;

  if(export_window->application_context != NULL){
    g_object_unref(export_window->application_context);
  }
  
  G_OBJECT_CLASS(ags_export_window_parent_class)->finalize(gobject);
}

void
ags_export_window_show(GtkWidget *widget)
{
  GTK_WIDGET_CLASS(ags_export_window_parent_class)->show(widget);
}

gboolean
ags_export_window_delete_event(GtkWidget *widget, GdkEventAny *event)
{
  gtk_widget_hide(widget);

  //  GTK_WIDGET_CLASS(ags_export_window_parent_class)->delete_event(widget, event);

  return(TRUE);
}

/**
 * ags_export_window_new:
 * 
 * Instantiate an #AgsExportWindow
 * 
 * Returns: an #AgsExportWindow
 * 
 * Since: 0.4
 */
AgsExportWindow*
ags_export_window_new()
{
  AgsExportWindow *export_window;

  export_window = (AgsExportWindow *) g_object_new(AGS_TYPE_EXPORT_WINDOW, NULL);

  return(export_window);
}
