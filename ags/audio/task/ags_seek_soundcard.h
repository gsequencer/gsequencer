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

#ifndef __AGS_SEEK_SOUNDCARD_H__
#define __AGS_SEEK_SOUNDCARD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/object/ags_soundcard.h>

#define AGS_TYPE_SEEK_SOUNDCARD                (ags_seek_soundcard_get_type())
#define AGS_SEEK_SOUNDCARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SEEK_SOUNDCARD, AgsSeekSoundcard))
#define AGS_SEEK_SOUNDCARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SEEK_SOUNDCARD, AgsSeekSoundcardClass))
#define AGS_IS_SEEK_SOUNDCARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SEEK_SOUNDCARD))
#define AGS_IS_SEEK_SOUNDCARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SEEK_SOUNDCARD))
#define AGS_SEEK_SOUNDCARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SEEK_SOUNDCARD, AgsSeekSoundcardClass))

typedef struct _AgsSeekSoundcard AgsSeekSoundcard;
typedef struct _AgsSeekSoundcardClass AgsSeekSoundcardClass;

struct _AgsSeekSoundcard
{
  AgsTask task;

  GObject *soundcard;
  guint steps;
  gboolean move_forward;
};

struct _AgsSeekSoundcardClass
{
  AgsTaskClass task;
};

GType ags_seek_soundcard_get_type();

AgsSeekSoundcard* ags_seek_soundcard_new(GObject *soundcard,
					 guint steps,
					 gboolean move_forward);

#endif /*__AGS_SEEK_SOUNDCARD_H__*/
