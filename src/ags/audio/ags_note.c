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

#include <ags/audio/ags_note.h>

#include <ags/object/ags_connectable.h>

#include <stdlib.h>

void ags_note_class_init(AgsNoteClass *note);
void ags_note_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_note_init(AgsNote *note);
void ags_note_connect(AgsConnectable *connectable);
void ags_note_disconnect(AgsConnectable *connectable);
void ags_note_finalize(GObject *object);

static gpointer ags_note_parent_class = NULL;

GType
ags_note_get_type()
{
  static GType ags_type_note = 0;

  if(!ags_type_note){
    static const GTypeInfo ags_note_info = {
      sizeof(AgsNoteClass),
      NULL,
      NULL,
      (GClassInitFunc) ags_note_class_init,
      NULL,
      NULL,
      sizeof(AgsNote),
      0,
      (GInstanceInitFunc) ags_note_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_note_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_note = g_type_register_static(G_TYPE_OBJECT,
					   "AgsNote\0",
					   &ags_note_info,
					   0);
    
    g_type_add_interface_static(ags_type_note,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }

  return(ags_type_note);
}

void 
ags_note_class_init(AgsNoteClass *note)
{
  GObjectClass *gobject;

  ags_note_parent_class = g_type_class_peek_parent(note);

  gobject = (GObjectClass *) note;

  gobject->finalize = ags_note_finalize;
}

void
ags_note_connectable_interface_init(AgsConnectableInterface *connectable)
{
  connectable->connect = ags_note_connect;
  connectable->disconnect = ags_note_disconnect;
}

void
ags_note_init(AgsNote *note)
{
  note->flags = 0;

  note->x[0] = 0;
  note->x[1] = 0;
  note->y = 0;
}

void
ags_note_connect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_note_disconnect(AgsConnectable *connectable)
{
  /* empty */
}

void
ags_note_finalize(GObject *gobject)
{
  /* empty */

  G_OBJECT_CLASS(ags_note_parent_class)->finalize(gobject);
}

AgsNote*
ags_note_duplicate(AgsNote *note)
{
  AgsNote *copy;

  copy = ags_note_new();

  copy->flags = 0;

  copy->x[0] = note->x[0];
  copy->x[1] = note->x[1];
  copy->y = note->y;

  return(copy);
}

AgsNote*
ags_note_new()
{
  AgsNote *note;

  note = (AgsNote *) g_object_new(AGS_TYPE_NOTE, NULL);

  return(note);
}
