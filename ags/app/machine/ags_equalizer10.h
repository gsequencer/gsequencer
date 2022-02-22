/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2020 Joël Krähemann
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

#ifndef __AGS_EQUALIZER10_H__
#define __AGS_EQUALIZER10_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

#include <ags/app/ags_machine.h>

G_BEGIN_DECLS

#define AGS_TYPE_EQUALIZER10                (ags_equalizer10_get_type())
#define AGS_EQUALIZER10(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EQUALIZER10, AgsEqualizer10))
#define AGS_EQUALIZER10_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EQUALIZER10, AgsEqualizer10Class))
#define AGS_IS_EQUALIZER10(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EQUALIZER10))
#define AGS_IS_EQUALIZER10_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EQUALIZER10))
#define AGS_EQUALIZER10_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_EQUALIZER10, AgsEqualizer10Class))

typedef struct _AgsEqualizer10 AgsEqualizer10;
typedef struct _AgsEqualizer10Class AgsEqualizer10Class;

struct _AgsEqualizer10
{
  AgsMachine machine;

  gchar *name;
  gchar *xml_type;
  
  guint mapped_output_pad;
  guint mapped_input_pad;

  AgsRecallContainer *eq10_play_container;
  AgsRecallContainer *eq10_recall_container;

  GtkScale *peak_28hz;
  GtkScale *peak_56hz;
  GtkScale *peak_112hz;
  GtkScale *peak_224hz;
  GtkScale *peak_448hz;
  GtkScale *peak_896hz;
  GtkScale *peak_1792hz;
  GtkScale *peak_3584hz;
  GtkScale *peak_7168hz;
  GtkScale *peak_14336hz;

  GtkScale *pressure;

  GList *peak_28hz_play_port;
  GList *peak_28hz_recall_port;

  GList *peak_56hz_play_port;
  GList *peak_56hz_recall_port;

  GList *peak_112hz_play_port;
  GList *peak_112hz_recall_port;

  GList *peak_224hz_play_port;
  GList *peak_224hz_recall_port;

  GList *peak_448hz_play_port;
  GList *peak_448hz_recall_port;

  GList *peak_896hz_play_port;
  GList *peak_896hz_recall_port;

  GList *peak_1792hz_play_port;
  GList *peak_1792hz_recall_port;

  GList *peak_3584hz_play_port;
  GList *peak_3584hz_recall_port;

  GList *peak_7168hz_play_port;
  GList *peak_7168hz_recall_port;

  GList *peak_14336hz_play_port;
  GList *peak_14336hz_recall_port;

  GList *pressure_play_port;
  GList *pressure_recall_port;
};

struct _AgsEqualizer10Class
{
  AgsMachineClass machine;
};

GType ags_equalizer10_get_type(void);

AgsPort* ags_equalizer10_find_specifier(GList *recall, gchar *specifier);

void ags_equalizer10_remap_port(AgsEqualizer10 *equalizer10);

void ags_equalizer10_input_map_recall(AgsEqualizer10 *equalizer10,
				      guint audio_channel_start,
				      guint input_pad_start);
void ags_equalizer10_output_map_recall(AgsEqualizer10 *equalizer10,
				       guint audio_channel_start,
				       guint output_pad_start);

AgsEqualizer10* ags_equalizer10_new(GObject *soundcard);

G_END_DECLS

#endif /*__AGS_EQUALIZER10_H__*/
