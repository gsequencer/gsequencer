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

#ifndef __AGS_FUNCTIONAL_TEST_UTIL_H__
#define __AGS_FUNCTIONAL_TEST_UTIL_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_OUTPUT_TAB (0)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_INPUT_TAB (1)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_OUTPUT_TAB (2)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_BULK_INPUT_TAB (3)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_PROPERTIES_RESIZE_TAB (4)

void ags_functional_test_util_setup_and_launch();
void ags_functional_test_util_setup_and_launch_filename(gchar *filename);
void ags_functional_test_util_idle();

GtkMenu* ags_functional_test_util_submenu_find(GtkMenu *menu,
					       gchar *item_label);
gboolean ags_functional_test_util_menu_bar_click(gchar *item_label);
gboolean ags_functional_test_util_menu_click(GtkMenu *menu,
					     gchar *item_label);

gboolean ags_functional_test_util_button_click(GtkButton *button);

/* generic dialog */
gboolean ags_functional_test_util_dialog_apply(GtkDialog *dialog);
gboolean ags_functional_test_util_dialog_ok(GtkDialog *dialog);
gboolean ags_functional_test_util_dialog_cancel(GtkDialog *dialog);

#define ags_functional_test_util_dialog_close ags_functional_test_util_dialog_cancel

/* file chooser */
gboolean ags_functional_test_util_file_chooser_open_path(GtkFileChooser *file_chooser,
							 gchar *path);
gboolean ags_functional_test_util_file_chooser_select_filename(GtkFileChooser *file_chooser,
							       gchar *filename);
gboolean ags_functional_test_util_file_chooser_select_filenames(GtkFileChooser *file_chooser,
								GSList *filename);
gboolean ags_functional_test_util_file_chooser_select_all(GtkFileChooser *file_chooser);

/* file */
gboolean ags_functional_test_util_open();

gboolean ags_functional_test_util_save();
gboolean ags_functional_test_util_save_as();

gboolean ags_functional_test_util_export_open();
gboolean ags_functional_test_util_export_close();
gboolean ags_funcitonal_test_util_export_add();
gboolean ags_funcitonal_test_util_export_tact(gdouble tact);

gboolean ags_funcitonal_test_util_export_remove(guint nth);
gboolean ags_funcitonal_test_util_export_set_backend(guint nth,
						     gchar *backend);
gboolean ags_funcitonal_test_util_export_set_device(guint nth,
						    gchar *device);
gboolean ags_funcitonal_test_util_export_set_filename(guint nth,
						      gchar *filename);
gboolean ags_funcitonal_test_util_export_open(guint nth);
gboolean ags_funcitonal_test_util_export_set_format(guint nth,
						    gchar *format);

gboolean ags_funcitonal_test_util_export_do_export();

gboolean ags_functional_test_util_quit();

/* edit */
gboolean ags_functional_test_util_add_machine(gchar *submenu,
					      gchar *machine_name);

gboolean ags_functional_test_util_automation_open();
gboolean ags_functional_test_util_automation_close();

gboolean ags_functional_test_util_preferences_open();
gboolean ags_functional_test_util_preferences_close();

/* help */
gboolean ags_functional_test_util_about_open();
gboolean ags_functional_test_util_about_close();

/* navigation */
gboolean ags_functional_test_util_navigation_expand();

gboolean ags_functional_test_util_navigation_bpm(gdouble bpm);

gboolean ags_functional_test_util_navigation_rewind();
gboolean ags_functional_test_util_navigation_prev();
gboolean ags_functional_test_util_navigation_play();
gboolean ags_functional_test_util_navigation_stop();
gboolean ags_functional_test_util_navigation_next();
gboolean ags_functional_test_util_navigation_forward();

gboolean ags_functional_test_util_navigation_loop();

gboolean ags_functional_test_util_navigation_position(gdouble position);

gboolean ags_functional_test_util_navigation_loop_left(gdouble loop_left);
gboolean ags_functional_test_util_navigation_loop_right(gdouble loop_right);

gboolean ags_functional_test_util_navigation_exclude_sequencers();

/* toolbar */
gboolean ags_functional_test_util_toolbar_cursor_click();
gboolean ags_functional_test_util_toolbar_edit_click();
gboolean ags_functional_test_util_toolbar_delete_click();
gboolean ags_functional_test_util_toolbar_select_click();
gboolean ags_functional_test_util_toolbar_invert_click();

gboolean ags_functional_test_util_toolbar_paste_click();
gboolean ags_functional_test_util_toolbar_copy_click();
gboolean ags_functional_test_util_toolbar_cut_click();

gboolean ags_functional_test_util_toolbar_zoom(guint nth_zoom);

/* machine selector */
gboolean ags_functional_test_util_machine_selector_select(guint nth_machine);

/* machine selection */
gboolean ags_functional_test_util_machine_selection_select(guint nth_machine);

gboolean ags_functional_test_util_machine_selection_remove_index();
gboolean ags_functional_test_util_machine_selection_add_index();
gboolean ags_functional_test_util_machine_selection_link_index();
gboolean ags_functional_test_util_machine_selection_reverse_mapping();
gboolean ags_functional_test_util_machine_selection_shift_piano(guint nth_shift);

/* pattern edit */
gboolean ags_functional_test_util_pattern_edit_delete_point(guint nth_index,
							    guint x,
							    guint y);
gboolean ags_functional_test_util_pattern_edit_add_point(guint nth_index,
							 guint x,
							 guint y);
gboolean ags_functional_test_util_pattern_edit_select_region(guint nth_index,
							     guint x0, guint x1,
							     guint y0, guint y1);

/* note edit */
gboolean ags_functional_test_util_note_edit_delete_point(guint nth_index,
							 guint x,
							 guint y);
gboolean ags_functional_test_util_note_edit_add_point(guint nth_index,
						      guint x0, guint x1,
						      guint y);
gboolean ags_functional_test_util_note_edit_select_region(guint nth_index,
							  guint x0, guint x1,
							  guint y0, guint y1);

/* automation toolbar */
gboolean ags_functional_test_util_automation_toolbar_cursor_click();
gboolean ags_functional_test_util_automation_toolbar_edit_click();
gboolean ags_functional_test_util_automation_toolbar_delete_click();
gboolean ags_functional_test_util_automation_toolbar_select_click();

gboolean ags_functional_test_util_automation_toolbar_paste_click();
gboolean ags_functional_test_util_automation_toolbar_copy_click();
gboolean ags_functional_test_util_automation_toolbar_cut_click();

gboolean ags_functional_test_util_automation_toolbar_zoom(guint nth_zoom);

/* automation edit */
gboolean ags_functional_test_util_automation_edit_audio_click();
gboolean ags_functional_test_util_automation_edit_output_click();
gboolean ags_functional_test_util_automation_edit_input_click();

/* automation area */
gboolean ags_functional_test_util_automation_edit_delete_point(guint nth_index,
							       guint x,
							       guint y);
gboolean ags_functional_test_util_automation_edit_add_point(guint nth_index,
							    guint x,
							    guint y);
gboolean ags_functional_test_util_automation_edit_select_region(guint nth_index,
								guint x0, guint x1,
								guint y0, guint y1);

/* generic machine */
gboolean ags_functional_test_util_machine_move_up(guint nth_machine);
gboolean ags_functional_test_util_machine_move_down(guint nth_machine);

gboolean ags_functional_test_util_machine_hide(guint nth_machine);
gboolean ags_functional_test_util_machine_show(guint nth_machine);

gboolean ags_functional_test_util_machine_destroy(guint nth_machine);

gboolean ags_functional_test_util_machine_rename_open(guint nth_machine);
gboolean ags_functional_test_util_machine_rename_close(guint nth_machine);
gboolean ags_functional_test_util_machine_rename_set_name(guint nth_machine,
							  gchar *name);

gboolean ags_functional_test_util_machine_properties_open(guint nth_machine);
gboolean ags_functional_test_util_machine_properties_close(guint nth_machine);

gboolean ags_functional_test_util_machine_properties_click_tab(guint nth_machine,
							       guint nth_tab);
gboolean ags_functional_test_util_machine_properties_click_enable(guint nth_machine);

gboolean ags_functional_test_util_machine_properties_link_set(guint nth_machine,
							      guint pad, guint audio_channel,
							      gchar *link_name, guint link_nth_line);
gboolean ags_functional_test_util_machine_properties_link_open(guint nth_machine,
							       guint pad, guint audio_channel);

gboolean ags_functional_test_util_machine_properties_effect_add(guint nth_machine,
								guint pad, guint audio_channel);
gboolean ags_functional_test_util_machine_properties_effect_remove(guint nth_machine,
								   guint pad, guint audio_channel,
								   guint nth);

gboolean ags_functional_test_util_machine_properties_effect_plugin_type(guint nth_machine,
									guint pad, guint audio_channel,
									gchar *plugin_type);

gboolean ags_functional_test_util_machine_properties_ladspa_filename(guint nth_machine,
								     guint pad, guint audio_channel,
								     gchar *filename);
gboolean ags_functional_test_util_machine_properties_ladspa_effect(guint nth_machine,
								   guint pad, guint audio_channel,
								   gchar *effect);

gboolean ags_functional_test_util_machine_properties_lv2_filename(guint nth_machine,
								  guint pad, guint audio_channel,
								  gchar *filename);
gboolean ags_functional_test_util_machine_properties_lv2_effect(guint nth_machine,
								guint pad, guint audio_channel,
								gchar *effect);

gboolean ags_functional_test_util_machine_properties_bulk_add(guint nth_machine);
gboolean ags_functional_test_util_machine_properties_bulk_remove(guint nth_machine,
								 guint nth_bulk);

gboolean ags_functional_test_util_machine_properties_bulk_link(guint nth_machine,
							       guint nth_bulk,
							       gchar *link);
gboolean ags_functional_test_util_machine_properties_bulk_first_line(guint nth_machine,
								     guint nth_bulk,
								     guint first_line);
gboolean ags_functional_test_util_machine_properties_bulk_link_line(guint nth_machine,
								    guint nth_bulk,
								    guint first_link_line);
gboolean ags_functional_test_util_machine_properties_bulk_count(guint nth_machine,
								guint nth_bulk,
								guint count);

gboolean ags_functional_test_util_machine_properties_resize_audio_channels(guint nth_machine,
									   guint audio_channels);
gboolean ags_functional_test_util_machine_properties_resize_inputs(guint nth_machine,
								   guint inputs);
gboolean ags_functional_test_util_machine_properties_resize_outputs(guint nth_machine,
								    guint outputs);

gboolean ags_functional_test_util_machine_edit_copy(guint nth_machine);

/* line member */
gboolean ags_functional_test_util_line_member_set_toggled(gchar *specifier,
							  gboolean is_toggled);
gboolean ags_functional_test_util_line_member_set_value(gchar *specifier,
							gdouble value);

/* audio connection */
gboolean ags_functional_test_util_machine_audio_connection_click_tab(guint nth_machine,
								     guint nth_tab);
gboolean ags_functional_test_util_machine_audio_connection_click_enable(guint nth_machine);

gboolean ags_functional_test_util_machine_audio_connection_set_device(guint nth_machine,
								      guint pad, guint audio_channel,
								      gchar *device);
gboolean ags_functional_test_util_machine_audio_connection_set_line(guint nth_machine,
								    uint pad, guint audio_channel,
								    guint line);

gboolean ags_functional_test_util_machine_audio_connection_bulk_add(guint nth_machine);
gboolean ags_functional_test_util_machine_audio_connection_bulk_remove(guint nth_machine,
								       guint nth_bulk);

gboolean ags_functional_test_util_machine_audio_connection_bulk_first_line(guint nth_machine,
									   guint nth_bulk,
									   guint first_line);
gboolean ags_functional_test_util_machine_audio_connection_bulk_count(guint nth_machine,
								      guint nth_bulk,
								      guint count);
gboolean ags_functional_test_util_machine_audio_connection_bulk_device(guint nth_machine,
								       guint nth_bulk,
								       gchar *device);

/* midi connection */
gboolean ags_functional_test_util_machine_midi_connection_midi_channel(guint nth_machine,
								       guint midi_channel);

gboolean ags_functional_test_util_machine_midi_connection_audio_start_mapping(guint nth_machine,
									      guint audio_start_mapping);
gboolean ags_functional_test_util_machine_midi_connection_audio_end_mapping(guint nth_machine,
									    guint audio_end_mapping);

gboolean ags_functional_test_util_machine_midi_connection_midi_start_mapping(guint nth_machine,
									     guint midi_start_mapping);
gboolean ags_functional_test_util_machine_midi_connection_midi_end_mapping(guint nth_machine,
									   guint midi_end_mapping);
gboolean ags_functional_test_util_machine_midi_connection_midi_device(guint nth_machine,
								      gchar *device);

/* generic pad */
gboolean ags_functional_test_util_pad_group(guint nth_machine,
					    guint nth_pad);
gboolean ags_functional_test_util_pad_mute(guint nth_machine,
					   guint nth_pad);
gboolean ags_functional_test_util_pad_solo(guint nth_machine,
					   guint nth_pad);

gboolean ags_functional_test_util_pad_play(guint nth_machine,
					   guint nth_pad);

/* generic line */
gboolean ags_functional_test_util_expander(guint nth_machine,
					   guint nth_pad, guint nth_audio_channel);

/* generic effect bulk */
gboolean ags_functional_test_util_effect_bulk_add(guint nth_machine);

gboolean ags_functional_test_util_effect_bulk_remove(guint nth_machine,
						     guint nth_bulk_effect);

gboolean ags_functional_test_util_effect_bulk_ladspa_filename(guint nth_machine,
							      gchar *filename);
gboolean ags_functional_test_util_effect_bulk_ladspa_effect(guint nth_machine,
							    gchar *effect);

gboolean ags_functional_test_util_effect_bulk_lv2_filename(guint nth_machine,
							   gchar *filename);
gboolean ags_functional_test_util_effect_bulk_lv2_effect(guint nth_machine,
							 gchar *effect);

/* panel */
gboolean ags_functional_test_util_panel_mute_line(guint nth_machine,
						  guint nth_pad, guint nth_audio_channel);

/* mixer input line */
gboolean ags_functional_test_util_mixer_input_line_volume(guint nth_machine,
							  guint nth_pad, guint nth_audio_channel,
							  gdouble volume);

/* drum */
gboolean ags_functional_test_util_drum_open(guint nth_machine);

gboolean ags_functional_test_util_drum_run(guint nth_machine);
gboolean ags_functional_test_util_drum_loop(guint nth_machine);

gboolean ags_functional_test_util_drum_bank_0(guint nth_machine,
					      guint bank_0);
gboolean ags_functional_test_util_drum_bank_1(guint nth_machine,
					      guint bank_1);

gboolean ags_functional_test_util_drum_length(guint nth_machine,
					      guint length);
gboolean ags_functional_test_util_drum_index(guint nth_machine,
					     guint nth_radio);

gboolean ags_functional_test_util_drum_pad(guint nth_machine,
					   guint nth_pad);

/* drum input pad */
gboolean ags_functional_test_util_drum_pad_open(guint nth_machine,
						guint nth_pad);
gboolean ags_functional_test_util_drum_pad_edit(guint nth_machine,
						guint nth_pad);

/* drum input line */
gboolean ags_functional_test_util_drum_input_line_volume(guint nth_machine,
							 guint nth_pad, guint nth_audio_channel,
							 gdouble volume);

/* matrix */
gboolean ags_functional_test_util_matrix_run(guint nth_machine);
gboolean ags_functional_test_util_matrix_loop(guint nth_machine);

gboolean ags_functional_test_util_matrix_bank_1(guint nth_machine,
						guint bank_1);

gboolean ags_functional_test_util_matrix_length(guint nth_machine,
						guint length);

gboolean ags_functional_test_util_matrix_gutter(guint nth_machine,
						guint nth_gutter);

gboolean ags_functional_test_util_drum_cell(guint nth_machine,
					    guint x, guint y);

/* synth */
gboolean ags_functional_test_util_synth_auto_update(guint nth_machine);
gboolean ags_functional_test_util_synth_update(guint nth_machine);

gboolean ags_functional_test_util_oscillator_wave(guint nth_machine,
						  guint nth_oscillator,
						  gchar *wave);
gboolean ags_functional_test_util_oscillator_phase(guint nth_machine,
						   guint nth_oscillator,
						   gdouble phase);
gboolean ags_functional_test_util_oscillator_attack(guint nth_machine,
						    guint nth_oscillator,
						    gdouble attack);
gboolean ags_functional_test_util_oscillator_frequency(guint nth_machine,
						       guint nth_oscillator,
						       gdouble frequency);
gboolean ags_functional_test_util_oscillator_length(guint nth_machine,
						    guint nth_oscillator,
						    gdouble length);
gboolean ags_functional_test_util_oscillator_volume(guint nth_machine,
						    guint nth_oscillator,
						    gdouble volume);

/* ffplayer */
gboolean ags_functional_test_util_ffplayer_open(guint nth_machine);

gboolean ags_functional_test_util_ffplayer_preset(guint nth_machine,
						  gchar *preset);
gboolean ags_functional_test_util_ffplayer_instrument(guint nth_machine,
						      gchar *instrument);

#endif /*__AGS_FUNCTIONAL_TEST_UTIL_H__*/
