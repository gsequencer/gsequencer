/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#ifndef __AGS_MACHINE_COUNTER_H__
#define __AGS_MACHINE_COUNTER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_COUNTER                (ags_machine_counter_get_type())
#define AGS_MACHINE_COUNTER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_COUNTER, AgsMachineCounter))
#define AGS_MACHINE_COUNTER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_COUNTER, AgsMachineCounterClass))
#define AGS_IS_MACHINE_COUNTER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_COUNTER))
#define AGS_IS_MACHINE_COUNTER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_COUNTER))
#define AGS_MACHINE_COUNTER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), AGS_TYPE_MACHINE_COUNTER, AgsMachineCounterClass))

typedef struct _AgsMachineCounter AgsMachineCounter;
typedef struct _AgsMachineCounterClass AgsMachineCounterClass;

struct _AgsMachineCounter
{
  GObject gobject;

  gchar *version;
  gchar *build_id;

  GType machine_type;

  gchar *filename;
  gchar *effect;
  
  guint counter;
};

struct _AgsMachineCounterClass
{
  GObjectClass gobject;
};

GType ags_machine_counter_get_type(void);

AgsMachineCounter* ags_machine_counter_new(GType machine_type,
					   gchar *filename, gchar *effect);

G_END_DECLS

#endif /*__AGS_MACHINE_COUNTER_H__*/
