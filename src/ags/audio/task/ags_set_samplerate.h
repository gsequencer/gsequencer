/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#ifndef __AGS_SET_SAMPLERATE_H__
#define __AGS_SET_SAMPLERATE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#define AGS_TYPE_SET_SAMPLERATE                (ags_set_samplerate_get_type())
#define AGS_SET_SAMPLERATE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SET_SAMPLERATE, AgsSetSamplerate))
#define AGS_SET_SAMPLERATE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SET_SAMPLERATE, AgsSetSamplerateClass))
#define AGS_IS_SET_SAMPLERATE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SET_SAMPLERATE))
#define AGS_IS_SET_SAMPLERATE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SET_SAMPLERATE))
#define AGS_SET_SAMPLERATE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SET_SAMPLERATE, AgsSetSamplerateClass))

typedef struct _AgsSetSamplerate AgsSetSamplerate;
typedef struct _AgsSetSamplerateClass AgsSetSamplerateClass;

struct _AgsSetSamplerate
{
  AgsTask task;

  GObject *gobject;
  guint samplerate;
};

struct _AgsSetSamplerateClass
{
  AgsTaskClass task;
};

GType ags_set_samplerate_get_type();

AgsSetSamplerate* ags_set_samplerate_new(GObject *gobject,
					 guint samplerate);

#endif /*__AGS_SET_SAMPLERATE_H__*/
