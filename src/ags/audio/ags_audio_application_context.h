/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_AUDIO_APPLICATION_CONTEXT_H__
#define __AGS_AUDIO_APPLICATION_CONTEXT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/object/ags_application_context.h>
#include <ags/object/ags_soundcard.h>

#define AGS_TYPE_AUDIO_APPLICATION_CONTEXT                (ags_audio_application_context_get_type())
#define AGS_AUDIO_APPLICATION_CONTEXT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContext))
#define AGS_AUDIO_APPLICATION_CONTEXT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContextClass))
#define AGS_IS_AUDIO_APPLICATION_CONTEXT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUDIO_APPLICATION_CONTEXT))
#define AGS_IS_AUDIO_APPLICATION_CONTEXT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUDIO_APPLICATION_CONTEXT))
#define AGS_AUDIO_APPLICATION_CONTEXT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_AUDIO_APPLICATION_CONTEXT, AgsAudioApplicationContextClass))

#define AGS_AUDIO_RT_PRIORITY (49)

#define AGS_AUDIO_BUILD_ID "Thu Apr  2 13:04:21 GMT 2015\0"
#define AGS_AUDIO_DEFAULT_VERSION "0.4.3\0"

#define AGS_EFFECTS_DEFAULT_VERSION "0.4.2\0"

typedef struct _AgsAudioApplicationContext AgsAudioApplicationContext;
typedef struct _AgsAudioApplicationContextClass AgsAudioApplicationContextClass;

typedef enum{
  AGS_AUDIO_APPLICATION_CONTEXT_USE_ALSA      = 1,
}AgsAudioApplicationContextFlags;

struct _AgsAudioApplicationContext
{
  AgsApplicationContext appliacation_context;

  guint flags;

  gchar *version;
  gchar *build_id;

  GList *soundcard;
};

struct _AgsAudioApplicationContextClass
{
  AgsApplicationContextClass appliacation_context;
};

GType ags_audio_application_context_get_type();

AgsAudioApplicationContext* ags_audio_application_context_new();

#endif /*__AGS_AUDIO_APPLICATION_CONTEXT_H__*/
