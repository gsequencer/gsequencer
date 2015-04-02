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

#ifndef __AGS_CHANGE_INDICATOR_H__
#define __AGS_CHANGE_INDICATOR_H__

#include <glib-object.h>

#include <ags/audio/ags_task.h>

#include <ags/widget/ags_indicator.h>

#define AGS_TYPE_CHANGE_INDICATOR                (ags_change_indicator_get_type())
#define AGS_CHANGE_INDICATOR(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_CHANGE_INDICATOR, AgsChangeIndicator))
#define AGS_CHANGE_INDICATOR_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_CHANGE_INDICATOR, AgsChangeIndicatorClass))
#define AGS_IS_CHANGE_INDICATOR(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_CHANGE_INDICATOR))
#define AGS_IS_CHANGE_INDICATOR_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_CHANGE_INDICATOR))
#define AGS_CHANGE_INDICATOR_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_CHANGE_INDICATOR, AgsChangeIndicatorClass))

typedef struct _AgsChangeIndicator AgsChangeIndicator;
typedef struct _AgsChangeIndicatorClass AgsChangeIndicatorClass;

struct _AgsChangeIndicator
{
  AgsTask task;

  AgsIndicator *indicator;
  gdouble value;
};

struct _AgsChangeIndicatorClass
{
  AgsTaskClass task;
};

GType ags_change_indicator_get_type();

AgsChangeIndicator* ags_change_indicator_new(AgsIndicator *indicator,
					     gdouble value);

#endif /*__AGS_CHANGE_INDICATOR_H__*/
