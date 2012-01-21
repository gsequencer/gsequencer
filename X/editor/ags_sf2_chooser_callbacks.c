#include <ags/X/editor/ags_sf2_chooser_callbacks.h>

#include <ags/object/ags_playable.h>

void
ags_sf2_chooser_preset_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser)
{
  AgsPlayable *playable;
  gchar *preset;
  gchar **instrument;
  GError *error;

  playable = AGS_PLAYABLE(sf2_chooser->ipatch->reader);

  preset = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));

  error = NULL;
  ags_playable_level_select(playable,
			    0, preset,
			    &error);

  if(error != NULL){
    g_error(error->message);
  }

  instrument = ags_playable_sublevel_names(playable);

  while(*instrument != NULL){
    gtk_combo_box_text_append_text(sf2_chooser->instrument,
				   *instrument);


    instrument++;
  }
}

void
ags_sf2_chooser_instrument_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser)
{
  AgsPlayable *playable;
  gchar *instrument;
  gchar **sample;
  GError *error;

  playable = AGS_PLAYABLE(sf2_chooser->ipatch->reader);

  instrument = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));

  error = NULL;
  ags_playable_level_select(playable,
			    0, instrument,
			    &error);

  if(error != NULL){
    g_error(error->message);
  }

  sample = ags_playable_sublevel_names(playable);

  while(*sample != NULL){
    gtk_combo_box_text_append_text(sf2_chooser->sample,
				   *sample);


    sample++;
  }
}

void
ags_sf2_chooser_sample_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser)
{
  /* perhaps empty */
}
