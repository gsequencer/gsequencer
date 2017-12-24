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

#ifndef __AGS_REMOVE_SOUNDCARD_H__
#define __AGS_REMOVE_SOUNDCARD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/object/ags_application_context.h>

#define AGS_TYPE_REMOVE_SOUNDCARD                (ags_remove_soundcard_get_type())
#define AGS_REMOVE_SOUNDCARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_REMOVE_SOUNDCARD, AgsRemoveSoundcard))
#define AGS_REMOVE_SOUNDCARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_REMOVE_SOUNDCARD, AgsRemoveSoundcardClass))
#define AGS_IS_REMOVE_SOUNDCARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_REMOVE_SOUNDCARD))
#define AGS_IS_REMOVE_SOUNDCARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_REMOVE_SOUNDCARD))
#define AGS_REMOVE_SOUNDCARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_REMOVE_SOUNDCARD, AgsRemoveSoundcardClass))

typedef struct _AgsRemoveSoundcard AgsRemoveSoundcard;
typedef struct _AgsRemoveSoundcardClass AgsRemoveSoundcardClass;

struct _AgsRemoveSoundcard
{
  AgsTask task;

  AgsApplicationContext *application_context;

  GObject *soundcard;
};

struct _AgsRemoveSoundcardClass
{
  AgsTaskClass task;
};

GType ags_remove_soundcard_get_type();

AgsRemoveSoundcard* ags_remove_soundcard_new(AgsApplicationContext *application_context,
					     GObject *soundcard);

#endif /*__AGS_REMOVE_SOUNDCARD_H__*/
