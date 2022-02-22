/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_FFPLAYER_INPUT_PAD_H__
#define __AGS_FFPLAYER_INPUT_PAD_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_effect_pad.h>

G_BEGIN_DECLS

#define AGS_TYPE_FFPLAYER_INPUT_PAD                (ags_ffplayer_input_pad_get_type())
#define AGS_FFPLAYER_INPUT_PAD(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER_INPUT_PAD, AgsFFPlayerInputPad))
#define AGS_FFPLAYER_INPUT_PAD_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER_INPUT_PAD, AgsFFPlayerInputPadClass))
#define AGS_IS_FFPLAYER_INPUT_PAD(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER_INPUT_PAD))
#define AGS_IS_FFPLAYER_INPUT_PAD_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_FFPLAYER_INPUT_PAD))
#define AGS_FFPLAYER_INPUT_PAD_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_FFPLAYER_INPUT_PAD, AgsFFPlayerInputPadClass))

typedef struct _AgsFFPlayerInputPad AgsFFPlayerInputPad;
typedef struct _AgsFFPlayerInputPadClass AgsFFPlayerInputPadClass;

struct _AgsFFPlayerInputPad
{
  AgsEffectPad effect_pad;
};

struct _AgsFFPlayerInputPadClass
{
  AgsEffectPadClass effect_pad;
};

GType ags_ffplayer_input_pad_get_type(void);

AgsFFPlayerInputPad* ags_ffplayer_input_pad_new(AgsChannel *channel);

G_END_DECLS

#endif /*__AGS_FFPLAYER_INPUT_PAD_H__*/
