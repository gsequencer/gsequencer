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

#ifndef __AGS_PLAYBACK_DOMAIN_H__
#define __AGS_PLAYBACK_DOMAIN_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_PLAYBACK_DOMAIN                (ags_playback_domain_get_type())
#define AGS_PLAYBACK_DOMAIN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomain))
#define AGS_PLAYBACK_DOMAIN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomain))
#define AGS_IS_PLAYBACK_DOMAIN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_PLAYBACK_DOMAIN))
#define AGS_IS_PLAYBACK_DOMAIN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_PLAYBACK_DOMAIN))
#define AGS_PLAYBACK_DOMAIN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_PLAYBACK_DOMAIN, AgsPlaybackDomainClass))

typedef struct _AgsPlaybackDomain AgsPlaybackDomain;
typedef struct _AgsPlaybackDomainClass AgsPlaybackDomainClass;

struct _AgsPlaybackDomain
{
  GObject gobject;

  GObject *domain;
  
  gboolean playback;
  gboolean sequencer;
  gboolean notation;

  GList *devout_play;
};

struct _AgsPlaybackDomainClass
{
  GObjectClass gobject;
};

GType ags_playback_domain_get_type();

AgsPlaybackDomain* ags_playback_domain_new(GObject *application_context);

#endif /*__AGS_PLAYBACK_DOMAIN_H__*/
