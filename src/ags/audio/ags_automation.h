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

#ifndef __AGS_AUTOMATION_H__
#define __AGS_AUTOMATION_H__

#include <glib.h>
#include <glib-object.h>

#define AGS_TYPE_AUTOMATION                (ags_automation_get_type())
#define AGS_AUTOMATION(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_AUTOMATION, AgsAutomation))
#define AGS_AUTOMATION_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_AUTOMATION, AgsAutomationClass))
#define AGS_IS_AUTOMATION(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_AUTOMATION))
#define AGS_IS_AUTOMATION_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_AUTOMATION))
#define AGS_AUTOMATION_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_AUTOMATION, AgsAutomationClass))

typedef struct _AgsAutomation AgsAutomation;
typedef struct _AgsAutomationClass AgsAutomationClass;

struct _AgsAutomation
{
  GObject gobject;

  guint flags;
};

struct _AgsAutomationClass
{
  GObjectClass gobject;
};

GType ags_automation_get_type(void);

AgsAutomation* ags_automation_new();

#endif /*__AGS_AUTOMATION_H__*/
