/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2017 Joël Krähemann
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

#ifndef __AGS_SYNCSYNTH_INPUT_PAD_H__
#define __AGS_SYNCSYNTH_INPUT_PAD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/X/ags_pad.h>

#define AGS_TYPE_SYNCSYNTH_INPUT_PAD                (ags_syncsynth_input_pad_get_type())
#define AGS_SYNCSYNTH_INPUT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNCSYNTH_INPUT_PAD, AgsSyncsynthInputPad))
#define AGS_SYNCSYNTH_INPUT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST(class, AGS_TYPE_SYNCSYNTH_INPUT_PAD, AgsSyncsynthInputPadClass))
#define AGS_IS_SYNCSYNTH_INPUT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNCSYNTH_INPUT_PAD))
#define AGS_IS_SYNCSYNTH_INPUT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNCSYNTH_INPUT_PAD))
#define AGS_SYNCSYNTH_INPUT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS(obj, AGS_TYPE_SYNCSYNTH_INPUT_PAD, AgsSyncsynthInputPadClass))

typedef struct _AgsSyncsynthInputPad AgsSyncsynthInputPad;
typedef struct _AgsSyncsynthInputPadClass AgsSyncsynthInputPadClass;

struct _AgsSyncsynthInputPad
{
  AgsPad pad;

  gchar *name;
  gchar *xml_type;
};

struct _AgsSyncsynthInputPadClass
{
  AgsPadClass pad;
};

GType ags_syncsynth_input_pad_get_type();

AgsSyncsynthInputPad* ags_syncsynth_input_pad_new(AgsChannel *channel);

#endif /*__AGS_SYNCSYNTH_INPUT_PAD_H__*/
