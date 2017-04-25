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

#include <ags/X/export/ags_midi_export_wizard.h>
#include <ags/X/export/ags_midi_export_wizard_callbacks.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

#include <ags/X/ags_window.h>

#include <ags/X/export/ags_machine_collection.h>
#include <ags/X/export/ags_machine_collection_entry.h>

void ags_midi_export_wizard_class_init(AgsMidiExportWizardClass *midi_export_wizard);
void ags_midi_export_wizard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_export_wizard_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_export_wizard_init(AgsMidiExportWizard *midi_export_wizard);
void ags_midi_export_wizard_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_midi_export_wizard_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_midi_export_wizard_connect(AgsConnectable *connectable);
void ags_midi_export_wizard_disconnect(AgsConnectable *connectable);
void ags_midi_export_wizard_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_export_wizard_apply(AgsApplicable *applicable);
void ags_midi_export_wizard_reset(AgsApplicable *applicable);
void ags_midi_export_wizard_show(GtkWidget *widget);

/**
 * SECTION:ags_midi_export_wizard
 * @short_description: pack pad editors.
 * @title: AgsMidiExportWizard
 * @section_id:
 * @include: ags/X/ags_midi_export_wizard.h
 *
 * #AgsMidiExportWizard is a wizard to select machines in order
 * to export to midi files.
 */

static gpointer ags_midi_export_wizard_parent_class = NULL;

enum{
  PROP_0,
  PROP_APPLICATION_CONTEXT,
  PROP_MAIN_WINDOW,
};

GType
ags_midi_export_wizard_get_type(void)
{
  static GType ags_type_midi_export_wizard = 0;

  if(!ags_type_midi_export_wizard){
    static const GTypeInfo ags_midi_export_wizard_info = {
      sizeof (AgsMidiExportWizardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_export_wizard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiExportWizard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_export_wizard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_export_wizard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_export_wizard_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_export_wizard = g_type_register_static(GTK_TYPE_DIALOG,
							 "AgsMidiExportWizard\0", &ags_midi_export_wizard_info,
							 0);

    g_type_add_interface_static(ags_type_midi_export_wizard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_export_wizard,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_midi_export_wizard);
}

void
ags_midi_export_wizard_class_init(AgsMidiExportWizardClass *midi_export_wizard)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;
  GParamSpec *param_spec;

  ags_midi_export_wizard_parent_class = g_type_class_peek_parent(midi_export_wizard);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_export_wizard;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) midi_export_wizard;

  gobject->set_property = ags_midi_export_wizard_set_property;
  gobject->get_property = ags_midi_export_wizard_get_property;

  /* properties */
  /**
   * AgsMidiExportWizard:application-context:
   *
   * The assigned #AgsApplicationContext to give control of application.
   * 
   * Since: 0.8.0
   */
  param_spec = g_param_spec_object("application-context\0",
				   "assigned application context\0",
				   "The AgsApplicationContext it is assigned with\0",
				   G_TYPE_OBJECT,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_APPLICATION_CONTEXT,
				  param_spec);

  /**
   * AgsMidiExportWizard:main-window:
   *
   * The assigned #AgsWindow.
   * 
   * Since: 0.8.0
   */
  param_spec = g_param_spec_object("main-window\0",
				   "assigned main window\0",
				   "The assigned main window\0",
				   AGS_TYPE_WINDOW,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_MAIN_WINDOW,
				  param_spec);

  /* GtkWidget */
  widget->show = ags_midi_export_wizard_show;
}

void
ags_midi_export_wizard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_midi_export_wizard_connect;
  connectable->disconnect = ags_midi_export_wizard_disconnect;
}

void
ags_midi_export_wizard_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_export_wizard_set_update;
  applicable->apply = ags_midi_export_wizard_apply;
  applicable->reset = ags_midi_export_wizard_reset;
}

void
ags_midi_export_wizard_init(AgsMidiExportWizard *midi_export_wizard)
{
  GtkAlignment *alignment;

  midi_export_wizard->flags = AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION;

  midi_export_wizard->application_context = NULL;

  midi_export_wizard->main_window = NULL;

  /* midi builder */
  midi_export_wizard->midi_builder = ags_midi_builder_new(NULL);
  midi_export_wizard->pulse_unit = AGS_MIDI_EXPORT_WIZARD_DEFAULT_PULSE_UNIT;
  
  /* machine collection */
  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.0,
						 1.0, 1.0);
  gtk_widget_set_no_show_all((GtkWidget *) alignment,
			     TRUE);
  gtk_box_pack_start((GtkBox *) midi_export_wizard->dialog.vbox,
		     (GtkWidget*) alignment,
		     TRUE, TRUE,
		     0);

  midi_export_wizard->machine_collection = (GtkWidget *) ags_machine_collection_new(AGS_TYPE_MACHINE_COLLECTION_ENTRY,
										    0,
										    NULL);
  gtk_container_add((GtkContainer *) alignment,
		    midi_export_wizard->machine_collection);
  
  /* file chooser */
  alignment = g_object_new(GTK_TYPE_ALIGNMENT,
			   NULL);
  gtk_widget_set_no_show_all((GtkWidget *) alignment,
			     TRUE);
  gtk_box_pack_start((GtkBox *) midi_export_wizard->dialog.vbox,
		     (GtkWidget*) alignment,
		     TRUE, TRUE,
		     0);
  
  midi_export_wizard->file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);
  gtk_file_chooser_set_filename(midi_export_wizard->file_chooser,
				AGS_MIDI_EXPORT_WIZARD_DEFAULT_FILENAME);
  gtk_container_add((GtkContainer *) alignment,
		    midi_export_wizard->file_chooser);
  
  gtk_dialog_add_buttons((GtkDialog *) midi_export_wizard,
			 GTK_STOCK_GO_BACK, GTK_RESPONSE_REJECT,
			 GTK_STOCK_GO_FORWARD, GTK_RESPONSE_ACCEPT,
			 GTK_STOCK_OK, GTK_RESPONSE_OK,
			 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			 NULL);
}

void
ags_midi_export_wizard_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      AgsApplicationContext *application_context;

      application_context = (AgsApplicationContext *) g_value_get_object(value);

      if((AgsApplicationContext *) midi_export_wizard->application_context == application_context){
	return;
      }
      
      if(midi_export_wizard->application_context != NULL){
	g_object_unref(midi_export_wizard->application_context);
      }

      if(application_context != NULL){
	g_object_ref(application_context);
      }

      midi_export_wizard->application_context = (GObject *) application_context;
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      AgsWindow *main_window;

      main_window = (AgsWindow *) g_value_get_object(value);

      if((AgsWindow *) midi_export_wizard->main_window == main_window){
	return;
      }

      if(midi_export_wizard->main_window != NULL){
	g_object_unref(midi_export_wizard->main_window);
      }

      if(main_window != NULL){
	g_object_ref(main_window);
      }

      midi_export_wizard->main_window = (GObject *) main_window;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_export_wizard_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(gobject);

  switch(prop_id){
  case PROP_APPLICATION_CONTEXT:
    {
      g_value_set_object(value, midi_export_wizard->application_context);
    }
    break;
  case PROP_MAIN_WINDOW:
    {
      g_value_set_object(value, midi_export_wizard->main_window);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_export_wizard_connect(AgsConnectable *connectable)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(connectable);

  if((AGS_MIDI_EXPORT_WIZARD_CONNECTED & (midi_export_wizard->flags)) != 0){
    return;
  }

  midi_export_wizard->flags |= AGS_MIDI_EXPORT_WIZARD_CONNECTED;
  
  g_signal_connect(midi_export_wizard, "response\0",
		   G_CALLBACK(ags_midi_export_wizard_response_callback), NULL);

  ags_connectable_connect(AGS_CONNECTABLE(midi_export_wizard->machine_collection));
}

void
ags_midi_export_wizard_disconnect(AgsConnectable *connectable)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(connectable);

  if((AGS_MIDI_EXPORT_WIZARD_CONNECTED & (midi_export_wizard->flags)) == 0){
    return;
  }

  midi_export_wizard->flags &= (~AGS_MIDI_EXPORT_WIZARD_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(midi_export_wizard->machine_collection));
}

void
ags_midi_export_wizard_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(midi_export_wizard->machine_collection), update);
}

void
ags_midi_export_wizard_apply(AgsApplicable *applicable)
{
  AgsWindow *window;
  AgsMidiExportWizard *midi_export_wizard;

  FILE *file;

  GList *list, *list_start;
  
  gchar *filename;

  guint track_count;
  guint division;
  guint times;
  guint bpm;
  guint clicks;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(applicable);

  window = midi_export_wizard->main_window;
  
  /* retrieve BPM */
  bpm = gtk_spin_button_get_value_as_int(window->navigation->bpm);
  
  /* find tracks */
  list =
    list_start = gtk_container_get_children(AGS_MACHINE_COLLECTION(midi_export_wizard->machine_collection)->child);

  while(list != NULL){
    if(gtk_toggle_button_get_active(AGS_MACHINE_COLLECTION_ENTRY(list->data)->enabled)){
      track_count++;
    }
    
    list = list->next;
  }

  g_list_free(list_start);

  if(track_count == 0){
    return;
  }

  /* set division, times and clicks */
  division = AGS_MIDI_EXPORT_WIZARD_DEFAULT_DIVISION;

  times = AGS_MIDI_EXPORT_WIZARD_DEFAULT_TIMES;
  clicks = AGS_MIDI_EXPORT_WIZARD_DEFAULT_CLICKS;  

  /* set pulse unit */
  midi_export_wizard->pulse_unit = (16.0 * bpm / 60.0 * 1.0 / (division >> 8) / (0xff & division) * 1000000.0);
  
  /* open file */
  filename = gtk_file_chooser_get_filename(midi_export_wizard->file_chooser);
  
  file = fopen(filename, "w\0");
  g_object_set(midi_export_wizard->midi_builder,
	       "file\0", file,
	       NULL);

  /* add header */
  ags_midi_builder_append_header(midi_export_wizard->midi_builder,
				 0, 1,
				 track_count, division,
				 times, bpm,
				 clicks);
  
  /* apply tracks */
  ags_applicable_apply(AGS_APPLICABLE(midi_export_wizard->machine_collection));

  /* build and write */
  ags_midi_builder_build(midi_export_wizard->midi_builder);

  fwrite(midi_export_wizard->midi_builder->data,
	 sizeof(unsigned char),
	 midi_export_wizard->midi_builder->length,
	 file);
}

void
ags_midi_export_wizard_reset(AgsApplicable *applicable)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(applicable);

  ags_machine_collection_reload(midi_export_wizard->machine_collection);
  
  ags_applicable_reset(AGS_APPLICABLE(midi_export_wizard->machine_collection));
}

void
ags_midi_export_wizard_show(GtkWidget *widget)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(widget);

  GTK_WIDGET_CLASS(ags_midi_export_wizard_parent_class)->show(widget);

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (midi_export_wizard->flags)) != 0){
    gtk_widget_show(midi_export_wizard->file_chooser->parent);
    gtk_widget_show_all(midi_export_wizard->file_chooser);
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION & (midi_export_wizard->flags)) != 0){
    gtk_widget_show(midi_export_wizard->machine_collection->parent);
    gtk_widget_show_all(midi_export_wizard->machine_collection);
  }
}

/**
 * ags_midi_export_wizard_new:
 *
 * Creates an #AgsMidiExportWizard
 *
 * Returns: a new #AgsMidiExportWizard
 *
 * Since: 0.8.0
 */
AgsMidiExportWizard*
ags_midi_export_wizard_new()
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = (AgsMidiExportWizard *) g_object_new(AGS_TYPE_MIDI_EXPORT_WIZARD,
							    NULL);
  
  return(midi_export_wizard);
}
