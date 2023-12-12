/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2023 Joël Krähemann
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

#ifndef __AGS_SET_BACKLOG_H__
#define __AGS_SET_BACKLOG_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/ags_audio_signal.h>

G_BEGIN_DECLS

#define AGS_TYPE_SET_BACKLOG                (ags_set_backlog_get_type())
#define AGS_SET_BACKLOG(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SET_BACKLOG, AgsSetBacklog))
#define AGS_SET_BACKLOG_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SET_BACKLOG, AgsSetBacklogClass))
#define AGS_IS_SET_BACKLOG(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SET_BACKLOG))
#define AGS_IS_SET_BACKLOG_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SET_BACKLOG))
#define AGS_SET_BACKLOG_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SET_BACKLOG, AgsSetBacklogClass))

typedef struct _AgsSetBacklog AgsSetBacklog;
typedef struct _AgsSetBacklogClass AgsSetBacklogClass;

struct _AgsSetBacklog
{
  AgsTask task;

  GList *scope;
  
  gboolean has_backlog;
};

struct _AgsSetBacklogClass
{
  AgsTaskClass task;
};

GType ags_set_backlog_get_type();

void ags_set_backlog_add(AgsSetBacklog *set_backlog,
			 AgsAudioSignal *scope);

void ags_set_backlog_remove(AgsSetBacklog *set_backlog,
			    AgsAudioSignal *scope);

AgsSetBacklog* ags_set_backlog_get_instance();

AgsSetBacklog* ags_set_backlog_new();

G_END_DECLS

#endif /*__AGS_SET_BACKLOG_H__*/
