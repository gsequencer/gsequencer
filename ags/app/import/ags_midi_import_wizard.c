/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#include <ags/app/import/ags_midi_import_wizard.h>
#include <ags/app/import/ags_midi_import_wizard_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/import/ags_track_collection.h>
#include <ags/app/import/ags_track_mapper.h>

#include <ags/i18n.h>

void ags_midi_import_wizard_class_init(AgsMidiImportWizardClass *midi_import_wizard);
void ags_midi_import_wizard_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_midi_import_wizard_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_midi_import_wizard_init(AgsMidiImportWizard *midi_import_wizard);
void ags_midi_import_wizard_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_midi_import_wizard_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);

void ags_midi_import_wizard_connect(AgsConnectable *connectable);
void ags_midi_import_wizard_disconnect(AgsConnectable *connectable);

void ags_midi_import_wizard_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_import_wizard_apply(AgsApplicable *applicable);
void ags_midi_import_wizard_reset(AgsApplicable *applicable);

void ags_midi_import_wizard_show(GtkWidget *widget);

/**
 * SECTION:ags_midi_import_wizard
 * @short_description: Import MIDI using wizard
 * @title: AgsMidiImportWizard
 * @section_id:
 * @include: ags/app/ags_midi_import_wizard.h
 *
 * #AgsMidiImportWizard is a wizard to import midi files and do track mapping.
 */

static gpointer ags_midi_import_wizard_parent_class = NULL;

enum{
  PROP_0,
};

GType
ags_midi_import_wizard_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_midi_import_wizard = 0;

    static const GTypeInfo ags_midi_import_wizard_info = {
      sizeof (AgsMidiImportWizardClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_import_wizard_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiImportWizard),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_import_wizard_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_midi_import_wizard_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_midi_import_wizard_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_midi_import_wizard = g_type_register_static(GTK_TYPE_DIALOG,
							 "AgsMidiImportWizard", &ags_midi_import_wizard_info,
							 0);

    g_type_add_interface_static(ags_type_midi_import_wizard,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_midi_import_wizard,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_midi_import_wizard);
  }

  return g_define_type_id__volatile;
}

void
ags_midi_import_wizard_class_init(AgsMidiImportWizardClass *midi_import_wizard)
{
  GObjectClass *gobject;
  GtkWidgetClass *widget;

  ags_midi_import_wizard_parent_class = g_type_class_peek_parent(midi_import_wizard);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_import_wizard;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) midi_import_wizard;

  gobject->set_property = ags_midi_import_wizard_set_property;
  gobject->get_property = ags_midi_import_wizard_get_property;

  /* GtkWidget */
  widget->show = ags_midi_import_wizard_show;
}

void
ags_midi_import_wizard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_midi_import_wizard_connect;
  connectable->disconnect = ags_midi_import_wizard_disconnect;
}

void
ags_midi_import_wizard_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_midi_import_wizard_set_update;
  applicable->apply = ags_midi_import_wizard_apply;
  applicable->reset = ags_midi_import_wizard_reset;
}

void
ags_midi_import_wizard_init(AgsMidiImportWizard *midi_import_wizard)
{
  AgsApplicationContext *application_context;
  
  gtk_window_set_hide_on_close((GtkWindow *) midi_import_wizard,
			       TRUE);

  gtk_window_set_default_size((GtkWindow *) midi_import_wizard,
			      600, 480);

  application_context = ags_application_context_get_instance();

  gtk_window_set_transient_for((GtkWindow *) midi_import_wizard,
			       (GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));
  
  midi_import_wizard->flags = AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER;

  /* file chooser */  
  midi_import_wizard->file_chooser = gtk_file_chooser_widget_new(GTK_FILE_CHOOSER_ACTION_OPEN);

  gtk_widget_set_halign(midi_import_wizard->file_chooser,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(midi_import_wizard->file_chooser,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand(midi_import_wizard->file_chooser,
			 TRUE);
  gtk_widget_set_vexpand(midi_import_wizard->file_chooser,
			TRUE);
  
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) midi_import_wizard),
		     (GtkWidget *) midi_import_wizard->file_chooser);

  /* track collection */
  midi_import_wizard->track_collection = (GtkWidget *) ags_track_collection_new(AGS_TYPE_TRACK_MAPPER,
										0,
										NULL,
										NULL);

  gtk_widget_set_halign(midi_import_wizard->track_collection,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(midi_import_wizard->track_collection,
			GTK_ALIGN_FILL);
  
  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) midi_import_wizard),
		 (GtkWidget*) midi_import_wizard->track_collection);
  
  gtk_dialog_add_buttons((GtkDialog *) midi_import_wizard,
			 i18n("_Back"), GTK_RESPONSE_CANCEL,
			 i18n("_Forward"), GTK_RESPONSE_OK,
			 i18n("_OK"), GTK_RESPONSE_ACCEPT,
			 i18n("_Cancel"), GTK_RESPONSE_REJECT,
			 NULL);
}

void
ags_midi_import_wizard_set_property(GObject *gobject,
				    guint prop_id,
				    const GValue *value,
				    GParamSpec *param_spec)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_import_wizard_get_property(GObject *gobject,
				    guint prop_id,
				    GValue *value,
				    GParamSpec *param_spec)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(gobject);

  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_midi_import_wizard_connect(AgsConnectable *connectable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (midi_import_wizard->connectable_flags)) != 0){
    return;
  }

  midi_import_wizard->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
  
  g_signal_connect(midi_import_wizard, "response",
		   G_CALLBACK(ags_midi_import_wizard_response_callback), NULL);

  ags_connectable_connect(AGS_CONNECTABLE(midi_import_wizard->track_collection));
}

void
ags_midi_import_wizard_disconnect(AgsConnectable *connectable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (midi_import_wizard->connectable_flags)) == 0){
    return;
  }

  midi_import_wizard->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(midi_import_wizard->track_collection));
}

void
ags_midi_import_wizard_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(applicable);

  ags_applicable_set_update(AGS_APPLICABLE(midi_import_wizard->track_collection), update);
}

void
ags_midi_import_wizard_apply(AgsApplicable *applicable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(applicable);

  ags_applicable_apply(AGS_APPLICABLE(midi_import_wizard->track_collection));
}

void
ags_midi_import_wizard_reset(AgsApplicable *applicable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(applicable);

  ags_applicable_reset(AGS_APPLICABLE(midi_import_wizard->track_collection));
}


void
ags_midi_import_wizard_show(GtkWidget *widget)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(widget);

  GTK_WIDGET_CLASS(ags_midi_import_wizard_parent_class)->show(widget);
  
  if((AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER & (midi_import_wizard->flags)) == 0){
    gtk_widget_hide(midi_import_wizard->file_chooser);
  }

  if((AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION & (midi_import_wizard->flags)) == 0){
    gtk_widget_hide(midi_import_wizard->track_collection);
  }
}

/**
 * ags_midi_import_wizard_test_flags:
 * @midi_import_wizard: the #AgsMidiImportWizard
 * @flags: the flags
 * 
 * Test @flags of @midi_import_wizard.
 * 
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.8.0
 */
gboolean
ags_midi_import_wizard_test_flags(AgsMidiImportWizard *midi_import_wizard,
				  guint flags)
{
  gboolean success;
  
  if(!AGS_IS_MIDI_IMPORT_WIZARD(midi_import_wizard)){
    return(FALSE);
  }

  success = (flags & (midi_import_wizard->flags)) ? TRUE: FALSE;
  
  return(success);
}

/**
 * ags_midi_import_wizard_set_flags:
 * @midi_import_wizard: the #AgsMidiImportWizard
 * @flags: the flags
 * 
 * Set @flags of @midi_import_wizard.
 * 
 * Since: 3.8.0
 */
void
ags_midi_import_wizard_set_flags(AgsMidiImportWizard *midi_import_wizard,
				 guint flags)
{
  if(!AGS_IS_MIDI_IMPORT_WIZARD(midi_import_wizard)){
    return;
  }

  if((AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER & (flags)) != 0){
    gtk_widget_show(midi_import_wizard->file_chooser);
  }

  if((AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION & (flags)) != 0){
    gtk_widget_show(midi_import_wizard->track_collection);
  }

  midi_import_wizard->flags |= flags;
}

/**
 * ags_midi_import_wizard_unset_flags:
 * @midi_import_wizard: the #AgsMidiImportWizard
 * @flags: the flags
 * 
 * Unset @flags of @midi_import_wizard.
 * 
 * Since: 3.8.0
 */
void
ags_midi_import_wizard_unset_flags(AgsMidiImportWizard *midi_import_wizard,
				   guint flags)
{
  if(!AGS_IS_MIDI_IMPORT_WIZARD(midi_import_wizard)){
    return;
  }

  if((AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER & (flags)) != 0){
    gtk_widget_hide(midi_import_wizard->file_chooser);
  }

  if((AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION & (flags)) != 0){
    gtk_widget_hide(midi_import_wizard->track_collection);
  }

  midi_import_wizard->flags &= (~flags);
}

/**
 * ags_midi_import_wizard_new:
 *
 * Create a new instance of #AgsMidiImportWizard
 *
 * Returns: the new #AgsMidiImportWizard
 *
 * Since: 3.0.0
 */
AgsMidiImportWizard*
ags_midi_import_wizard_new()
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = (AgsMidiImportWizard *) g_object_new(AGS_TYPE_MIDI_IMPORT_WIZARD,
							    NULL);
  
  return(midi_import_wizard);
}
