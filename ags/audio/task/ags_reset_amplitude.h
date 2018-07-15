/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#ifndef __AGS_RESET_AMPLITUDE_H__
#define __AGS_RESET_AMPLITUDE_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/libags.h>

#include <ags/audio/recall/ags_analyse_channel.h>

#define AGS_TYPE_RESET_AMPLITUDE                (ags_reset_amplitude_get_type())
#define AGS_RESET_AMPLITUDE(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESET_AMPLITUDE, AgsResetAmplitude))
#define AGS_RESET_AMPLITUDE_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESET_AMPLITUDE, AgsResetAmplitudeClass))
#define AGS_IS_RESET_AMPLITUDE(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESET_AMPLITUDE))
#define AGS_IS_RESET_AMPLITUDE_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESET_AMPLITUDE))
#define AGS_RESET_AMPLITUDE_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESET_AMPLITUDE, AgsResetAmplitudeClass))

typedef struct _AgsResetAmplitude AgsResetAmplitude;
typedef struct _AgsResetAmplitudeClass AgsResetAmplitudeClass;

struct _AgsResetAmplitude
{
  AgsTask task;

  GList *analyse_channel;
};

struct _AgsResetAmplitudeClass
{
  AgsTaskClass task;
};

GType ags_reset_amplitude_get_type();

void ags_reset_amplitude_add(AgsResetAmplitude *reset_amplitude,
			     AgsAnalyseChannel *analyse_channel);
void ags_reset_amplitude_remove(AgsResetAmplitude *reset_amplitude,
				AgsAnalyseChannel *analyse_channel);

AgsResetAmplitude* ags_reset_amplitude_get_instance();

AgsResetAmplitude* ags_reset_amplitude_new();

#endif /*__AGS_RESET_AMPLITUDE_H__*/
