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

#ifndef __AGS_LV2_LOG_MANAGER_H__
#define __AGS_LV2_LOG_MANAGER_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>
#include <lv2/lv2plug.in/ns/ext/log/log.h>

#define AGS_TYPE_LV2_LOG_MANAGER                (ags_lv2_log_manager_get_type())
#define AGS_LV2_LOG_MANAGER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_LV2_LOG_MANAGER, AgsLv2LogManager))
#define AGS_LV2_LOG_MANAGER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_LV2_LOG_MANAGER, AgsLv2LogManagerClass))
#define AGS_IS_LV2_LOG_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_LV2_LOG_MANAGER))
#define AGS_IS_LV2_LOG_MANAGER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_LV2_LOG_MANAGER))
#define AGS_LV2_LOG_MANAGER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_LV2_LOG_MANAGER, AgsLv2LogManagerClass))

typedef struct _AgsLv2LogManager AgsLv2LogManager;
typedef struct _AgsLv2LogManagerClass AgsLv2LogManagerClass;

struct _AgsLv2LogManager
{
  GObject gobject;
};

struct _AgsLv2LogManagerClass
{
  GObjectClass gobject;
};

GType ags_lv2_log_manager_get_type(void);

int ags_lv2_log_manager_printf(LV2_Log_Handle handle,
			       LV2_URID type,
			       const char* fmt,
			       ...);
int ags_lv2_log_manager_vprintf(LV2_Log_Handle handle,
				LV2_URID type,
				const char* fmt,
				va_list ap);

AgsLv2LogManager* ags_lv2_log_manager_get_instance();
AgsLv2LogManager* ags_lv2_log_manager_new();

#endif /*__AGS_LV2_LOG_MANAGER_H__*/
