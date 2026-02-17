/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2025 Joël Krähemann
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

#include <ags/app/editor/ags_import_notation_smf_popover.h>

#include <ags/ags_api_config.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_window.h>
#include <ags/app/ags_composite_editor.h>
#include <ags/app/ags_machine.h>

#include <ags/app/editor/ags_notation_edit.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_import_notation_smf_popover_class_init(AgsImportNotationSMFPopoverClass *import_notation_smf_popover);
void ags_import_notation_smf_popover_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_import_notation_smf_popover_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_import_notation_smf_popover_init(AgsImportNotationSMFPopover *import_notation_smf_popover);
void ags_import_notation_smf_popover_finalize(GObject *gobject);

xmlNode* ags_import_notation_smf_popover_xml_compose(AgsConnectable *connectable);
void ags_import_notation_smf_popover_xml_parse(AgsConnectable *connectable,
					       xmlNode *node);

gboolean ags_import_notation_smf_popover_is_connected(AgsConnectable *connectable);
void ags_import_notation_smf_popover_connect(AgsConnectable *connectable);
void ags_import_notation_smf_popover_disconnect(AgsConnectable *connectable);

void ags_import_notation_smf_popover_set_update(AgsApplicable *applicable, gboolean update);
void ags_import_notation_smf_popover_apply(AgsApplicable *applicable);
void ags_import_notation_smf_popover_reset(AgsApplicable *applicable);

void ags_import_notation_smf_popover_parse(AgsImportNotationSMFPopover *import_notation_smf_popover);

gboolean ags_import_notation_smf_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
							      guint keyval,
							      guint keycode,
							      GdkModifierType state,
							      AgsImportNotationSMFPopover *import_notation_smf_popover);
void ags_import_notation_smf_popover_key_released_callback(GtkEventControllerKey *event_controller,
							   guint keyval,
							   guint keycode,
							   GdkModifierType state,
							   AgsImportNotationSMFPopover *import_notation_smf_popover);
gboolean ags_import_notation_smf_popover_modifiers_callback(GtkEventControllerKey *event_controller,
							    GdkModifierType keyval,
							    AgsImportNotationSMFPopover *import_notation_smf_popover);

void ags_import_notation_smf_popover_activate_button_callback(GtkButton *activate_button,
							      AgsImportNotationSMFPopover *import_notation_smf_popover);

/**
 * SECTION:ags_import_notation_smf_popover
 * @short_description: crop tool
 * @title: AgsImportNotationSMFPopover
 * @section_id:
 * @include: ags/app/editor/ags_import_notation_smf_popover.h
 *
 * The #AgsImportNotationSMFPopover lets you crop notes.
 */

static gpointer ags_import_notation_smf_popover_parent_class = NULL;

GType
ags_import_notation_smf_popover_get_type(void)
{
  static gsize g_define_type_id__static = 0;

  if(g_once_init_enter(&g_define_type_id__static)){
    GType ags_type_import_notation_smf_popover = 0;

    static const GTypeInfo ags_import_notation_smf_popover_info = {
      sizeof (AgsImportNotationSMFPopoverClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_import_notation_smf_popover_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsImportNotationSMFPopover),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_import_notation_smf_popover_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_import_notation_smf_popover_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_import_notation_smf_popover_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_import_notation_smf_popover = g_type_register_static(GTK_TYPE_POPOVER,
								  "AgsImportNotationSMFPopover", &ags_import_notation_smf_popover_info,
								  0);
    
    g_type_add_interface_static(ags_type_import_notation_smf_popover,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_import_notation_smf_popover,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__static, ags_type_import_notation_smf_popover);
  }

  return(g_define_type_id__static);
}

void
ags_import_notation_smf_popover_class_init(AgsImportNotationSMFPopoverClass *import_notation_smf_popover)
{
  GObjectClass *gobject;

  ags_import_notation_smf_popover_parent_class = g_type_class_peek_parent(import_notation_smf_popover);

  /* GObjectClass */
  gobject = (GObjectClass *) import_notation_smf_popover;

  gobject->finalize = ags_import_notation_smf_popover_finalize;
}

void
ags_import_notation_smf_popover_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = NULL;
  connectable->has_resource = NULL;

  connectable->is_ready = NULL;
  connectable->add_to_registry = NULL;
  connectable->remove_from_registry = NULL;

  connectable->list_resource = NULL;
  connectable->xml_compose = ags_import_notation_smf_popover_xml_compose;
  connectable->xml_parse = ags_import_notation_smf_popover_xml_parse;

  connectable->is_connected = ags_import_notation_smf_popover_is_connected;  
  connectable->connect = ags_import_notation_smf_popover_connect;
  connectable->disconnect = ags_import_notation_smf_popover_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_import_notation_smf_popover_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_import_notation_smf_popover_set_update;
  applicable->apply = ags_import_notation_smf_popover_apply;
  applicable->reset = ags_import_notation_smf_popover_reset;
}

void
ags_import_notation_smf_popover_init(AgsImportNotationSMFPopover *import_notation_smf_popover)
{
  GtkBox *vbox;
  GtkBox *hbox;
  GtkLabel *label;
  AgsFileWidget *file_widget;
  
  GtkEventController *event_controller;

  AgsApplicationContext *application_context;

  gchar *recently_used_filename;
  gchar *bookmark_filename;
  gchar *home_path;
  gchar *sandbox_path;
  gchar *current_path;
  gchar *str;

  application_context = ags_application_context_get_instance();

  import_notation_smf_popover->connectable_flags = 0;

  event_controller = gtk_event_controller_key_new();
  gtk_widget_add_controller((GtkWidget *) import_notation_smf_popover,
			    event_controller);

  g_signal_connect(event_controller, "key-pressed",
		   G_CALLBACK(ags_import_notation_smf_popover_key_pressed_callback), import_notation_smf_popover);
  
  g_signal_connect(event_controller, "key-released",
		   G_CALLBACK(ags_import_notation_smf_popover_key_released_callback), import_notation_smf_popover);

  g_signal_connect(event_controller, "modifiers",
		   G_CALLBACK(ags_import_notation_smf_popover_modifiers_callback), import_notation_smf_popover);
  
  vbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) vbox,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) vbox,
			 FALSE);
  gtk_widget_set_hexpand((GtkWidget *) vbox,
			 FALSE);
  
  gtk_popover_set_child((GtkPopover *) import_notation_smf_popover,
			(GtkWidget *) vbox);

  /* set focus */
  import_notation_smf_popover->set_focus = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("set focus"));
  gtk_check_button_set_active(import_notation_smf_popover->set_focus,
			      TRUE);
  gtk_box_append(vbox,
		 (GtkWidget *) import_notation_smf_popover->set_focus);

  /* position x - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* position x - label */
  label = (GtkLabel *) gtk_label_new(i18n("import x"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* position x - spin button */
  import_notation_smf_popover->position_x = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											     AGS_IMPORT_NOTATION_SMF_MAX_BEATS,
											     1.0);
  gtk_spin_button_set_value(import_notation_smf_popover->position_x,
			    0.0);
  gtk_box_append((GtkBox *) hbox,
		 (GtkWidget *) import_notation_smf_popover->position_x);
  
  /* position y - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

#if 0
  /* position y - label */
  label = (GtkLabel *) gtk_label_new(i18n("import y"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* position y - spin button */
  import_notation_smf_popover->position_y = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											     AGS_IMPORT_NOTATION_SMF_MAX_KEYS,
											     1.0);
  gtk_spin_button_set_value(import_notation_smf_popover->position_y,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) import_notation_smf_popover->position_y);
#endif
  
  /* MIDI channel - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);
  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* MIDI channel - label */
  label = (GtkLabel *) gtk_label_new(i18n("MIDI channel"));
  gtk_box_append(hbox,
		 (GtkWidget *) label);

  /* MIDI channel - spin button */
  import_notation_smf_popover->midi_channel = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0,
											       15.0,
											       1.0);
  gtk_spin_button_set_value(import_notation_smf_popover->midi_channel,
			    0.0);
  gtk_box_append(hbox,
		 (GtkWidget *) import_notation_smf_popover->midi_channel);

  /* file widget - hbox */
  hbox = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
				AGS_UI_PROVIDER_DEFAULT_SPACING);

  gtk_widget_set_valign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) hbox,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) hbox,
			 TRUE);
  gtk_widget_set_hexpand((GtkWidget *) hbox,
			 TRUE);

  gtk_box_append(vbox,
		 (GtkWidget *) hbox);

  /* file widget */
  file_widget = 
    import_notation_smf_popover->file_widget = ags_file_widget_new();
  
  gtk_widget_set_valign((GtkWidget *) import_notation_smf_popover->file_widget,
			GTK_ALIGN_FILL);
  gtk_widget_set_halign((GtkWidget *) import_notation_smf_popover->file_widget,
			GTK_ALIGN_FILL);

  gtk_widget_set_vexpand((GtkWidget *) import_notation_smf_popover->file_widget,
			 TRUE);  
  gtk_widget_set_hexpand((GtkWidget *) import_notation_smf_popover->file_widget,
			 TRUE);  

  gtk_widget_set_size_request(GTK_WIDGET(import_notation_smf_popover->file_widget),
  			      AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_WIDTH, AGS_UI_PROVIDER_DEFAULT_OPEN_DIALOG_HEIGHT);


  gtk_box_append(hbox,
		 (GtkWidget *) import_notation_smf_popover->file_widget);

  import_notation_smf_popover->midi_doc = NULL;

  home_path = ags_file_widget_get_home_path(file_widget);

  sandbox_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  sandbox_path = g_strdup_printf("%s/Library/Containers/%s/Data",
  				 home_path,
				 AGS_DEFAULT_BUNDLE_ID);

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  if((str = getenv("HOME")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif

#if defined(AGS_SNAP_SANDBOX)
  if((str = getenv("SNAP_USER_DATA")) != NULL){
    sandbox_path = g_strdup_printf("%s",
				   str);
  }

  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   sandbox_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
				      sandbox_path,
				      AGS_DEFAULT_DIRECTORY);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  recently_used_filename = g_strdup_printf("%s/%s/gsequencer_app_recently_used.xml",
					   home_path,
					   AGS_DEFAULT_DIRECTORY);

  bookmark_filename = g_strdup_printf("%s/%s/gsequencer_app_bookmark.xml",
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

  /* current path */
  current_path = NULL;
  
#if defined(AGS_MACOS_SANDBOX)
  current_path = g_strdup_printf("%s/Music",
				 home_path);
#endif

#if defined(AGS_FLATPAK_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif

#if defined(AGS_SNAP_SANDBOX)
  ags_file_widget_set_flags(file_widget,
			    AGS_FILE_WIDGET_APP_SANDBOX);

  current_path = g_strdup(sandbox_path);
#endif
  
#if !defined(AGS_MACOS_SANDBOX) && !defined(AGS_FLATPAK_SANDBOX) && !defined(AGS_SNAP_SANDBOX)
  current_path = g_strdup(home_path);
#endif

   ags_file_widget_set_current_path(file_widget,
				    current_path);

  g_free(current_path);

  ags_file_widget_refresh(file_widget);
  
#if !defined(AGS_MACOS_SANDBOX)
  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_DESKTOP,
			       NULL);

  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_DOCUMENTS,
			       NULL);  
#endif
  
  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_FOLDER_MUSIC,
			       NULL);

#if !defined(AGS_MACOS_SANDBOX)
  ags_file_widget_add_location(file_widget,
			       AGS_FILE_WIDGET_LOCATION_OPEN_USER_HOME,
			       NULL);
#endif
  
  ags_file_widget_set_file_action(file_widget,
				  AGS_FILE_WIDGET_OPEN);

  ags_file_widget_set_default_bundle(file_widget,
				     AGS_DEFAULT_BUNDLE_ID);

  /* buttons */
  import_notation_smf_popover->action_area = (GtkBox *) gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
								    AGS_UI_PROVIDER_DEFAULT_SPACING);
  
  gtk_widget_set_halign((GtkWidget *) import_notation_smf_popover->action_area,
			GTK_ALIGN_END);

  gtk_box_append(vbox,
		 (GtkWidget *) import_notation_smf_popover->action_area);

  import_notation_smf_popover->activate_button = (GtkButton *) gtk_button_new_with_label(i18n("apply"));

  gtk_box_append(import_notation_smf_popover->action_area,
		 (GtkWidget *) import_notation_smf_popover->activate_button);

  g_signal_connect(import_notation_smf_popover->activate_button, "clicked",
		   G_CALLBACK(ags_import_notation_smf_popover_activate_button_callback), import_notation_smf_popover);

  gtk_popover_set_default_widget((GtkPopover *) import_notation_smf_popover,
				 (GtkWidget *) import_notation_smf_popover->activate_button);
}

xmlNode*
ags_import_notation_smf_popover_xml_compose(AgsConnectable *connectable)
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;
  
  xmlNode *node;

  gchar *str;
  
  import_notation_smf_popover = AGS_IMPORT_NOTATION_SMF_POPOVER(connectable);

  node = xmlNewNode(NULL,
		    BAD_CAST "ags-import-notation-smf-popover");

  /* set focus */
  str = g_strdup_printf("%s",
			((gtk_check_button_get_active(import_notation_smf_popover->set_focus)) ? "true": "false"));
  
  xmlNewProp(node,
	     BAD_CAST "set-focus",
	     BAD_CAST str);
  
  g_free(str);

  /* position x */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(import_notation_smf_popover->position_x));
  
  xmlNewProp(node,
	     BAD_CAST "position-x",
	     BAD_CAST str);
  
  g_free(str);

  /* position y */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(import_notation_smf_popover->position_y));
  
  xmlNewProp(node,
	     BAD_CAST "position-y",
	     BAD_CAST str);
  
  g_free(str);

  /* midi channel */
  str = g_strdup_printf("%f",
			gtk_spin_button_get_value(import_notation_smf_popover->midi_channel));
  
  xmlNewProp(node,
	     BAD_CAST "midi-channel",
	     BAD_CAST str);
  
  g_free(str);
  
  return(node);
}

void
ags_import_notation_smf_popover_xml_parse(AgsConnectable *connectable,
					  xmlNode *node)
{  
  AgsImportNotationSMFPopover *import_notation_smf_popover;

  xmlChar *str;
  
  import_notation_smf_popover = AGS_IMPORT_NOTATION_SMF_POPOVER(connectable);

  /* set-focus */
  str = xmlGetProp(node,
		   "set-focus");

  gtk_check_button_set_active(import_notation_smf_popover->set_focus,
			      ((!g_ascii_strncasecmp(str, "false", 6) == FALSE) ? TRUE: FALSE));

  xmlFree(str);

  /* position x */
  str = xmlGetProp(node,
		   "position-x");

  gtk_spin_button_set_value(import_notation_smf_popover->position_x,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* position y */
  str = xmlGetProp(node,
		   "position-y");

  gtk_spin_button_set_value(import_notation_smf_popover->position_y,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);

  /* midi channel */
  str = xmlGetProp(node,
		   "midi-channel");

  gtk_spin_button_set_value(import_notation_smf_popover->midi_channel,
			    g_ascii_strtod(str,
					   NULL));
  
  xmlFree(str);
}

gboolean
ags_import_notation_smf_popover_is_connected(AgsConnectable *connectable)
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;
  
  gboolean is_connected;
  
  import_notation_smf_popover = AGS_IMPORT_NOTATION_SMF_POPOVER(connectable);

  /* check is connected */
  is_connected = ((AGS_CONNECTABLE_CONNECTED & (import_notation_smf_popover->connectable_flags)) != 0) ? TRUE: FALSE;

  return(is_connected);
}

void
ags_import_notation_smf_popover_connect(AgsConnectable *connectable)
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;

  import_notation_smf_popover = AGS_IMPORT_NOTATION_SMF_POPOVER(connectable);

  if(ags_connectable_is_connected(connectable)){
    return;
  }

  import_notation_smf_popover->connectable_flags |= AGS_CONNECTABLE_CONNECTED;
}

void
ags_import_notation_smf_popover_disconnect(AgsConnectable *connectable)
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;

  import_notation_smf_popover = AGS_IMPORT_NOTATION_SMF_POPOVER(connectable);

  if(!ags_connectable_is_connected(connectable)){
    return;
  }

  import_notation_smf_popover->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);
}

void
ags_import_notation_smf_popover_finalize(GObject *gobject)
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;

  import_notation_smf_popover = (AgsImportNotationSMFPopover *) gobject;

  ags_connectable_disconnect(AGS_CONNECTABLE(import_notation_smf_popover));
  
  G_OBJECT_CLASS(ags_import_notation_smf_popover_parent_class)->finalize(gobject);
}

void
ags_import_notation_smf_popover_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_import_notation_smf_popover_apply(AgsApplicable *applicable)
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;
  AgsWindow *window;
  AgsMachine *machine;
  AgsCompositeEditor *composite_editor;
  AgsCompositeToolbar *composite_toolbar;
  AgsNotationEdit *notation_edit;
  GtkWidget *widget;
  
  GtkAdjustment *hadjustment;
  
  AgsMidiParser *midi_parser;

  AgsApplicationContext *application_context;

  xmlDoc *midi_doc;

  gchar *filename;
  
  gdouble zoom;
  //NOTE:JK: some deco
  //  guint map_height, height;
  guint history;
  guint x; //, y;
  
  import_notation_smf_popover = AGS_IMPORT_NOTATION_SMF_POPOVER(applicable);

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  composite_toolbar = composite_editor->toolbar;
    
  machine = composite_editor->selected_machine;

  history = gtk_combo_box_get_active(GTK_COMBO_BOX(composite_toolbar->zoom));
  
  if(machine == NULL){
    return;
  }
  
  zoom = exp2((double) history - 2.0);
  
  x = gtk_spin_button_get_value_as_int(import_notation_smf_popover->position_x);

  notation_edit = (AgsNotationEdit *) composite_editor->notation_edit->edit;

  if(notation_edit != NULL){
    notation_edit->cursor_position_x = 16 * x;
    notation_edit->cursor_position_y = 0.0;
  }

  hadjustment = gtk_range_get_adjustment(GTK_RANGE(notation_edit->hscrollbar));

  widget = (GtkWidget *) notation_edit->drawing_area;
    
  /* make visible */  
  if(hadjustment != NULL){
    gtk_adjustment_set_value(hadjustment,
			     ((x * 16 * 64 / zoom) * (gtk_adjustment_get_upper(hadjustment) / (AGS_NOTATION_DEFAULT_LENGTH / zoom))));
  }

  filename = ags_file_widget_get_filename(import_notation_smf_popover->file_widget);

  midi_parser = ags_midi_parser_new_from_filename(filename);

  midi_doc = ags_midi_parser_parse_full(midi_parser);
  
  import_notation_smf_popover->midi_doc = midi_doc;

  ags_import_notation_smf_popover_parse(import_notation_smf_popover);
  
  if(gtk_check_button_get_active(import_notation_smf_popover->set_focus)){
    gtk_widget_grab_focus(widget);
  }

  gtk_widget_queue_draw(widget);
}

void
ags_import_notation_smf_popover_reset(AgsApplicable *applicable)
{
  //TODO:JK: implement me
}

void
ags_import_notation_smf_popover_parse(AgsImportNotationSMFPopover *import_notation_smf_popover)
{
  AgsWindow *window;
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;
  
  AgsNotation *current_notation;
  AgsNote *note;

  AgsMidiUtil midi_util;  

  AgsTimestamp *timestamp;

  AgsApplicationContext *application_context;

  GList *start_notation, *notation;
  GList *list;

  xmlDoc *midi_doc;
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode *header_node, *tempo_node;
  xmlNode *time_signature_node;
  xmlNode *midi_track_per_channel;
  xmlNode **node;

  xmlChar *str;

  gchar *segmentation;

  gdouble sec_val;
  guint denominator, numerator;

  gint midi_channel;

  guint audio_channels;
  
  guint first_offset;
  guint first_note_256th_offset;
  gdouble bpm;

  glong tempo;
  glong division;
  gdouble delay_factor;
  
  guint default_length;

  guint x, y, velocity;
  guint x_256th;
  gboolean initial_offset;

  guint i, j;

  if(import_notation_smf_popover->midi_doc == NULL){
    return;
  }

  /* application context */
  application_context = ags_application_context_get_instance();

  window = (AgsWindow *) ags_ui_provider_get_window(AGS_UI_PROVIDER(application_context));
    
  composite_editor = window->composite_editor;

  machine = composite_editor->selected_machine;

  midi_doc = import_notation_smf_popover->midi_doc;
  
  midi_util.major = 1;
  midi_util.minor = 0;  

  audio_channels = ags_audio_get_audio_channels(machine->audio);
  
  /* MIDI header */
  header_node = NULL;
  
  xpath_context = xmlXPathNewContext(import_notation_smf_popover->midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-header",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	header_node = node[i];
      }
    }
  }

  /* tempo node */
  tempo_node = NULL;
  
  xpath_context = xmlXPathNewContext(import_notation_smf_popover->midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-tracks/midi-track/midi-message[@event=\"tempo-number\"]",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	tempo_node = node[i];
      }
    }
  }

  if(header_node == NULL){
    return;
  }

  /* division, tempo and bpm */
  division = 96;

  tempo = 500000;

  bpm = 120.0;
  
  first_offset = 0;
    
  first_note_256th_offset = 0;

  str = xmlGetProp(header_node,
		   BAD_CAST "division");

  division = g_ascii_strtoull(str,
			      NULL,
			      10);
  
  xmlFree(str);
    
  if(tempo_node != NULL){
    str = xmlGetProp(tempo_node,
		     BAD_CAST "tempo");

    tempo = g_ascii_strtoull(str,
			     NULL,
			     10);
    xmlFree(str);
    
    sec_val = ags_midi_parser_ticks_to_sec(NULL,
					   (guint) division,
					   (gint) division,
					   (guint) tempo);
    //  g_message("", sec_val);
    bpm = 60.0 / sec_val;
  }
  //  g_message("bpm %f", bpm);

  delay_factor = AGS_SOUNDCARD_DEFAULT_DELAY_FACTOR;  

  /* time signature and default length */
  time_signature_node = NULL;
  
  denominator = 4;
  numerator = 4;

  xpath_context = xmlXPathNewContext(midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-tracks/midi-track/midi-message[@event=\"time-signature\"]",
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	time_signature_node = node[i];	
      }
    }

    if(time_signature_node != NULL){
      str = xmlGetProp(time_signature_node,
		       BAD_CAST "timesig");

      if(str != NULL){
	sscanf(str, "%d/%d", &numerator, &denominator);
      }
    }
  }

  default_length = 16 / denominator;

  /* segmentation */
  segmentation = ags_config_get_value(ags_config_get_instance(),
				      AGS_CONFIG_GENERIC,
				      "segmentation");

  if(segmentation != NULL){
    guint denominator, numerator;
    
    sscanf(segmentation, "%d/%d",
	   &denominator,
	   &numerator);
    
    delay_factor = 1.0 / (16.0 / (double) denominator);

    g_free(segmentation);
  }

  /*  */
  timestamp = ags_timestamp_new();

  timestamp->flags &= (~AGS_TIMESTAMP_UNIX);
  timestamp->flags |= AGS_TIMESTAMP_OFFSET;

  timestamp->timer.ags_offset.offset = 0;
  
  /* MIDI track */
  xpath_context = xmlXPathNewContext(midi_doc);
  xpath_object = xmlXPathEval((xmlChar *) "//midi-tracks/midi-track",
			      xpath_context);

  initial_offset = TRUE;
  
  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;
    
    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){	
	xmlNode *child;
	xmlXPathContext *xpath_context;
	xmlXPathObject *xpath_object;

	guint midi_channel;

	midi_channel = (guint) gtk_spin_button_get_value(import_notation_smf_popover->midi_channel);
	  
	midi_track_per_channel = xmlNewNode(NULL,
					    "midi-track");

	child = node[i]->children;
	  
	while(child != NULL){
	  if(child->type == XML_ELEMENT_NODE){
	    xmlNode *dup_child;
	      
	    xmlChar *midi_event;
	    xmlChar *key;

	    glong delta_time;

	    if(!g_ascii_strcasecmp(child->name,
				   "midi-message")){
	      midi_event = xmlGetProp(child,
				      AGS_MIDI_EVENT);

	      if(midi_event != NULL){
		if(!g_ascii_strcasecmp(midi_event,
				       "note-on")){
		  /* midi channel */
		  key = xmlGetProp(child,
				   "key");

		  if(g_ascii_strtoull(key, NULL, 10) != midi_channel){
		    xmlFree(key);

		    child = child->next;
	      
		    continue;
		  }

		  xmlFree(key);
		  
		  /* delta time */
		  str = xmlGetProp(child,
				   BAD_CAST "delta-time");

		  delta_time = g_ascii_strtod(str,
					      NULL);

		  xmlFree(str);

		  /* x */
		  x = ags_midi_util_delta_time_to_offset(&midi_util,
							 delay_factor,
							 division,
							 tempo,
							 (glong) bpm,
							 delta_time);
		  
		  x -= first_offset;

		  x_256th = ags_midi_util_delta_time_to_note_256th_offset(&midi_util,
									  delay_factor,
									  division,
									  tempo,
									  (glong) bpm,
									  delta_time);
		  x_256th -= first_note_256th_offset;	  
		  
		  if(initial_offset){
		    initial_offset = FALSE;
		    
		    first_offset = x - (guint) (16.0 * gtk_spin_button_get_value(import_notation_smf_popover->position_x));
    
		    first_note_256th_offset = x_256th - (guint) (256.0 * gtk_spin_button_get_value(import_notation_smf_popover->position_x));

		    x = (guint) (16.0 * gtk_spin_button_get_value(import_notation_smf_popover->position_x));
		    x_256th = (guint) (256.0 * gtk_spin_button_get_value(import_notation_smf_popover->position_x));
		  }
		  
		  /* y */
		  str = xmlGetProp(child,
				   BAD_CAST "note");

		  y = (guint) g_ascii_strtoull(str,
					       NULL,
					       10);
		  xmlFree(str);

		  /* velocity */
		  str = xmlGetProp(child,
				   BAD_CAST "velocity");

		  velocity = (guint) g_ascii_strtoull(str,
						      NULL,
						      10);
		  xmlFree(str);

		  for(j = 0; j < audio_channels; j++){
		    /* new note */
		    note = ags_note_new();
		    note->x[0] = x;
		    note->x[1] = x + default_length;

		    note->x_256th[0] = x_256th;
		    note->x_256th[1] = x_256th + 16 * default_length;

		    note->y = y;

		    /* find notation */
		    start_notation = ags_audio_get_notation(machine->audio);
		    
		    ags_timestamp_set_ags_offset(timestamp,
						 AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));

		    notation = ags_notation_find_near_timestamp(start_notation, j,
								timestamp);
	    
		    if(notation == NULL){
		      current_notation = ags_notation_new(NULL,
							  j);
		      ags_timestamp_set_ags_offset(current_notation->timestamp,
						   AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));
	      
		      start_notation = ags_notation_add(start_notation,
							current_notation);
		    }else{
		      current_notation = notation->data;
		    }

		    /* add note */
		    ags_notation_add_note(current_notation,
					  note,
					  FALSE);

		    g_list_free_full(start_notation,
				     (GDestroyNotify) g_object_unref);
		  }
		}else if(!g_ascii_strcasecmp(midi_event,
					     "note-off")){
		  key = xmlGetProp(child,
				   "key");

		  if(g_ascii_strtoull(key, NULL, 10) != midi_channel){
		    xmlFree(key);

		    child = child->next;
	      
		    continue;
		  }

		  xmlFree(key);

		  /* delta-time */
		  str = xmlGetProp(child,
				   BAD_CAST "delta-time");

		  delta_time = g_ascii_strtod(str,
					      NULL);

		  xmlFree(str);

		  /* x */
		  x = ags_midi_util_delta_time_to_offset(&midi_util,
							 delay_factor,
							 division,
							 tempo,
							 (glong) bpm,
							 delta_time);
		  x -= first_offset;

		  x_256th = ags_midi_util_delta_time_to_note_256th_offset(&midi_util,
									  delay_factor,
									  division,
									  tempo,
									  (glong) bpm,
									  delta_time);
		  x_256th -= first_note_256th_offset;

		  /* y */
		  str = xmlGetProp(child,
				   BAD_CAST "note");
		  
		  y = (guint) g_ascii_strtoull(str,
					       NULL,
					       10);
		  xmlFree(str);

		  /* velocity */
		  str = xmlGetProp(child,
				   BAD_CAST "velocity");

		  velocity = (guint) g_ascii_strtoull(str,
						      NULL,
						      10);
		  xmlFree(str);

		  /*  */
		  for(j = 0; j < audio_channels; j++){
		    list = NULL;
	    
		    /* find notation */
		    start_notation = ags_audio_get_notation(machine->audio);

		    ags_timestamp_set_ags_offset(timestamp,
						 AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET));

		    notation = ags_notation_find_near_timestamp(start_notation, j,
								timestamp);

		    if(notation == NULL){
		      /* find prev notation */
		      if(AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET >= 0){
			ags_timestamp_set_ags_offset(timestamp,
						     AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET);

			notation = ags_notation_find_near_timestamp(start_notation, j,
								    timestamp);

			/* find prev */
			if(notation != NULL){
			  list = ags_note_find_prev(AGS_NOTATION(notation->data)->note,
						    x, y);
			}
		      }
		    }else{
		      /* find prev */
		      if(notation != NULL){
			list = ags_note_find_prev(AGS_NOTATION(notation->data)->note,
						  x, y);
		      }

		      if(list == NULL){
			/* find prev notation */
			if(AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET >= 0){
			  ags_timestamp_set_ags_offset(timestamp,
						       AGS_NOTATION_DEFAULT_OFFSET * floor(x / AGS_NOTATION_DEFAULT_OFFSET) - AGS_NOTATION_DEFAULT_OFFSET);

			  notation = ags_notation_find_near_timestamp(start_notation, j,
								      timestamp);

			  /* find prev */
			  if(notation != NULL){
			    list = ags_note_find_prev(AGS_NOTATION(notation->data)->note,
						      x, y);
			  }
			}
		      }
		    }
		    
		    /* set x[1] and x_256th[1] */
		    if(list != NULL){
		      note = list->data;

		      if(note->x[0] == x){
			note->x[1] = x + 1;
		      }else{
			note->x[1] = x;
		      }

		      if(note->x_256th[0] == x_256th){
			note->x_256th[1] = x_256th + 1;
		      }else{
			note->x_256th[1] = x_256th;
		      }
		
		      note->y = y;
		    }	    

		    g_list_free_full(start_notation,
				     (GDestroyNotify) g_object_unref);
		  }
		}

		xmlFree(midi_event);
	      }
	    }
	  }

	  child = child->next;
	}
      }
    }
  }
}

gboolean
ags_import_notation_smf_popover_key_pressed_callback(GtkEventControllerKey *event_controller,
						     guint keyval,
						     guint keycode,
						     GdkModifierType state,
						     AgsImportNotationSMFPopover *import_notation_smf_popover)
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
ags_import_notation_smf_popover_key_released_callback(GtkEventControllerKey *event_controller,
						      guint keyval,
						      guint keycode,
						      GdkModifierType state,
						      AgsImportNotationSMFPopover *import_notation_smf_popover)
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
	gtk_popover_popdown((GtkPopover *) import_notation_smf_popover);	
      }
      break;
    }
  }
}

gboolean
ags_import_notation_smf_popover_modifiers_callback(GtkEventControllerKey *event_controller,
						   GdkModifierType keyval,
						   AgsImportNotationSMFPopover *import_notation_smf_popover)
{
  return(FALSE);
}

void
ags_import_notation_smf_popover_activate_button_callback(GtkButton *activate_button,
							 AgsImportNotationSMFPopover *import_notation_smf_popover)
{
  ags_applicable_apply(AGS_APPLICABLE(import_notation_smf_popover));
}

/**
 * ags_import_notation_smf_popover_new:
 *
 * Create a new #AgsImportNotationSMFPopover.
 *
 * Returns: a new #AgsImportNotationSMFPopover
 *
 * Since: 8.2.0
 */
AgsImportNotationSMFPopover*
ags_import_notation_smf_popover_new()
{
  AgsImportNotationSMFPopover *import_notation_smf_popover;

  import_notation_smf_popover = (AgsImportNotationSMFPopover *) g_object_new(AGS_TYPE_IMPORT_NOTATION_SMF_POPOVER,
									     NULL);

  return(import_notation_smf_popover);
}
