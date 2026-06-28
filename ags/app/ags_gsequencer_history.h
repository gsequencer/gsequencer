/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2026 Joël Krähemann
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

#ifndef __AGS_GSEQUENCER_HISTORY_H__
#define __AGS_GSEQUENCER_HISTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSEQUENCER_HISTORY                (ags_gsequencer_history_get_type())
#define AGS_GSEQUENCER_HISTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_GSEQUENCER_HISTORY, AgsGsequencerHistory))
#define AGS_GSEQUENCER_HISTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_GSEQUENCER_HISTORY, AgsGsequencerHistoryClass))
#define AGS_IS_GSEQUENCER_HISTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_GSEQUENCER_HISTORY))
#define AGS_IS_GSEQUENCER_HISTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_GSEQUENCER_HISTORY))
#define AGS_GSEQUENCER_HISTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_GSEQUENCER_HISTORY, AgsGsequencerHistoryClass))

typedef struct _AgsGsequencerHistory AgsGsequencerHistory;
typedef struct _AgsGsequencerHistoryClass AgsGsequencerHistoryClass;

struct _AgsGsequencerHistory
{
  GObject gobject;

  gint insert_action_position;
  
  GList *action_journal;
};

struct _AgsGsequencerHistoryClass
{
  GObjectClass gobject;
};

GType ags_gsequencer_history_get_type(void);

void ags_gsequencer_history_add_action(AgsGSequencerHistory *history,
				       AgsGSequencerAction *action);

/* undo/redo */
void ags_gsequencer_history_undo(AgsGSequencerHistory *history,
				 AgsGSequencerAction *action);

void ags_gsequencer_history_redo(AgsGSequencerHistory *history,
				 AgsGSequencerAction *action);

/* instance */
AgsGsequencerHistory* ags_gsequencer_history_get_instance();

AgsGsequencerHistory* ags_gsequencer_history_new();

G_END_DECLS

#endif /*__AGS_GSEQUENCER_HISTORY_H__*/
