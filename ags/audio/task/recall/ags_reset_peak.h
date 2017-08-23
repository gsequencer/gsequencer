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

#ifndef __AGS_RESET_PEAK_H__
#define __AGS_RESET_PEAK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>

#include <ags/audio/recall/ags_peak_channel.h>

#define AGS_TYPE_RESET_PEAK                (ags_reset_peak_get_type())
#define AGS_RESET_PEAK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RESET_PEAK, AgsResetPeak))
#define AGS_RESET_PEAK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RESET_PEAK, AgsResetPeakClass))
#define AGS_IS_RESET_PEAK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RESET_PEAK))
#define AGS_IS_RESET_PEAK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RESET_PEAK))
#define AGS_RESET_PEAK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RESET_PEAK, AgsResetPeakClass))

typedef struct _AgsResetPeak AgsResetPeak;
typedef struct _AgsResetPeakClass AgsResetPeakClass;

struct _AgsResetPeak
{
  AgsTask task;

  GList *peak_channel;
};

struct _AgsResetPeakClass
{
  AgsTaskClass task;
};

GType ags_reset_peak_get_type();

void ags_reset_peak_add(AgsResetPeak *reset_peak,
			AgsPeakChannel *peak_channel);
void ags_reset_peak_remove(AgsResetPeak *reset_peak,
			   AgsPeakChannel *peak_channel);

AgsResetPeak* ags_reset_peak_get_instance();

AgsResetPeak* ags_reset_peak_new();

#endif /*__AGS_RESET_PEAK_H__*/
