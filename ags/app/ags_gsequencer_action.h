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

#ifndef __AGS_GSEQUENCER_ACTION__
#define __AGS_GSEQUENCER_ACTION__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_gsequencer_actions.h>
#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_GSEQUENCER_ACTION                (ags_gsequencer_action_get_type())
#define AGS_GSEQUENCER_ACTION(ptr) ((AgsGsequencerAction *)(ptr))

typedef struct _AgsGsequencerAction AgsGsequencerAction;

struct _AgsGsequencerAction
{
  gchar *action_detail;

  AgsMachine *selected_machine;

  GtkWidget *action_widget;

  guint action_param_count;
  
  gchar **action_param_strv;
  GValue *action_param;
};

GType ags_gsequencer_action_get_type(void);

AgsGsequencerAction* ags_gsequencer_action_alloc();

gpointer ags_gsequencer_action_copy(AgsGsequencerAction *ptr);
void ags_gsequencer_action_free(AgsGsequencerAction *ptr);

G_END_DECLS

#endif /*__AGS_GSEQUENCER_ACTION__*/
