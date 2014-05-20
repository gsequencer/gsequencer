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

#ifndef __AGS_SEEK_EDITOR_H__
#define __AGS_SEEK_EDITOR_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/X/ags_editor.h>

#define AGS_TYPE_SEEK_EDITOR                (ags_seek_editor_get_type())
#define AGS_SEEK_EDITOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEEK_EDITOR, AgsSeekEditor))
#define AGS_SEEK_EDITOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SEEK_EDITOR, AgsSeekEditorClass))
#define AGS_IS_SEEK_EDITOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SEEK_EDITOR))
#define AGS_IS_SEEK_EDITOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SEEK_EDITOR))
#define AGS_SEEK_EDITOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SEEK_EDITOR, AgsSeekEditorClass))

typedef struct _AgsSeekEditor AgsSeekEditor;
typedef struct _AgsSeekEditorClass AgsSeekEditorClass;

struct _AgsSeekEditor
{
  AgsTask task;

  AgsEditor *editor;
  gdouble steps;
  gboolean forward;
};

struct _AgsSeekEditorClass
{
  AgsTaskClass task;
};

GType ags_seek_editor_get_type();

AgsSeekEditor* ags_seek_editor_new(AgsEditor *editor,
				   gdouble steps,
				   gboolean forward);

#endif /*__AGS_SEEK_EDITOR_H__*/
