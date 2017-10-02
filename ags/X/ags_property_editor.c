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

#include <ags/X/ags_property_editor.h>
#include <ags/X/ags_property_editor_callbacks.h>

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_property_editor_class_init(AgsPropertyEditorClass *property_editor);
void ags_property_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_property_editor_init(AgsPropertyEditor *property_editor);
void ags_property_editor_connect(AgsConnectable *connectable);
void ags_property_editor_disconnect(AgsConnectable *connectable);
void ags_property_editor_show(GtkWidget *widget);

/**
 * SECTION:ags_property_editor
 * @short_description: Property of audio, channels or pads in bulk mode.
 * @title: AgsPropertyEditor
 * @section_id:
 * @include: ags/X/ags_property_editor.h
 *
 * #AgsPropertyEditor is a composite widget to property. A property editor 
 * should be packed by a #AgsPropertyCollectionEditor.
 */

GType
ags_property_editor_get_type(void)
{
  static GType ags_type_property_editor = 0;

  if(!ags_type_property_editor){
    static const GTypeInfo ags_property_editor_info = {
      sizeof (AgsPropertyEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_property_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsPropertyEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_property_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_property_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_property_editor = g_type_register_static(GTK_TYPE_VBOX,
						      "AgsPropertyEditor", &ags_property_editor_info,
						      0);
    
    g_type_add_interface_static(ags_type_property_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_property_editor);
}

void
ags_property_editor_class_init(AgsPropertyEditorClass *property_editor)
{
}

void
ags_property_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->is_ready = NULL;
  connectable->is_connected = NULL;
  connectable->connect = ags_property_editor_connect;
  connectable->disconnect = ags_property_editor_disconnect;
}

void
ags_property_editor_init(AgsPropertyEditor *property_editor)
{
  property_editor->flags = 0;

  property_editor->enabled = (GtkCheckButton *) gtk_check_button_new_with_label(i18n("enabled"));
  gtk_box_pack_start(GTK_BOX(property_editor),
		     GTK_WIDGET(property_editor->enabled),
		     FALSE, FALSE,
		     0);
}

void
ags_property_editor_connect(AgsConnectable *connectable)
{
  AgsPropertyEditor *property_editor;
  
  /* AgsPropertyEditor */
  property_editor = AGS_PROPERTY_EDITOR(connectable);

  if((AGS_PROPERTY_EDITOR_CONNECTED & (property_editor->flags)) != 0){
    return;
  }

  property_editor->flags |= AGS_PROPERTY_EDITOR_CONNECTED;
  
  g_signal_connect_after(G_OBJECT(property_editor->enabled), "toggled",
			 G_CALLBACK(ags_property_editor_enable_callback), property_editor);
}

void
ags_property_editor_disconnect(AgsConnectable *connectable)
{
  AgsPropertyEditor *property_editor;
  
  /* AgsPropertyEditor */
  property_editor = AGS_PROPERTY_EDITOR(connectable);

  if((AGS_PROPERTY_EDITOR_CONNECTED & (property_editor->flags)) == 0){
    return;
  }

  property_editor->flags &= (~AGS_PROPERTY_EDITOR_CONNECTED);
  
  g_object_disconnect(G_OBJECT(property_editor->enabled),
		      "toggled",
		      G_CALLBACK(ags_property_editor_enable_callback),
		      property_editor,
		      NULL);
}

void
ags_property_editor_show(GtkWidget *widget)
{
  AgsPropertyEditor *property_editor = (AgsPropertyEditor *) widget;
}

/**
 * ags_property_editor_new:
 *
 * Creates an #AgsPropertyEditor
 *
 * Returns: a new #AgsPropertyEditor
 *
 * Since: 1.0.0
 */
AgsPropertyEditor*
ags_property_editor_new()
{
  AgsPropertyEditor *property_editor;

  property_editor = (AgsPropertyEditor *) g_object_new(AGS_TYPE_PROPERTY_EDITOR,
						       NULL);
  
  return(property_editor);
}
