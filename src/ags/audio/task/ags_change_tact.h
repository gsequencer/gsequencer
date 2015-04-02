/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2014 Joël Krähemann
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

#ifndef __AGS_CHANGE_TACT_H__
#define __AGS_CHANGE_TACT_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/audio/ags_task.h>
#include <ags/X/ags_navigation.h>

#define AGS_TYPE_CHANGE_TACT                (ags_change_tact_get_type())
#define AGS_CHANGE_TACT(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANGE_TACT, AgsChangeTact))
#define AGS_CHANGE_TACT_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANGE_TACT, AgsChangeTactClass))
#define AGS_IS_CHANGE_TACT(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANGE_TACT))
#define AGS_IS_CHANGE_TACT_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CHANGE_TACT))
#define AGS_CHANGE_TACT_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANGE_TACT, AgsChangeTactClass))

typedef struct _AgsChangeTact AgsChangeTact;
typedef struct _AgsChangeTactClass AgsChangeTactClass;

struct _AgsChangeTact
{
  AgsTask task;

  AgsNavigation *navigation;
};

struct _AgsChangeTactClass
{
  AgsTaskClass task;
};

GType ags_change_tact_get_type();

AgsChangeTact* ags_change_tact_new(AgsNavigation *navigation);

#endif /*__AGS_CHANGE_TACT_H__*/
