/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_ADD_SOUNDCARD_EDITOR_SINK_H__
#define __AGS_ADD_SOUNDCARD_EDITOR_SINK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/X/ags_soundcard_editor.h>

#define AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK                (ags_add_soundcard_editor_sink_get_type())
#define AGS_ADD_SOUNDCARD_EDITOR_SINK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK, AgsAddSoundcardEditorSink))
#define AGS_ADD_SOUNDCARD_EDITOR_SINK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK, AgsAddSoundcardEditorSinkClass))
#define AGS_IS_ADD_SOUNDCARD_EDITOR_SINK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK))
#define AGS_IS_ADD_SOUNDCARD_EDITOR_SINK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK))
#define AGS_ADD_SOUNDCARD_EDITOR_SINK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_SOUNDCARD_EDITOR_SINK, AgsAddSoundcardEditorSinkClass))

typedef struct _AgsAddSoundcardEditorSink AgsAddSoundcardEditorSink;
typedef struct _AgsAddSoundcardEditorSinkClass AgsAddSoundcardEditorSinkClass;

struct _AgsAddSoundcardEditorSink
{
  AgsTask task;

  AgsSoundcardEditor *soundcard_editor;
};

struct _AgsAddSoundcardEditorSinkClass
{
  AgsTaskClass task;
};

GType ags_add_soundcard_editor_sink_get_type();

AgsAddSoundcardEditorSink* ags_add_soundcard_editor_sink_new(AgsSoundcardEditor *soundcard_editor);

#endif /*__AGS_ADD_SOUNDCARD_EDITOR_SINK_H__*/
