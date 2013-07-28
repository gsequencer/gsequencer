/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <ags/X/ags_property_collection_editor.h>
#include <ags/X/ags_property_collection_editor_callbacks.h>

#include <ags-lib/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

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
void ags_property_collection_editor_destroy(GtkObject *object);
void ags_property_collection_editor_show(GtkWidget *widget);

AgsConnectableInterface *ags_property_collection_editor_parent_connectable_interface;

GType
ags_property_collection_editor_get_type(void)
{
  static GType ags_type_property_collection_editor = 0;

  if(!ags_type_property_collection_editor){
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
								 "AgsPropertyCollectionEditor\0", &ags_property_collection_editor_info,
								 0);
    
    g_type_add_interface_static(ags_type_property_collection_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_property_collection_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }

  return(ags_type_property_collection_editor);
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
  GtkAlignment *alignment;
  
  property_collection_editor->child = (GtkVBox *) gtk_vbox_new(FALSE, 16);
  gtk_box_pack_start(GTK_BOX(property_collection_editor),
		     GTK_WIDGET(property_collection_editor->child),
		     TRUE, TRUE,
		     0);

  alignment = (GtkAlignment *) gtk_alignment_new(0.0, 0.0,
						 0.0, 0.0);
  gtk_box_pack_start(GTK_BOX(property_collection_editor),
		     GTK_WIDGET(alignment),
		     FALSE, FALSE,
		     0);

  property_collection_editor->add_collection = (GtkButton *) gtk_button_new_from_stock(GTK_STOCK_ADD);
  gtk_container_add(GTK_CONTAINER(alignment),
		    GTK_WIDGET(property_collection_editor->add_collection));
}

void
ags_property_collection_editor_connect(AgsConnectable *connectable)
{
  AgsPropertyCollectionEditor *property_collection_editor;
  GList *pad_list;

  ags_property_collection_editor_parent_connectable_interface->connect(connectable);

  /* AgsPropertyCollectionEditor */
  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(connectable);

  g_signal_connect(G_OBJECT(property_collection_editor->add_collection), "clicked\0",
		   G_CALLBACK(ags_property_collection_editor_add_collection_callback), property_collection_editor);
}

void
ags_property_collection_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_property_collection_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsPropertyCollectionEditor *property_collection_editor;
  GList *list;

  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(applicable);

  /* empty */
}

void
ags_property_collection_editor_apply(AgsApplicable *applicable)
{
  AgsPropertyCollectionEditor *property_collection_editor;
  GtkWidget *child;
  GList *list;

  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(applicable);

  if((AGS_PROPERTY_EDITOR_ENABLED & (AGS_PROPERTY_EDITOR(property_collection_editor)->flags)) == 0)
    return;

  list = gtk_container_get_children(GTK_CONTAINER(property_collection_editor->child));

  while(list != NULL){
    child = GTK_WIDGET(g_object_get_data(G_OBJECT(list->data), "AgsChild\0"));
    ags_applicable_apply(AGS_APPLICABLE(child));

    list = list->next;
  }
}

void
ags_property_collection_editor_reset(AgsApplicable *applicable)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  property_collection_editor = AGS_PROPERTY_COLLECTION_EDITOR(applicable);

  /* empty */
}

void
ags_property_collection_editor_destroy(GtkObject *object)
{
  /* empty */
}

void
ags_property_collection_editor_show(GtkWidget *widget)
{
  /* empty */
}

AgsPropertyCollectionEditor*
ags_property_collection_editor_new(GType child_type,
				   guint child_parameter_count,
				   GParameter *child_parameter)
{
  AgsPropertyCollectionEditor *property_collection_editor;

  property_collection_editor = (AgsPropertyCollectionEditor *) g_object_new(AGS_TYPE_PROPERTY_COLLECTION_EDITOR,
									    NULL);

  property_collection_editor->child_type = child_type;
  property_collection_editor->child_parameter_count = child_parameter_count;
  property_collection_editor->child_parameter = child_parameter;
  
  return(property_collection_editor);
}
