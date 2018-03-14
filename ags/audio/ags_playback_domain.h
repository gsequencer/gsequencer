/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_PLAYBACK_DOMAIN_H__
#define __AGS_PLAYBACK_DOMAIN_H__

#include <glib.h>
#include <glib-object.h>

#include <pthread.h>

#include <ags/libags.h>

#include <ags/audio/ags_sound_enums.h>

#define AGS_TYPE_PLAYBACK_DOMAIN                (ags_playback_domain_get_type())
#define AGS_PLAYBACK_DOMAIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomain))
#define AGS_PLAYBACK_DOMAIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomain))
#define AGS_IS_PLAYBACK_DOMAIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAYBACK_DOMAIN))
#define AGS_IS_PLAYBACK_DOMAIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAYBACK_DOMAIN))
#define AGS_PLAYBACK_DOMAIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomainClass))

typedef struct _AgsPlaybackDomain AgsPlaybackDomain;
typedef struct _AgsPlaybackDomainClass AgsPlaybackDomainClass;

/**
 * AgsPlaybackDomainFlags:
 * @AGS_PLAYBACK_DOMAIN_CONNECTED: indicates the playback domain was connected by calling #AgsConnectable::connect()
 * @AGS_PLAYBACK_DOMAIN_SINGLE_THREADED: single threaded
 * @AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO: super threaded audio
 * 
 * Enum values to control the behavior or indicate internal state of #AgsPlaybackDomain by
 * enable/disable as flags.
 */
typedef enum{
  AGS_PLAYBACK_DOMAIN_CONNECTED                  = 1,
  AGS_PLAYBACK_DOMAIN_SINGLE_THREADED            = 1 <<  1,
  AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO       = 1 <<  2,
}AgsPlaybackDomainFlags;

struct _AgsPlaybackDomain
{
  GObject gobject;

  volatile guint flags;
  
  pthread_mutex_t *obj_mutex;
  pthread_mutexattr_t *obj_mutexattr;

  GObject *audio;

  AgsThread **audio_thread;

  GList *output_playback;
  GList *input_playback;
};

struct _AgsPlaybackDomainClass
{
  GObjectClass gobject;
};

GType ags_playback_domain_get_type();

pthread_mutex_t* ags_playback_domain_get_class_mutex();

/* get and set */
void ags_playback_domain_set_audio_thread(AgsPlaybackDomain *playback_domain,
					  AgsThread *thread,
					  gint sound_scope);
AgsThread* ags_playback_domain_get_audio_thread(AgsPlaybackDomain *playback_domain,
						gint sound_scope);

/* add and remove */
void ags_playback_domain_add_playback(AgsPlaybackDomain *playback_domain,
				      GObject *playback, GType channel_type);
void ags_playback_domain_remove_playback(AgsPlaybackDomain *playback_domain,
					 GObject *playback, GType channel_type);

/* instance */
AgsPlaybackDomain* ags_playback_domain_new(GObject *audio);

#endif /*__AGS_PLAYBACK_DOMAIN_H__*/
