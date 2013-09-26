/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#ifndef __AGS_DISPLAY_TACT_H__
#define __AGS_DISPLAY_TACT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/audio/ags_devout.h>

#define AGS_TYPE_DISPLAY_TACT                (ags_display_tact_get_type())
#define AGS_DISPLAY_TACT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DISPLAY_TACT, AgsDisplayTact))
#define AGS_DISPLAY_TACT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DISPLAY_TACT, AgsDisplayTactClass))
#define AGS_IS_DISPLAY_TACT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DISPLAY_TACT))
#define AGS_IS_DISPLAY_TACT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DISPLAY_TACT))
#define AGS_DISPLAY_TACT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DISPLAY_TACT, AgsDisplayTactClass))

typedef struct _AgsDisplayTact AgsDisplayTact;
typedef struct _AgsDisplayTactClass AgsDisplayTactClass;

struct _AgsDisplayTact
{
  AgsTask task;
};

struct _AgsDisplayTactClass
{
  AgsTaskClass task;
};

GType ags_display_tact_get_type();

AgsDisplayTact* ags_display_tact_new();

#endif /*__AGS_DISPLAY_TACT_H__*/
