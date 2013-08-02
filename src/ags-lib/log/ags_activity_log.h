/* AGS Lib - Advanced GTK Sequencer Lib
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

#ifndef __AGS_ACTIVITY_LOG_H__
#define __AGS_ACTIVITY_LOG_H__

#include <glib.h>
#include <glib-object.h>

#include <ags-cluster/cluster/ags_cluster.h>

#include <stdio.h>

#define AGS_TYPE_ACTIVITY_LOG                (ags_activity_log_get_type())
#define AGS_ACTIVITY_LOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ACTIVITY_LOG, AgsActivityLog))
#define AGS_ACTIVITY_LOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_ACTIVITY_LOG, AgsActivityLogClass))
#define AGS_IS_ACTIVITY_LOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_ACTIVITY_LOG))
#define AGS_IS_ACTIVITY_LOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_ACTIVITY_LOG))
#define AGS_ACTIVITY_LOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_ACTIVITY_LOG, AgsActivityLogClass))

typedef struct _AgsActivityLog AgsActivityLog;
typedef struct _AgsActivityLogClass AgsActivityLogClass;

struct _AgsActivityLog
{
  GObject object;

  GList *history;
};

struct _AgsActivityLogClass
{
  GObjectClass object;
};

GType ags_activity_log_get_type();

AgsActivityLog* ags_activity_log_new();

#endif /*__AGS_ACTIVITY_LOG_H__*/
