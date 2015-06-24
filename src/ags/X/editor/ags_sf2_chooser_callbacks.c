/* This file is part of GSequencer.
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
/* This file is part of GSequencer.
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
