/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2022 Joël Krähemann
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

#include <CUnit/CUnit.h>

#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_OUTPUT_TAB (0)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_INPUT_TAB (1)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_OUTPUT_TAB (2)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_BULK_INPUT_TAB (3)
#define AGS_FUNCTIONAL_TEST_UTIL_MACHINE_EDITOR_DIALOG_RESIZE_TAB (4)

#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_4_TO_1 (0)
#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_2_TO_1 (1)
#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_1 (2)
#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_2 (3)
#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_4 (4)
#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_8 (5)
#define AGS_FUNCTIONAL_TEST_UTIL_TOOLBAR_ZOOM_1_TO_16 (6)

#define AGS_FUNCTIONAL_TEST_UTIL_DRIVER_PROGRAM(ptr) ((AgsFunctionalTestUtilDriverProgram *)(ptr))
#define AGS_FUNCTIONAL_TEST_UTIL_IDLE_CONDITION(f) ((AgsFunctionalTestUtilIdleCondition)(f))

typedef struct _AgsFunctionalTestUtilDriverProgram AgsFunctionalTestUtilDriverProgram;
typedef struct _AgsFunctionalTestUtilListLengthCondition AgsFunctionalTestUtilListLengthCondition;

typedef void (*AgsFunctionalTestUtilAddTest)();

typedef void (*AgsFunctionalTestUtilDriverProgramFunc)(guint n_params,
						       gchar **param_strv,
						       GValue *param);

typedef gboolean (*AgsFunctionalTestUtilIdleCondition)(gpointer data);

struct _AgsFunctionalTestUtilDriverProgram
{
  AgsFunctionalTestUtilDriverProgramFunc driver_program_func;

  guint n_params;
  
  gchar **param_strv;
  GValue *param;
};

struct _AgsFunctionalTestUtilListLengthCondition
{
  GList **start_list;
  guint length;
};

void ags_functional_test_util_init(int *argc, char ***argv,
				   gchar *conf_str);

GThread* ags_functional_test_util_test_runner_thread();

void ags_functional_test_util_do_run(int argc, char **argv,
				     AgsFunctionalTestUtilAddTest add_test, volatile gboolean *is_available);

void ags_functional_test_util_add_test(AgsFunctionalTestUtilAddTest add_test,
				       volatile gboolean *is_available);

void ags_functional_test_util_notify_add_test(volatile gboolean *is_available);

void ags_functional_test_util_add_driver(gdouble timeout_s);

void ags_functional_test_util_add_driver_program(AgsFunctionalTestUtilDriverProgram *driver_program);

struct timespec* ags_functional_test_util_get_default_timeout();

#define AGS_FUNCTIONAL_TEST_UTIL_ASSERT(value) (ags_functional_test_util_assert((value), __FILE__, __LINE__, "", #value))

void ags_functional_test_util_assert(gboolean value, gchar *filename, guint line, gchar *function_str, gchar *condition_str);

/* synchronization */
void ags_functional_test_util_reaction_time();
void ags_functional_test_util_reaction_time_long();

void ags_functional_test_util_idle(gint64 idle_time);

void ags_functional_test_util_idle_condition_and_timeout(AgsFunctionalTestUtilIdleCondition idle_condition,
							 struct timespec *timeout,
							 gpointer data);

gboolean ags_functional_test_util_idle_test_widget_visible(GtkWidget **widget);
gboolean ags_functional_test_util_idle_test_widget_hidden(GtkWidget **widget);
gboolean ags_functional_test_util_idle_test_widget_realized(GtkWidget **widget);
gboolean ags_functional_test_util_idle_test_null(GtkWidget **widget);
gboolean ags_functional_test_util_idle_test_list_length(AgsFunctionalTestUtilListLengthCondition *condition);

/* leave */
void ags_functional_test_util_leave(GtkWidget *window);

/* mouse */
void ags_functional_test_util_fake_mouse_warp(gpointer display, guint screen, guint x, guint y);

void ags_functional_test_util_fake_mouse_button_press(gpointer display, guint button);
void ags_functional_test_util_fake_mouse_button_release(gpointer display, guint button);

void ags_functional_test_util_fake_mouse_button_click(gpointer display, guint button);

/* common */
void ags_functional_test_util_header_bar_menu_button_click(GtkMenuButton *menu_button,
							   gchar **item_path,
							   gchar *action);

void ags_functional_test_util_combo_box_click(GtkComboBox *combo_box,
					      guint nth);

void ags_functional_test_util_button_click(GtkButton *button);
void ags_functional_test_util_toggle_button_click(GtkToggleButton *toggle_button);
void ags_functional_test_util_check_button_click(GtkCheckButton *check_button);
void ags_functional_test_util_menu_button_click(GtkMenuButton *button);

/* generic dialog */
void ags_functional_test_util_dialog_apply(GtkDialog *dialog);
void ags_functional_test_util_dialog_ok(GtkDialog *dialog);
void ags_functional_test_util_dialog_cancel(GtkDialog *dialog);

#define ags_functional_test_util_dialog_close ags_functional_test_util_dialog_cancel

/* file chooser */
void ags_functional_test_util_file_chooser_open_path(GtkFileChooser *file_chooser,
						     gchar *path);
void ags_functional_test_util_file_chooser_select_filename(GtkFileChooser *file_chooser,
							   gchar *filename);
void ags_functional_test_util_file_chooser_select_filenames(GtkFileChooser *file_chooser,
							    GSList *filename);

/* UI control */
void ags_functional_test_util_file_default_window_resize();

/* file */
void ags_functional_test_util_open();

void ags_functional_test_util_save();
void ags_functional_test_util_save_as();

void ags_functional_test_util_export_open();
void ags_functional_test_util_export_close();
void ags_functional_test_util_export_add();
void ags_functional_test_util_export_tact(gdouble tact);

void ags_functional_test_util_export_remove(guint nth);
void ags_functional_test_util_export_set_backend(guint nth,
						 gchar *backend);
void ags_functional_test_util_export_set_device(guint nth,
						gchar *device);
void ags_functional_test_util_export_set_filename(guint nth,
						  gchar *filename);
void ags_functional_test_util_export_nth(guint nth);
void ags_functional_test_util_export_set_format(guint nth,
						gchar *format);

void ags_functional_test_util_export_do_export();

void ags_functional_test_util_quit();

/* edit */
void ags_functional_test_util_add_machine(gchar *submenu,
					  gchar *machine_name);

void ags_functional_test_util_preferences_open();
void ags_functional_test_util_preferences_close();

/* help */
void ags_functional_test_util_about_open();
void ags_functional_test_util_about_close();

/* navigation */
void ags_functional_test_util_navigation_expand();

void ags_functional_test_util_navigation_bpm(gdouble bpm);

void ags_functional_test_util_navigation_rewind();
void ags_functional_test_util_navigation_prev();
void ags_functional_test_util_navigation_play();
void ags_functional_test_util_navigation_stop();
void ags_functional_test_util_navigation_next();
void ags_functional_test_util_navigation_forward();

void ags_functional_test_util_navigation_loop();

void ags_functional_test_util_navigation_position(gdouble position);

void ags_functional_test_util_navigation_loop_left(gdouble loop_left);
void ags_functional_test_util_navigation_loop_right(gdouble loop_right);

void ags_functional_test_util_navigation_exclude_sequencers();

/* composite toolbar */
void ags_functional_test_util_composite_toolbar_cursor_click();
void ags_functional_test_util_composite_toolbar_edit_click();
void ags_functional_test_util_composite_toolbar_delete_click();
void ags_functional_test_util_composite_toolbar_select_click();
void ags_functional_test_util_composite_toolbar_invert_click();

void ags_functional_test_util_composite_toolbar_paste_click();
void ags_functional_test_util_composite_toolbar_copy_click();
void ags_functional_test_util_composite_toolbar_cut_click();

void ags_functional_test_util_composite_toolbar_zoom(guint nth_zoom);

/* machine selector */
void ags_functional_test_util_machine_selector_select(gchar *machine);

void ags_functional_test_util_machine_selector_reverse_mapping();
void ags_functional_test_util_machine_selector_shift_piano(guint nth_shift);

/* notation edit */
void ags_functional_test_util_notation_edit_delete_point(guint x,
							 guint y);
void ags_functional_test_util_notation_edit_add_point(guint x0, guint x1,
						      guint y);
void ags_functional_test_util_notation_edit_select_region(guint x0, guint x1,
							  guint y0, guint y1);

/* automation area */
void ags_functional_test_util_automation_edit_delete_point(guint nth_index,
							   guint x,
							   guint y);
void ags_functional_test_util_automation_edit_add_point(guint nth_index,
							guint x,
							guint y);
void ags_functional_test_util_automation_edit_select_region(guint nth_index,
							    guint x0, guint x1,
							    guint y0, guint y1);

/* preferences */
void ags_functional_test_util_preferences_click_tab(guint nth_tab);

void ags_functional_test_util_audio_preferences_buffer_size(guint nth_backend,
							    guint buffer_size);
void ags_functional_test_util_audio_preferences_samplerate(guint nth_backend,
							   guint samplerate);

/* generic machine */
void ags_functional_test_util_machine_menu_button_click(guint nth_machine,
							GtkMenuButton *menu_button,
							gchar **item_path,
							gchar *action);

void ags_functional_test_util_machine_move_up(guint nth_machine);
void ags_functional_test_util_machine_move_down(guint nth_machine);

void ags_functional_test_util_machine_hide(guint nth_machine);
void ags_functional_test_util_machine_show(guint nth_machine);

void ags_functional_test_util_machine_destroy(guint nth_machine);

void ags_functional_test_util_machine_rename_open(guint nth_machine);
void ags_functional_test_util_machine_rename_close(guint nth_machine);
void ags_functional_test_util_machine_rename_set_name(guint nth_machine,
						      gchar *name);

GtkWidget* ags_functional_test_util_get_machine_editor_dialog_line(GtkWidget *machine_editor_dialog,
								   guint nth_pad, guint nth_audio_channel,
								   gboolean is_output);

void ags_functional_test_util_machine_editor_dialog_open(guint nth_machine);

void ags_functional_test_util_machine_editor_dialog_click_tab(guint nth_machine,
							      guint nth_tab);
void ags_functional_test_util_machine_editor_dialog_click_enable(guint nth_machine);

void ags_functional_test_util_machine_editor_dialog_link_set(guint nth_machine,
							     guint pad, guint audio_channel,
							     gchar *link_name, guint link_nth_line);
void ags_functional_test_util_machine_editor_dialog_link_open(guint nth_machine,
							      guint pad, guint audio_channel);

void ags_functional_test_util_machine_editor_dialog_effect_add(guint nth_machine,
							       guint pad, guint audio_channel);
void ags_functional_test_util_machine_editor_dialog_effect_remove(guint nth_machine,
								  guint pad, guint audio_channel,
								  guint nth_effect);

void ags_functional_test_util_machine_editor_dialog_effect_plugin_type(guint nth_machine,
								       guint pad, guint audio_channel,
								       gchar *plugin_type);

void ags_functional_test_util_machine_editor_dialog_ladspa_filename(guint nth_machine,
								    guint pad, guint audio_channel,
								    gchar *filename);
void ags_functional_test_util_machine_editor_dialog_ladspa_effect(guint nth_machine,
								  guint pad, guint audio_channel,
								  gchar *effect);

void ags_functional_test_util_machine_editor_dialog_lv2_filename(guint nth_machine,
								 guint pad, guint audio_channel,
								 gchar *filename);
void ags_functional_test_util_machine_editor_dialog_lv2_effect(guint nth_machine,
							       guint pad, guint audio_channel,
							       gchar *effect);

void ags_functional_test_util_machine_editor_dialog_bulk_add(guint nth_machine);
void ags_functional_test_util_machine_editor_dialog_bulk_remove(guint nth_machine,
								guint nth_bulk);

void ags_functional_test_util_machine_editor_dialog_bulk_link(guint nth_machine,
							      guint nth_bulk,
							      gchar *link);
void ags_functional_test_util_machine_editor_dialog_bulk_first_line(guint nth_machine,
								    guint nth_bulk,
								    guint first_line);
void ags_functional_test_util_machine_editor_dialog_bulk_link_line(guint nth_machine,
								   guint nth_bulk,
								   guint first_link_line);
void ags_functional_test_util_machine_editor_dialog_bulk_count(guint nth_machine,
							       guint nth_bulk,
							       guint count);

void ags_functional_test_util_machine_editor_dialog_resize_audio_channels(guint nth_machine,
									  guint audio_channels);
void ags_functional_test_util_machine_editor_dialog_resize_inputs(guint nth_machine,
								  guint inputs);
void ags_functional_test_util_machine_editor_dialog_resize_outputs(guint nth_machine,
								   guint outputs);

void ags_functional_test_util_machine_edit_copy(guint nth_machine);

/* line member */
void ags_functional_test_util_line_member_set_toggled(gchar *specifier,
						      gboolean is_toggled);
void ags_functional_test_util_line_member_set_value(gchar *specifier,
						    gdouble value);

/* audio connection */
void ags_functional_test_util_machine_audio_connection_click_tab(guint nth_machine,
								 guint nth_tab);
void ags_functional_test_util_machine_audio_connection_click_enable(guint nth_machine);

void ags_functional_test_util_machine_audio_connection_set_device(guint nth_machine,
								  guint pad, guint audio_channel,
								  gchar *device);
void ags_functional_test_util_machine_audio_connection_set_line(guint nth_machine,
								uint pad, guint audio_channel,
								guint line);

void ags_functional_test_util_machine_audio_connection_bulk_add(guint nth_machine);
void ags_functional_test_util_machine_audio_connection_bulk_remove(guint nth_machine,
								   guint nth_bulk);

void ags_functional_test_util_machine_audio_connection_bulk_first_line(guint nth_machine,
								       guint nth_bulk,
								       guint first_line);
void ags_functional_test_util_machine_audio_connection_bulk_count(guint nth_machine,
								  guint nth_bulk,
								  guint count);
void ags_functional_test_util_machine_audio_connection_bulk_device(guint nth_machine,
								   guint nth_bulk,
								   gchar *device);

/* midi connection */
void ags_functional_test_util_machine_midi_connection_midi_channel(guint nth_machine,
								   guint midi_channel);

void ags_functional_test_util_machine_midi_connection_audio_start_mapping(guint nth_machine,
									  guint audio_start_mapping);
void ags_functional_test_util_machine_midi_connection_audio_end_mapping(guint nth_machine,
									guint audio_end_mapping);

void ags_functional_test_util_machine_midi_connection_midi_start_mapping(guint nth_machine,
									 guint midi_start_mapping);
void ags_functional_test_util_machine_midi_connection_midi_end_mapping(guint nth_machine,
								       guint midi_end_mapping);
void ags_functional_test_util_machine_midi_connection_midi_device(guint nth_machine,
								  gchar *device);

/* generic pad */
void ags_functional_test_util_pad_group(guint nth_machine,
					guint nth_pad);
void ags_functional_test_util_pad_mute(guint nth_machine,
				       guint nth_pad);
void ags_functional_test_util_pad_solo(guint nth_machine,
				       guint nth_pad);

void ags_functional_test_util_pad_play(guint nth_machine,
				       guint nth_pad);

/* generic line */
void ags_functional_test_util_expander(guint nth_machine,
				       guint nth_pad, guint nth_audio_channel);

/* generic effect bulk */
void ags_functional_test_util_effect_bulk_add(guint nth_machine);

void ags_functional_test_util_effect_bulk_remove(guint nth_machine,
						 guint nth_bulk_effect);

void ags_functional_test_util_effect_bulk_ladspa_filename(guint nth_machine,
							  gchar *filename);
void ags_functional_test_util_effect_bulk_ladspa_effect(guint nth_machine,
							gchar *effect);

void ags_functional_test_util_effect_bulk_lv2_filename(guint nth_machine,
						       gchar *filename);
void ags_functional_test_util_effect_bulk_lv2_effect(guint nth_machine,
						     gchar *effect);

/* panel */
void ags_functional_test_util_panel_mute_line(guint nth_machine,
					      guint nth_pad, guint nth_audio_channel);

/* mixer input line */
void ags_functional_test_util_mixer_input_line_volume(guint nth_machine,
						      guint nth_pad, guint nth_audio_channel,
						      gdouble volume);

/* drum */
void ags_functional_test_util_drum_open(guint nth_machine);

void ags_functional_test_util_drum_run(guint nth_machine);
void ags_functional_test_util_drum_loop(guint nth_machine);

void ags_functional_test_util_drum_bank_0(guint nth_machine,
					  guint bank_0);
void ags_functional_test_util_drum_bank_1(guint nth_machine,
					  guint bank_1);

void ags_functional_test_util_drum_length(guint nth_machine,
					  guint length);
void ags_functional_test_util_drum_index(guint nth_machine,
					 guint nth_radio);

void ags_functional_test_util_drum_pad(guint nth_machine,
				       guint nth_pad);

/* drum input pad */
void ags_functional_test_util_drum_pad_open(guint nth_machine,
					    guint nth_pad);
void ags_functional_test_util_drum_pad_edit(guint nth_machine,
					    guint nth_pad);

/* drum input line */
void ags_functional_test_util_drum_input_line_volume(guint nth_machine,
						     guint nth_pad, guint nth_audio_channel,
						     gdouble volume);

/* matrix */
void ags_functional_test_util_matrix_run(guint nth_machine);
void ags_functional_test_util_matrix_loop(guint nth_machine);

void ags_functional_test_util_matrix_bank_1(guint nth_machine,
					    guint bank_1);

void ags_functional_test_util_matrix_length(guint nth_machine,
					    guint length);

void ags_functional_test_util_matrix_gutter(guint nth_machine,
					    guint nth_gutter);

void ags_functional_test_util_drum_cell(guint nth_machine,
					guint x, guint y);

/* synth */
void ags_functional_test_util_synth_auto_update(guint nth_machine);
void ags_functional_test_util_synth_update(guint nth_machine);

void ags_functional_test_util_oscillator_wave(guint nth_machine,
					      guint nth_oscillator,
					      gchar *wave);
void ags_functional_test_util_oscillator_phase(guint nth_machine,
					       guint nth_oscillator,
					       gdouble phase);
void ags_functional_test_util_oscillator_attack(guint nth_machine,
						guint nth_oscillator,
						gdouble attack);
void ags_functional_test_util_oscillator_frequency(guint nth_machine,
						   guint nth_oscillator,
						   gdouble frequency);
void ags_functional_test_util_oscillator_length(guint nth_machine,
						guint nth_oscillator,
						gdouble length);
void ags_functional_test_util_oscillator_volume(guint nth_machine,
						guint nth_oscillator,
						gdouble volume);

/* ffplayer */
void ags_functional_test_util_ffplayer_open(guint nth_machine);

void ags_functional_test_util_ffplayer_preset(guint nth_machine,
					      gchar *preset);
void ags_functional_test_util_ffplayer_instrument(guint nth_machine,
						  gchar *instrument);

#endif /*__AGS_FUNCTIONAL_TEST_UTIL_H__*/
