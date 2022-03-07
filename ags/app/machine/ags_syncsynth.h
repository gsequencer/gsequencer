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

#ifndef __AGS_SYNCSYNTH_H__
#define __AGS_SYNCSYNTH_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

#include <ags/app/machine/ags_oscillator.h>

G_BEGIN_DECLS

#define AGS_TYPE_SYNCSYNTH                (ags_syncsynth_get_type())
#define AGS_SYNCSYNTH(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_SYNCSYNTH, AgsSyncsynth))
#define AGS_SYNCSYNTH_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_SYNCSYNTH, AgsSyncsynthClass))
#define AGS_IS_SYNCSYNTH(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_SYNCSYNTH))
#define AGS_IS_SYNCSYNTH_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_SYNCSYNTH))
#define AGS_SYNCSYNTH_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_SYNCSYNTH, AgsSyncsynthClass))

#define AGS_SYNCSYNTH_BASE_NOTE_MAX (72.0)
#define AGS_SYNCSYNTH_BASE_NOTE_MIN (-72.0)

typedef struct _AgsSyncsynth AgsSyncsynth;
typedef struct _AgsSyncsynthClass AgsSyncsynthClass;

typedef enum{
  AGS_SYNCSYNTH_AUTO_UPDATE          = 1,
}AgsSyncsynthFlags;

struct _AgsSyncsynth
{
  AgsMachine machine;

  guint flags;

  gchar *name;
  gchar *xml_type;

  guint mapped_input_pad;
  guint mapped_output_pad;

  AgsRecallContainer *playback_play_container;
  AgsRecallContainer *playback_recall_container;

  AgsRecallContainer *notation_play_container;
  AgsRecallContainer *notation_recall_container;

  AgsRecallContainer *volume_play_container;
  AgsRecallContainer *volume_recall_container;

  AgsRecallContainer *envelope_play_container;
  AgsRecallContainer *envelope_recall_container;
  
  AgsRecallContainer *buffer_play_container;
  AgsRecallContainer *buffer_recall_container;

  GList *oscillator;
  
  GtkBox *oscillator_box;
  
  GtkButton *add;
  GtkButton *remove;
  
  GtkCheckButton *auto_update;
  GtkButton *update;

  GtkSpinButton *lower; // how many channels until to lowest freq
  GtkSpinButton *loop_start;
  GtkSpinButton *loop_end;

  GtkScale *volume;
};

struct _AgsSyncsynthClass
{
  AgsMachineClass machine;
};

GType ags_syncsynth_get_type(void);

gboolean ags_syncsynth_test_flags(AgsSyncsynth *syncsynth, guint flags);
void ags_syncsynth_set_flags(AgsSyncsynth *syncsynth, guint flags);
void ags_syncsynth_unset_flags(AgsSyncsynth *syncsynth, guint flags);

GList* ags_syncsynth_get_oscillator(AgsSyncsynth *syncsynth);
void ags_syncsynth_add_oscillator(AgsSyncsynth *syncsynth,
				  AgsOscillator *oscillator);
void ags_syncsynth_remove_oscillator(AgsSyncsynth *syncsynth,
				     AgsOscillator *oscillator);

void ags_syncsynth_reset_loop(AgsSyncsynth *syncsynth);
void ags_syncsynth_update(AgsSyncsynth *syncsynth);

AgsSyncsynth* ags_syncsynth_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_SYNCSYNTH_H__*/
