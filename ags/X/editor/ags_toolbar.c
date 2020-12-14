/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_toolbar_callbacks.h>

#include <ags/X/ags_menu_bar.h>

#include <ags/X/editor/ags_move_note_dialog.h>
#include <ags/X/editor/ags_crop_note_dialog.h>
#include <ags/X/editor/ags_select_note_dialog.h>
#include <ags/X/editor/ags_position_notation_cursor_dialog.h>

#include <ags/i18n.h>

void ags_toolbar_class_init(AgsToolbarClass *toolbar);
void ags_toolbar_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_toolbar_init(AgsToolbar *toolbar);

AgsUUID* ags_toolbar_get_uuid(AgsConnectable *connectable);
gboolean ags_toolbar_has_resource(AgsConnectable *connectable);
gboolean ags_toolbar_is_ready(AgsConnectable *connectable);
void ags_toolbar_add_to_registry(AgsConnectable *connectable);
void ags_toolbar_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_toolbar_list_resource(AgsConnectable *connectable);
xmlNode* ags_toolbar_xml_compose(AgsConnectable *connectable);
void ags_toolbar_xml_parse(AgsConnectable *connectable,
			   xmlNode *node);
gboolean ags_toolbar_is_connected(AgsConnectable *connectable);
void ags_toolbar_connect(AgsConnectable *connectable);
void ags_toolbar_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_toolbar
 * @short_description: toolbar
 * @title: AgsToolbar
 * @section_id:
 * @include: ags/X/editor/ags_toolbar.h
 *
 * The #AgsToolbar lets you choose edit tool.
 */

GType
ags_toolbar_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_toolbar = 0;

    static const GTypeInfo ags_toolbar_info = {
      sizeof (AgsToolbarClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_toolbar_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsToolbar),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_toolbar_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_toolbar_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_toolbar = g_type_register_static(GTK_TYPE_TOOLBAR,
					      "AgsToolbar", &ags_toolbar_info,
					      0);
    
    g_type_add_interface_static(ags_type_toolbar,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_toolbar);
  }

  return g_define_type_id__volatile;
}

void
ags_toolbar_class_init(AgsToolbarClass *toolbar)
{
  /* empty */
}

void
ags_toolbar_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_toolbar_get_uuid;
  connectable->has_resource = ags_toolbar_has_resource;

  connectable->is_ready = ags_toolbar_is_ready;
  connectable->add_to_registry = ags_toolbar_add_to_registry;
  connectable->remove_from_registry = ags_toolbar_remove_from_registry;

  connectable->list_resource = ags_toolbar_list_resource;
  connectable->xml_compose = ags_toolbar_xml_compose;
  connectable->xml_parse = ags_toolbar_xml_parse;

  connectable->is_connected = ags_toolbar_is_connected;
  connectable->connect = ags_toolbar_connect;
  connectable->disconnect = ags_toolbar_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_toolbar_init(AgsToolbar *toolbar)
{
  toolbar->flags = 0;
  toolbar->tool = 0;
  toolbar->action = 0;
  toolbar->option = 0;

  /* uuid */
  toolbar->uuid = ags_uuid_alloc();
  ags_uuid_generate(toolbar->uuid);

  toolbar->selected_tool = NULL;

  toolbar->position = NULL;
  toolbar->edit = NULL;
  toolbar->clear = NULL;
  toolbar->select = NULL;

  toolbar->invert = NULL;
  toolbar->copy = NULL;
  toolbar->cut = NULL;
  
  toolbar->paste_mode = 0;
  toolbar->paste = NULL;
  toolbar->paste_popup = NULL;

  toolbar->menu_tool_dialog = NULL;
  toolbar->menu_tool_value = NULL;
  toolbar->menu_tool = NULL;
  toolbar->menu_tool_popup = NULL;

  toolbar->selected_zoom = NULL;
  
  toolbar->port = NULL;

  toolbar->zoom = NULL;

  toolbar->opacity = NULL;

  toolbar->notation_dialog = NULL;
  toolbar->sheet_dialog = NULL;
  toolbar->automation_dialog = NULL;
  toolbar->wave_dialog = NULL;

  toolbar->move_note = NULL;
  toolbar->crop_note = NULL;
  toolbar->select_note = NULL;
  toolbar->position_notation_cursor = NULL;

  toolbar->position_sheet_cursor = NULL;

  toolbar->select_acceleration = NULL;
  toolbar->ramp_acceleration = NULL;
  toolbar->position_automation_cursor = NULL;

  toolbar->select_buffer = NULL;
  toolbar->position_wave_cursor = NULL;
}

AgsUUID*
ags_toolbar_get_uuid(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;
  
  AgsUUID *ptr;

  toolbar = AGS_TOOLBAR(connectable);

  ptr = toolbar->uuid;

  return(ptr);
}

gboolean
ags_toolbar_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_toolbar_is_ready(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;
  
  gboolean is_ready;

  toolbar = AGS_TOOLBAR(connectable);

  /* check is added */
  is_ready = ags_toolbar_test_flags(toolbar, AGS_TOOLBAR_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_toolbar_add_to_registry(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  toolbar = AGS_TOOLBAR(connectable);

  ags_toolbar_set_flags(toolbar, AGS_TOOLBAR_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = toolbar->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) toolbar);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_toolbar_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_toolbar_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_toolbar_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_toolbar_xml_parse(AgsConnectable *connectable,
		      xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_toolbar_is_connected(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;
  
  gboolean is_connected;

  toolbar = AGS_TOOLBAR(connectable);

  /* check is connected */
  is_connected = ags_toolbar_test_flags(toolbar, AGS_TOOLBAR_CONNECTED);
  
  return(is_connected);
}

void
ags_toolbar_connect(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;
  
  toolbar = AGS_TOOLBAR(connectable);

  if((AGS_TOOLBAR_CONNECTED & (toolbar->flags)) != 0){
    return;
  }

  toolbar->flags |= AGS_TOOLBAR_CONNECTED;

  //TODO:JK: implement me
}

void
ags_toolbar_disconnect(AgsConnectable *connectable)
{
  AgsToolbar *toolbar;
  
  toolbar = AGS_TOOLBAR(connectable);

  if((AGS_TOOLBAR_CONNECTED & (toolbar->flags)) == 0){
    return;
  }

  toolbar->flags &= (~AGS_TOOLBAR_CONNECTED);

  //TODO:JK: implement me
}

/**
 * ags_toolbar_test_flags:
 * @toolbar: the #AgsToolbar
 * @flags: the flags
 *
 * Test @flags to be set on @toolbar.
 * 
 * Returns: %TRUE if flags are set, else %FALSE
 *
 * Since: 3.7.9
 */
gboolean
ags_toolbar_test_flags(AgsToolbar *toolbar, guint flags)
{
  gboolean retval;  
  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (flags & (toolbar->flags)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_toolbar_set_flags:
 * @toolbar: the #AgsToolbar
 * @flags: see enum AgsToolbarFlags
 *
 * Enable a feature of #AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_set_flags(AgsToolbar *toolbar, guint flags)
{
  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  /* set flags */
  toolbar->flags |= flags;
}
    
/**
 * ags_toolbar_unset_flags:
 * @toolbar: the #AgsToolbar
 * @flags: see enum AgsToolbarFlags
 *
 * Disable a feature of AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_unset_flags(AgsToolbar *toolbar, guint flags)
{  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  /* unset flags */
  toolbar->flags &= (~flags);
}

/**
 * ags_toolbar_test_tool:
 * @toolbar: the #AgsToolbar
 * @tool: the tool
 *
 * Test @tool to be set on @toolbar.
 * 
 * Returns: %TRUE if tool are set, else %FALSE
 *
 * Since: 3.7.9
 */
gboolean
ags_toolbar_test_tool(AgsToolbar *toolbar, guint tool)
{
  gboolean retval;  
  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (tool & (toolbar->tool)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_toolbar_set_tool:
 * @toolbar: the #AgsToolbar
 * @tool: see enum AgsToolbarTool
 *
 * Enable a feature of #AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_set_tool(AgsToolbar *toolbar, guint tool)
{
  gint position;
  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  position = 0;

  /* position tool */
  if((AGS_TOOLBAR_TOOL_POSITION & tool) != 0 &&
     toolbar->position == NULL){
    toolbar->position = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(toolbar->position,
		 "label", i18n("Position"),
		 "icon-name", "go-jump",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->position,
		       position);
    
    position++;
  }else if(toolbar->position != NULL){
    position++;
  }

  /* edit tool */
  if((AGS_TOOLBAR_TOOL_EDIT & tool) != 0 &&
     toolbar->edit == NULL){
    toolbar->edit = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(toolbar->edit,
		 "label", i18n("Edit"),
		 "icon-name", "document-edit",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->edit,
		       position);
    
    position++;
  }else if(toolbar->edit != NULL){
    position++;
  }

  /* clear tool */
  if((AGS_TOOLBAR_TOOL_CLEAR & tool) != 0 &&
     toolbar->clear == NULL){
    toolbar->clear = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(toolbar->clear,
		 "label", i18n("Clear"),
		 "icon-name", "edit-clear",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->clear,
		       position);
    
    position++;
  }else if(toolbar->clear != NULL){
    position++;
  }

  /* select tool */
  if((AGS_TOOLBAR_TOOL_SELECT & tool) != 0 &&
     toolbar->select == NULL){
    toolbar->select = (GtkToggleToolButton *) gtk_toggle_tool_button_new();
    g_object_set(toolbar->select,
		 "label", i18n("Select"),
		 "icon-name", "edit-select",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->select,
		       position);
    
    position++;
  }else if(toolbar->select != NULL){
    position++;
  }
  
  /* set tool */
  toolbar->tool |= tool;
}
    
/**
 * ags_toolbar_unset_tool:
 * @toolbar: the #AgsToolbar
 * @tool: see enum AgsToolbarTool
 *
 * Disable a feature of AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_unset_tool(AgsToolbar *toolbar, guint tool)
{  
  gint position;

  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  if((AGS_TOOLBAR_TOOL_POSITION & tool) != 0 &&
     toolbar->position != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->position);

    toolbar->position = NULL;
  }

  if((AGS_TOOLBAR_TOOL_EDIT & tool) != 0 &&
     toolbar->edit != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->edit);

    toolbar->edit = NULL;
  }

  if((AGS_TOOLBAR_TOOL_CLEAR & tool) != 0 &&
     toolbar->clear != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->clear);

    toolbar->clear = NULL;
  }

  if((AGS_TOOLBAR_TOOL_SELECT & tool) != 0 &&
     toolbar->select != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->select);

    toolbar->select = NULL;
  }
  
  /* unset tool */
  toolbar->tool &= (~tool);
}

/**
 * ags_toolbar_test_action:
 * @toolbar: the #AgsToolbar
 * @action: the action
 *
 * Test @action to be set on @toolbar.
 * 
 * Returns: %TRUE if action are set, else %FALSE
 *
 * Since: 3.7.9
 */
gboolean
ags_toolbar_test_action(AgsToolbar *toolbar, guint action)
{
  gboolean retval;  
  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (action & (toolbar->action)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_toolbar_set_action:
 * @toolbar: the #AgsToolbar
 * @action: see enum AgsToolbarAction
 *
 * Enable a feature of #AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_set_action(AgsToolbar *toolbar, guint action)
{
  gint position;

  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  position = 0;
  
  if(toolbar->position != NULL){
    position++;
  }

  if(toolbar->edit != NULL){
    position++;
  }

  if(toolbar->clear != NULL){
    position++;
  }

  if(toolbar->select != NULL){
    position++;
  }

  /* invert action */
  if((AGS_TOOLBAR_ACTION_INVERT & action) != 0 &&
     toolbar->invert == NULL){
    toolbar->invert = (GtkToolButton *) gtk_tool_button_new();
    g_object_set(toolbar->invert,
		 "label", i18n("Invert"),
		 "icon-name", "object-flip-vertical",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->invert,
		       position);
    
    position++;
  }else if(toolbar->invert != NULL){
    position++;
  }

  /* copy action */
  if((AGS_TOOLBAR_ACTION_COPY & action) != 0 &&
     toolbar->copy == NULL){
    toolbar->copy = (GtkToolButton *) gtk_tool_button_new();
    g_object_set(toolbar->copy,
		 "label", i18n("Copy"),
		 "icon-name", "edit-copy",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->copy,
		       position);
    
    position++;
  }else if(toolbar->copy != NULL){
    position++;
  }

  /* cut action */
  if((AGS_TOOLBAR_ACTION_CUT & action) != 0 &&
     toolbar->cut == NULL){
    toolbar->cut = (GtkToolButton *) gtk_tool_button_new();
    g_object_set(toolbar->cut,
		 "label", i18n("Cut"),
		 "icon-name", "edit-cut",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->cut,
		       position);
    
    position++;
  }else if(toolbar->cut != NULL){
    position++;
  }

  /* paste action */
  if((AGS_TOOLBAR_ACTION_PASTE & action) != 0 &&
     toolbar->paste == NULL){
    toolbar->paste = (GtkMenuToolButton *) gtk_menu_tool_button_new();
    g_object_set(toolbar->paste,
		 "label", i18n("Paste"),
		 "icon-name", "edit-paste",
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->paste,
		       position);

    toolbar->paste_popup = (GtkMenu *) ags_toolbar_paste_popup_new(toolbar->paste_mode);
    gtk_menu_tool_button_set_menu(toolbar->paste,
				  (GtkWidget *) toolbar->paste_popup);

    position++;
  }else if(toolbar->paste != NULL){
    position++;
  }
  
  /* set action */
  toolbar->action |= action;
}
    
/**
 * ags_toolbar_unset_action:
 * @toolbar: the #AgsToolbar
 * @action: see enum AgsToolbarAction
 *
 * Disable a feature of AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_unset_action(AgsToolbar *toolbar, guint action)
{  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  if((AGS_TOOLBAR_ACTION_INVERT & action) != 0 &&
     toolbar->invert != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->invert);

    toolbar->invert = NULL;
  }

  if((AGS_TOOLBAR_ACTION_COPY & action) != 0 &&
     toolbar->copy != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->copy);

    toolbar->copy = NULL;
  }

  if((AGS_TOOLBAR_ACTION_CUT & action) != 0 &&
     toolbar->cut != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->cut);

    toolbar->cut = NULL;
  }

  if((AGS_TOOLBAR_ACTION_PASTE & action) != 0 &&
     toolbar->paste != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->paste);

    //TODO:JK: check if needed
    //    toolbar->paste_mode = 0;

    toolbar->paste = NULL;
    toolbar->paste_popup = NULL;
  }
  
  /* unset action */
  toolbar->action &= (~action);
}

/**
 * ags_toolbar_test_option:
 * @toolbar: the #AgsToolbar
 * @option: the option
 *
 * Test @option to be set on @toolbar.
 * 
 * Returns: %TRUE if option are set, else %FALSE
 *
 * Since: 3.7.9
 */
gboolean
ags_toolbar_test_option(AgsToolbar *toolbar, guint option)
{
  gboolean retval;  
  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return(FALSE);
  }
    
  /* test */
  retval = (option & (toolbar->option)) ? TRUE: FALSE;

  return(retval);
}

/**
 * ags_toolbar_set_option:
 * @toolbar: the #AgsToolbar
 * @option: see enum AgsToolbarOption
 *
 * Enable a feature of #AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_set_option(AgsToolbar *toolbar, guint option)
{
  gint position;

  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  position = 0;
  
  if(toolbar->position != NULL){
    position++;
  }

  if(toolbar->edit != NULL){
    position++;
  }

  if(toolbar->clear != NULL){
    position++;
  }

  if(toolbar->select != NULL){
    position++;
  }

  if(toolbar->invert != NULL){
    position++;
  }
  
  if(toolbar->copy != NULL){
    position++;
  }

  if(toolbar->cut != NULL){
    position++;
  }

  if(toolbar->paste != NULL){
    position++;
  }

  /* menu tool */
  if((AGS_TOOLBAR_HAS_MENU_TOOL & option) != 0 &&
     toolbar->menu_tool == NULL){
    toolbar->menu_tool = (GtkMenuToolButton *) gtk_menu_tool_button_new();
    g_object_set(toolbar->menu_tool,
		 "label", i18n("Tool"),
		 NULL);
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) toolbar->menu_tool,
		       position);

    toolbar->menu_tool_popup = (GtkMenu *) ags_toolbar_menu_tool_popup_new(toolbar->menu_tool_dialog,
									   toolbar->menu_tool_value);
    gtk_menu_tool_button_set_menu(toolbar->menu_tool,
				  (GtkWidget *) toolbar->menu_tool_popup);
  }else if(toolbar->menu_tool != NULL){
    position++;
  }

  /* zoom */
  if((AGS_TOOLBAR_HAS_ZOOM & option) != 0 &&
     toolbar->zoom == NULL){
    GtkToolItem *tool_item;
    GtkHBox *hbox;
    GtkLabel *label;

    tool_item = gtk_tool_item_new();
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) tool_item,
		       position);
    
    hbox = gtk_hbox_new(FALSE,
			0);
    gtk_container_add((GtkContainer *) tool_item,
		      (GtkWidget *) hbox);


    label = (GtkLabel *) gtk_label_new(i18n("Zoom"));
    gtk_box_pack_start(hbox,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);

    toolbar->selected_zoom = 2;
    toolbar->zoom = ags_zoom_combo_box_new();
    gtk_combo_box_set_active(GTK_COMBO_BOX(toolbar->zoom),
			     2);
    gtk_box_pack_start(hbox,
		       (GtkWidget *) toolbar->zoom,
		       FALSE, FALSE,
		       0);
  }else if(toolbar->zoom != NULL){
    position++;
  }
  
  /* opacity */
  if((AGS_TOOLBAR_HAS_OPACITY & option) != 0 &&
     toolbar->opacity == NULL){
    GtkToolItem *tool_item;
    GtkHBox *hbox;
    GtkLabel *label;

    tool_item = gtk_tool_item_new();
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) tool_item,
		       -1);

    hbox = gtk_hbox_new(FALSE,
			0);
    gtk_container_add((GtkContainer *) tool_item,
		      (GtkWidget *) hbox);

    label = (GtkLabel *) gtk_label_new(i18n("Opacity"));
    gtk_box_pack_start(hbox,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);

    toolbar->opacity = (GtkSpinButton *) gtk_spin_button_new_with_range(0.0, 1.0, 0.001);
    gtk_spin_button_set_digits(toolbar->opacity, 4);
    gtk_spin_button_set_value(toolbar->opacity, 1.0);
    gtk_box_pack_start(hbox,
		       (GtkWidget *) toolbar->opacity,
		       FALSE, FALSE,
		       0);
  }else if(toolbar->opacity != NULL){
    position++;
  }
  
  /* port */
  if((AGS_TOOLBAR_HAS_PORT & option) != 0 &&
     toolbar->port == NULL){
    GtkToolItem *tool_item;
    GtkHBox *hbox;
    GtkLabel *label;

    GtkCellRenderer *cell_renderer_toggle;
    GtkCellRenderer *scope_cell_renderer_text;
    GtkCellRenderer *port_cell_renderer_text;

    tool_item = gtk_tool_item_new();
    gtk_toolbar_insert((GtkToolbar *) toolbar,
		       (GtkWidget *) tool_item,
		       position);

    hbox = gtk_hbox_new(FALSE,
			0);
    gtk_container_add((GtkContainer *) tool_item,
		      (GtkWidget *) hbox);
    
    label = (GtkLabel *) gtk_label_new(i18n("Port"));
    gtk_box_pack_start(hbox,
		       (GtkWidget *) label,
		       FALSE, FALSE,
		       0);
    
    toolbar->port = (GtkComboBox *) gtk_combo_box_new();

    cell_renderer_toggle = gtk_cell_renderer_toggle_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(toolbar->port),
			       cell_renderer_toggle,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(toolbar->port), cell_renderer_toggle,
				   "active", 0,
				   NULL);
    gtk_cell_renderer_toggle_set_activatable(GTK_CELL_RENDERER_TOGGLE(cell_renderer_toggle),
					     TRUE);
  
    scope_cell_renderer_text = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(toolbar->port),
			       scope_cell_renderer_text,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(toolbar->port), scope_cell_renderer_text,
				   "text", 1,
				   NULL);

    port_cell_renderer_text = gtk_cell_renderer_text_new();
    gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(toolbar->port),
			       port_cell_renderer_text,
			       FALSE);
    gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(toolbar->port), port_cell_renderer_text,
				   "text", 2,
				   NULL);
  
    gtk_box_pack_start(hbox,
		       (GtkWidget *) toolbar->port,
		       FALSE, FALSE,
		       0);

    position++;
  }else if(toolbar->port != NULL){
    position++;
  }

  /* set option */
  toolbar->option |= option;
}
    
/**
 * ags_toolbar_unset_option:
 * @toolbar: the #AgsToolbar
 * @option: see enum AgsToolbarOption
 *
 * Disable a feature of AgsToolbar.
 *
 * Since: 3.7.9
 */
void
ags_toolbar_unset_option(AgsToolbar *toolbar, guint option)
{  
  if(!AGS_IS_TOOLBAR(toolbar)){
    return;
  }

  if((AGS_TOOLBAR_HAS_MENU_TOOL & option) != 0 &&
     toolbar->menu_tool != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->menu_tool);

    toolbar->menu_tool = NULL;
    toolbar->menu_tool_popup = NULL;
  }

  if((AGS_TOOLBAR_HAS_OPACITY & option) != 0 &&
     toolbar->opacity != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->opacity);

    toolbar->opacity = NULL;
  }

  if((AGS_TOOLBAR_HAS_PORT & option) != 0 &&
     toolbar->port != NULL){
    gtk_widget_destroy((GtkWidget *) toolbar->port);

    toolbar->port = NULL;
  }
  
  /* unset option */
  toolbar->option &= (~option);
}

/**
 * ags_toolbar_paste_popup_new:
 * @paste_mode: the paste mode
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.7.9
 */
GtkMenu*
ags_toolbar_paste_popup_new(guint paste_mode)
{
  GtkMenu *menu;
  GtkMenuItem *item;
  
  menu = (GtkMenu *) gtk_menu_new();

  if((AGS_TOOLBAR_PASTE_MATCH_AUDIO_CHANNEL & paste_mode) != 0){
    item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
			"label", i18n("match audio channel"),
			"active", TRUE,
			NULL);
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);
  }

  if((AGS_TOOLBAR_PASTE_MATCH_LINE & past_mode) != 0){
    item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
			"label", "match line",
			"active", TRUE,
			NULL);
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);
  }

  if((AGS_TOOLBAR_PASTE_NO_DUPLICATES & past_mode) != 0){
    item = g_object_new(GTK_TYPE_CHECK_MENU_ITEM,
			"label", i18n("no duplicates"),
			"active", TRUE,
			NULL);
    gtk_menu_shell_append((GtkMenuShell *) menu,
			  (GtkWidget *) item);
  }
  
  return(menu);
}

/**
 * ags_toolbar_menu_tool_popup_new:
 * @dialog: the tool dialog string vector
 * @value: the #GValue-struct array
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.7.9
 */
GtkMenu*
ags_toolbar_tool_popup_new(gchar **dialog,
			   GValue *value)
{
  GtkMenu *menu;
  GtkMenuItem *item;

  guint i, j;
  
  menu = (GtkMenu *) gtk_menu_new();

  if(dialog != NULL &&
     value != NULL){
    for(i = 0, j = 0; i < AGS_TOOLBAR_EDITOR_COUNT; i++){
      guint current_value;

      if(dialog[j] != NULL){
	current_value = g_value_get_uint(value + j);
	
	if(!g_strcmp0(dialog[j],
		      "notation")){
	  if((AGS_TOOLBAR_NOTATION_DIALOG_MOVE_NOTE & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("move notes"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }else if((AGS_TOOLBAR_NOTATION_DIALOG_CROP_NOTE & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("crop notes"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
 	  }else if((AGS_TOOLBAR_NOTATION_DIALOG_SELECT_NOTE & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select notes"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }else if((AGS_TOOLBAR_NOTATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }
	  
	  j++;
	}else if(!g_strcmp0(dialog[j],
			    "sheet")){	
	  if((AGS_TOOLBAR_SHEET_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }
	  
	  j++;
	}else if(!g_strcmp0(dialog[j],
			    "automation")){
	  if((AGS_TOOLBAR_AUTOMATION_DIALOG_SELECT_ACCELERATION & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select acceleration"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }else if((AGS_TOOLBAR_AUTOMATION_DIALOG_RAMP_ACCELERATION & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("ramp acceleration"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }else if((AGS_TOOLBAR_AUTOMATION_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }
	
	  j++;
	}else if(!g_strcmp0(dialog[j],
			    "wave")){
	  if((AGS_TOOLBAR_WAVE_DIALOG_SELECT_BUFFER & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("select buffer"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }else if((AGS_TOOLBAR_WAVE_DIALOG_POSITION_CURSOR & current_value) != 0){
	    item = (GtkMenuItem *) gtk_menu_item_new_with_label(i18n("position cursor"));
	    gtk_menu_shell_append((GtkMenuShell *) tool_popup,
				  (GtkWidget *) item);
	  }
	
	  j++;
	}else{
	  g_warning("unknown dialog");
	}
      }
    }
  }
  
  return(menu);
}

/**
 * ags_toolbar_new:
 *
 * Create a new #AgsToolbar.
 *
 * Returns: a new #AgsToolbar
 *
 * Since: 3.7.9
 */
AgsToolbar*
ags_toolbar_new()
{
  AgsToolbar *toolbar;

  toolbar = (AgsToolbar *) g_object_new(AGS_TYPE_TOOLBAR,
					NULL);

  return(toolbar);
}
