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

  //TODO:JK: implement me
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
 * ags_toolbar_tool_popup_new:
 * @toolbar: the #AgsToolbar
 *
 * Create a new #GtkMenu suitable for menu tool button.
 *
 * Returns: a new #GtkMenu
 *
 * Since: 3.7.9
 */
GtkMenu*
ags_toolbar_tool_popup_new(AgsToolbar *toolbar)
{
  //TODO:JK: implement me
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
