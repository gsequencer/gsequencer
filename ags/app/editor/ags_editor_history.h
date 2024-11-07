/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2024 Joël Krähemann
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

#ifndef __AGS_EDITOR_HISTORY_H__
#define __AGS_EDITOR_HISTORY_H__

#include <glib.h>
#include <glib-object.h>

#include <gio/gio.h>

#include <libxml/tree.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <gtk/gtk.h>

#include <ags/libags.h>
#include <ags/libags-audio.h>

#include <ags/libags-gui.h>

G_BEGIN_DECLS

#define AGS_TYPE_EDITOR_HISTORY                (ags_editor_history_get_type())
#define AGS_EDITOR_HISTORY(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_EDITOR_HISTORY, AgsEditorHistory))
#define AGS_EDITOR_HISTORY_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_EDITOR_HISTORY, AgsEditorHistoryClass))
#define AGS_IS_EDITOR_HISTORY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), AGS_TYPE_EDITOR_HISTORY))
#define AGS_IS_EDITOR_HISTORY_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE ((class), AGS_TYPE_EDITOR_HISTORY))
#define AGS_EDITOR_HISTORY_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS (obj, AGS_TYPE_EDITOR_HISTORY, AgsEditorHistoryClass))

#define AGS_EXTERN_DATA(ptr) ((AgsExternData *)(ptr))
#define AGS_EDITOR_JOURNAL(ptr) ((AgsEditorJournal *)(ptr))

#define AGS_EDITOR_HISTORY_SCOPE_NOTATION "ags-notation"
#define AGS_EDITOR_HISTORY_SCOPE_AUTOMATION "ags-automation"
#define AGS_EDITOR_HISTORY_SCOPE_WAVE "ags-wave"

#define AGS_EDITOR_HISTORY_JOURNAL_TYPE_EDIT "edit"
#define AGS_EDITOR_HISTORY_JOURNAL_TYPE_UNDO "undo"
#define AGS_EDITOR_HISTORY_JOURNAL_TYPE_REDO "redo"

#define AGS_EDITOR_HISTORY_ACTION_NOTE_ADD "add-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_RESIZE "resize-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_REMOVE "remove-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_COPY "copy-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_CUT "cut-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_PASTE "paste-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_INVERT "invert-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_SELECT "select-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_POSITION "position-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_MOVE "move-note"
#define AGS_EDITOR_HISTORY_ACTION_NOTE_CROP "crop-note"

#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_ADD "add-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_RESIZE "resize-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_REMOVE "remove-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_COPY "copy-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_CUT "cut-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_PASTE "paste-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_SELECT "select-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_POSITION "position-acceleration"
#define AGS_EDITOR_HISTORY_ACTION_ACCELERATION_RAMP "ramp-acceleration"

#define AGS_EDITOR_HISTORY_ACTION_BUFFER_ADD "add-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_REMOVE "remove-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_COPY "copy-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_CUT "cut-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_PASTE "paste-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_SELECT "select-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_POSITION "position-buffer"
#define AGS_EDITOR_HISTORY_ACTION_BUFFER_TIME_STRETCH "time-stretch-buffer"

#define AGS_EDITOR_HISTORY_DATA_ACCESS_XML_SERIALIZER "xml-serializer"
#define AGS_EDITOR_HISTORY_DATA_ACCESS_RAW_AUDIO_BASE64_SERIALIZER "raw-audio-base64-serializer"

typedef struct _AgsEditorHistory AgsEditorHistory;
typedef struct _AgsEditorHistoryClass AgsEditorHistoryClass;
typedef struct _AgsExternData AgsExternData;
typedef struct _AgsEditorJournal AgsEditorJournal;

struct _AgsEditorHistory
{
  GObject gobject;

  gchar *encoding;
  
  gint journal_position;
  gint edit_position;

  GList *data_entry;
  GList *journal_entry;
};

struct _AgsEditorHistoryClass
{
  GObjectClass gobject;
};

struct _AgsExternData
{
  gint ref_count;
  
  gchar *data_uuid;

  gsize data_length;
  gchar *data;
};

struct _AgsEditorJournal
{
  gchar *selected_machine_uuid;
  GtkWidget *selected_machine;

  guint pad;
  guint audio_channel;
  guint line;
  
  gchar *scope;
  gchar *journal_type;

  gchar *action;
  gchar *detail;

  gchar *data_access_type;
  gchar *content_type;

  guint64 orig_data_offset;
  gsize orig_data_length;
  gchar *orig_data;

  AgsExternData *extern_orig_data;
  
  guint64 new_data_offset;
  gsize new_data_length;
  gchar *new_data;

  AgsExternData *extern_new_data;
};

GType ags_editor_history_get_type(void);

AgsExternData* ags_extern_data_alloc();
void ags_extern_data_free(AgsExternData *extern_data);

gint ags_extern_data_cmp(AgsExternData *a,
			 AgsExternData *b);

AgsEditorJournal* ags_editor_journal_alloc();
void ags_editor_journal_free(AgsEditorJournal *editor_journal);

void ags_editor_history_append(AgsEditorHistory *editor_history,
			       AgsEditorJournal *editor_journal);

void ags_editor_history_undo(AgsEditorHistory *editor_history);
void ags_editor_history_redo(AgsEditorHistory *editor_history);

void ags_editor_history_release_unused(AgsEditorHistory *editor_history);

void ags_editor_history_export_to_path(AgsEditorHistory *editor_history,
				       gchar *location);
void ags_editor_history_import_from_path(AgsEditorHistory *editor_history,
					 gchar *location);

/*  */
AgsEditorHistory* ags_editor_history_get_instance();

AgsEditorHistory* ags_editor_history_new();

G_END_DECLS

#endif /*__AGS_EDITOR_HISTORY_H__*/
