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

#ifndef __AGS_FFPLAYER_H__
#define __AGS_FFPLAYER_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/GSequencer/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_FFPLAYER                (ags_ffplayer_get_type())
#define AGS_FFPLAYER(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_FFPLAYER, AgsFFPlayer))
#define AGS_FFPLAYER_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_FFPLAYER, AgsFFPlayerClass))
#define AGS_IS_FFPLAYER(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_FFPLAYER))
#define AGS_IS_FFPLAYER_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_FFPLAYER))
#define AGS_FFPLAYER_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_FFPLAYER, AgsFFPlayerClass))

#define AGS_FFPLAYER_DEFAULT_CONTROL_WIDTH (12)
#define AGS_FFPLAYER_DEFAULT_CONTROL_HEIGHT (40)

typedef struct _AgsFFPlayer AgsFFPlayer;
typedef struct _AgsFFPlayerClass AgsFFPlayerClass;

typedef enum{
  AGS_FFPLAYER_NO_LOAD          = 1,
}AgsFFPlayerFlags;

struct _AgsFFPlayer
{
  AgsMachine machine;

  guint flags;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  gchar *name;
  gchar *xml_type;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *notation_play_container;
  AgsRecallContainer *notation_recall_container;

  AgsRecallContainer *two_pass_aliase_play_container;
  AgsRecallContainer *two_pass_aliase_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  AgsAudioContainer *audio_container;

  GtkEntry *filename;
  GtkButton *open;
  
  AgsSF2Loader *sf2_loader;

  gchar *load_preset;
  gchar *load_instrument;
  
  gint position;
  GtkSpinner *sf2_loader_spinner;  

  guint control_width;
  guint control_height;
  
  GtkDrawingArea *drawing_area;
  GtkScrollbar *hscrollbar;
  GtkAdjustment *hadjustment;

  GtkComboBox *preset;
  GtkComboBox *instrument;

  GtkCheckButton *enable_synth_generator;

  GtkComboBox *pitch_function;
  
  GtkSpinButton *lower;
  GtkSpinButton *key_count;
  
  GtkButton *update;

  GtkCheckButton *enable_aliase;

  AgsDial *aliase_a_amount;
  AgsDial *aliase_a_phase;

  AgsDial *aliase_b_amount;
  AgsDial *aliase_b_phase;

  GtkScale *volume;
  
  GtkWidget *open_dialog;
};

struct _AgsFFPlayerClass
{
  AgsMachineClass machine;
};

GType ags_ffplayer_get_type(void);

void ags_ffplayer_paint(AgsFFPlayer *ffplayer);
// char* ags_ffplayer_sound_string();

void ags_ffplayer_open_filename(AgsFFPlayer *ffplayer,
				gchar *filename);

void ags_ffplayer_load_preset(AgsFFPlayer *ffplayer);
void ags_ffplayer_load_instrument(AgsFFPlayer *ffplayer);

void ags_ffplayer_update(AgsFFPlayer *ffplayer);

gboolean ags_ffplayer_sf2_loader_completed_timeout(AgsFFPlayer *ffplayer);

AgsFFPlayer* ags_ffplayer_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_FFPLAYER_H__*/
