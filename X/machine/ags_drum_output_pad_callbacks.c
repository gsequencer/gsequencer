#include "ags_drum_output_pad_callbacks.h"

#include "ags_drum.h"

void
ags_drum_output_pad_play_done(AgsRecall *recall, AgsRecallID *recall_id, AgsDrumOutputPad *drum_output_pad)
{
  AgsDrum *drum;

  drum = (AgsDrum *) gtk_widget_get_ancestor((GtkWidget *) drum_output_pad, AGS_TYPE_DRUM);

  /*
  drum->play_ref++;

  if(drum->play_ref == drum->machine.audio->output_lines){
    drum->play_ref = 0;
    drum->machine.audio->devout_play->flags |= AGS_DEVOUT_PLAY_DONE;
    gtk_toggle_button_set_active(drum->run, FALSE);
  }
  */
}

void
ags_drum_output_pad_play_cancel(AgsRecall *recall, AgsRecallID *recall_id, AgsDrumOutputPad *drum_output_pad)
{
}
