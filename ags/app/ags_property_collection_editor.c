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

#include <ags/app/ags_property_collection_editor.h>
#include <ags/app/ags_property_collection_editor_callbacks.h>

#include <string.h>

void ags_property_collection_editor_class_init(AgsPropertyCollectionEditorClass *property_collection_editor);
void ags_property_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_property_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_property_collection_editor_init(AgsPropertyCollectionEditor *property_collection_editor);

void ags_property_collection_editor_connect(AgsConnectable *connectable);
void ags_property_collection_editor_disconnect(AgsConnectable *connectable);

void ags_property_collection_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_property_collection_editor_apply(AgsApplicable *applicable);
void ags_property_collection_editor_reset(AgsApplicable *applicable);

/**
 * SECTION:ags_property_collection_editor
 * @short_description: Edit propertys in bulk mode.
 * @title: AgsPropertyCollectionEditor
 * @section_id:
 * @include: ags/app/ags_property_collection_editor.h
 *
 * #AgsPropertyCollectionEditor is a composite widget to modify propertys in bulk mode. A property collection
 * editor should be packed by a #AgsMachineEditor.
 */

AgsConnectableInterface *ags_property_collection_editor_parent_connectable_interface;

GType
ags_property_collection_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_property_collection_editor = 0;

    static const GTypeInfo ags_property_collection_editor_info = {
      sizeof (AgsPropertyCollectionEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_property_collection_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPropertyCollectionEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_property_collection_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_property_collection_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_property_collection_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_property_collection_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
								 "AgsPropertyCollectionEditor", &ags_property_collection_editor_info,
								 0);
    
    g_type_add_interface_static(ags_type_property_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_property_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_property_collection_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_property_collection_editor_class_init(AgsPropertyCollectionEditorClass *property_collection_editor)
{
}

void
ags_property_collection_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_property_collection_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_property_collection_editor_connect;
  connectable->disconnect = ags_property_collection_editor_disconnect;
}

void
ags_property_collection_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_property_collection_editor_set_update;
  applicable->apply = ags_property_collection_editor_apply;
  applicable->reset = ags_property_collection_editor_reset;
}

void
ags_property_collection_editor_init(AgsPropertyCollectionEditor *property_collection_editor)
{
  property_collection_editor->flags = 0;

  property_collection_editor->child_type = G_TYPE_NONE;

  property_collection_editor->child_strv = NULL;
  property_collection_editor->child_value = NULL;
  
  property_collection_editor->child = (GtkBox *) gtk_box_new(GTK_ORIENTATION_VERTICAL,
							     16);
  gtk_box_pack_start(GTK_BOX(property_collection_editor),
		     GTK_WIDGET(property_collection_editor->child),
		     TRUE, TRUE,
		     0);

  property_collection_editor->add_collection = (GtkButton *) gtk_button_new_from_icon_name("list-add",
											   GTK_ICON_SIZE_BUTTON);
  gtk_box_pack_start(property_collection_editor,
		     (GtkWidget * ) property_collection_editor->add_collection,
		     FALSE, FALSE,
		     0);
}

void
ags_property_collection_editor_connect(AgsConnectable *connectable)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  if((AGS_PROPERTY_EDITOR_CONNECTED & (AGS_PROPERTY_EDITOR(connectable)->flags)) != 0){
    return;
  }
  
  /* call parent */
  ags_property_collection_editor_parent_connectable_interface->connect(connectable);

  /* AgsPropertyCollectionEditor */
  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(connectable);

  g_signal_connect(G_OBJECT(property_collection_editor->add_collection), "clicked",
		   G_CALLBACK(ags_property_collection_editor_add_collection_callback), property_collection_editor);
}

void
ags_property_collection_editor_disconnect(AgsConnectable *connectable)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  if((AGS_PROPERTY_EDITOR_CONNECTED & (AGS_PROPERTY_EDITOR(connectable)->flags)) == 0){
    return;
  }
  
  /* AgsPropertyCollectionEditor */
  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(connectable);

  g_object_disconnect(G_OBJECT(property_collection_editor->add_collection),
		      "any_signal::clicked",
		      G_CALLBACK(ags_property_collection_editor_add_collection_callback),
		      property_collection_editor,
		      NULL);

  /* call parent */
  ags_property_collection_editor_parent_connectable_interface->disconnect(connectable);
}

void
ags_property_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  /* empty */
}

void
ags_property_collection_editor_apply(AgsApplicable *applicable)
{
  AgsPropertyCollectionEditor *property_collection_editor;
  GtkWidget *child;

  GList *start_list, *list;

  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(property_collection_editor)->flags)) == 0){
    return;
  }

  list =
    start_list = gtk_container_get_children(GTK_CONTAINER(property_collection_editor->child));

  while(list != NULL){
    child = GTK_WIDGET(g_object_get_data(G_OBJECT(list->data), "AgsChild"));

    ags_applicable_apply(AGS_APPLICABLE(child));

    list = list->next;
  }

  g_list_free(start_list);
}

void
ags_property_collection_editor_reset(AgsApplicable *applicable)
{
  /* empty */
}

/**
 * ags_property_collection_editor_new:
 * @child_type: either %AGS_TYPE_INPUT or %AGS_TYPE_OUTPUT
 * @child_strv: child string vector
 * @child_value: child value
 *
 * Creates an #AgsPropertyCollectionEditor
 *
 * Returns: a new #AgsPropertyCollectionEditor
 *
 * Since: 3.0.0
 */
AgsPropertyCollectionEditor*
ags_property_collection_editor_new(GType child_type,
				   gchar **child_strv,
				   GValue *child_value)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  property_collection_editor = (AgsPropertyCollectionEditor *) g_object_new(AGS_TYPE_PROPERTY_COLLECTION_EDITOR,
									    NULL);

  property_collection_editor->child_type = child_type;
  property_collection_editor->child_strv = child_strv;
  property_collection_editor->child_value = child_value;
  
  return(property_collection_editor);
}
