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

#ifndef __AGS_CHANGE_BPM_H__
#define __AGS_CHANGE_BPM_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_CHANGE_BPM                (ags_change_bpm_get_type())
#define AGS_CHANGE_BPM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANGE_BPM, AgsChangeBpm))
#define AGS_CHANGE_BPM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANGE_BPM, AgsChangeBpmClass))
#define AGS_IS_CHANGE_BPM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANGE_BPM))
#define AGS_IS_CHANGE_BPM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CHANGE_BPM))
#define AGS_CHANGE_BPM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANGE_BPM, AgsChangeBpmClass))

typedef struct _AgsChangeBpm AgsChangeBpm;
typedef struct _AgsChangeBpmClass AgsChangeBpmClass;

struct _AgsChangeBpm
{
  AgsTask task;

  AgsDevout *devout;

  gdouble bpm;
};

struct _AgsChangeBpmClass
{
  AgsTaskClass task;
};

GType ags_change_bpm_get_type();

AgsChangeBpm* ags_change_bpm_new(AgsDevout *devout,
				 gdouble bpm);

#endif /*__AGS_CHANGE_BPM_H__*/
