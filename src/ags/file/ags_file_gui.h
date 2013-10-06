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

#ifndef __AGS_FILE_GUI_H__
#define __AGS_FILE_GUI_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <libxml/tree.h>

#include <ags/X/ags_window.h>
#include <ags/X/ags_menu_bar.h>
#include <ags/X/ags_machine.h>
#include <ags/X/ags_pad.h>
#include <ags/X/ags_line.h>
#include <ags/X/ags_line_member.h>
#include <ags/X/ags_editor.h>
#include <ags/X/ags_navigation.h>
#include <ags/X/ags_machine_editor.h>
#include <ags/X/ags_pad_editor.h>
#include <ags/X/ags_line_editor.h>
#include <ags/X/ags_link_editor.h>
#include <ags/X/ags_line_member_editor.h>

#include <ags/X/editor/ags_toolbar.h>
#include <ags/X/editor/ags_notebook.h>

#include <ags/X/machine/ags_panel.h>
#include <ags/X/machine/ags_mixer.h>
#include <ags/X/machine/ags_drum.h>
#include <ags/X/machine/ags_matrix.h>
#include <ags/X/machine/ags_synth.h>
#include <ags/X/machine/ags_oscillator.h>
#include <ags/X/machine/ags_ffplayer.h>

/* AgsWindow */
void ags_file_read_window(xmlNode *node, AgsWindow **window);
void ags_file_write_window(xmlNode *parent, AgsWindow *window);

/* AgsMenuBar */
void ags_file_read_menu_bar(xmlNode *node, AgsMenuBar **menu_bar);
void ags_file_write_menu_bar(xmlNode *parent, AgsMenuBar *menu_bar);

/* AgsMachineCounter */
void ags_file_read_machine_counter(xmlNode *node, AgsMachineCounter **machine_counter);
void ags_file_write_machine_counter(xmlNode *parent, AgsMachineCounter *machine_counter);

void ags_file_read_machine_counter_list(xmlNode *node, GList **machine_counter);
void ags_file_write_machine_counter_list(xmlNode *parent, GList *machine_counter);

/* AgsMachine */
void ags_file_read_machine(xmlNode *node, AgsMachine **machine);
void ags_file_write_machine(xmlNode *parent, AgsMachine *machine);

void ags_file_read_machine_list(xmlNode *node, GList **machine);
void ags_file_write_machine_list(xmlNode *parent, GList *machine);

void ags_file_read_panel(xmlNode *node, AgsMachine *panel);
void ags_file_write_panel(xmlNode *parent, AgsMachine *panel);

void ags_file_read_mixer(xmlNode *node, AgsMachine *mixer);
void ags_file_write_mixer(xmlNode *parent, AgsMachine *mixer);

void ags_file_read_drum(xmlNode *node, AgsMachine *drum);
void ags_file_write_drum(xmlNode *parent, AgsMachine *drum);

void ags_file_read_matrix(xmlNode *node, AgsMachine *matrix);
void ags_file_write_matrix(xmlNode *parent, AgsMachine *matrix);

void ags_file_read_synth(xmlNode *node, AgsMachine *synth);
void ags_file_write_synth(xmlNode *parent, AgsMachine *synth);

void ags_file_read_ffplayer(xmlNode *node, AgsMachine *ffplayer);
void ags_file_write_ffplayer(xmlNode *parent, AgsMachine *ffplayer);

/* AgsPad */
void ags_file_read_pad(xmlNode *node, AgsPad **pad);
void ags_file_write_pad(xmlNode *parent, AgsPad *pad);

void ags_file_read_pad_list(xmlNode *node, GList **pad);
void ags_file_write_pad_list(xmlNode *parent, GList *pad);

void ags_file_read_mixer_input_pad(xmlNode *node, AgsPad *mixer_input_pad);
void ags_file_write_mixer_input_pad(xmlNode *parent, AgsPad *mixer_input_pad);

void ags_file_read_mixer_output_pad(xmlNode *node, AgsPad *mixer_output_pad);
void ags_file_write_mixer_output_pad(xmlNode *parent, AgsPad *mixer_output_pad);

void ags_file_read_drum_input_pad(xmlNode *node, AgsPad *drum_input_pad);
void ags_file_write_drum_input_pad(xmlNode *parent, AgsPad *drum_input_pad);

void ags_file_read_drum_output_pad(xmlNode *node, AgsPad *drum_output_pad);
void ags_file_write_drum_output_pad(xmlNode *parent, AgsPad *drum_output_pad);

/* AgsLine */
void ags_file_read_line(xmlNode *node, AgsLine **line);
void ags_file_write_line(xmlNode *parent, AgsLine *line);

void ags_file_read_line_list(xmlNode *node, GList **line);
void ags_file_write_line_list(xmlNode *parent, GList *line);

void ags_file_read_drum_input_line(xmlNode *node, AgsLine *drum_output_line);
void ags_file_write_drum_input_line(xmlNode *parent, AgsLine *drum_output_line);

void ags_file_read_drum_output_line(xmlNode *node, AgsLine *drum_output_line);
void ags_file_write_drum_output_line(xmlNode *parent, AgsLine *drum_output_line);

/* AgsLineMember */
void ags_file_read_line_member(xmlNode *node, AgsLineMember **line_member);
void ags_file_write_line_member(xmlNode *parent, AgsLineMember *line_member);

void ags_file_read_line_member_list(xmlNode *node, GList **line_member);
void ags_file_write_line_member_list(xmlNode *parent, GList *line_member);

/* AgsMachineEditor */
//TODO:JK: dialogs are missing for now

/* AgsEditor */
void ags_file_read_editor(xmlNode *node, AgsEditor **editor);
void ags_file_write_editor(xmlNode *parent, AgsEditor *editor);

/* AgsToolbar */
void ags_file_read_toolbar(xmlNode *node, AgsToolbar **toolbar);
void ags_file_write_toolbar(xmlNode *parent, AgsToolbar *toolbar);

/* AgsNotebook */
void ags_file_read_notebook(xmlNode *node, AgsNotebook **notebook);
void ags_file_write_notebook(xmlNode *parent, AgsNotebook *notebook);

/* AgsNavigation */
void ags_file_read_navigation(xmlNode *node, AgsNavigation **navigation);
void ags_file_write_navigation(xmlNode *parent, AgsNavigation *navigation);


#endif /*__AGS_FILE_GUI_H__*/
