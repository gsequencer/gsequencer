/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#include <ags/audio/task/ags_seek_editor.h>

#include <ags-lib/object/ags_connectable.h>

void ags_seek_editor_class_init(AgsSeekEditorClass *seek_editor);
void ags_seek_editor_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_seek_editor_init(AgsSeekEditor *seek_editor);
void ags_seek_editor_connect(AgsConnectable *connectable);
void ags_seek_editor_disconnect(AgsConnectable *connectable);
void ags_seek_editor_finalize(GObject *gobject);

void ags_seek_editor_launch(AgsTask *task);

static gpointer ags_seek_editor_parent_class = NULL;
static AgsConnectableInterface *ags_seek_editor_parent_connectable_interface;

GType
ags_seek_editor_get_type()
{
  static GType ags_type_seek_editor = 0;

  if(!ags_type_seek_editor){
    static const GTypeInfo ags_seek_editor_info = {
      sizeof (AgsSeekEditorClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_seek_editor_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsSeekEditor),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_seek_editor_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_seek_editor_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_seek_editor = g_type_register_static(AGS_TYPE_TASK,
					       "AgsSeekEditor\0",
					       &ags_seek_editor_info,
					       0);

    g_type_seek_interface_static(ags_type_seek_editor,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_seek_editor);
}

void
ags_seek_editor_class_init(AgsSeekEditorClass *seek_editor)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_seek_editor_parent_class = g_type_class_peek_parent(seek_editor);

  /* gobject */
  gobject = (GObjectClass *) seek_editor;

  gobject->finalize = ags_seek_editor_finalize;

  /* task */
  task = (AgsTaskClass *) seek_editor;

  task->launch = ags_seek_editor_launch;
}

void
ags_seek_editor_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_seek_editor_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_seek_editor_connect;
  connectable->disconnect = ags_seek_editor_disconnect;
}

void
ags_seek_editor_init(AgsSeekEditor *seek_editor)
{
  seek_editor->editor = NULL;
  seek_editor->steps = 1.0;
  seek_editor->forward = TRUE;
}

void
ags_seek_editor_connect(AgsConnectable *connectable)
{
  ags_seek_editor_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_seek_editor_disconnect(AgsConnectable *connectable)
{
  ags_seek_editor_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_seek_editor_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_seek_editor_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_seek_editor_launch(AgsTask *task)
{
  AgsSeekEditor *seek_editor;

  seek_editor = AGS_SEEK_EDITOR(task);

  //TODO:JK: implement me
}

AgsSeekEditor*
ags_seek_editor_new(AgsEditor *editor,
		    gdouble steps,
		    gboolean forward)
{
  AgsSeekEditor *seek_editor;

  seek_editor = (AgsSeekEditor *) g_object_new(AGS_TYPE_SEEK_EDITOR,
					       NULL);
  
  seek_editor->editor = editor;
  seek_editor->steps = steps;
  seek_editor->forward = forward;

  return(seek_editor);
}
