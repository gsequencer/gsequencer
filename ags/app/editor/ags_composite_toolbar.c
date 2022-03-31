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

#include <ags/app/editor/ags_composite_toolbar.h>
#include <ags/app/editor/ags_composite_toolbar_callbacks.h>

#include <ags/app/ags_ui_provider.h>
#include <ags/app/ags_machine.h>
#include <ags/app/ags_composite_editor.h>

#include <ags/app/editor/ags_move_note_dialog.h>
#include <ags/app/editor/ags_crop_note_dialog.h>
#include <ags/app/editor/ags_select_note_dialog.h>
#include <ags/app/editor/ags_position_notation_cursor_dialog.h>

#include <ags/app/editor/ags_position_sheet_cursor_dialog.h>
#include <ags/app/editor/ags_add_sheet_page_dialog.h>
#include <ags/app/editor/ags_remove_sheet_page_dialog.h>

#include <ags/app/editor/ags_select_acceleration_dialog.h>
#include <ags/app/editor/ags_ramp_acceleration_dialog.h>
#include <ags/app/editor/ags_position_automation_cursor_dialog.h>

#include <ags/app/editor/ags_select_buffer_dialog.h>
#include <ags/app/editor/ags_position_wave_cursor_dialog.h>
#include <ags/app/editor/ags_time_stretch_buffer_dialog.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_composite_toolbar_class_init(AgsCompositeToolbarClass *composite_toolbar);
void ags_composite_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_composite_toolbar_init(AgsCompositeToolbar *composite_toolbar);

AgsUUID* ags_composite_toolbar_get_uuid(AgsConnectable *connectable);
gboolean ags_composite_toolbar_has_resource(AgsConnectable *connectable);
gboolean ags_composite_toolbar_is_ready(AgsConnectable *connectable);
void ags_composite_toolbar_add_to_registry(AgsConnectable *connectable);
void ags_composite_toolbar_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_composite_toolbar_list_resource(AgsConnectable *connectable);
xmlNode* ags_composite_toolbar_xml_compose(AgsConnectable *connectable);
void ags_composite_toolbar_xml_parse(AgsConnectable *connectable,
				     xmlNode *node);
gboolean ags_composite_toolbar_is_connected(AgsConnectable *connectable);
void ags_composite_toolbar_connect(AgsConnectable *connectable);
void ags_composite_toolbar_disconnect(AgsConnectable *connectable);
void ags_composite_toolbar_connect_connection(AgsConnectable *connectable,
					      GObject *connection);
void ags_composite_toolbar_disconnect_connection(AgsConnectable *connectable,
						 GObject *connection);

static gpointer ags_composite_toolbar_parent_class = NULL;

/**
 * SECTION:ags_composite_toolbar
 * @short_description: composite_toolbar
 * @title: AgsCompositeToolbar
 * @section_id:
 * @include: ags/app/editor/ags_composite_toolbar.h
 *
 * The #AgsCompositeToolbar lets you choose edit tool.
 */

GType
ags_composite_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_composite_toolbar = 0;

    static const GTypeInfo ags_composite_toolbar_info = {
      sizeof (AgsCompositeToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_composite_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCompositeToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_composite_toolbar_init,
    };
    
    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_composite_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_composite_toolbar = g_type_register_static(GTK_TYPE_BOX,
							"AgsCompositeToolbar", &ags_composite_toolbar_info,
							0);
    
    g_type_add_interface_static(ags_type_composite_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_composite_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_composite_toolbar_class_init(AgsCompositeToolbarClass *composite_toolbar)
{
  ags_composite_toolbar_parent_class = g_type_class_peek_parent(composite_toolbar);
}

void
ags_composite_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_composite_toolbar_get_uuid;
  connectable->has_resource = ags_composite_toolbar_has_resource;

  connectable->is_ready = ags_composite_toolbar_is_ready;
  connectable->add_to_registry = ags_composite_toolbar_add_to_registry;
  connectable->remove_from_registry = ags_composite_toolbar_remove_from_registry;

  connectable->list_resource = ags_composite_toolbar_list_resource;
  connectable->xml_compose = ags_composite_toolbar_xml_compose;
  connectable->xml_parse = ags_composite_toolbar_xml_parse;

  connectable->is_connected = ags_composite_toolbar_is_connected;
  connectable->connect = ags_composite_toolbar_connect;
  connectable->disconnect = ags_composite_toolbar_disconnect;

  connectable->connect_connection = ags_composite_toolbar_connect_connection;
  connectable->disconnect_connection = ags_composite_toolbar_disconnect_connection;
}

void
ags_composite_toolbar_init(AgsCompositeToolbar *composite_toolbar)
{
  GSimpleActionGroup *action_group;
  GSimpleAction *action;

  action_group = g_simple_action_group_new();
  gtk_widget_insert_action_group((GtkWidget *) composite_toolbar,
				 "composite_toolbar",
				 action_group);
  
  /* match audio channel */
  action = g_simple_action_new_stateful("paste_match_audio_channel",
					g_variant_type_new("b"),
					g_variant_new_boolean(TRUE));
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_composite_toolbar_paste_match_audio_channel_callback), composite_toolbar);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* match line */
  action = g_simple_action_new_stateful("paste_match_line",
					g_variant_type_new("b"),
					g_variant_new_boolean(TRUE));
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_composite_toolbar_paste_match_line_callback), composite_toolbar);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));
  
  /* no duplicates */
  action = g_simple_action_new_stateful("paste_no_duplicates",
					g_variant_type_new("b"),
					g_variant_new_boolean(TRUE));
  g_signal_connect(action, "activate",
		   G_CALLBACK(ags_composite_toolbar_paste_no_duplicates_callback), composite_toolbar);
  g_action_map_add_action(G_ACTION_MAP(action_group),
			  G_ACTION(action));

  
  composite_toolbar->flags = 0;
  composite_toolbar->connectable_flags = 0;
  
  composite_toolbar->tool = 0;
  composite_toolbar->action = 0;
  composite_toolbar->option = 0;

  composite_toolbar->version = g_strdup(AGS_COMPOSITE_TOOLBAR_DEFAULT_VERSION);
  composite_toolbar->build_id = g_strdup(AGS_COMPOSITE_TOOLBAR_DEFAULT_BUILD_ID);
  
  /* uuid */
  composite_toolbar->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_toolbar->uuid);

  composite_toolbar->block_selected_tool = FALSE;
  composite_toolbar->selected_tool = NULL;

  composite_toolbar->position = NULL;
  composite_toolbar->edit = NULL;
  composite_toolbar->clear = NULL;
  composite_toolbar->select = NULL;

  composite_toolbar->invert = NULL;
  composite_toolbar->copy = NULL;
  composite_toolbar->cut = NULL;
  
  composite_toolbar->paste_mode = 0;
  composite_toolbar->paste = NULL;
  composite_toolbar->paste_popup = NULL;

  composite_toolbar->menu_tool_dialog = NULL;
  composite_toolbar->menu_tool_value = NULL;
  composite_toolbar->menu_tool = NULL;
  composite_toolbar->menu_tool_popup = NULL;

  composite_toolbar->selected_zoom = 2;
  
  composite_toolbar->beats = NULL;
  composite_toolbar->beats_type = NULL;
  composite_toolbar->note = NULL;
  
  composite_toolbar->port = NULL;

  composite_toolbar->zoom = NULL;

  composite_toolbar->opacity = NULL;

  composite_toolbar->notation_move_note = (GtkDialog *) ags_move_note_dialog_new();
  composite_toolbar->notation_crop_note = (GtkDialog *) ags_crop_note_dialog_new();
  composite_toolbar->notation_select_note = (GtkDialog *) ags_select_note_dialog_new();
  composite_toolbar->notation_position_cursor = (GtkDialog *) ags_position_notation_cursor_dialog_new();

  composite_toolbar->sheet_position_cursor = (GtkDialog *) ags_position_sheet_cursor_dialog_new();
  composite_toolbar->sheet_add_page = (GtkDialog *) ags_add_sheet_page_dialog_new();
  composite_toolbar->sheet_remove_page = (GtkDialog *) ags_remove_sheet_page_dialog_new();

  composite_toolbar->automation_select_acceleration = (GtkDialog *) ags_select_acceleration_dialog_new();
  composite_toolbar->automation_ramp_acceleration = (GtkDialog *) ags_ramp_acceleration_dialog_new();
  composite_toolbar->automation_position_cursor = (GtkDialog *) ags_position_automation_cursor_dialog_new();

  composite_toolbar->wave_select_buffer = (GtkDialog *) ags_select_buffer_dialog_new();
  composite_toolbar->wave_position_cursor = (GtkDialog *) ags_position_wave_cursor_dialog_new();
  composite_toolbar->wave_time_stretch_buffer = (GtkDialog *) ags_time_stretch_buffer_dialog_new();
}

AgsUUID*
ags_composite_toolbar_get_uuid(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  AgsUUID *ptr;

  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  ptr = composite_toolbar->uuid;

  return(ptr);
}

gboolean
ags_composite_toolbar_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_composite_toolbar_is_ready(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  gboolean is_ready;

  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  /* check is added */
  is_ready = (AGS_CONNECTABLE_ADDED_TO_REGISTRY & (composite_toolbar->connectable_flags));
  
  return(is_ready);
}

void
ags_composite_toolbar_add_to_registry(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  composite_toolbar->connectable_flags |= AGS_CONNECTABLE_ADDED_TO_REGISTRY;

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = composite_toolbar->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) composite_toolbar);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_composite_toolbar_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_composite_toolbar_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_composite_toolbar_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_composite_toolbar_xml_parse(AgsConnectable *connectable,
				xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_composite_toolbar_is_connected(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  gboolean is_connected;

  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  /* check is connected */
  is_connected = (AGS_CONNECTABLE_CONNECTED & (composite_toolbar->connectable_flags));
  
  return(is_connected);
}

void
ags_composite_toolbar_connect(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (composite_toolbar->connectable_flags)) != 0){
    return;
  }

  composite_toolbar->connectable_flags |= AGS_CONNECTABLE_CONNECTED;

  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->notation_move_note));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->notation_crop_note));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->notation_select_note));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->notation_position_cursor));

  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->sheet_position_cursor));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->sheet_add_page));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->sheet_remove_page));
  
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->automation_select_acceleration));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->automation_ramp_acceleration));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->automation_position_cursor));

  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->wave_select_buffer));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->wave_position_cursor));
  ags_connectable_connect(AGS_CONNECTABLE(composite_toolbar->wave_time_stretch_buffer));
}

void
ags_composite_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsCompositeToolbar *composite_toolbar;
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if((AGS_CONNECTABLE_CONNECTED & (composite_toolbar->connectable_flags)) == 0){
    return;
  }

  composite_toolbar->connectable_flags &= (~AGS_CONNECTABLE_CONNECTED);

  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->notation_move_note));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->notation_crop_note));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->notation_select_note));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->notation_position_cursor));

  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->sheet_position_cursor));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->sheet_add_page));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->sheet_remove_page));
  
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->automation_select_acceleration));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->automation_ramp_acceleration));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->automation_position_cursor));

  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->wave_select_buffer));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->wave_position_cursor));
  ags_connectable_disconnect(AGS_CONNECTABLE(composite_toolbar->wave_time_stretch_buffer));
}

void
ags_composite_toolbar_connect_connection(AgsConnectable *connectable,
					 GObject *connection)
{
  AgsCompositeToolbar *composite_toolbar;

  if(connection == NULL){
    return;
  }
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if(composite_toolbar->position == connection){
    g_signal_connect_after(connection, "toggled",
			   G_CALLBACK(ags_composite_toolbar_position_callback), composite_toolbar);
  }

  if(composite_toolbar->edit == connection){
    g_signal_connect_after(connection, "toggled",
			   G_CALLBACK(ags_composite_toolbar_edit_callback), composite_toolbar);
  }

  if(composite_toolbar->clear == connection){
    g_signal_connect_after(connection, "toggled",
			   G_CALLBACK(ags_composite_toolbar_position_callback), composite_toolbar);
  }

  if(composite_toolbar->select == connection){
    g_signal_connect_after(connection, "toggled",
			   G_CALLBACK(ags_composite_toolbar_select_callback), composite_toolbar);
  }

  if(composite_toolbar->invert == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_invert_callback), composite_toolbar);
  }

  if(composite_toolbar->copy == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_copy_callback), composite_toolbar);
  }

  if(composite_toolbar->cut == connection){
    g_signal_connect_after(connection, "clicked",
			   G_CALLBACK(ags_composite_toolbar_cut_callback), composite_toolbar);
  }

  if(composite_toolbar->port == connection){
    g_signal_connect_after(connection, "changed",
			   G_CALLBACK(ags_composite_toolbar_port_callback), composite_toolbar);
  }

  if(composite_toolbar->zoom == connection){
    g_signal_connect_after(connection, "changed",
			   G_CALLBACK(ags_composite_toolbar_zoom_callback), composite_toolbar);
  }

  if(composite_toolbar->opacity == connection){
    g_signal_connect_after(connection, "value-changed",
			   G_CALLBACK(ags_composite_toolbar_opacity_callback), composite_toolbar);
  }
}

void
ags_composite_toolbar_disconnect_connection(AgsConnectable *connectable,
					    GObject *connection)
{
  AgsCompositeToolbar *composite_toolbar;

  if(connection == NULL){
    return;
  }
  
  composite_toolbar = AGS_COMPOSITE_TOOLBAR(connectable);

  if(composite_toolbar->position == connection){
    g_object_disconnect(connection,
			"any_signal::toggled",
			G_CALLBACK(ags_composite_toolbar_position_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->edit == connection){
    g_object_disconnect(connection, "any_signal::toggled",
			G_CALLBACK(ags_composite_toolbar_edit_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->clear == connection){
    g_object_disconnect(connection, "any_signal::toggled",
			G_CALLBACK(ags_composite_toolbar_position_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->select == connection){
    g_object_disconnect(connection, "any_signal::toggled",
			G_CALLBACK(ags_composite_toolbar_select_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->invert == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_invert_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->copy == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_copy_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->cut == connection){
    g_object_disconnect(connection, "any_signal::clicked",
			G_CALLBACK(ags_composite_toolbar_cut_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->port == connection){
    g_object_disconnect(connection, "any_signal::changed",
			G_CALLBACK(ags_composite_toolbar_port_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->zoom == connection){
    g_object_disconnect(connection, "any_signal::changed",
			G_CALLBACK(ags_composite_toolbar_zoom_callback),
			composite_toolbar,
			NULL);
  }

  if(composite_toolbar->opacity == connection){
    g_object_disconnect(connection, "any_signal::value-changed",
			G_CALLBACK(ags_composite_toolbar_opacity_callback),
			composite_toolbar,
			NULL);
  }
}

/**
 * ags_composite_toolbar_test_flags:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @flags: the flags
 *
 * Test @flags to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_flags(AgsCompositeToolbar *composite_toolbar, guint flags)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (flags & (composite_toolbar->flags)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_flags:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @flags: see enum AgsCompositeToolbarFlags
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_flags(AgsCompositeToolbar *composite_toolbar, guint flags)
{
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  /* set flags */
  composite_toolbar->flags |= flags;
}
    
/**
 * ags_composite_toolbar_unset_flags:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @flags: see enum AgsCompositeToolbarFlags
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_flags(AgsCompositeToolbar *composite_toolbar, guint flags)
{  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  /* unset flags */
  composite_toolbar->flags &= (~flags);
}

/**
 * ags_composite_toolbar_test_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @tool: the tool
 *
 * Test @tool to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if tool are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_tool(AgsCompositeToolbar *composite_toolbar, guint tool)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (tool & (composite_toolbar->tool)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @tool: see enum AgsCompositeToolbarTool
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_tool(AgsCompositeToolbar *composite_toolbar, guint tool)
{
  GtkWidget *sibling;
    
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  sibling = NULL;

  /* position tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_POSITION & tool) != 0 &&
     composite_toolbar->position == NULL){
    composite_toolbar->position = (GtkToggleButton *) gtk_toggle_button_new();
    g_object_set(composite_toolbar->position,
		 "label", i18n("Position"),
		 "icon-name", "go-jump",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->position,
			       sibling);

    sibling = composite_toolbar->position;
  }else if(composite_toolbar->position != NULL){
    sibling = composite_toolbar->position;
  }

  /* edit tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_EDIT & tool) != 0 &&
     composite_toolbar->edit == NULL){
    composite_toolbar->edit = (GtkToggleButton *) gtk_toggle_button_new();
    g_object_set(composite_toolbar->edit,
		 "label", i18n("Edit"),
		 "icon-name", "document-edit",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->edit,
			       sibling);
    
    sibling = composite_toolbar->edit;
  }else if(composite_toolbar->edit != NULL){
    sibling = composite_toolbar->edit;
  }

  /* clear tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR & tool) != 0 &&
     composite_toolbar->clear == NULL){
    composite_toolbar->clear = (GtkToggleButton *) gtk_toggle_button_new();
    g_object_set(composite_toolbar->clear,
		 "label", i18n("Clear"),
		 "icon-name", "edit-clear",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->clear,
			       sibling);
    
    sibling = composite_toolbar->clear;
  }else if(composite_toolbar->clear != NULL){
    sibling = composite_toolbar->clear;
  }

  /* select tool */
  if((AGS_COMPOSITE_TOOLBAR_TOOL_SELECT & tool) != 0 &&
     composite_toolbar->select == NULL){
    composite_toolbar->select = (GtkToggleButton *) gtk_toggle_button_new();
    g_object_set(composite_toolbar->select,
		 "label", i18n("Select"),
		 "icon-name", "edit-select",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->select,
			       sibling);
    
    sibling = composite_toolbar->select;
  }else if(composite_toolbar->select != NULL){
    sibling = composite_toolbar->select;
  }
  
  /* set tool */
  composite_toolbar->tool |= tool;
}
    
/**
 * ags_composite_toolbar_unset_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @tool: see enum AgsCompositeToolbarTool
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_tool(AgsCompositeToolbar *composite_toolbar, guint tool)
{  
  gint position;

  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_POSITION & tool) != 0 &&
     composite_toolbar->position != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->position);

    g_object_run_dispose(composite_toolbar->position);
    g_object_unref(composite_toolbar->position);
    
    composite_toolbar->position = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_EDIT & tool) != 0 &&
     composite_toolbar->edit != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->edit);

    g_object_run_dispose(composite_toolbar->edit);
    g_object_unref(composite_toolbar->edit);

    composite_toolbar->edit = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR & tool) != 0 &&
     composite_toolbar->clear != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->clear);

    g_object_run_dispose(composite_toolbar->clear);
    g_object_unref(composite_toolbar->clear);

    composite_toolbar->clear = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_TOOL_SELECT & tool) != 0 &&
     composite_toolbar->select != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->select);

    g_object_run_dispose(composite_toolbar->select);
    g_object_unref(composite_toolbar->select);

    composite_toolbar->select = NULL;
  }
  
  /* unset tool */
  composite_toolbar->tool &= (~tool);
}

/**
 * ags_composite_toolbar_test_action:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @action: the action
 *
 * Test @action to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if action are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_action(AgsCompositeToolbar *composite_toolbar, guint action)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (action & (composite_toolbar->action)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_action:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @action: see enum AgsCompositeToolbarAction
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_action(AgsCompositeToolbar *composite_toolbar, guint action)
{
  GtkWidget *sibling;
    
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  sibling = NULL;
  
  if(composite_toolbar->position != NULL){
    sibling = composite_toolbar->position;
  }

  if(composite_toolbar->edit != NULL){
    sibling = composite_toolbar->edit;
  }

  if(composite_toolbar->clear != NULL){
    sibling = composite_toolbar->clear;
  }

  if(composite_toolbar->select != NULL){
    sibling = composite_toolbar->select;
  }

  /* invert action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_INVERT & action) != 0 &&
     composite_toolbar->invert == NULL){
    composite_toolbar->invert = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							   NULL);
    g_object_set(composite_toolbar->invert,
		 "label", i18n("Invert"),
		 "icon-name", "object-flip-vertical",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->invert,
			       sibling);

    sibling = composite_toolbar->invert;
  }else if(composite_toolbar->invert != NULL){
    sibling = composite_toolbar->invert;
  }

  /* copy action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_COPY & action) != 0 &&
     composite_toolbar->copy == NULL){
    composite_toolbar->copy = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							 NULL);
    g_object_set(composite_toolbar->copy,
		 "label", i18n("Copy"),
		 "icon-name", "edit-copy",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->copy,
			       sibling);
    
    sibling = composite_toolbar->copy;
  }else if(composite_toolbar->copy != NULL){
    sibling = composite_toolbar->copy;
  }

  /* cut action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_CUT & action) != 0 &&
     composite_toolbar->cut == NULL){
    composite_toolbar->cut = (GtkButton *) g_object_new(GTK_TYPE_BUTTON,
							NULL);
    g_object_set(composite_toolbar->cut,
		 "label", i18n("Cut"),
		 "icon-name", "edit-cut",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->cut,
			       sibling);
    
    sibling = composite_toolbar->cut;
  }else if(composite_toolbar->cut != NULL){
    sibling = composite_toolbar->cut;
  }

  /* paste action */
  if((AGS_COMPOSITE_TOOLBAR_ACTION_PASTE & action) != 0 &&
     composite_toolbar->paste == NULL){
    composite_toolbar->paste = (GtkMenuButton *) g_object_new(GTK_TYPE_MENU_BUTTON,
							      NULL);
    g_object_set(composite_toolbar->paste,
		 "label", i18n("Paste"),
		 "icon-name", "edit-paste",
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->paste,
			       sibling);

    composite_toolbar->paste_popup = (GMenuModel *) ags_composite_toolbar_paste_popup_new(composite_toolbar,
											  composite_toolbar->paste_mode);
    gtk_menu_button_set_menu_model(composite_toolbar->paste,
				   composite_toolbar->paste_popup);

    sibling = composite_toolbar->paste;
  }else if(composite_toolbar->paste != NULL){
    sibling = composite_toolbar->paste;
  }
  
  /* set action */
  composite_toolbar->action |= action;
}
    
/**
 * ags_composite_toolbar_unset_action:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @action: see enum AgsCompositeToolbarAction
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_action(AgsCompositeToolbar *composite_toolbar, guint action)
{  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_INVERT & action) != 0 &&
     composite_toolbar->invert != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->invert);

    g_object_run_dispose(composite_toolbar->invert);
    g_object_unref(composite_toolbar->invert);
    
    composite_toolbar->invert = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_COPY & action) != 0 &&
     composite_toolbar->copy != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->copy);

    g_object_run_dispose(composite_toolbar->copy);
    g_object_unref(composite_toolbar->copy);

    composite_toolbar->copy = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_CUT & action) != 0 &&
     composite_toolbar->cut != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->cut);

    g_object_run_dispose(composite_toolbar->cut);
    g_object_unref(composite_toolbar->cut);

    composite_toolbar->cut = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_ACTION_PASTE & action) != 0 &&
     composite_toolbar->paste != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->paste);

    g_object_run_dispose(composite_toolbar->paste);
    g_object_unref(composite_toolbar->paste);

    //TODO:JK: check if needed
    //    composite_toolbar->paste_mode = 0;

    composite_toolbar->paste = NULL;
    composite_toolbar->paste_popup = NULL;
  }
  
  /* unset action */
  composite_toolbar->action &= (~action);
}

/**
 * ags_composite_toolbar_test_option:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @option: the option
 *
 * Test @option to be set on @composite_toolbar.
 * 
 * Returns: %TRUE if option are set, else %FALSE
 *
 * Since: 3.8.0
 */
gboolean
ags_composite_toolbar_test_option(AgsCompositeToolbar *composite_toolbar, guint option)
{
  gboolean retval;  
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (option & (composite_toolbar->option)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_composite_toolbar_set_option:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @option: see enum AgsCompositeToolbarOption
 *
 * Enable a feature of #AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_option(AgsCompositeToolbar *composite_toolbar, guint option)
{
  GtkWidget *sibling;

  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  sibling = NULL;
  
  if(composite_toolbar->position != NULL){
    sibling = composite_toolbar->position;
  }

  if(composite_toolbar->edit != NULL){
    sibling = composite_toolbar->edit;
  }

  if(composite_toolbar->clear != NULL){
    sibling = composite_toolbar->clear;
  }

  if(composite_toolbar->select != NULL){
    sibling = composite_toolbar->select;
  }

  if(composite_toolbar->invert != NULL){
    sibling = composite_toolbar->invert;
  }
  
  if(composite_toolbar->copy != NULL){
    sibling = composite_toolbar->copy;
  }
  
  if(composite_toolbar->cut != NULL){
    sibling = composite_toolbar->cut;
  }

  if(composite_toolbar->paste != NULL){
    sibling = composite_toolbar->paste;
  }

  /* menu tool */
  if((AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL & option) != 0 &&
     composite_toolbar->menu_tool == NULL){
    composite_toolbar->menu_tool = (GtkMenuButton *) g_object_new(GTK_TYPE_MENU_BUTTON,
								  NULL);
    g_object_set(composite_toolbar->menu_tool,
		 "label", i18n("tool"),
		 "margin-end", AGS_UI_PROVIDER_DEFAULT_MARGIN_END,
		 NULL);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) composite_toolbar->menu_tool,
			       sibling);

    composite_toolbar->menu_tool_popup = (GMenuModel *) ags_composite_toolbar_menu_tool_popup_new(composite_toolbar,
												  composite_toolbar->menu_tool_dialog,
												  composite_toolbar->menu_tool_value);
    gtk_menu_button_set_menu_model(composite_toolbar->menu_tool,
				   composite_toolbar->menu_tool_popup);

    sibling = composite_toolbar->menu_tool;
  }else if(composite_toolbar->menu_tool != NULL){
    sibling = composite_toolbar->menu_tool;
  }
  
  /* zoom */
  if((AGS_COMPOSITE_TOOLBAR_HAS_ZOOM & option) != 0 &&
     composite_toolbar->zoom == NULL){
    GtkBox *box;
    GtkLabel *label;
    
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) box,
			       sibling);

    label = (GtkLabel *) gtk_label_new(i18n("zoom"));
    gtk_box_append(box,
		   (GtkWidget *) label);

    composite_toolbar->selected_zoom = 2;
    composite_toolbar->zoom = ags_zoom_combo_box_new();
    gtk_combo_box_set_active(GTK_COMBO_BOX(composite_toolbar->zoom),
			     2);
    gtk_box_append(box,
		   (GtkWidget *) composite_toolbar->zoom);

    sibling = box;
  }else if(composite_toolbar->zoom != NULL){
    sibling = gtk_widget_get_parent(composite_toolbar->zoom);
  }
  
  /* opacity */
  if((AGS_COMPOSITE_TOOLBAR_HAS_OPACITY & option) != 0 &&
     composite_toolbar->opacity == NULL){
    GtkBox *box;
    GtkLabel *label;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) box,
			       sibling);

    label = (GtkLabel *) gtk_label_new(i18n("opacity"));
    gtk_box_append(box,
		   (GtkWidget *) label);

    composite_toolbar->opacity = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.001);
    gtk_spin_button_set_digits(composite_toolbar->opacity, 4);
    gtk_spin_button_set_value(composite_toolbar->opacity, 1.0);
    gtk_box_append(box,
		   (GtkWidget *) composite_toolbar->opacity);

    sibling = box;
  }else if(composite_toolbar->opacity != NULL){
    sibling = gtk_widget_get_parent(composite_toolbar->opacity);
  }
  
  /* port */
  if((AGS_COMPOSITE_TOOLBAR_HAS_PORT & option) != 0 &&
     composite_toolbar->port == NULL){
    GtkBox *box;
    GtkLabel *label;

    GtkCellRenderer *cell_renderer_toggle;
    GtkCellRenderer *scope_cell_renderer_text;
    GtkCellRenderer *port_cell_renderer_text;


    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) box,
			       sibling);
    
    label = (GtkLabel *) gtk_label_new(i18n("Port"));
    gtk_box_append(box,
		   (GtkWidget *) label);
    
    composite_toolbar->port = (GtkComboBox *) gtk_combo_box_new();

    cell_renderer_toggle = gtk_cell_renderer_toggle_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(composite_toolbar->port),
			       cell_renderer_toggle,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(composite_toolbar->port), cell_renderer_toggle,
				   "active", 0,
				   NULL);
    gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(cell_renderer_toggle),
					     TRUE);
  
    scope_cell_renderer_text = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(composite_toolbar->port),
			       scope_cell_renderer_text,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(composite_toolbar->port), scope_cell_renderer_text,
				   "text", 1,
				   NULL);

    port_cell_renderer_text = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(composite_toolbar->port),
			       port_cell_renderer_text,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(composite_toolbar->port), port_cell_renderer_text,
				   "text", 2,
				   NULL);
  
    gtk_box_append(box,
		   (GtkWidget *) composite_toolbar->port);
    
    sibling = box;
  }else if(composite_toolbar->port != NULL){
    sibling = gtk_widget_get_parent(composite_toolbar->port);
  }
  
  /* beats */
  if((AGS_COMPOSITE_TOOLBAR_HAS_BEATS & option) != 0 &&
     composite_toolbar->beats == NULL){
    GtkBox *box;
    GtkLabel *label;

    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) box,
			       sibling);
    
    label = (GtkLabel *) gtk_label_new(i18n("beats"));
    gtk_box_append(box,
		   (GtkWidget *) label);
    
    composite_toolbar->beats = (GtkComboBox *) gtk_combo_box_text_new();

    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "1");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "2");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "3");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "4");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "5");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "6");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "7");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats,
				   "8");
    
    gtk_box_append(box,
		   (GtkWidget *) composite_toolbar->beats);
    
    sibling = box;
  }else if(composite_toolbar->beats != NULL){
    sibling = gtk_widget_get_parent(composite_toolbar->beats);
  }
  
  /* beats type */
  if((AGS_COMPOSITE_TOOLBAR_HAS_BEATS_TYPE & option) != 0 &&
     composite_toolbar->beats_type == NULL){
    GtkBox *box;
    GtkLabel *label;
    
    box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,
		      0);
    gtk_box_insert_child_after(composite_toolbar,
			       (GtkWidget *) box,
			       sibling);
    
    label = (GtkLabel *) gtk_label_new(i18n("beats type"));
    gtk_box_append(box,
		   (GtkWidget *) label);
    
    composite_toolbar->beats_type = (GtkComboBox *) gtk_combo_box_text_new();
    
    gtk_box_append(box,
		   (GtkWidget *) composite_toolbar->beats_type);
    
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats_type,
				   "1");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats_type,
				   "2");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats_type,
				   "4");
    gtk_combo_box_text_append_text((GtkComboBoxText *) composite_toolbar->beats_type,
				   "8");
    
    sibling = box;
  }else if(composite_toolbar->beats_type != NULL){
    sibling = gtk_widget_get_parent(composite_toolbar->beats_type);
  }
  
  /* set option */
  composite_toolbar->option |= option;
}
  
/**
 * ags_composite_toolbar_unset_option:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @option: see enum AgsCompositeToolbarOption
 *
 * Disable a feature of AgsCompositeToolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_unset_option(AgsCompositeToolbar *composite_toolbar, guint option)
{  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL & option) != 0 &&
     composite_toolbar->menu_tool != NULL){
    gtk_box_remove(composite_toolbar,
		   (GtkWidget *) composite_toolbar->menu_tool);

    g_object_run_dispose(composite_toolbar->menu_tool);
    g_object_unref(composite_toolbar->menu_tool);

    composite_toolbar->menu_tool = NULL;
    composite_toolbar->menu_tool_popup = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_OPACITY & option) != 0 &&
     composite_toolbar->opacity != NULL){
    gtk_box_remove(composite_toolbar,
		   gtk_widget_get_parent(composite_toolbar->opacity));

    g_object_run_dispose(composite_toolbar->opacity);
    g_object_unref(composite_toolbar->opacity);
    
    composite_toolbar->opacity = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_PORT & option) != 0 &&
     composite_toolbar->port != NULL){
    gtk_box_remove(composite_toolbar,
		   gtk_widget_get_parent(composite_toolbar->port));

    g_object_run_dispose(composite_toolbar->port);
    g_object_unref(composite_toolbar->port);

    composite_toolbar->port = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_BEATS & option) != 0 &&
     composite_toolbar->beats != NULL){
    gtk_box_remove(composite_toolbar,
		   gtk_widget_get_parent(composite_toolbar->beats));

    g_object_run_dispose(composite_toolbar->beats);
    g_object_unref(composite_toolbar->beats);

    composite_toolbar->beats = NULL;
  }

  if((AGS_COMPOSITE_TOOLBAR_HAS_BEATS_TYPE & option) != 0 &&
     composite_toolbar->beats_type != NULL){
    gtk_box_remove(composite_toolbar,
		   gtk_widget_get_parent(composite_toolbar->beats_type));

    g_object_run_dispose(composite_toolbar->beats_type);
    g_object_unref(composite_toolbar->beats_type);

    composite_toolbar->beats_type = NULL;
  }
  
  /* unset option */
  composite_toolbar->option &= (~option);
}

/**
 * ags_composite_toolbar_paste_popup_new:
 * @paste_mode: the paste mode
 *
 * Create a new #GMenuModel suitable for menu tool button.
 *
 * Returns: a new #GMenuModel
 *
 * Since: 3.8.0
 */
GMenuModel*
ags_composite_toolbar_paste_popup_new(AgsCompositeToolbar *composite_toolbar,
				      guint paste_mode)
{
  GMenu *menu;
  GMenuItem *item;
  
  menu = (GMenu *) g_menu_new();

  if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL & paste_mode) != 0){
    item = g_menu_item_new(i18n("match audio channel"),
			   "composite_toolbar.paste_match_audio_channel");
    g_menu_append_item(menu,
		       item);
  }

  if((AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE & paste_mode) != 0){
    item = g_menu_item_new(i18n("match line"),
			   "composite_toolbar.paste_match_line");
    g_menu_append_item(menu,
		       item);
  }

  if((AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES & paste_mode) != 0){
    item = g_menu_item_new(i18n("no duplicates"),
			   "composite_toolbar.paste_no_duplicates");
    g_menu_append_item(menu,
		       item);
  }
  
  return(menu);
}

/**
 * ags_composite_toolbar_menu_tool_popup_new:
 * @dialog: the tool dialog string vector
 * @value: the #GValue-struct array
 *
 * Create a new #GMenuModel suitable for menu tool button.
 *
 * Returns: a new #GMenuModel
 *
 * Since: 3.8.0
 */
GMenuModel*
ags_composite_toolbar_menu_tool_popup_new(AgsCompositeToolbar *composite_toolbar,
					  gchar **dialog,
					  GValue *value)
{
  GMenu *menu;
  GMenuItem *item;
  
  guint i, j;
  
  menu = (GMenu *) g_menu_new();

  if(dialog != NULL &&
     value != NULL){
    for(i = 0, j = 0; dialog[i] != NULL; i++){
      guint current_value;

      if(dialog[i] != NULL){
	if(!g_strcmp0(dialog[i],
		      AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON)){
	  current_value = g_value_get_uint(value + i);
	
	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	    item = g_menu_item_new(i18n("enable all audio channels"),
				   "composite_toolbar.enable_all_audio_channels");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	  
	  current_value = g_value_get_uint(value + i);

	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS & current_value) != 0){
	    item = g_menu_item_new(i18n("disable all audio channels"),
				   "composite_toolbar.disable_all_audio_channels");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	  
	  current_value = g_value_get_uint(value + i);

	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES & current_value) != 0){
	    item = g_menu_item_new(i18n("enable all lines"),
				   "composite_toolbar.enable_all_lines");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	  
	  current_value = g_value_get_uint(value + i);

	  if((AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES & current_value) != 0){
	    item = g_menu_item_new(i18n("disable all lines"),
				   "composite_toolbar.disable_all_lines");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	}else if(!g_strcmp0(dialog[i],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION)){
	  current_value = g_value_get_uint(value + i);
	    
	  if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE & current_value) != 0){
	    item = g_menu_item_new(i18n("move notes"),
				   "composite_toolbar.move_notes");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }

	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE & current_value) != 0){
	    item = g_menu_item_new(i18n("crop notes"),
				   "composite_toolbar.crop_notes");
	    g_menu_append_item(menu,
			       item);

	    j++;
 	  }
	  
	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE & current_value) != 0){
	    item = g_menu_item_new(i18n("select notes"),
				   "composite_toolbar.select_notes");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }

	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = g_menu_item_new(i18n("position notation cursor"),
				   "composite_toolbar.position_notation_cursor");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	}else if(!g_strcmp0(dialog[i],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET)){
	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = g_menu_item_new(i18n("position sheet cursor"),
				   "composite_toolbar.position_sheet_cursor");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }

	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_ADD_PAGE & current_value) != 0){
	    item = g_menu_item_new(i18n("add page"),
				   "composite_toolbar.add_page");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }

	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_REMOVE_PAGE & current_value) != 0){
	    item = g_menu_item_new(i18n("remove page"),
				   "composite_toolbar.remove_page");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	}else if(!g_strcmp0(dialog[i],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION)){
	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION & current_value) != 0){
	    item = g_menu_item_new(i18n("select acceleration"),
				   "composite_toolbar.select_acceleration");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	  
	  current_value = g_value_get_uint(value + i);

	  if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION & current_value) != 0){
	    item = g_menu_item_new(i18n("ramp acceleration"),
				   "composite_toolbar.ramp_acceleration");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	  
	  current_value = g_value_get_uint(value + i);

	  if((AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = g_menu_item_new(i18n("position automation cursor"),
				   "composite_toolbar.position_automation_cursor");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	}else if(!g_strcmp0(dialog[i],
			    AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE)){
	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER & current_value) != 0){
	    item = g_menu_item_new(i18n("select buffer"),
				   "composite_toolbar.select_buffer");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	  
	  current_value = g_value_get_uint(value + i);

	  if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = g_menu_item_new(i18n("position wave cursor"),
				   "composite_toolbar.position_wave_cursor");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }

	  current_value = g_value_get_uint(value + i);
	  
	  if((AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_TIME_STRETCH_BUFFER & current_value) != 0){
	    item = g_menu_item_new(i18n("time stretch buffer"),
				   "composite_toolbar.time_stretch_buffer");
	    g_menu_append_item(menu,
			       item);

	    j++;
	  }
	}else{
	  g_warning("unknown dialog");
	}
      }
    }
  }
  
  return(menu);
}

/**
 * ags_composite_toolbar_load_port:
 * @composite_toolbar: the #AgsCompositeToolbar
 * 
 * Load ports of @composite_toolbar.
 *
 * Since: 3.12.2
 */
void
ags_composite_toolbar_load_port(AgsCompositeToolbar *composite_toolbar)
{
  AgsCompositeEditor *composite_editor;
  AgsMachine *machine;

  GtkListStore *list_store;
  GtkTreeIter iter;

  AgsChannel *start_channel;
  AgsChannel *channel;

  GList *start_port, *port;

  gchar **collected_specifier;

  guint length;
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }
  
  composite_editor = (AgsCompositeEditor *) gtk_widget_get_ancestor((GtkWidget *) composite_toolbar,
								    AGS_TYPE_COMPOSITE_EDITOR);
  machine = composite_editor->selected_machine;
  
  if(machine == NULL){
    gtk_combo_box_set_model(composite_toolbar->port,
			    NULL);
    
    return;
  }

  collected_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;
  
  list_store = gtk_list_store_new(3,
				  G_TYPE_BOOLEAN,
				  G_TYPE_STRING,
				  G_TYPE_STRING);
  
  gtk_combo_box_set_model(composite_toolbar->port,
			  GTK_TREE_MODEL(list_store));

  /* audio */
  port =
    start_port = ags_audio_collect_all_audio_ports(machine->audio);

  while(port != NULL){
    AgsPluginPort *plugin_port;

    gchar *specifier;

    gboolean is_enabled;
    gboolean contains_control_name;

    specifier = NULL;
    plugin_port = NULL;
    
    g_object_get(port->data,
		 "specifier", &specifier,
		 "plugin-port", &plugin_port,
		 NULL);

    if(specifier == NULL){
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
      
      /* iterate */
      port = port->next;

      continue;
    }
    
#ifdef HAVE_GLIB_2_44
    contains_control_name = g_strv_contains(collected_specifier,
					    specifier);
#else
    contains_control_name = ags_strv_contains(collected_specifier,
					      specifier);
#endif

    if(plugin_port != NULL &&
       !contains_control_name){
      /* create list store entry */
      is_enabled = (ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
										    G_TYPE_NONE,
										    specifier)) ? TRUE: FALSE;

      gtk_list_store_append(list_store, &iter);
      gtk_list_store_set(list_store, &iter,
			 0, is_enabled,
			 1, g_strdup("audio"),
			 2, g_strdup(specifier),
			 -1);      

      /* add to collected specifier */
      collected_specifier = (gchar **) realloc(collected_specifier,
					       (length + 1) * sizeof(gchar *));
      collected_specifier[length - 1] = g_strdup(specifier);
      collected_specifier[length] = NULL;

      length++;
    }

    g_free(specifier);
    
    if(plugin_port != NULL){
      g_object_unref(plugin_port);
    }
    
    /* iterate */
    port = port->next;
  }
  
  g_strfreev(collected_specifier);

  g_list_free_full(start_port,
		   g_object_unref);

  /* output */
  start_channel = NULL;
  
  g_object_get(machine->audio,
	       "output", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  collected_specifier = (gchar **) malloc(sizeof(gchar*));

  collected_specifier[0] = NULL;
  length = 1;
  
  while(channel != NULL){
    AgsChannel *next;
    
    /* output */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;

      specifier = NULL;
      plugin_port = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

      if(specifier == NULL){
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}
      
	/* iterate */
	port = port->next;

	continue;
      }
      
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);

      if(plugin_port != NULL &&
	 !contains_control_name){
	/* create list store entry */
	is_enabled = (ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
										      AGS_TYPE_OUTPUT,
										      specifier)) ? TRUE: FALSE;
    
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, is_enabled,
			   1, g_strdup("output"),
			   2, g_strdup(specifier),
			   -1);      

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }

      g_free(specifier);
      
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }
  
  /* input */
  start_channel = NULL;
  
  g_object_get(machine->audio,
	       "input", &start_channel,
	       NULL);

  channel = start_channel;

  if(channel != NULL){
    g_object_ref(channel);
  }
  
  while(channel != NULL){
    AgsChannel *next;
    
    /* input */
    port =
      start_port = ags_channel_collect_all_channel_ports(channel);

    while(port != NULL){
      AgsChannel *next;
      AgsPluginPort *plugin_port;

      gchar *specifier;

      gboolean is_enabled;
      gboolean contains_control_name;

      specifier = NULL;
      plugin_port = NULL;
    
      g_object_get(port->data,
		   "specifier", &specifier,
		   "plugin-port", &plugin_port,
		   NULL);

      if(specifier == NULL){
	if(plugin_port != NULL){
	  g_object_unref(plugin_port);
	}
      
	/* iterate */
	port = port->next;

	continue;
      }
      
      contains_control_name = g_strv_contains(collected_specifier,
					      specifier);

      if(plugin_port != NULL &&
	 !contains_control_name){
	/* create list store entry */
	is_enabled = (ags_machine_automation_port_find_channel_type_with_control_name(machine->enabled_automation_port,
										      AGS_TYPE_INPUT,
										      specifier)) ? TRUE: FALSE;
    
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter,
			   0, is_enabled,
			   1, g_strdup("input"),
			   2, g_strdup(specifier),
			   -1);      

	/* add to collected specifier */
	collected_specifier = (gchar **) realloc(collected_specifier,
						 (length + 1) * sizeof(gchar *));
	collected_specifier[length - 1] = g_strdup(specifier);
	collected_specifier[length] = NULL;

	length++;
      }

      g_free(specifier);
      
      if(plugin_port != NULL){
	g_object_unref(plugin_port);
      }
    
      /* iterate */
      port = port->next;
    }

    g_list_free_full(start_port,
		     g_object_unref);
    
    /* iterate */
    next = ags_channel_next(channel);

    g_object_unref(channel);

    channel = next;
  }

  /* unref */
  if(start_channel != NULL){
    g_object_unref(start_channel);
  }

  g_strfreev(collected_specifier);
  
  gtk_list_store_append(list_store, &iter);
  gtk_list_store_set(list_store, &iter,
		     0, FALSE,
		     1, g_strdup(""),
		     2, g_strdup(""),
		     -1);
  gtk_combo_box_set_active_iter(composite_toolbar->port,
				&iter);
}

/**
 * ags_composite_toolbar_set_selected_tool:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @selected_tool: the #GtkToggleButton
 * 
 * Set @selected_tool of @composite_toolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_set_selected_tool(AgsCompositeToolbar *composite_toolbar,
					GtkToggleButton *selected_tool)
{
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }
  
  if(composite_toolbar->selected_tool != selected_tool){
    GtkToggleButton *old_selected_tool;

    old_selected_tool = composite_toolbar->selected_tool;
    
    composite_toolbar->selected_tool = selected_tool;
    
    if(old_selected_tool != NULL){
      gtk_toggle_button_set_active(old_selected_tool,
				   FALSE);
    }
  }else{
    if(selected_tool != NULL &&
       !gtk_toggle_button_get_active(selected_tool)){
      gtk_toggle_button_set_active(selected_tool,
				   TRUE);
    }
  }
}

/**
 * ags_composite_toolbar_scope_create_and_connect:
 * @composite_toolbar: the #AgsCompositeToolbar
 * @scope: the scope
 * 
 * Create and connect @scope of @composite_toolbar.
 *
 * Since: 3.8.0
 */
void
ags_composite_toolbar_scope_create_and_connect(AgsCompositeToolbar *composite_toolbar,
					       gchar *scope)
{
  gboolean success;
  
  if(!AGS_IS_COMPOSITE_TOOLBAR(composite_toolbar)){
    return;
  }

  success = FALSE;

  /* destroy current */
  ags_composite_toolbar_unset_tool(composite_toolbar,
				   (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				    AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				    AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				    AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));
  ags_composite_toolbar_unset_action(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_ACTION_INVERT |
				      AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
				      AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
				      AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
  ags_composite_toolbar_unset_option(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
				      AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
				      AGS_COMPOSITE_TOOLBAR_HAS_OPACITY |
				      AGS_COMPOSITE_TOOLBAR_HAS_BEATS |
				      AGS_COMPOSITE_TOOLBAR_HAS_BEATS_TYPE |
				      AGS_COMPOSITE_TOOLBAR_HAS_PORT));

  composite_toolbar->selected_tool = NULL;
  
  /* create new */
  if(scope != NULL){
    if(!g_strcmp0(scope,
		  AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION)){
      static gchar* notation_menu_tool_dialog[] = {
	AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
	AGS_COMPOSITE_TOOLBAR_SCOPE_NOTATION,
	NULL,
      };

      static GValue *notation_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	notation_menu_tool_value = (GValue *) g_new0(GValue,
						     2);

	g_value_init(notation_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(notation_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS));

	g_value_init(notation_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(notation_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE |
			  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_CROP_NOTE |
			  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE |
			  AGS_COMPOSITE_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR));
	
	initialized = TRUE;
      }

      composite_toolbar->menu_tool_dialog = notation_menu_tool_dialog;
      composite_toolbar->menu_tool_value = notation_menu_tool_value;
      
      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				      AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL |
				       AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_INVERT |
					AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY));

      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->edit);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->clear);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->invert);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);
      
      gtk_toggle_button_set_active(composite_toolbar->position,
				   TRUE);
      
      success = TRUE;
    }else if(!g_strcmp0(scope,
			AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET)){
      static gchar* sheet_menu_tool_dialog[] = {
	AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
	AGS_COMPOSITE_TOOLBAR_SCOPE_SHEET,
	NULL,
      };

      static GValue *sheet_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	sheet_menu_tool_value = (GValue *) g_new0(GValue,
						  2);

	g_value_init(sheet_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(sheet_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS));

	g_value_init(sheet_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(sheet_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR |
			  AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_ADD_PAGE |
			  AGS_COMPOSITE_TOOLBAR_SHEET_DIALOG_REMOVE_PAGE));
	
	initialized = TRUE;
      }

      composite_toolbar->menu_tool_dialog = sheet_menu_tool_dialog;
      composite_toolbar->menu_tool_value = sheet_menu_tool_value;
      
      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				      AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL |
				       AGS_COMPOSITE_TOOLBAR_PASTE_NO_DUPLICATES);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_INVERT |
					AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY |
					AGS_COMPOSITE_TOOLBAR_HAS_BEATS |
					AGS_COMPOSITE_TOOLBAR_HAS_BEATS_TYPE));

      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->edit);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->clear);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->invert);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);
      
      gtk_toggle_button_set_active(composite_toolbar->position,
				   TRUE);
      
      success = TRUE;
    }else if(!g_strcmp0(scope,
			AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION)){
      static gchar* automation_menu_tool_dialog[] = {
	AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
	AGS_COMPOSITE_TOOLBAR_SCOPE_AUTOMATION,
	NULL,
      };

      static GValue *automation_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	automation_menu_tool_value = (GValue *) g_new0(GValue,
						       2);

	g_value_init(automation_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(automation_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_LINES |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_LINES));

	g_value_init(automation_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(automation_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION |
			  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION |
			  AGS_COMPOSITE_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR));
	
	initialized = TRUE;
      }

      composite_toolbar->menu_tool_dialog = automation_menu_tool_dialog;
      composite_toolbar->menu_tool_value = automation_menu_tool_value;
      
      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_EDIT |
				      AGS_COMPOSITE_TOOLBAR_TOOL_CLEAR |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY |
					AGS_COMPOSITE_TOOLBAR_HAS_PORT));

      ags_composite_toolbar_load_port(composite_toolbar);
      
      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->edit);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->clear);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->port);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);

      gtk_toggle_button_set_active(composite_toolbar->position,
				   TRUE);

      success = TRUE;
    }else if(!g_strcmp0(scope,
			AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE)){
      static gchar* wave_menu_tool_dialog[] = {
	AGS_COMPOSITE_TOOLBAR_SCOPE_COMMON,
	AGS_COMPOSITE_TOOLBAR_SCOPE_WAVE,
	NULL,
      };

      static GValue *wave_menu_tool_value = NULL;

      static gboolean initialized = FALSE;

      /* static initializers */
      if(!initialized){
	wave_menu_tool_value = (GValue *) g_new0(GValue,
						 2);

	g_value_init(wave_menu_tool_value,
		     G_TYPE_UINT);
	g_value_set_uint(wave_menu_tool_value,
			 (AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_ENABLE_ALL_AUDIO_CHANNELS |
			  AGS_COMPOSITE_TOOLBAR_COMMON_DIALOG_DISABLE_ALL_AUDIO_CHANNELS));

	g_value_init(wave_menu_tool_value + 1,
		     G_TYPE_UINT);
	g_value_set_uint(wave_menu_tool_value + 1,
			 (AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER |
			  AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR |
			  AGS_COMPOSITE_TOOLBAR_WAVE_DIALOG_TIME_STRETCH_BUFFER));
	
	initialized = TRUE;
      }

      composite_toolbar->menu_tool_dialog = wave_menu_tool_dialog;
      composite_toolbar->menu_tool_value = wave_menu_tool_value;
      
      /* set tool, action and option */
      ags_composite_toolbar_set_tool(composite_toolbar,
				     (AGS_COMPOSITE_TOOLBAR_TOOL_POSITION |
				      AGS_COMPOSITE_TOOLBAR_TOOL_SELECT));

      composite_toolbar->paste_mode = (AGS_COMPOSITE_TOOLBAR_PASTE_MATCH_LINE);
      ags_composite_toolbar_set_action(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_ACTION_COPY |
					AGS_COMPOSITE_TOOLBAR_ACTION_CUT |
					AGS_COMPOSITE_TOOLBAR_ACTION_PASTE));
      
      ags_composite_toolbar_set_option(composite_toolbar,
				       (AGS_COMPOSITE_TOOLBAR_HAS_MENU_TOOL |
					AGS_COMPOSITE_TOOLBAR_HAS_ZOOM |
					AGS_COMPOSITE_TOOLBAR_HAS_OPACITY));

      /* connect */
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->position);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->select);
      
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->copy);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->cut);
      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->paste);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->menu_tool);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->zoom);

      ags_connectable_connect_connection(AGS_CONNECTABLE(composite_toolbar),
					 composite_toolbar->opacity);

      gtk_toggle_button_set_active(composite_toolbar->position,
				   TRUE);

      success = TRUE;
    }
  }

  gtk_widget_show(composite_toolbar);
}

/**
 * ags_composite_toolbar_new:
 *
 * Create a new #AgsCompositeToolbar.
 *
 * Returns: a new #AgsCompositeToolbar
 *
 * Since: 3.8.0
 */
AgsCompositeToolbar*
ags_composite_toolbar_new()
{
  AgsCompositeToolbar *composite_toolbar;
  
  composite_toolbar = (AgsCompositeToolbar *) g_object_new(AGS_TYPE_COMPOSITE_TOOLBAR,
							   NULL);
  
  return(composite_toolbar);
}

/**
 * ags_zoom_combo_box_new:
 *
 * Creates an #GtkComboBox to select zoom.
 *
 * Returns: a new #GtkComboBox
 *
 * Since: 3.0.0
 */
GtkComboBox*
ags_zoom_combo_box_new()
{
  GtkComboBoxText *combo_box;

  combo_box = (GtkComboBoxText *) gtk_combo_box_text_new();

  //  gtk_combo_box_text_append_text(combo_box,
  //				 "16:1");
  //  gtk_combo_box_text_append_text(combo_box,
  //				 "8:1");
  gtk_combo_box_text_append_text(combo_box,
				 "4:1");
  gtk_combo_box_text_append_text(combo_box,
				 "2:1");
  gtk_combo_box_text_append_text(combo_box,
				 "1:1");
  gtk_combo_box_text_append_text(combo_box,
				 "1:2");
  gtk_combo_box_text_append_text(combo_box,
				 "1:4");
  gtk_combo_box_text_append_text(combo_box,
				 "1:8");
  gtk_combo_box_text_append_text(combo_box,
				 "1:16");

  return((GtkComboBox *) combo_box);
}
