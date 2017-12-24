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

#ifndef __AGS_NOTIFY_SOUNDCARD_H__
#define __AGS_NOTIFY_SOUNDCARD_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#define AGS_TYPE_NOTIFY_SOUNDCARD                (ags_notify_soundcard_get_type())
#define AGS_NOTIFY_SOUNDCARD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_NOTIFY_SOUNDCARD, AgsNotifySoundcard))
#define AGS_NOTIFY_SOUNDCARD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_NOTIFY_SOUNDCARD, AgsNotifySoundcardClass))
#define AGS_IS_NOTIFY_SOUNDCARD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_NOTIFY_SOUNDCARD))
#define AGS_IS_NOTIFY_SOUNDCARD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_NOTIFY_SOUNDCARD))
#define AGS_NOTIFY_SOUNDCARD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_NOTIFY_SOUNDCARD, AgsNotifySoundcardClass))

typedef struct _AgsNotifySoundcard AgsNotifySoundcard;
typedef struct _AgsNotifySoundcardClass AgsNotifySoundcardClass;

/**
 * AgsNotifySoundcardFlags:
 * @AGS_NOTIFY_SOUNDCARD_WAIT_RETURN: sync wait
 * @AGS_NOTIFY_SOUNDCARD_DONE_RETURN: sync done
 * 
 * Enum values to control the behavior or indicate internal state of #AgsNotifySoundcard by
 * enable/disable as flags.
 */
typedef enum{
  AGS_NOTIFY_SOUNDCARD_WAIT_RETURN   = 1,
  AGS_NOTIFY_SOUNDCARD_DONE_RETURN   = 1 <<  1,
}AgsNotifySoundcardFlags;

struct _AgsNotifySoundcard
{
  AgsTask task;

  volatile guint flags;
  
  pthread_mutexattr_t *return_mutexattr;
  pthread_mutex_t *return_mutex;
  pthread_cond_t *return_cond;

  GObject *soundcard_thread;
};

struct _AgsNotifySoundcardClass
{
  AgsTaskClass task;
};

GType ags_notify_soundcard_get_type();

AgsNotifySoundcard* ags_notify_soundcard_new(GObject *soundcard_thread);

#endif /*__AGS_NOTIFYo_SOUNDCARD_H__*/

