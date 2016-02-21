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

#ifndef __AGS_MIXER_H__
#define __AGS_MIXER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_machine.h>

#define AGS_TYPE_MIXER                (ags_mixer_get_type())
#define AGS_MIXER(obj)                (G_TYPE_CHECK_INSTANCE_CAST(obj, AGS_TYPE_MIXER, AgsMixer))
#define AGS_MIXER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_MIXER, AgsMixerClass))
#define AGS_IS_MIXER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE(obj, AGS_TYPE_MIXER))
#define AGS_IS_MIXER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_MIXER))
#define AGS_MIXER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_MIXER, AgsMixerClass))

typedef struct _AgsMixer AgsMixer;
typedef struct _AgsMixerClass AgsMixerClass;

struct _AgsMixer
{
  AgsMachine machine;

  gchar *name;
  gchar *xml_type;

  GtkHBox *input_pad;
};

struct _AgsMixerClass
{
  AgsMachineClass machine;
};

GType ags_mixer_get_type(void);

AgsMixer* ags_mixer_new(GObject *soundcard);

#endif /*__AGS_MIXER_H__*/
