/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_PANEL_H__
#define __AGS_PANEL_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_PANEL                (ags_panel_get_type())
#define AGS_PANEL(obj)                ((AgsPanel*) G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_PANEL, AgsPanel))
#define AGS_PANEL_CLASS(class)        ((AgsPanelClass*) G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_PANEL, AgsPanelClass))
#define AGS_IS_PANEL(obj)             (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_PANEL))
#define AGS_IS_PANEL_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_PANEL))
#define AGS_PANEL_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_PANEL, AgsPanelClass))

typedef struct _AgsPanel AgsPanel;
typedef struct _AgsPanelClass AgsPanelClass;

struct _AgsPanel
{
  AgsMachine machine;

  gchar *name;
  gchar *xml_type;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;
  
  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  GtkBox *vbox;
};

struct _AgsPanelClass
{
  AgsMachineClass machine;
};

GType ags_panel_get_type(void);

AgsPanel* ags_panel_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_PANEL_H__*/
