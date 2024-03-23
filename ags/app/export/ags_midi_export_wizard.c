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

#include <ags/app/export/ags_midi_export_wizard.h>
#include <ags/app/export/ags_midi_export_wizard_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/export/ags_machine_collection.h>
#include <ags/app/export/ags_machine_mapper.h>

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

/**
 * SECTION:ags_midi_export_wizard
 * @short_description: Export to MIDI using wizard
 * @title: AgsMidiExportWizard
 * @section_id:
 * @include: ags/app/ags_midi_export_wizard.h
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

    ags_type_midi_export_wizard = g_type_register_static(GTK_TYPE_WINDOW,
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

  ags_midi_export_wizard_parent_class = g_type_class_peek_parent(midi_export_wizard);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_export_wizard;

  /* GtkWidgetClass */
  widget = (GtkWidgetClass *) midi_export_wizard;

  gobject->set_property = ags_midi_export_wizard_set_property;
  gobject->get_property = ags_midi_export_wizard_get_property;

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
  AgsFileWidget *file_widget;
  GtkBox *vbox;
  GtkBox *action_area;
  GtkButton *button;
  
  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *home_path;
  gchar *sandbox_path;

  application_context = ags_application_context_get_instance();
  
  midi_export_wizard->flags = AGS_MIDI_EXPORT_WIZARD_SHOW_MACHINE_COLLECTION;

  gtk_window_set_hide_on_close((GtkWindow *) midi_export_wizard,
			       TRUE);

  gtk_widget_set_size_request(GTK_WIDGET(midi_export_wizard),
			      AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT);

  gtk_window_set_transient_for((GtkWindow *) midi_export_wizard,
			       (GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));

  /* vbox */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				6);
  gtk_window_set_child((GtkWindow *) midi_export_wizard,
		       (GtkWidget *) vbox);

  /* midi builder */
  midi_export_wizard->midi_builder = ags_midi_builder_new();
  midi_export_wizard->pulse_unit = AGS_MIDI_EXPORT_WIZARD_DEFAULT_PULSE_UNIT;
  
  /* machine collection */
  midi_export_wizard->machine_collection = (GtkWidget *) ags_machine_collection_new(AGS_TYPE_MACHINE_MAPPER,
										    0,
										    NULL,
										    NULL);

  gtk_widget_set_halign(midi_export_wizard->machine_collection,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(midi_export_wizard->machine_collection,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand(midi_export_wizard->machine_collection,
			 TRUE);
  gtk_widget_set_vexpand(midi_export_wizard->machine_collection,
			TRUE);

  gtk_box_append((GtkBox *) gtk_dialog_get_content_area((GtkDialog *) midi_export_wizard),
		 (GtkWidget*) midi_export_wizard->machine_collection);
  
  /* file chooser */  
  file_widget = ags_file_widget_new();

  midi_export_wizard->file_widget = file_widget;

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/%s",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);
#else
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);
#endif

  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

#if defined(AGS_MACOS_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#else
  ags_file_widget_set_current_path(file_widget,
				   home_path);
#endif

  ags_file_widget_refresh(file_widget);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);

  ags_file_widget_set_file_action(file_widget,
				  AGS_FILE_WIDGET_SAVE_AS);

  ags_file_widget_set_default_bundle(file_widget,
				     AGS_DEFAULT_BUNDLE_ID);

  gtk_widget_set_halign(file_widget,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(file_widget,
			GTK_ALIGN_FILL);

  gtk_widget_set_hexpand(file_widget,
			 TRUE);
  gtk_widget_set_vexpand(file_widget,
			 TRUE);

  gtk_widget_set_visible(file_widget,
			 FALSE);
  
  gtk_box_append(vbox,
		 (GtkWidget*) file_widget);

  /*  */
  action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       6);

  gtk_widget_set_halign(action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) action_area);

  midi_export_wizard->prev_button = (GtkButton *) gtk_button_new_with_label(i18n("previous"));
  gtk_box_append(action_area,
		 (GtkWidget *) midi_export_wizard->prev_button);

  midi_export_wizard->next_button = (GtkButton *) gtk_button_new_with_label(i18n("next"));
  gtk_box_append(action_area,
		 (GtkWidget *) midi_export_wizard->next_button);
  
  midi_export_wizard->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_box_append(action_area,
		 (GtkWidget *) midi_export_wizard->activate_button);
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

  if((AGS_CONNECTABLE_CONNECTED & (midi_export_wizard->connectable_flags)) != 0){
    return;
  }

  midi_export_wizard->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(midi_export_wizard->machine_collection));
}

void
ags_midi_export_wizard_disconnect(AgsConnectable *connectable)
{
  AgsMidiExportWizard *midi_export_wizard;

  midi_export_wizard = AGS_MIDI_EXPORT_WIZARD(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (midi_export_wizard->connectable_flags)) == 0){
    return;
  }

  midi_export_wizard->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

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

  GList *list, *start_list;
  
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
    start_list = ags_machine_collection_get_machine_mapper(AGS_MACHINE_COLLECTION(midi_export_wizard->machine_collection));

  track_count = 0;
  
  while(list != NULL){
    if(gtk_check_button_get_active(AGS_MACHINE_MAPPER(list->data)->enabled)){
      track_count++;
    }
    
    list = list->next;
  }

  g_list_free(start_list);

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
  filename = ags_file_widget_get_filename(midi_export_wizard->file_widget);

  ags_midi_builder_open_filename(midi_export_wizard->midi_builder,
				 filename);

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

  ags_midi_builder_write(midi_export_wizard->midi_builder);

  g_free(filename);
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
    gtk_widget_show(midi_export_wizard->machine_collection);
  }

  if((AGS_MIDI_EXPORT_WIZARD_SHOW_FILE_CHOOSER & (flags)) != 0){
    gtk_widget_show(midi_export_wizard->file_widget);
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
    gtk_widget_hide(midi_export_wizard->file_widget);
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
