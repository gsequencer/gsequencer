/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2021 Joël Krähemann
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

#ifndef __AGS_DRUM_H__
#define __AGS_DRUM_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_machine.h>

#include <ags/GSequencer/machine/ags_drum_input_pad.h>
#include <ags/GSequencer/machine/ags_pattern_box.h>

G_BEGIN_DECLS

#define AGS_TYPE_DRUM                (ags_drum_get_type())
#define AGS_DRUM(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_DRUM, AgsDrum))
#define AGS_DRUM_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_DRUM, AgsDrumClass))
#define AGS_IS_DRUM(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_DRUM))
#define AGS_IS_DRUM_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_DRUM))
#define AGS_DRUM_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_DRUM, AgsDrumClass))

typedef struct _AgsDrum AgsDrum;
typedef struct _AgsDrumClass AgsDrumClass;

struct _AgsDrum
{
  AgsMachine machine;

  guint flags;
  
  gchar *name;
  gchar *xml_type;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *pattern_play_container;
  AgsRecallContainer *pattern_recall_container;

  AgsRecallContainer *notation_play_container;
  AgsRecallContainer *notation_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;

  AgsRecallContainer *peak_play_container;
  AgsRecallContainer *peak_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;
  
  GtkButton *open;
  GtkWidget *open_dialog;
  
  GtkCheckButton *loop_button;
  GtkSpinButton *length_spin;

  GtkToggleButton *run;

  GtkBox *vbox;

  GtkBox *input_pad;
  GtkBox *output_pad;

  GtkToggleButton *selected_edit_button;

  AgsDrumInputPad *selected_pad;
  AgsLine *selected_line;

  GtkToggleButton *index0[4];
  GtkToggleButton *selected0;

  GtkToggleButton *index1[12];
  GtkToggleButton *selected1;

  AgsPatternBox *pattern_box;
};

struct _AgsDrumClass
{
  AgsMachineClass machine;
};

GType ags_drum_get_type(void);

AgsDrum* ags_drum_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_DRUM_H__*/
