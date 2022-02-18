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

#include <ags/app/ags_property_listing_editor.h>

#include <ags/i18n.h>

void ags_property_listing_editor_class_init(AgsPropertyListingEditorClass *property_listing_editor);
void ags_property_listing_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_property_listing_editor_init(AgsPropertyListingEditor *property_listing_editor);

void ags_property_listing_editor_connect(AgsConnectable *connectable);
void ags_property_listing_editor_disconnect(AgsConnectable *connectable);

/**
 * SECTION:ags_property_listing_editor
 * @short_description: Pack child widgets using #GtkVBox
 * @title: AgsPropertyListingEditor
 * @section_id:
 * @include: ags/X/ags_property_listing_editor.h
 *
 * #AgsPropertyListingEditor is a composite widget to pack using a #GtkVBox.
 */

static AgsConnectableInterface* ags_property_listing_editor_parent_connectable_interface;

GType
ags_property_listing_editor_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_property_listing_editor = 0;

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
							      "AgsPropertyListingEditor",
							      &ags_property_listing_editor_info,
							      0);
    
    g_type_add_interface_static(ags_type_property_listing_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_property_listing_editor);
  }

  return g_define_type_id__volatile;
}

void
ags_property_listing_editor_class_init(AgsPropertyListingEditorClass *property_listing_editor)
{
  /* empty */
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
  /* empty */
}

void
ags_property_listing_editor_connect(AgsConnectable *connectable)
{
  ags_property_listing_editor_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_property_listing_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */

  ags_property_listing_editor_parent_connectable_interface->disconnect(connectable);
}

/**
 * ags_property_listing_editor_new:
 * 
 * Create a new instance of #AgsPropertyListingEditor
 * 
 * Returns: the new #AgsPropertyListingEditor
 * 
 * Since: 3.0.0
 */
AgsPropertyListingEditor*
ags_property_listing_editor_new()
{
  AgsPropertyListingEditor *property_listing_editor;

  property_listing_editor = (AgsPropertyListingEditor *) g_object_new(AGS_TYPE_PROPERTY_LISTING_EDITOR,
								      NULL);
  
  return(property_listing_editor);
}
