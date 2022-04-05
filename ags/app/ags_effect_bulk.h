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

#ifndef __AGS_EFFECT_BULK_H__
#define __AGS_EFFECT_BULK_H__

#include <glib.h>
#include <glib-object.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>
#include <ags/libags-gui.h>

#include <ags/app/ags_effect_bulk_entry.h>
#include <ags/app/ags_bulk_member.h>

G_BEGIN_DECLS

#define AGS_TYPE_EFFECT_BULK                (ags_effect_bulk_get_type())
#define AGS_EFFECT_BULK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EFFECT_BULK, AgsEffectBulk))
#define AGS_EFFECT_BULK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EFFECT_BULK, AgsEffectBulkClass))
#define AGS_IS_EFFECT_BULK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_EFFECT_BULK))
#define AGS_IS_EFFECT_BULK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_EFFECT_BULK))
#define AGS_EFFECT_BULK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_EFFECT_BULK, AgsEffectBulkClass))

#define AGS_EFFECT_BULK_PLUGIN(ptr) ((AgsEffectBulkPlugin *)(ptr))

#define AGS_EFFECT_BULK_DEFAULT_VERSION "4.0.0"
#define AGS_EFFECT_BULK_DEFAULT_BUILD_ID "Sun Feb 27 22:00:52 UTC 2022"

#define AGS_EFFECT_BULK_COLUMNS_COUNT (4)

typedef struct _AgsEffectBulk AgsEffectBulk;
typedef struct _AgsEffectBulkPlugin AgsEffectBulkPlugin;
typedef struct _AgsEffectBulkClass AgsEffectBulkClass;

typedef enum{
  AGS_EFFECT_BULK_MAPPED_RECALL       = 1,
  AGS_EFFECT_BULK_PREMAPPED_RECALL    = 1 <<  1,
  AGS_EFFECT_BULK_HIDE_BUTTONS        = 1 <<  2,
  AGS_EFFECT_BULK_HIDE_ENTRIES        = 1 <<  3,
  AGS_EFFECT_BULK_SHOW_LABELS         = 1 <<  4,
}AgsEffectBulkFlags;

struct _AgsEffectBulk
{
  GtkBox box;

  guint flags;
  guint connectable_flags;
  
  gchar *name;

  gchar *version;
  gchar *build_id;

  GType channel_type;
  AgsAudio *audio;

  GtkBox *control_box;
  
  GtkButton *add;
  GtkButton *remove;

  GList *bulk_member_entry;
  
  GtkBox *bulk_member_entry_box;
  
  GList *bulk_member;
  
  GtkGrid *bulk_member_grid;

  GList *plugin;
  GtkDialog *plugin_browser;

  GList *queued_drawing;
};

struct _AgsEffectBulkClass
{
  GtkBoxClass box;

  void (*add_plugin)(AgsEffectBulk *effect_bulk,
		     GList *control_type_name,
		     AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
		     gchar *plugin_name,
		     gchar *filename,
		     gchar *effect,
		     guint start_audio_channel, guint stop_audio_channel,
		     guint start_pad, guint stop_pad,
		     gint position,
		     guint create_flags, guint recall_flags);
  void (*remove_plugin)(AgsEffectBulk *effect_bulk,
			guint nth);

  void (*resize_audio_channels)(AgsEffectBulk *effect_bulk,
				guint new_size,
				guint old_size);
  void (*resize_pads)(AgsEffectBulk *effect_bulk,
		      guint new_size,
		      guint old_size);

  void (*map_recall)(AgsEffectBulk *effect_bulk);
  GList* (*find_port)(AgsEffectBulk *effect_bulk);
};

struct _AgsEffectBulkPlugin
{  
  AgsRecallContainer *play_container;
  AgsRecallContainer *recall_container;

  gchar *plugin_name;
  
  gchar *filename;
  gchar *effect;
  
  GList *control_type_name;

  guint control_count;
};

GType ags_effect_bulk_get_type(void);

gboolean ags_effect_bulk_test_flags(AgsEffectBulk *effect_bulk,
				    guint flags);
void ags_effect_bulk_set_flags(AgsEffectBulk *effect_bulk,
			       guint flags);
void ags_effect_bulk_unset_flags(AgsEffectBulk *effect_bulk,
				 guint flags);

AgsEffectBulkPlugin* ags_effect_bulk_plugin_alloc(AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
						  gchar *plugin_name,
						  gchar *filename,
						  gchar *effect);
void ags_effect_bulk_plugin_free(AgsEffectBulkPlugin *effect_bulk_plugin);

void ags_effect_bulk_add_plugin(AgsEffectBulk *effect_bulk,
				GList *control_type_name,
				AgsRecallContainer *play_container, AgsRecallContainer *recall_container,
				gchar *plugin_name,
				gchar *filename,
				gchar *effect,
				guint start_audio_channel, guint stop_audio_channel,
				guint start_pad, guint stop_pad,
				gint position,
				guint create_flags, guint recall_flags);
void ags_effect_bulk_remove_plugin(AgsEffectBulk *effect_bulk,
				   guint nth);

GList* ags_effect_bulk_get_bulk_member_entry(AgsEffectBulk *effect_bulk);
void ags_effect_bulk_add_bulk_member_entry(AgsEffectBulk *effect_bulk,
					   AgsEffectBulkEntry *effect_bulk_entry);
void ags_effect_bulk_remove_bulk_member_entry(AgsEffectBulk *effect_bulk,
					      AgsEffectBulkEntry *effect_bulk_entry);

GList* ags_effect_bulk_get_bulk_member(AgsEffectBulk *effect_bulk);
void ags_effect_bulk_add_bulk_member(AgsEffectBulk *effect_bulk,
				     AgsBulkMember *bulk_member,
				     guint x, guint y,
				     guint width, guint height);
void ags_effect_bulk_remove_bulk_member(AgsEffectBulk *effect_bulk,
					AgsBulkMember *bulk_member);

void ags_effect_bulk_resize_audio_channels(AgsEffectBulk *effect_bulk,
					   guint new_size,
					   guint old_size);
void ags_effect_bulk_resize_pads(AgsEffectBulk *effect_bulk,
				 guint new_size,
				 guint old_size);

void ags_effect_bulk_map_recall(AgsEffectBulk *effect_bulk);
GList* ags_effect_bulk_find_port(AgsEffectBulk *effect_bulk);

gboolean ags_effect_bulk_indicator_queue_draw_timeout(GtkWidget *widget);

AgsEffectBulk* ags_effect_bulk_new(AgsAudio *audio,
				   GType channel_type);

G_END_DECLS

#endif /*__AGS_EFFECT_BULK_H__*/
