/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2013 Joël Krähemann
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

#include <ags/X/ags_audio_preferences_callbacks.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_preferences.h>

void
ags_audio_preferences_card_changed_callback(GtkComboBox *combo,
					    AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							      AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);

  g_object_set(G_OBJECT(devout),
	       "device\0", gtk_combo_box_get_active_text(audio_preferences->card),
	       NULL);

  ags_audio_preferences_reset(audio_preferences);
}

void
ags_audio_preferences_audio_channels_changed(GtkSpinButton *spin_button,
					     AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							      AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);
  
  g_object_set(G_OBJECT(devout),
	       "pcm_channels\0", (guint) gtk_spin_button_get_value(spin_button),
	       NULL);
}

void
ags_audio_preferences_samplerate_changed(GtkSpinButton *spin_button,
					 AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
							      AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);
  
  g_object_set(G_OBJECT(devout),
	       "frequency\0", (guint) gtk_spin_button_get_value(spin_button),
	       NULL);
}

void
ags_audio_preferences_buffer_size_changed(GtkSpinButton *spin_button,
					  AgsAudioPreferences *audio_preferences)
{
  AgsWindow *window;
  AgsDevout *devout;

  window = AGS_WINDOW(AGS_PREFERENCES(gtk_widget_get_ancestor(GTK_WIDGET(audio_preferences),
									 AGS_TYPE_PREFERENCES))->window);
  devout = AGS_DEVOUT(window->devout);

  g_object_set(G_OBJECT(devout),
	       "buffer_size\0", (guint) gtk_spin_button_get_value(spin_button),
	       NULL);
}
