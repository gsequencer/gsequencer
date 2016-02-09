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

#ifndef __AGS_SYNTH_OUTPUT_PAD_H__
#define __AGS_SYNTH_OUTPUT_PAD_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/audio/ags_channel.h>

#include <ags/X/ags_effect_pad.h>

#define AGS_TYPE_SYNTH_OUTPUT_PAD                (ags_synth_output_pad_get_type())
#define AGS_SYNTH_OUTPUT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNTH_OUTPUT_PAD, AgsSynthOutputPad))
#define AGS_SYNTH_OUTPUT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNTH_OUTPUT_PAD, AgsSynthOutputPadClass))
#define AGS_IS_SYNTH_OUTPUT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNTH_OUTPUT_PAD))
#define AGS_IS_SYNTH_OUTPUT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNTH_OUTPUT_PAD))
#define AGS_SYNTH_OUTPUT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNTH_OUTPUT_PAD, AgsSynthOutputPadClass))

typedef struct _AgsSynthOutputPad AgsSynthOutputPad;
typedef struct _AgsSynthOutputPadClass AgsSynthOutputPadClass;

struct _AgsSynthOutputPad
{
  AgsEffectPad effect_pad;
};

struct _AgsSynthOutputPadClass
{
  AgsEffectPadClass effect_pad;
};

GType ags_synth_output_pad_get_type(void);

AgsSynthOutputPad* ags_synth_output_pad_new(AgsChannel *channel);

#endif /*__AGS_SYNTH_OUTPUT_PAD_H__*/
