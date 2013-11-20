/* AGS - Advanced GTK Sequencer
 * Copyright (C) 2005-2011 Joël Krähemann
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

#include <ags/audio/recall/ags_copy_pattern_channel.h>

#include <ags/audio/recall/ags_copy_pattern_audio.h>
#include <ags/audio/recall/ags_copy_pattern_audio_run.h>
#include <ags/audio/recall/ags_copy_pattern_channel_run.h>

void ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel);
void ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel);
void ags_copy_pattern_channel_set_property(GObject *gobject,
					   guint prop_id,
					   const GValue *value,
					   GParamSpec *param_spec);
void ags_copy_pattern_channel_get_property(GObject *gobject,
					   guint prop_id,
					   GValue *value,
					   GParamSpec *param_spec);
void ags_copy_pattern_channel_finalize(GObject *gobject);

enum{
  PROP_0,
  PROP_PATTERN,
};

static gpointer ags_copy_pattern_channel_parent_class = NULL;

GType
ags_copy_pattern_channel_get_type()
{
  static GType ags_type_copy_pattern_channel = 0;

  if(!ags_type_copy_pattern_channel){
    static const GTypeInfo ags_copy_pattern_channel_info = {
      sizeof (AgsCopyPatternChannelClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_copy_pattern_channel_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsCopyPatternChannel),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_copy_pattern_channel_init,
    };

    ags_type_copy_pattern_channel = g_type_register_static(AGS_TYPE_RECALL_CHANNEL,
							   "AgsCopyPatternChannel\0",
							   &ags_copy_pattern_channel_info,
							   0);
  }

  return(ags_type_copy_pattern_channel);
}

void
ags_copy_pattern_channel_class_init(AgsCopyPatternChannelClass *copy_pattern_channel)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;

  ags_copy_pattern_channel_parent_class = g_type_class_peek_parent(copy_pattern_channel);

  /* GObjectClass */
  gobject = (GObjectClass *) copy_pattern_channel;

  gobject->set_property = ags_copy_pattern_channel_set_property;
  gobject->get_property = ags_copy_pattern_channel_get_property;

  gobject->finalize = ags_copy_pattern_channel_finalize;

  /* properties */
  param_spec = g_param_spec_object("pattern\0",
				   "pattern to play\0",
				   "The pattern which has to be played\0",
				   AGS_TYPE_PATTERN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);
}

void
ags_copy_pattern_channel_init(AgsCopyPatternChannel *copy_pattern_channel)
{
  copy_pattern_channel->pattern = NULL;
}

void
ags_copy_pattern_channel_set_property(GObject *gobject,
				      guint prop_id,
				      const GValue *value,
				      GParamSpec *param_spec)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PATTERN:
    {
      AgsPattern *pattern;

      pattern = (AgsPattern *) g_value_get_object(value);

      if(copy_pattern_channel->pattern == pattern)
	return;

      if(copy_pattern_channel->pattern != NULL)
	g_object_unref(G_OBJECT(copy_pattern_channel->pattern));
      
      if(pattern != NULL)
	g_object_ref(G_OBJECT(pattern));

      copy_pattern_channel->pattern = pattern;
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  };
}

void
ags_copy_pattern_channel_get_property(GObject *gobject,
				      guint prop_id,
				      GValue *value,
				      GParamSpec *param_spec)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  switch(prop_id){
  case PROP_PATTERN:
    {
      g_value_set_object(value, copy_pattern_channel->pattern);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_copy_pattern_channel_finalize(GObject *gobject)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(gobject);

  if(AGS_RECALL_CHANNEL(copy_pattern_channel)->destination != NULL)
    g_object_unref(G_OBJECT(AGS_RECALL_CHANNEL(copy_pattern_channel)->destination));

  G_OBJECT_CLASS(ags_copy_pattern_channel_parent_class)->finalize(gobject);
}

GList*
ags_copy_pattern_channel_template_find_source_and_destination(GList *recall,
							      AgsChannel *destination,
							      AgsChannel *source)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  while(recall != NULL){
    recall = ags_recall_template_find_type(recall, AGS_TYPE_COPY_PATTERN_CHANNEL);

    if(recall == NULL)
      break;

    copy_pattern_channel = AGS_COPY_PATTERN_CHANNEL(recall->data);

    if(AGS_RECALL_CHANNEL(copy_pattern_channel)->destination == destination &&
       AGS_RECALL_CHANNEL(copy_pattern_channel)->source == source)
      break;

    recall = recall->next;
  }

  return(recall);
}

AgsCopyPatternChannel*
ags_copy_pattern_channel_new(AgsChannel *destination,
			     AgsChannel *source,
			     AgsPattern *pattern)
{
  AgsCopyPatternChannel *copy_pattern_channel;

  copy_pattern_channel = (AgsCopyPatternChannel *) g_object_new(AGS_TYPE_COPY_PATTERN_CHANNEL,
								"destination\0", destination,
								"channel\0", source,
								"pattern\0", pattern,
								NULL);

  return(copy_pattern_channel);
}
