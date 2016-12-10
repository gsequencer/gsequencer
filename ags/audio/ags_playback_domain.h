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

#ifndef __AGS_PLAYBACK_DOMAIN_H__
#define __AGS_PLAYBACK_DOMAIN_H__

#include <glib.h>
#include <glib-object.h>

#ifdef AGS_USE_LINUX_THREADS
#include <ags/thread/ags_thread-kthreads.h>
#else
#include <ags/thread/ags_thread-posix.h>
#endif 

#define AGS_TYPE_PLAYBACK_DOMAIN                (ags_playback_domain_get_type())
#define AGS_PLAYBACK_DOMAIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomain))
#define AGS_PLAYBACK_DOMAIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomain))
#define AGS_IS_PLAYBACK_DOMAIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAYBACK_DOMAIN))
#define AGS_IS_PLAYBACK_DOMAIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAYBACK_DOMAIN))
#define AGS_PLAYBACK_DOMAIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomainClass))

typedef struct _AgsPlaybackDomain AgsPlaybackDomain;
typedef struct _AgsPlaybackDomainClass AgsPlaybackDomainClass;

typedef enum{
  AGS_PLAYBACK_DOMAIN_PLAYBACK                   = 1,
  AGS_PLAYBACK_DOMAIN_SEQUENCER                  = 1 <<  1,
  AGS_PLAYBACK_DOMAIN_NOTATION                   = 1 <<  2,
  AGS_PLAYBACK_DOMAIN_SINGLE_THREADED            = 1 <<  3,
  AGS_PLAYBACK_DOMAIN_SUPER_THREADED_AUDIO       = 1 <<  4,
}AgsPlaybackDomainFlags;

struct _AgsPlaybackDomain
{
  GObject gobject;

  volatile guint flags;

  AgsThread **audio_thread;

  GObject *domain;
  GList *playback;
};

struct _AgsPlaybackDomainClass
{
  GObjectClass gobject;
};

GType ags_playback_domain_get_type();

AgsPlaybackDomain* ags_playback_domain_new();

#endif /*__AGS_PLAYBACK_DOMAIN_H__*/
