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

#ifndef __AGS_LV2_EVENT_MANAGER_H__
#define __AGS_LV2_EVENT_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/event/event.h>

#define AGS_TYPE_LV2_EVENT_MANAGER                (ags_lv2_event_manager_get_type())
#define AGS_LV2_EVENT_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_EVENT_MANAGER, AgsLv2EventManager))
#define AGS_LV2_EVENT_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_EVENT_MANAGER, AgsLv2EventManagerClass))
#define AGS_IS_LV2_EVENT_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_EVENT_MANAGER))
#define AGS_IS_LV2_EVENT_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_EVENT_MANAGER))
#define AGS_LV2_EVENT_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_EVENT_MANAGER, AgsLv2EventManagerClass))

typedef struct _AgsLv2EventManager AgsLv2EventManager;
typedef struct _AgsLv2EventManagerClass AgsLv2EventManagerClass;

struct _AgsLv2EventManager
{
  GObject gobject;
};

struct _AgsLv2EventManagerClass
{
  GObjectClass gobject;
};

GType ags_lv2_event_manager_get_type(void);

uint32_t ags_lv2_event_manager_lv2_event_ref(LV2_Event_Callback_Data callback_data,
					     LV2_Event *event);
uint32_t ags_lv2_event_manager_lv2_event_unref(LV2_Event_Callback_Data callback_data,
					       LV2_Event *event);

AgsLv2EventManager* ags_lv2_event_manager_get_instance();
AgsLv2EventManager* ags_lv2_event_manager_new();

#endif /*__AGS_LV2_EVENT_MANAGER_H__*/
