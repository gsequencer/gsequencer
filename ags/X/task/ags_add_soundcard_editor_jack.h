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

#ifndef __AGS_ADD_SOUNDCARD_EDITOR_JACK_H__
#define __AGS_ADD_SOUNDCARD_EDITOR_JACK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/X/ags_soundcard_editor.h>

#define AGS_TYPE_ADD_SOUNDCARD_EDITOR_JACK                (ags_add_soundcard_editor_jack_get_type())
#define AGS_ADD_SOUNDCARD_EDITOR_JACK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_SOUNDCARD_EDITOR_JACK, AgsAddSoundcardEditorJack))
#define AGS_ADD_SOUNDCARD_EDITOR_JACK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_SOUNDCARD_EDITOR_JACK, AgsAddSoundcardEditorJackClass))
#define AGS_IS_ADD_SOUNDCARD_EDITOR_JACK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_SOUNDCARD_EDITOR_JACK))
#define AGS_IS_ADD_SOUNDCARD_EDITOR_JACK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_SOUNDCARD_EDITOR_JACK))
#define AGS_ADD_SOUNDCARD_EDITOR_JACK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_SOUNDCARD_EDITOR_JACK, AgsAddSoundcardEditorJackClass))

typedef struct _AgsAddSoundcardEditorJack AgsAddSoundcardEditorJack;
typedef struct _AgsAddSoundcardEditorJackClass AgsAddSoundcardEditorJackClass;

struct _AgsAddSoundcardEditorJack
{
  AgsTask task;

  AgsSoundcardEditor *soundcard_editor;
};

struct _AgsAddSoundcardEditorJackClass
{
  AgsTaskClass task;
};

GType ags_add_soundcard_editor_jack_get_type();

AgsAddSoundcardEditorJack* ags_add_soundcard_editor_jack_new(AgsSoundcardEditor *soundcard_editor);

#endif /*__AGS_ADD_SOUNDCARD_EDITOR_JACK_H__*/
