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

#include <ags/audio/task/ags_toggle_pattern_bit.h>

#include <ags/i18n.h>

void ags_toggle_pattern_bit_class_init(AgsTogglePatternBitClass *toggle_pattern_bit);
void ags_toggle_pattern_bit_init(AgsTogglePatternBit *toggle_pattern_bit);
void ags_toggle_pattern_bit_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_toggle_pattern_bit_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_toggle_pattern_bit_dispose(GObject *gobject);
void ags_toggle_pattern_bit_finalize(GObject *gobject);

void ags_toggle_pattern_bit_launch(AgsTask *task);

/**
 * SECTION:ags_toggle_pattern_bit
 * @short_description: toggle the pattern
 * @title: AgsTogglePatternBit
 * @section_id:
 * @include: ags/audio/task/ags_toggle_pattern_bit.h
 *
 * The #AgsTogglePatternBit task toggles the specified #AgsPattern.
 */

enum{
  PROP_0,
  PROP_PATTERN,
  PROP_LINE,
  PROP_INDEX_I,
  PROP_INDEX_J,
  PROP_BIT,
};

static gpointer ags_toggle_pattern_bit_parent_class = NULL;

GType
ags_toggle_pattern_bit_get_type()
{
  static gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_toggle_pattern_bit = 0;

    static const GTypeInfo ags_toggle_pattern_bit_info = {
      sizeof(AgsTogglePatternBitClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_toggle_pattern_bit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsTogglePatternBit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_toggle_pattern_bit_init,
    };

    ags_type_toggle_pattern_bit = g_type_register_static(AGS_TYPE_TASK,
							 "AgsTogglePatternBit",
							 &ags_toggle_pattern_bit_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_toggle_pattern_bit);
  }

  return g_define_type_id__volatile;
}

void
ags_toggle_pattern_bit_class_init(AgsTogglePatternBitClass *toggle_pattern_bit)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  GParamSpec *param_spec;

  ags_toggle_pattern_bit_parent_class = g_type_class_peek_parent(toggle_pattern_bit);

  /* gobject */
  gobject = (GObjectClass *) toggle_pattern_bit;

  gobject->set_property = ags_toggle_pattern_bit_set_property;
  gobject->get_property = ags_toggle_pattern_bit_get_property;

  gobject->dispose = ags_toggle_pattern_bit_dispose;
  gobject->finalize = ags_toggle_pattern_bit_finalize;

  /* properties */
  /**
   * AgsTogglePatternBit:pattern:
   *
   * The assigned #AgsPattern
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_object("pattern",
				   i18n_pspec("pattern of toggle pattern bit"),
				   i18n_pspec("The pattern of toggle pattern bit task"),
				   AGS_TYPE_PATTERN,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_PATTERN,
				  param_spec);
  
  /**
   * AgsTogglePatternBit:line:
   *
   * The line.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("line",
				 i18n_pspec("line"),
				 i18n_pspec("The line"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_LINE,
				  param_spec);

  /**
   * AgsTogglePatternBit:index-i:
   *
   * The index-i.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("index-i",
				 i18n_pspec("index-i"),
				 i18n_pspec("The index-i"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX_I,
				  param_spec);

  /**
   * AgsTogglePatternBit:index-j:
   *
   * The index-j.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("index-j",
				 i18n_pspec("index-j"),
				 i18n_pspec("The index-j"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_INDEX_J,
				  param_spec);

  /**
   * AgsTogglePatternBit:bit:
   *
   * The bit.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_uint("bit",
				 i18n_pspec("bit"),
				 i18n_pspec("The bit"),
				 0,
				 G_MAXUINT,
				 0,
				 G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_BIT,
				  param_spec);

  /* task */
  task = (AgsTaskClass *) toggle_pattern_bit;

  task->launch = ags_toggle_pattern_bit_launch;
}

void
ags_toggle_pattern_bit_init(AgsTogglePatternBit *toggle_pattern_bit)
{
  toggle_pattern_bit->pattern = NULL;

  toggle_pattern_bit->line = 0;

  toggle_pattern_bit->index_i = 0;
  toggle_pattern_bit->index_j = 0;
  toggle_pattern_bit->bit = 0;
}

void
ags_toggle_pattern_bit_set_property(GObject *gobject,
					guint prop_id,
					const GValue *value,
					GParamSpec *param_spec)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(gobject);

  switch(prop_id){
  case PROP_PATTERN:
    {
      AgsPattern *pattern;

      pattern = (AgsPattern *) g_value_get_object(value);

      if(toggle_pattern_bit->pattern == pattern){
	return;
      }

      if(toggle_pattern_bit->pattern != NULL){
	g_object_unref(toggle_pattern_bit->pattern);
      }

      if(pattern != NULL){
	g_object_ref(pattern);
      }

      toggle_pattern_bit->pattern = pattern;
    }
    break;
  case PROP_LINE:
    {
      toggle_pattern_bit->line = g_value_get_uint(value);
    }
    break;
  case PROP_INDEX_I:
    {
      toggle_pattern_bit->index_i = g_value_get_uint(value);
    }
    break;
  case PROP_INDEX_J:
    {
      toggle_pattern_bit->index_j = g_value_get_uint(value);
    }
    break;
  case PROP_BIT:
    {
      toggle_pattern_bit->bit = g_value_get_uint(value);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_toggle_pattern_bit_get_property(GObject *gobject,
					guint prop_id,
					GValue *value,
					GParamSpec *param_spec)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(gobject);

  switch(prop_id){
  case PROP_PATTERN:
    {
      g_value_set_object(value, toggle_pattern_bit->pattern);
    }
    break;
  case PROP_LINE:
    {
      g_value_set_uint(value, toggle_pattern_bit->line);
    }
    break;
  case PROP_INDEX_I:
    {
      g_value_set_uint(value, toggle_pattern_bit->index_i);
    }
    break;
  case PROP_INDEX_J:
    {
      g_value_set_uint(value, toggle_pattern_bit->index_j);
    }
    break;
  case PROP_BIT:
    {
      g_value_set_uint(value, toggle_pattern_bit->bit);
    }
    break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_toggle_pattern_bit_dispose(GObject *gobject)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(gobject);

  if(toggle_pattern_bit->pattern != NULL){
    g_object_unref(toggle_pattern_bit->pattern);

    toggle_pattern_bit->pattern = NULL;
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_toggle_pattern_bit_parent_class)->dispose(gobject);
}

void
ags_toggle_pattern_bit_finalize(GObject *gobject)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(gobject);

  if(toggle_pattern_bit->pattern != NULL){
    g_object_unref(toggle_pattern_bit->pattern);
  }
  
  /* call parent */
  G_OBJECT_CLASS(ags_toggle_pattern_bit_parent_class)->finalize(gobject);
}

void
ags_toggle_pattern_bit_launch(AgsTask *task)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(task);

  /* toggle */
#ifdef AGS_DEBUG
  g_message("toggle pattern");
#endif
  
  ags_pattern_toggle_bit((AgsPattern *) toggle_pattern_bit->pattern,
			 toggle_pattern_bit->index_i, toggle_pattern_bit->index_j,
			 toggle_pattern_bit->bit);
}

/**
 * ags_toggle_pattern_bit_new:
 * @pattern: the #AgsPattern to toggle
 * @line: the affected line
 * @index_i: bank 0
 * @index_j: bank 1
 * @bit: the index within pattern
 *
 * Creates an #AgsTogglePatternBit.
 *
 * Returns: an new #AgsTogglePatternBit.
 *
 * Since: 3.0.0
 */
AgsTogglePatternBit*
ags_toggle_pattern_bit_new(AgsPattern *pattern,
			   guint line,
			   guint index_i, guint index_j,
			   guint bit)
{
  AgsTogglePatternBit *toggle_pattern_bit;
  
  toggle_pattern_bit = (AgsTogglePatternBit *) g_object_new(AGS_TYPE_TOGGLE_PATTERN_BIT,
							    "pattern", pattern,
							    "line", line,
							    "index-i", index_i,
							    "index-j", index_j,
							    "bit", bit,
							    NULL);

  return(toggle_pattern_bit);
}
