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

#include <ags/app/import/ags_midi_import_wizard.h>
#include <ags/app/import/ags_midi_import_wizard_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>

#include <ags/app/import/ags_track_collection.h>
#include <ags/app/import/ags_track_mapper.h>

#include <libxml/tree.h>

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

gboolean ags_midi_import_wizard_is_connected(AgsConnectable *connectable);
void ags_midi_import_wizard_connect(AgsConnectable *connectable);
void ags_midi_import_wizard_disconnect(AgsConnectable *connectable);

void ags_midi_import_wizard_set_update(AgsApplicable *applicable, gboolean update);
void ags_midi_import_wizard_apply(AgsApplicable *applicable);
void ags_midi_import_wizard_reset(AgsApplicable *applicable);

void ags_midi_import_wizard_show(GtkWidget *widget);

void ags_midi_import_wizard_close_request_callback(GtkWindow *window,
						   AgsMidiImportWizard *midi_import_wizard);

void ags_midi_import_wizard_previous_button_callback(GtkButton *activate_button,
						     AgsMidiImportWizard *midi_import_wizard);
void ags_midi_import_wizard_next_button_callback(GtkButton *activate_button,
						 AgsMidiImportWizard *midi_import_wizard);
void ags_midi_import_wizard_activate_button_callback(GtkButton *activate_button,
						     AgsMidiImportWizard *midi_import_wizard);

gboolean ags_midi_import_wizard_key_pressed_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsMidiImportWizard *midi_import_wizard);
void ags_midi_import_wizard_key_released_callback(GtkEventControllerKey *event_controller,
						  guint keyval,
						  guint keycode,
						  GdkModifierType state,
						  AgsMidiImportWizard *midi_import_wizard);
gboolean ags_midi_import_wizard_modifiers_callback(GtkEventControllerKey *event_controller,
						   GdkModifierType keyval,
						   AgsMidiImportWizard *midi_import_wizard);

void ags_midi_import_wizard_real_response(AgsMidiImportWizard *midi_import_wizard,
					  gint response_id);

/**
 * SECTION:ags_midi_import_wizard
 * @short_description: Import MIDI using wizard
 * @title: AgsMidiImportWizard
 * @section_id:
 * @include: ags/app/ags_midi_import_wizard.h
 *
 * #AgsMidiImportWizard is a wizard to import midi files and do track mapping.
 */

enum{
  RESPONSE,
  LAST_SIGNAL,
};

static gpointer ags_midi_import_wizard_parent_class = NULL;

static guint midi_import_wizard_signals[LAST_SIGNAL];

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

    ags_type_midi_import_wizard = g_type_register_static(GTK_TYPE_WINDOW,
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

  /* AgsMidiImportWizard */
  midi_import_wizard->response = ags_midi_import_wizard_real_response;
  
  /* signals */
  /**
   * AgsMidiImportWizard::response:
   * @midi_import_wizard: the #AgsMidiImportWizard
   *
   * The ::response signal notifies about window interaction.
   *
   * Since: 6.6.0
   */
  midi_import_wizard_signals[RESPONSE] =
    g_signal_new("response",
		 G_TYPE_FROM_CLASS(midi_import_wizard),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsMidiImportWizardClass, response),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__INT,
		 G_TYPE_NONE, 1,
		 G_TYPE_INT);
}

void
ags_midi_import_wizard_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = NULL;
  connectable->xml_parse = NULL;

  connectable->is_connected = ags_midi_import_wizard_is_connected;  
  connectable->connect = ags_midi_import_wizard_connect;
  connectable->disconnect = ags_midi_import_wizard_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
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
  AgsFileWidget *file_widget;
  GtkBox *vbox;
  GtkBox *action_area;
  GtkButton *button;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *str;

  application_context = ags_application_context_get_instance();
  
  midi_import_wizard->flags = AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER;
  midi_import_wizard->connectable_flags = 0;

  gtk_window_set_hide_on_close((GtkWindow *) midi_import_wizard,
			       TRUE);
  
  gtk_widget_set_size_request(GTK_WIDGET(midi_import_wizard),
			      AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT);

  gtk_window_set_transient_for((GtkWindow *) midi_import_wizard,
			       (GtkWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context)));
  
  g_signal_connect(midi_import_wizard, "close-request",
		   G_CALLBACK(ags_midi_import_wizard_close_request_callback), midi_import_wizard);

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) midi_import_wizard,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_midi_import_wizard_key_pressed_callback), midi_import_wizard);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_midi_import_wizard_key_released_callback), midi_import_wizard);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_midi_import_wizard_modifiers_callback), midi_import_wizard);

  /* vbox */
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_window_set_child((GtkWindow *) midi_import_wizard,
		       (GtkWidget *) vbox);

  /* file chooser */  
  file_widget = ags_file_widget_new();

  midi_import_wizard->file_widget = file_widget;

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/%s",
				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_smf_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_smf_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_smf_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_smf_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_smf_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_smf_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_smf_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_smf_bookmark.xml",
				      home_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

  /* recently-used */
  ags_file_widget_set_recently_used_filename(file_widget,
					     recently_used_filename);
  
  ags_file_widget_read_recently_used(file_widget);

  /* bookmark */
  ags_file_widget_set_bookmark_filename(file_widget,
					bookmark_filename);

  ags_file_widget_read_bookmark(file_widget);

#if defined(AGS_MACOS_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  ags_file_widget_set_current_path(file_widget,
				   sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
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
  
  gtk_box_append(vbox,
		 (GtkWidget *) file_widget);

  /* track collection */
  midi_import_wizard->track_collection = (GtkWidget *) ags_track_collection_new(AGS_TYPE_TRACK_MAPPER,
										0,
										NULL,
										NULL);

  gtk_widget_set_halign(midi_import_wizard->track_collection,
			GTK_ALIGN_FILL);
  gtk_widget_set_valign(midi_import_wizard->track_collection,
			GTK_ALIGN_FILL);
  
  gtk_box_append(vbox,
		 (GtkWidget*) midi_import_wizard->track_collection);
  
  /* buttons */
  action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				       AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_halign(action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) action_area);

  midi_import_wizard->prev_button = (GtkButton *) gtk_button_new_with_label(i18n("previous"));
  gtk_box_append(action_area,
		 (GtkWidget *) midi_import_wizard->prev_button);

  g_signal_connect(midi_import_wizard->prev_button, "clicked",
		   G_CALLBACK(ags_midi_import_wizard_previous_button_callback), midi_import_wizard);

  midi_import_wizard->next_button = (GtkButton *) gtk_button_new_with_label(i18n("next"));
  gtk_box_append(action_area,
		 (GtkWidget *) midi_import_wizard->next_button);
  
  g_signal_connect(midi_import_wizard->next_button, "clicked",
		   G_CALLBACK(ags_midi_import_wizard_next_button_callback), midi_import_wizard);

  midi_import_wizard->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("open"));
  gtk_box_append(action_area,
		 (GtkWidget *) midi_import_wizard->activate_button);

  g_signal_connect(midi_import_wizard->activate_button, "clicked",
		   G_CALLBACK(ags_midi_import_wizard_activate_button_callback), midi_import_wizard);
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

gboolean
ags_midi_import_wizard_is_connected(AgsConnectable *connectable)
{
  AgsMidiImportWizard *midi_import_wizard;
  
  gboolean is_connected;
  
  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (midi_import_wizard->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_midi_import_wizard_connect(AgsConnectable *connectable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  midi_import_wizard->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(midi_import_wizard->track_collection));
}

void
ags_midi_import_wizard_disconnect(AgsConnectable *connectable)
{
  AgsMidiImportWizard *midi_import_wizard;

  midi_import_wizard = AGS_MIDI_IMPORT_WIZARD(connectable);

  if(!ags_connectable_is_connected(connectable)){
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
    gtk_widget_hide(midi_import_wizard->file_widget);
  }

  if((AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION & (midi_import_wizard->flags)) == 0){
    gtk_widget_hide(midi_import_wizard->track_collection);
  }
}

void
ags_midi_import_wizard_close_request_callback(GtkWindow *window,
					      AgsMidiImportWizard *midi_import_wizard)
{
  ags_midi_import_wizard_response(midi_import_wizard,
				  GTK_RESPONSE_CLOSE);
}

void
ags_midi_import_wizard_previous_button_callback(GtkButton *previous_button,
						AgsMidiImportWizard *midi_import_wizard)
{
  ags_midi_import_wizard_response(midi_import_wizard,
				  GTK_RESPONSE_CANCEL);
}

void
ags_midi_import_wizard_next_button_callback(GtkButton *next_button,
					    AgsMidiImportWizard *midi_import_wizard)
{
  ags_midi_import_wizard_response(midi_import_wizard,
				  GTK_RESPONSE_OK);
}

void
ags_midi_import_wizard_activate_button_callback(GtkButton *activate_button,
						AgsMidiImportWizard *midi_import_wizard)
{
  ags_midi_import_wizard_response(midi_import_wizard,
				  GTK_RESPONSE_ACCEPT);
}

gboolean
ags_midi_import_wizard_key_pressed_callback(GtkEventControllerKey *event_controller,
					    guint keyval,
					    guint keycode,
					    GdkModifierType state,
					    AgsMidiImportWizard *midi_import_wizard)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }
  
  return(key_handled);
}

void
ags_midi_import_wizard_key_released_callback(GtkEventControllerKey *event_controller,
					     guint keyval,
					     guint keycode,
					     GdkModifierType state,
					     AgsMidiImportWizard *midi_import_wizard)
{
  gboolean key_handled;

  key_handled = TRUE;

  if(keyval == GDK_KEY_Tab ||
     keyval == GDK_KEY_ISO_Left_Tab ||
     keyval == GDK_KEY_Shift_L ||
     keyval == GDK_KEY_Shift_R ||
     keyval == GDK_KEY_Alt_L ||
     keyval == GDK_KEY_Alt_R ||
     keyval == GDK_KEY_Control_L ||
     keyval == GDK_KEY_Control_R){
    key_handled = FALSE;
  }else{
    switch(keyval){
    case GDK_KEY_Escape:
      {
	ags_midi_import_wizard_response(midi_import_wizard,
					GTK_RESPONSE_CLOSE);	
      }
      break;
    }
  }
}

gboolean
ags_midi_import_wizard_modifiers_callback(GtkEventControllerKey *event_controller,
					  GdkModifierType keyval,
					  AgsMidiImportWizard *midi_import_wizard)
{
  return(FALSE);
}

void
ags_midi_import_wizard_real_response(AgsMidiImportWizard *midi_import_wizard,
				     gint response_id)
{
  AgsApplicationContext *application_context;

  application_context = ags_application_context_get_instance();
  
  switch(response_id){
  case GTK_RESPONSE_CANCEL:
    {
      if(ags_midi_import_wizard_test_flags(midi_import_wizard, AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION)){
	/* show/hide */
	ags_midi_import_wizard_unset_flags(midi_import_wizard,
					   AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION);
	
	ags_midi_import_wizard_set_flags(midi_import_wizard,
					 AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER);
      }
    }
    break;
  case GTK_RESPONSE_OK:
    {      
      if(ags_midi_import_wizard_test_flags(midi_import_wizard, AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER)){
	AgsMidiParser *midi_parser;

	xmlDoc *midi_doc;

	gchar *filename;

	/* show/hide */
	ags_midi_import_wizard_unset_flags(midi_import_wizard,
					   AGS_MIDI_IMPORT_WIZARD_SHOW_FILE_CHOOSER);

	ags_midi_import_wizard_set_flags(midi_import_wizard,
					 AGS_MIDI_IMPORT_WIZARD_SHOW_TRACK_COLLECTION);

	/* parse */
	filename = ags_file_widget_get_filename(midi_import_wizard->file_widget);

	midi_parser = ags_midi_parser_new_from_filename(filename);
	midi_doc = ags_midi_parser_parse_full(midi_parser);
	//	xmlSaveFormatFileEnc("-", midi_doc, "UTF-8", 1);
	
	g_object_set(midi_import_wizard->track_collection,
		     "midi-document", midi_doc,
		      NULL);
	ags_track_collection_parse((AgsTrackCollection *) midi_import_wizard->track_collection);
      }
    }
    break;
  case GTK_RESPONSE_ACCEPT:
    {
      ags_applicable_apply(AGS_APPLICABLE(midi_import_wizard));
    }
  case GTK_RESPONSE_DELETE_EVENT:
  case GTK_RESPONSE_CLOSE:
  case GTK_RESPONSE_REJECT:
    {
      ags_ui_provider_set_midi_import_wizard(AGS_UI_PROVIDER(application_context),
					     NULL);
      
      gtk_window_destroy((GtkWindow *) midi_import_wizard);
    }
    break;
  default:
    g_warning("unknown response");
  }
}

/**
 * ags_midi_import_wizard_response:
 * @midi_import_wizard: the #AgsMidiImportWizard
 * @response: the response
 *
 * Response @midi_import_wizard due to user action.
 * 
 * Since: 6.6.0
 */
void
ags_midi_import_wizard_response(AgsMidiImportWizard *midi_import_wizard,
				gint response)
{
  g_return_if_fail(AGS_IS_MIDI_IMPORT_WIZARD(midi_import_wizard));
  
  g_object_ref((GObject *) midi_import_wizard);
  g_signal_emit(G_OBJECT(midi_import_wizard),
		midi_import_wizard_signals[RESPONSE], 0,
		response);
  g_object_unref((GObject *) midi_import_wizard);
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
    gtk_widget_show(midi_import_wizard->file_widget);
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
    gtk_widget_hide(midi_import_wizard->file_widget);
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
