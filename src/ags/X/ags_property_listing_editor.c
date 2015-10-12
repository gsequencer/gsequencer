/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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
#include <ags/X/ags_property_listing_editor.h>

#include <ags/object/ags_connectable.h>

void ags_property_listing_editor_class_init(AgsPropertyListingEditorClass *property_listing_editor);
void ags_property_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_property_listing_editor_init(AgsPropertyListingEditor *property_listing_editor);
void ags_property_listing_editor_connect(AgsConnectable *connectable);
void ags_property_listing_editor_disconnect(AgsConnectable *connectable);
void ags_property_listing_editor_destroy(GtkObject *object);
void ags_property_listing_editor_show(GtkWidget *widget);

static AgsConnectableInterface* ags_property_listing_editor_parent_connectable_interface;

GType
ags_property_listing_editor_get_type(void)
{
  static GType ags_type_property_listing_editor = 0;

  if(!ags_type_property_listing_editor){
    static const GTypeInfo ags_property_listing_editor_info = {
      sizeof (AgsPropertyListingEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_property_listing_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPropertyListingEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_property_listing_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_property_listing_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_property_listing_editor = g_type_register_static(AGS_TYPE_PROPERTY_EDITOR,
							      "AgsPropertyListingEditor\0",
							      &ags_property_listing_editor_info,
							      0);
    
    g_type_add_interface_static(ags_type_property_listing_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_property_listing_editor);
}

void
ags_property_listing_editor_class_init(AgsPropertyListingEditorClass *property_listing_editor)
{
}

void
ags_property_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_property_listing_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_property_listing_editor_connect;
  connectable->disconnect = ags_property_listing_editor_disconnect;
}

void
ags_property_listing_editor_init(AgsPropertyListingEditor *property_listing_editor)
{
}

void
ags_property_listing_editor_connect(AgsConnectable *connectable)
{
  AgsPropertyListingEditor *property_listing_editor;
  GList *pad_list;

  ags_property_listing_editor_parent_connectable_interface->connect(connectable);

  /* AgsPropertyListingEditor */
  property_listing_editor = AGS_PROPERTY_LISTING_EDITOR(connectable);
}

void
ags_property_listing_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_property_listing_editor_destroy(GtkObject *object)
{
  AgsPropertyListingEditor *property_listing_editor;

  property_listing_editor = (AgsPropertyListingEditor *) object;
}

void
ags_property_listing_editor_show(GtkWidget *widget)
{
  AgsPropertyListingEditor *property_listing_editor = (AgsPropertyListingEditor *) widget;
}

AgsPropertyListingEditor*
ags_property_listing_editor_new()
{
  AgsPropertyListingEditor *property_listing_editor;

  property_listing_editor = (AgsPropertyListingEditor *) g_object_new(AGS_TYPE_PROPERTY_LISTING_EDITOR,
								      NULL);
  
  return(property_listing_editor);
}
