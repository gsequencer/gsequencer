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

#ifndef __AGS_SYNTH_INPUT_LINE_H__
#define __AGS_SYNTH_INPUT_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/ags_line.h>

#include <ags/X/machine/ags_oscillator.h>

#define AGS_TYPE_SYNTH_INPUT_LINE                (ags_synth_input_line_get_type())
#define AGS_SYNTH_INPUT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH_INPUT_LINE, AgsSynthInputLine))
#define AGS_SYNTH_INPUT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SYNTH_INPUT_LINE, AgsSynthInputLineClass))
#define AGS_IS_SYNTH_INPUT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH_INPUT_LINE))
#define AGS_IS_SYNTH_INPUT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH_INPUT_LINE))
#define AGS_SYNTH_INPUT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SYNTH_INPUT_LINE, AgsSynthInputLineClass))

typedef struct _AgsSynthInputLine AgsSynthInputLine;
typedef struct _AgsSynthInputLineClass AgsSynthInputLineClass;

struct _AgsSynthInputLine
{
  AgsLine line;

  gchar *name;
  gchar *xml_type;

  AgsOscillator *oscillator;
};

struct _AgsSynthInputLineClass
{
  AgsLineClass line;
};

GType ags_synth_input_line_get_type();

AgsSynthInputLine* ags_synth_input_line_new(AgsChannel *channel);

#endif /*__AGS_SYNTH_INPUT_LINE_H__*/

