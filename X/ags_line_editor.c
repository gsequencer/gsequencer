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

#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_line_editor_callbacks.h>

#include <ags/object/ags_connectable.h>
#include <ags/object/ags_applicable.h>

void ags_line_editor_class_init(AgsLineEditorClass *line_editor);
void ags_line_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_line_editor_applicable_interface_init(AgsApplicableInterface *applicable);
void ags_line_editor_init(AgsLineEditor *line_editor);
void ags_line_editor_set_property(GObject *gobject,
				  guint prop_id,
				  const GValue *value,
				  GParamSpec *param_spec);
void ags_line_editor_get_property(GObject *gobject,
				  guint prop_id,
				  GValue *value,
				  GParamSpec *param_spec);
void ags_line_editor_connect(AgsConnectable *connectable);
void ags_line_editor_disconnect(AgsConnectable *connectable);
void ags_line_editor_set_update(AgsApplicable *applicable, gboolean update);
void ags_line_editor_apply(AgsApplicable *applicable);
void ags_line_editor_reset(AgsApplicable *applicable);
void ags_line_editor_destroy(GtkObject *object);
void ags_line_editor_show(GtkWidget *widget);

enum{
  PROP_0,
  PROP_CHANNEL,
};

GType
ags_line_editor_get_type(void)
{
  static GType ags_type_line_editor = 0;

  if(!ags_type_line_editor){
    static const GTypeInfo ags_line_editor_info = {
      sizeof (AgsLineEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_line_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsLineEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_line_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_line_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    static const GInterfaceInfo ags_applicable_interface_info = {
      (GInterfaceInitFunc) ags_line_editor_applicable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_line_editor = g_type_register_static(GTK_TYPE_VBOX,
						  "AgsLineEditor\0", &ags_line_editor_info,
						  0);

    g_type_add_interface_static(ags_type_line_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);

    g_type_add_interface_static(ags_type_line_editor,
				AGS_TYPE_APPLICABLE,
				&ags_applicable_interface_info);
  }
  
  return(ags_type_line_editor);
}

void
ags_line_editor_class_init(AgsLineEditorClass *line_editor)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  gobject = (GObjectClass *) line_editor;

  gobject->set_property = ags_line_editor_set_property;
  gobject->get_property = ags_line_editor_get_property;

  param_spec = g_param_spec_object("channel\0",
				   "assigned channel\0",
				   "The channel which this pad editor is assigned with\0",
				   AGS_TYPE_CHANNEL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_CHANNEL,
				  param_spec);
}

void
ags_line_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_line_editor_connect;
  connectable->disconnect = ags_line_editor_disconnect;
}

void
ags_line_editor_applicable_interface_init(AgsApplicableInterface *applicable)
{
  applicable->set_update = ags_line_editor_set_update;
  applicable->apply = ags_line_editor_apply;
  applicable->reset = ags_line_editor_reset;
}

void
ags_line_editor_init(AgsLineEditor *line_editor)
{
  line_editor->link_editor = NULL;
}

void
ags_line_editor_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    {
      AgsChannel *channel;

      channel = (AgsChannel *) g_value_get_object(value);

      ags_line_editor_set_channel(line_editor, channel);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_editor_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(gobject);

  switch(prop_id){
  case PROP_CHANNEL:
    g_value_set_object(value, line_editor->channel);
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_line_editor_connect(AgsConnectable *connectable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(connectable);

  g_signal_connect((GObject *) line_editor, "destroy\0",
		   G_CALLBACK(ags_line_editor_destroy_callback), (gpointer) line_editor);

  g_signal_connect((GObject *) line_editor, "show\0",
  		   G_CALLBACK(ags_line_editor_show_callback), (gpointer) line_editor);

  ags_connectable_connect(AGS_CONNECTABLE(line_editor->link_editor));
}

void
ags_line_editor_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_line_editor_set_update(AgsApplicable *applicable, gboolean update)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(applicable);

  if(line_editor->link_editor != NULL){
    ags_applicable_set_update(AGS_APPLICABLE(line_editor->link_editor), update);
  }
}

void
ags_line_editor_apply(AgsApplicable *applicable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(applicable);

  if(line_editor->link_editor != NULL){
    ags_applicable_apply(AGS_APPLICABLE(line_editor->link_editor));
  }
}

void
ags_line_editor_reset(AgsApplicable *applicable)
{
  AgsLineEditor *line_editor;

  line_editor = AGS_LINE_EDITOR(applicable);

  if(line_editor->link_editor != NULL){
    ags_applicable_reset(AGS_APPLICABLE(line_editor->link_editor));
  }
}

void
ags_line_editor_destroy(GtkObject *object)
{
}

void
ags_line_editor_show(GtkWidget *widget)
{
  AgsLineEditor *line_editor = (AgsLineEditor *) widget;

  gtk_widget_show((GtkWidget *) line_editor->link_editor);
}

void
ags_line_editor_set_channel(AgsLineEditor *line_editor,
			    AgsChannel *channel)
{
  AgsLinkEditor *link_editor;

  if(line_editor->link_editor != NULL){
    link_editor = line_editor->link_editor;
    line_editor->link_editor = NULL;
    gtk_widget_destroy(GTK_WIDGET(line_editor->link_editor));
  }

  line_editor->channel = channel;

  if(channel != NULL){
    line_editor->link_editor = ags_link_editor_new();
    gtk_box_pack_start(GTK_BOX(line_editor),
		       GTK_WIDGET(line_editor->link_editor),
		       FALSE, FALSE,
		       0);
  }
}

AgsLineEditor*
ags_line_editor_new(AgsChannel *channel)
{
  AgsLineEditor *line_editor;

  line_editor = (AgsLineEditor *) g_object_new(AGS_TYPE_LINE_EDITOR,
					       "channel\0", channel,
					       NULL);

  return(line_editor);
}
