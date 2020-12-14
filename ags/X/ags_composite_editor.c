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

#include <ags/X/ags_composite_editor.h>
#include <ags/X/ags_composite_editor_callbacks.h>

#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

void ags_composite_editor_class_init(AgsCompositeEditorClass *composite_editor);
void ags_composite_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_composite_editor_init(AgsCompositeEditor *composite_editor);

AgsUUID* ags_composite_editor_get_uuid(AgsConnectable *connectable);
gboolean ags_composite_editor_has_resource(AgsConnectable *connectable);
gboolean ags_composite_editor_is_ready(AgsConnectable *connectable);
void ags_composite_editor_add_to_registry(AgsConnectable *connectable);
void ags_composite_editor_remove_from_registry(AgsConnectable *connectable);
xmlNode* ags_composite_editor_list_resource(AgsConnectable *connectable);
xmlNode* ags_composite_editor_xml_compose(AgsConnectable *connectable);
void ags_composite_editor_xml_parse(AgsConnectable *connectable,
				    xmlNode *node);
gboolean ags_composite_editor_is_connected(AgsConnectable *connectable);
void ags_composite_editor_connect(AgsConnectable *connectable);
void ags_composite_editor_disconnect(AgsConnectable *connectable);

void ags_composite_editor_real_machine_changed(AgsCompositeEditor *composite_editor,
					       AgsMachine *machine);

enum{
  MACHINE_CHANGED,
  LAST_SIGNAL,
};

static gpointer ags_composite_editor_parent_class = NULL;
static guint composite_editor_signals[LAST_SIGNAL];

/**
 * SECTION:ags_composite_editor
 * @short_description: composite editor
 * @title: AgsCompositeEditor
 * @section_id:
 * @include: ags/X/ags_composite_editor.h
 *
 * The #AgsCompositeEditor is a composite abstraction of edit implementation.
 */

GType
ags_composite_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_composite_editor = 0;

    static const GTypeInfo ags_composite_editor_info = {
      sizeof (AgsCompositeEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_composite_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCompositeEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_composite_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_composite_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_composite_editor = g_type_register_static(GTK_TYPE_BOX,
						       "AgsCompositeEditor", &ags_composite_editor_info,
						       0);
    
    g_type_add_interface_static(ags_type_composite_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_composite_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_composite_editor_class_init(AgsCompositeEditorClass *composite_editor)
{
  ags_composite_editor_parent_class = g_type_class_peek_parent(composite_editor);

  /* AgsCompositeEditorClass */
  composite_editor->machine_changed = ags_composite_editor_real_machine_changed;

  /* signals */
  /**
   * AgsCompositeEditor::machine-changed:
   * @editor: the object to change machine.
   * @machine: the #AgsMachine to set
   *
   * The ::machine-changed signal notifies about changed machine.
   * 
   * Since: 3.7.9
   */
  composite_editor_signals[MACHINE_CHANGED] =
    g_signal_new("machine-changed",
                 G_TYPE_FROM_CLASS(composite_editor),
                 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsCompositeEditorClass, machine_changed),
                 NULL, NULL,
                 g_cclosure_marshal_VOID__OBJECT,
                 G_TYPE_NONE, 1,
		 G_TYPE_OBJECT);
}

void
ags_composite_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->get_uuid = ags_composite_editor_get_uuid;
  connectable->has_resource = ags_composite_editor_has_resource;

  connectable->is_ready = ags_composite_editor_is_ready;
  connectable->add_to_registry = ags_composite_editor_add_to_registry;
  connectable->remove_from_registry = ags_composite_editor_remove_from_registry;

  connectable->list_resource = ags_composite_editor_list_resource;
  connectable->xml_compose = ags_composite_editor_xml_compose;
  connectable->xml_parse = ags_composite_editor_xml_parse;

  connectable->is_connected = ags_composite_editor_is_connected;
  connectable->connect = ags_composite_editor_connect;
  connectable->disconnect = ags_composite_editor_disconnect;

  connectable->connect_connection = NULL;
  connectable->disconnect_connection = NULL;
}

void
ags_composite_editor_init(AgsCompositeEditor *composite_editor)
{
  gtk_orientable_set_orientation(GTK_ORIENTABLE(composite_editor),
				 GTK_ORIENTATION_VERTICAL);

  composite_editor->flags = 0;

  composite_editor->version = g_strdup(AGS_COMPOSITE_EDITOR_DEFAULT_VERSION);
  composite_editor->build_id = g_strdup(AGS_COMPOSITE_EDITOR_DEFAULT_BUILD_ID);

  /* uuid */
  composite_editor->uuid = ags_uuid_alloc();
  ags_uuid_generate(composite_editor->uuid);
}

AgsUUID*
ags_composite_editor_get_uuid(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  AgsUUID *ptr;

  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  ptr = composite_editor->uuid;

  return(ptr);
}

gboolean
ags_composite_editor_has_resource(AgsConnectable *connectable)
{
  return(FALSE);
}

gboolean
ags_composite_editor_is_ready(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  gboolean is_ready;

  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  /* check is added */
  is_ready = ags_composite_editor_test_flags(composite_editor, AGS_COMPOSITE_EDITOR_ADDED_TO_REGISTRY);
  
  return(is_ready);
}

void
ags_composite_editor_add_to_registry(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;

  AgsRegistry *registry;
  AgsRegistryEntry *entry;

  AgsApplicationContext *application_context;

  if(ags_connectable_is_ready(connectable)){
    return;
  }
  
  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  ags_composite_editor_set_flags(composite_editor, AGS_COMPOSITE_EDITOR_ADDED_TO_REGISTRY);

  application_context = ags_application_context_get_instance();

  registry = (AgsRegistry *) ags_service_provider_get_registry(AGS_SERVICE_PROVIDER(application_context));

  if(registry != NULL){
    entry = ags_registry_entry_alloc(registry);

    entry->id = composite_editor->uuid;
    g_value_set_object(entry->entry,
		       (gpointer) composite_editor);
    
    ags_registry_add_entry(registry,
			   entry);
  }
}

void
ags_composite_editor_remove_from_registry(AgsConnectable *connectable)
{
  if(!ags_connectable_is_ready(connectable)){
    return;
  }

  //TODO:JK: implement me
}

xmlNode*
ags_composite_editor_list_resource(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

xmlNode*
ags_composite_editor_xml_compose(AgsConnectable *connectable)
{
  xmlNode *node;
  
  node = NULL;

  //TODO:JK: implement me
  
  return(node);
}

void
ags_composite_editor_xml_parse(AgsConnectable *connectable,
			       xmlNode *node)
{
  //TODO:JK: implement me
}

gboolean
ags_composite_editor_is_connected(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  gboolean is_connected;

  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  /* check is connected */
  is_connected = ags_composite_editor_test_flags(composite_editor, AGS_COMPOSITE_EDITOR_CONNECTED);
  
  return(is_connected);
}

void
ags_composite_editor_connect(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  if((AGS_COMPOSITE_EDITOR_CONNECTED & (composite_editor->flags)) != 0){
    return;
  }

  composite_editor->flags |= AGS_COMPOSITE_EDITOR_CONNECTED;

  //TODO:JK: implement me
}

void
ags_composite_editor_disconnect(AgsConnectable *connectable)
{
  AgsCompositeEditor *composite_editor;
  
  composite_editor = AGS_COMPOSITE_EDITOR(connectable);

  if((AGS_COMPOSITE_EDITOR_CONNECTED & (composite_editor->flags)) == 0){
    return;
  }

  composite_editor->flags &= (~AGS_COMPOSITE_EDITOR_CONNECTED);

  //TODO:JK: implement me
}

void
ags_composite_editor_real_machine_changed(AgsCompositeEditor *composite_editor,
					  AgsMachine *machine)
{
  //TODO:JK: implement me
}

/**
 * ags_composite_editor_machine_changed:
 * @composite_editor: the #AgsCompositeEditor
 * @machine: the new #AgsMachine
 *
 * Is emitted as machine changed of composite_editor.
 *
 * Since: 3.7.9
 */
void
ags_composite_editor_machine_changed(AgsCompositeEditor *composite_editor,
				     AgsMachine *machine)
{
  g_return_if_fail(AGS_IS_COMPOSITE_EDITOR(composite_editor));

  g_object_ref((GObject *) composite_editor);
  g_signal_emit((GObject *) composite_editor,
		composite_editor_signals[MACHINE_CHANGED], 0,
		machine);
  g_object_unref((GObject *) composite_editor);
}

/**
 * ags_composite_editor_new:
 *
 * Create a new #AgsCompositeEditor.
 *
 * Returns: a new #AgsCompositeEditor
 *
 * Since: 3.7.9
 */
AgsCompositeEditor*
ags_composite_editor_new()
{
  AgsCompositeEditor *composite_editor;

  composite_editor = (AgsCompositeEditor *) g_object_new(AGS_TYPE_COMPOSITE_EDITOR,
							 NULL);

  return(composite_editor);
}
