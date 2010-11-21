#ifndef __AGS_DRUM_OUTPUT_PAD_CALLBACKS_H__
#define __AGS_DRUM_OUTPUT_PAD_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include <ags/X/machine/ags_drum_output_pad.h>

void ags_drum_output_pad_play_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrumOutputPad *drum_output_pad);
void ags_drum_output_pad_play_cancel(AgsRecall *recall, AgsRecallID *recall_id, AgsDrumOutputPad *drum_output_pad);

#endif /*__AGS_DRUM_OUTPUT_PAD_CALLBACKS_H__*/
