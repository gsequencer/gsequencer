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

#ifndef __AGS_MACHINE_MAPPER_H__
#define __AGS_MACHINE_MAPPER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_MACHINE_MAPPER                (ags_machine_mapper_get_type())
#define AGS_MACHINE_MAPPER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_MACHINE_MAPPER, AgsMachineMapper))
#define AGS_MACHINE_MAPPER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_MACHINE_MAPPER, AgsMachineMapperClass))
#define AGS_IS_MACHINE_MAPPER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_MACHINE_MAPPER))
#define AGS_IS_MACHINE_MAPPER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_MACHINE_MAPPER))
#define AGS_MACHINE_MAPPER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MACHINE_MAPPER, AgsMachineMapperClass))

typedef struct _AgsMachineMapper AgsMachineMapper;
typedef struct _AgsMachineMapperClass AgsMachineMapperClass;

typedef enum{
  AGS_MACHINE_MAPPER_CONNECTED    = 1,
}AgsMachineMapperFlags;

struct _AgsMachineMapper
{
  GtkGrid grid;

  guint flags;
  
  GtkWidget *machine;

  GtkCheckButton *enabled;

  GtkLabel *label;

  GtkEntry *instrument;
  GtkEntry *sequence;
};

struct _AgsMachineMapperClass
{
  GtkGridClass grid;
};

GType ags_machine_mapper_get_type(void);

AgsMachineMapper* ags_machine_mapper_new(GtkWidget *machine);

G_END_DECLS

#endif /*__AGS_MACHINE_MAPPER_H__*/
