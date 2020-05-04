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

#ifndef __AGS_RESET_FX_PEAK_H__
#define __AGS_RESET_FX_PEAK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/fx/ags_fx_peak_channel.h>

G_BEGIN_DECLS

#define AGS_TYPE_RESET_FX_PEAK                (ags_reset_fx_peak_get_type())
#define AGS_RESET_FX_PEAK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESET_FX_PEAK, AgsResetFxPeak))
#define AGS_RESET_FX_PEAK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESET_FX_PEAK, AgsResetFxPeakClass))
#define AGS_IS_RESET_FX_PEAK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESET_FX_PEAK))
#define AGS_IS_RESET_FX_PEAK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESET_FX_PEAK))
#define AGS_RESET_FX_PEAK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESET_FX_PEAK, AgsResetFxPeakClass))

typedef struct _AgsResetFxPeak AgsResetFxPeak;
typedef struct _AgsResetFxPeakClass AgsResetFxPeakClass;

struct _AgsResetFxPeak
{
  AgsTask task;

  GList *fx_peak_channel;
};

struct _AgsResetFxPeakClass
{
  AgsTaskClass task;
};

GType ags_reset_fx_peak_get_type();

void ags_reset_fx_peak_add(AgsResetFxPeak *reset_fx_peak,
			   AgsFxPeakChannel *fx_peak_channel);
void ags_reset_fx_peak_remove(AgsResetFxPeak *reset_fx_peak,
			      AgsFxPeakChannel *fx_peak_channel);

AgsResetFxPeak* ags_reset_fx_peak_get_instance();

AgsResetFxPeak* ags_reset_fx_peak_new();

G_END_DECLS

#endif /*__AGS_RESET_FX_PEAK_H__*/
