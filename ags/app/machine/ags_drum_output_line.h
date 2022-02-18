/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2019 Joël Krähemann
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

#ifndef __AGS_DRUM_OUTPUT_LINE_H__
#define __AGS_DRUM_OUTPUT_LINE_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_line.h>

G_BEGIN_DECLS

#define AGS_TYPE_DRUM_OUTPUT_LINE                (ags_drum_output_line_get_type())
#define AGS_DRUM_OUTPUT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM_OUTPUT_LINE, AgsDrumOutputLine))
#define AGS_DRUM_OUTPUT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_DRUM_OUTPUT_LINE, AgsDrumOutputLineClass))
#define AGS_IS_DRUM_OUTPUT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM_OUTPUT_LINE))
#define AGS_IS_DRUM_OUTPUT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM_OUTPUT_LINE))
#define AGS_DRUM_OUTPUT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_DRUM_OUTPUT_LINE, AgsDrumOutputLineClass))

typedef struct _AgsDrumOutputLine AgsDrumOutputLine;
typedef struct _AgsDrumOutputLineClass AgsDrumOutputLineClass;

struct _AgsDrumOutputLine
{
  AgsLine line;

  gchar *name;
  gchar *xml_type;
};

struct _AgsDrumOutputLineClass
{
  AgsLineClass line;
};

GType ags_drum_output_line_get_type();

AgsDrumOutputLine* ags_drum_output_line_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_DRUM_OUTPUT_LINE_H__*/
