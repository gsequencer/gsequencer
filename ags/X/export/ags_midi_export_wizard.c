/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#include <ags/X/ags_ui_provider.h>
#include <ags/X/ags_window.h>

#include <ags/X/export/ags_machine_collection.h>
#include <ags/X/export/ags_machine_collection_entry.h>

#include <ags/i18n.h>

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
void ags_midi_export_wizard_show_all(GtkWidget *widget);

/**
 * SECTION:ags_midi_export_wizard
 * @short_description: Export to MIDI using wizard
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
};

GType
ags_midi_export_wizard_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_export_wizard = 0;

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
							 "AgsMidiExportWizard", &ags_midi_export_wizard_info,
							 0);

    g_type_add_interface_static(ags_type_midi_export_wizard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_export_wizard,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_export_wizard);
  }

  return g_define_type_id__volatile;
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

  /* GtkWidget */
  widget->show = ags_midi_export_wizard_show;
  widget->show_all = ags_midi_export_wizard_show_all;
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
  midi_export_wizard->flags = AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION;

  /* midi builder */
  midi_export_wizard->midi_builder = ags_midi_builder_new(NULL);
  midi_export_wizard->pulse_unit = AGS_MIDI_EXPORT_WIZARD_DEFAULT_PULSE_UNIT;
  
  /* machine collection */
  midi_export_wizard->machine_collection = (GtkWidget *) ags_machine_collection_new(AGS_TYPE_MACHINE_COLLECTION_ENTRY,
										    0,
										    NULL,
										    NULL);
//  gtk_widget_set_no_show_all((GtkWidget *) midi_export_wizard->machine_collection,
//			     TRUE);

  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) midi_export_wizard),
		     (GtkWidget*) midi_export_wizard->machine_collection,
		     TRUE, TRUE,
		     0);
  
  /* file chooser */  
  midi_export_wizard->file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_SAVE);
  gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(midi_export_wizard->file_chooser),
				AGS_MIDI_EXPORT_WIZARD_DEFAULT_FILENAME);

//  gtk_widget_set_no_show_all((GtkWidget *) midi_export_wizard->file_chooser,
//			     TRUE);

  gtk_box_pack_start((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) midi_export_wizard),
		     (GtkWidget*) midi_export_wizard->file_chooser,
		     TRUE, TRUE,
		     0);
  
  gtk_dialog_add_buttons((GtkDialog *) midi_export_wizard,
			 i18n("_Back"), GTK_RESPONSE_REJECT,
			 i18n("_Forward"), GTK_RESPONSE_ACCEPT,
			 i18n("_OK"), GTK_RESPONSE_OK,
			 i18n("_Cancel"), GTK_RESPONSE_CANCEL,
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
  
  g_signal_connect(midi_export_wizard, "response",
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
  AgsNavigation *navigation;
  AgsMidiExportWizard *midi_export_wizard;

  AgsApplicationContext *application_context;

  FILE *file;

  GList *list, *list_start;
  
  gchar *filename;

  guint track_count;
  guint division;
  guint times;
  guint bpm;
  guint clicks;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(applicable);

  application_context = ags_application_context_get_instance();
  
  /* retrieve BPM */
  navigation = ags_ui_provider_get_navigation(AGS_UI_PROVIDER(application_context));
  
  bpm = gtk_spin_button_get_value_as_int(navigation->bpm);
  
  /* find tracks */
  list =
    list_start = gtk_container_get_children(GTK_CONTAINER(AGS_MACHINE_COLLECTION(midi_export_wizard->machine_collection)->child));

  track_count = 0;
  
  while(list != NULL){
    if(gtk_toggle_button_get_active((GtkToggleButton *) AGS_MACHINE_COLLECTION_ENTRY(list->data)->enabled)){
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
  midi_export_wizard->pulse_unit = division / 16.0;
  
  /* open file */
  filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(midi_export_wizard->file_chooser));
  
  file = fopen(filename, "w");
  g_object_set(midi_export_wizard->midi_builder,
	       "file", file,
	       NULL);

  /* add header */
  ags_midi_builder_append_header(midi_export_wizard->midi_builder,
				 6, 1,
				 track_count, division,
				 times, AGS_MIDI_EXPORT_WIZARD_DEFAULT_BPM,
				 clicks);
  
  /* apply tracks */
  ags_applicable_apply(AGS_APPLICABLE(midi_export_wizard->machine_collection));

  /* build and write */
  ags_midi_builder_build(midi_export_wizard->midi_builder);

  fwrite(midi_export_wizard->midi_builder->data,
	 sizeof(unsigned char),
	 midi_export_wizard->midi_builder->length,
	 file);
  fflush(file);
  fclose(file);
}

void
ags_midi_export_wizard_reset(AgsApplicable *applicable)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(applicable);

  ags_machine_collection_reload(AGS_MACHINE_COLLECTION(midi_export_wizard->machine_collection));
  
  ags_applicable_reset(AGS_APPLICABLE(midi_export_wizard->machine_collection));
}

void
ags_midi_export_wizard_show(GtkWidget *widget)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(widget);

  GTK_WIDGET_CLASS(ags_midi_export_wizard_parent_class)->show(widget);

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (midi_export_wizard->flags)) == 0){
    gtk_widget_hide(midi_export_wizard->file_chooser);
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION & (midi_export_wizard->flags)) == 0){
    gtk_widget_hide(midi_export_wizard->machine_collection);
  }
}

void
ags_midi_export_wizard_show_all(GtkWidget *widget)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(widget);

  GTK_WIDGET_CLASS(ags_midi_export_wizard_parent_class)->show_all(widget);

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (midi_export_wizard->flags)) == 0){
    gtk_widget_hide(midi_export_wizard->file_chooser);
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION & (midi_export_wizard->flags)) == 0){
    gtk_widget_hide(midi_export_wizard->machine_collection);
  }
}

/**
 * ags_midi_export_wizard_test_flags:
 * @midi_export_wizard: the #AgsMidiExportWizard
 * @flags: the flags
 * 
 * Test @flags of @midi_export_wizard.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.8.0
 */
gboolean
ags_midi_export_wizard_test_flags(AgsMidiExportWizard *midi_export_wizard,
				  guint flags)
{
  gboolean success;
  
  if(!AGS_IS_MIDI_EXPORT_WIZARD(midi_export_wizard)){
    return(FALSE);
  }

  success = (flags & (midi_export_wizard->flags)) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_midi_export_wizard_set_flags:
 * @midi_export_wizard: the #AgsMidiExportWizard
 * @flags: the flags
 * 
 * Set @flags of @midi_export_wizard.
 * 
 * Since: 3.8.0
 */
void
ags_midi_export_wizard_set_flags(AgsMidiExportWizard *midi_export_wizard,
				 guint flags)
{
  if(!AGS_IS_MIDI_EXPORT_WIZARD(midi_export_wizard)){
    return;
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION & (flags)) != 0){
    gtk_widget_show_all(midi_export_wizard->machine_collection);
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (flags)) != 0){
    gtk_widget_show_all(midi_export_wizard->file_chooser);
  }

  midi_export_wizard->flags |= flags;
}

/**
 * ags_midi_export_wizard_unset_flags:
 * @midi_export_wizard: the #AgsMidiExportWizard
 * @flags: the flags
 * 
 * Unset @flags of @midi_export_wizard.
 * 
 * Since: 3.8.0
 */
void
ags_midi_export_wizard_unset_flags(AgsMidiExportWizard *midi_export_wizard,
				   guint flags)
{
  if(!AGS_IS_MIDI_EXPORT_WIZARD(midi_export_wizard)){
    return;
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION & (flags)) != 0){
    gtk_widget_hide(midi_export_wizard->machine_collection);
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (flags)) != 0){
    gtk_widget_hide(midi_export_wizard->file_chooser);
  }

  midi_export_wizard->flags &= (~flags);
}

/**
 * ags_midi_export_wizard_new:
 *
 * Create a new instance of #AgsMidiExportWizard
 *
 * Returns: the new #AgsMidiExportWizard
 *
 * Since: 3.0.0
 */
AgsMidiExportWizard*
ags_midi_export_wizard_new()
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = (AgsMidiExportWizard *) g_object_new(AGS_TYPE_MIDI_EXPORT_WIZARD,
							    NULL);
  
  return(midi_export_wizard);
}
