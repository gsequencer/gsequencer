#include <ags/X/editor/ags_sf2_chooser_callbacks.h>

#include <ags/object/ags_playable.h>
#include <ags/audio/file/ags_ipatch_sf2_reader.h>

void
ags_sf2_chooser_preset_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser)
{
  AgsPlayable *playable;
  gchar *preset;
  GError *error;
  guint nth_level;

  /* Ipatch related */
  playable = AGS_PLAYABLE(sf2_chooser->ipatch);
  nth_level = ags_playable_nth_level(playable);

  sf2_chooser->ipatch->nth_level = 1;

  preset = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));

  error = NULL;
  ags_playable_level_select(playable,
			    1, preset,
			    &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }
}

void
ags_sf2_chooser_instrument_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser)
{
  AgsPlayable *playable;
  gchar *instrument;
  GError *error;
  guint nth_level;

  /* Ipatch related */
  playable = AGS_PLAYABLE(sf2_chooser->ipatch);
  nth_level = ags_playable_nth_level(playable);

  sf2_chooser->ipatch->nth_level = 2;

  instrument = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));

  error = NULL;
  ags_playable_level_select(playable,
			    2, instrument,
			    &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }
}

void
ags_sf2_chooser_sample_changed(GtkComboBox *combo_box, AgsSF2Chooser *sf2_chooser)
{
  AgsPlayable *playable;
  gchar *sample;
  GError *error;
  guint nth_level;

  /* Ipatch related */
  playable = AGS_PLAYABLE(sf2_chooser->ipatch);
  nth_level = ags_playable_nth_level(playable);

  sf2_chooser->ipatch->nth_level = 3;

  sample = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo_box));

  error = NULL;
  ags_playable_level_select(playable,
			    3, sample,
			    &error);

  if(error != NULL){
    g_error("%s\0", error->message);
  }
}
