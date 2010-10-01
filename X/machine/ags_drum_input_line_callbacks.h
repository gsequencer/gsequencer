#ifndef __AGS_DRUM_INPUT_LINE_CALLBACKS_H__
#define __AGS_DRUM_INPUT_LINE_CALLBACKS_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

#include "ags_drum_input_line.h"
#include "../../audio/ags_audio.h"

/* AgsAudio */
void ags_drum_input_line_audio_set_pads_callback(AgsAudio *audio, GType type,
						 guint pads, guint pads_old,
						 AgsDrumInputLine *drum_input_line);

/* AgsRecall - recall */
void ags_drum_input_line_copy_pattern_done(AgsRecall *recall,
					   AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_copy_pattern_cancel(AgsRecall *recall,
					     AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_copy_pattern_loop(AgsRecall *recall,
					   AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_recall_volume_done(AgsRecall *recall,
					    AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_recall_volume_cancel(AgsRecall *recall,
					      AgsDrumInputLine *drum_input_line);

/* AgsRecall - play */
void ags_drum_input_line_play_channel_done(AgsRecall *recall,
					   AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_play_channel_cancel(AgsRecall *recall,
					     AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_play_volume_done(AgsRecall *recall,
					  AgsDrumInputLine *drum_input_line);
void ags_drum_input_line_play_volume_cancel(AgsRecall *recall,
					    AgsDrumInputLine *drum_input_line);

#endif /*__AGS_DRUM_INPUT_LINE_CALLBACKS_H__*/
