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

#ifndef __AGS_RECALL_LV2_RUN_H__
#define __AGS_RECALL_LV2_RUN_H__

#include <glib.h>
#include <glib-object.h>

#include <lv2.h>

#include <ags/audio/ags_recall.h>
#include <ags/audio/ags_recall_audio_signal.h>

#define AGS_TYPE_RECALL_LV2_RUN                (ags_recall_lv2_run_get_type())
#define AGS_RECALL_LV2_RUN(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_RECALL_LV2_RUN, AgsRecallLv2Run))
#define AGS_RECALL_LV2_RUN_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_RECALL_LV2_RUN, AgsRecallLv2RunClass))
#define AGS_IS_RECALL_LV2_RUN(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_RECALL_LV2_RUN))
#define AGS_IS_RECALL_LV2_RUN_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_RECALL_LV2_RUN))
#define AGS_RECALL_LV2_RUN_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_RECALL_LV2_RUN, AgsRecallLv2RunClass))

typedef struct _AgsRecallLv2Run AgsRecallLv2Run;
typedef struct _AgsRecallLv2RunClass AgsRecallLv2RunClass;

struct _AgsRecallLv2Run
{
  AgsRecallAudioSignal recall_audio_signal;

  LV2_Handle *lv2_handle;

  float *input;
  float *output;
};

struct _AgsRecallLv2RunClass
{
  AgsRecallAudioSignalClass recall_audio_signal;
};

GType ags_recall_lv2_run_get_type();

AgsRecallLv2Run* ags_recall_lv2_run_new(AgsAudioSignal *audio_signal);

#endif /*__AGS_RECALL_LV2_RUN_H__*/
