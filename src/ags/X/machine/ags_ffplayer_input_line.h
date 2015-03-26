/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_FFPLAYER_INPUT_LINE_H__
#define __AGS_FFPLAYER_INPUT_LINE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_effect_line.h>

#define AGS_TYPE_FFPLAYER_INPUT_LINE                (ags_ffplayer_input_line_get_type())
#define AGS_FFPLAYER_INPUT_LINE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER_INPUT_LINE, AgsFFPlayerInputLine))
#define AGS_FFPLAYER_INPUT_LINE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER_INPUT_LINE, AgsFFPlayerInputLineClass))
#define AGS_IS_FFPLAYER_INPUT_LINE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER_INPUT_LINE))
#define AGS_IS_FFPLAYER_INPUT_LINE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FFPLAYER_INPUT_LINE))
#define AGS_FFPLAYER_INPUT_LINE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FFPLAYER_INPUT_LINE, AgsFFPlayerInputLineClass))

typedef struct _AgsFFPlayerInputLine AgsFFPlayerInputLine;
typedef struct _AgsFFPlayerInputLineClass AgsFFPlayerInputLineClass;

struct _AgsFFPlayerInputLine
{
  AgsEffectLine effect_line;
};

struct _AgsFFPlayerInputLineClass
{
  AgsEffectLineClass effect_line;
};

GType ags_ffplayer_input_line_get_type(void);

AgsFFPlayerInputLine* ags_ffplayer_input_line_new(AgsChannel *channel);

#endif /*__AGS_FFPLAYER_INPUT_LINE_H__*/
