/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2015 Joël Krähemann
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

#ifndef __AGS_ADD_SEQUENCER_EDITOR_JACK_H__
#define __AGS_ADD_SEQUENCER_EDITOR_JACK_H__

#include <glib.h>
#include <glib-object.h>

#include <ags/thread/ags_task.h>
#include <ags/X/ags_sequencer_editor.h>

#define AGS_TYPE_ADD_SEQUENCER_EDITOR_JACK                (ags_add_sequencer_editor_jack_get_type())
#define AGS_ADD_SEQUENCER_EDITOR_JACK(obj)                (G_TYPE_CHECK_INSTANCE_CAST((obj), AGS_TYPE_ADD_SEQUENCER_EDITOR_JACK, AgsAddSequencerEditorJack))
#define AGS_ADD_SEQUENCER_EDITOR_JACK_CLASS(class)        (G_TYPE_CHECK_CLASS_CAST((class), AGS_TYPE_ADD_SEQUENCER_EDITOR_JACK, AgsAddSequencerEditorJackClass))
#define AGS_IS_ADD_SEQUENCER_EDITOR_JACK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE((obj), AGS_TYPE_ADD_SEQUENCER_EDITOR_JACK))
#define AGS_IS_ADD_SEQUENCER_EDITOR_JACK_CLASS(class)     (G_TYPE_CHECK_CLASS_TYPE((class), AGS_TYPE_ADD_SEQUENCER_EDITOR_JACK))
#define AGS_ADD_SEQUENCER_EDITOR_JACK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS((obj), AGS_TYPE_ADD_SEQUENCER_EDITOR_JACK, AgsAddSequencerEditorJackClass))

typedef struct _AgsAddSequencerEditorJack AgsAddSequencerEditorJack;
typedef struct _AgsAddSequencerEditorJackClass AgsAddSequencerEditorJackClass;

struct _AgsAddSequencerEditorJack
{
  AgsTask task;

  AgsSequencerEditor *sequencer_editor;
};

struct _AgsAddSequencerEditorJackClass
{
  AgsTaskClass task;
};

GType ags_add_sequencer_editor_jack_get_type();

AgsAddSequencerEditorJack* ags_add_sequencer_editor_jack_new(AgsSequencerEditor *sequencer_editor);

#endif /*__AGS_ADD_SEQUENCER_EDITOR_JACK_H__*/
