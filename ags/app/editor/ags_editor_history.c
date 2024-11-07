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

#include <ags/app/editor/ags_editor_history.h>

void ags_editor_history_class_init(AgsEditorHistoryClass *editor_history);
void ags_editor_history_init (AgsEditorHistory *editor_history);
void ags_editor_history_dispose(GObject *gobject);
void ags_editor_history_finalize(GObject *gobject);

/**
 * SECTION:ags_editor_history
 * @short_description: Singleton pattern to organize LADSPA
 * @title: AgsEditorHistory
 * @section_id:
 * @include: ags/plugin/ags_editor_history.h
 *
 * The #AgsEditorHistory loads/unloads LADSPA plugins.
 */

static gpointer ags_editor_history_parent_class = NULL;

AgsEditorHistory *ags_editor_history = NULL;

GType
ags_editor_history_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_editor_history = 0;

    static const GTypeInfo ags_editor_history_info = {
      sizeof (AgsEditorHistoryClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_editor_history_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsEditorHistory),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_editor_history_init,
    };

    ags_type_editor_history = g_type_register_static(G_TYPE_OBJECT,
						     "AgsEditorHistory",
						     &ags_editor_history_info,
						     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_editor_history);
  }

  return g_define_type_id__volatile;
}

void
ags_editor_history_class_init(AgsEditorHistoryClass *editor_history)
{
  GObjectClass *gobject;

  ags_editor_history_parent_class = g_type_class_peek_parent(editor_history);

  /* GObjectClass */
  gobject = (GObjectClass *) editor_history;

  gobject->dispose = ags_editor_history_dispose;
  gobject->finalize = ags_editor_history_finalize;
}

void
ags_editor_history_init(AgsEditorHistory *editor_history)
{
  editor_history->journal_position = -1;

  editor_history->journal_entry = NULL;
}

void
ags_editor_history_dispose(GObject *gobject)
{
  AgsEditorHistory *editor_history;

  editor_history = AGS_EDITOR_HISTORY(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_editor_history_parent_class)->dispose(gobject);
}

void
ags_editor_history_finalize(GObject *gobject)
{
  AgsEditorHistory *editor_history;
  
  editor_history = AGS_EDITOR_HISTORY(gobject);
  
  /* call parent */
  G_OBJECT_CLASS(ags_editor_history_parent_class)->finalize(gobject);
}

AgsEditorJournal*
ags_editor_journal_alloc()
{
  AgsEditorJournal *editor_journal;
  
  editor_journal = (AgsEditorJournal *) g_malloc(sizeof(AgsEditorJournal));

  editor_journal->selected_machine = NULL;

  editor_journal->scope = NULL;
  editor_journal->journal_type = NULL;

  editor_journal->action = NULL;
  editor_journal->detail = NULL;

  editor_journal->data_access_type = NULL;
  editor_journal->content_type = NULL;

  editor_journal->orig_data = NULL;
  editor_journal->new_data = NULL;

  return(editor_journal);
}

void
ags_editor_journal_free(AgsEditorJournal *editor_journal)
{
  g_return_if_fail(editor_journal != NULL);

  g_free(editor_journal);
}

void
ags_editor_history_append(AgsEditorHistory *editor_history,
			  AgsEditorJournal *editor_journal)
{
  g_return_if_fail(AGS_IS_EDITOR_HISTORY(editor_history));
  g_return_if_fail(editor_journal != NULL);
  
  editor_history->journal_entry = g_list_prepend(editor_history->journal_entry,
						 editor_journal);
}

void
ags_editor_history_export_to_data(AgsEditorHistory *editor_history,
				  gchar **exported_data,
				  guint *data_length)
{
  //TODO:JK: implement me
}

void
ags_editor_history_import_from_data(AgsEditorHistory *editor_history,
				    gchar *exported_data,
				    guint data_length)
{
  //TODO:JK: implement me
}

/**
 * ags_editor_history_get_instance:
 *
 * Get instance.
 *
 * Returns: (transfer none): the #AgsEditorHistory
 *
 * Since: 7.2.0
 */
AgsEditorHistory*
ags_editor_history_get_instance()
{
  if(ags_editor_history == NULL){
    ags_editor_history = ags_editor_history_new();
  }

  return(ags_editor_history);
}

/**
 * ags_editor_history_new:
 *
 * Create a new instance of #AgsEditorHistory
 *
 * Returns: the new #AgsEditorHistory
 *
 * Since: 7.2.0
 */
AgsEditorHistory*
ags_editor_history_new()
{
  AgsEditorHistory *editor_history;

  editor_history = (AgsEditorHistory *) g_object_new(AGS_TYPE_EDITOR_HISTORY,
						     NULL);

  return(editor_history);
}

