/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2015 Joël Krähemann
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

#include <ags/object/ags_connectable.h>

#include <ags/i18n.h>

void ags_toggle_pattern_bit_class_init(AgsTogglePatternBitClass *toggle_pattern_bit);
void ags_toggle_pattern_bit_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_toggle_pattern_bit_init(AgsTogglePatternBit *toggle_pattern_bit);
void ags_toggle_pattern_bit_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_toggle_pattern_bit_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_toggle_pattern_bit_connect(AgsConnectable *connectable);
void ags_toggle_pattern_bit_disconnect(AgsConnectable *connectable);
void ags_toggle_pattern_bit_finalize(GObject *gobject);

void ags_toggle_pattern_bit_launch(AgsTask *task);

/**
 * SECTION:ags_toggle_pattern_bit
 * @short_description: toggles the pattern
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

enum{
  REFRESH_GUI,
  LAST_SIGNAL,
};

static gpointer ags_toggle_pattern_bit_parent_class = NULL;
static AgsConnectableInterface *ags_toggle_pattern_bit_parent_connectable_interface;
static guint toggle_pattern_bit_signals[LAST_SIGNAL];

GType
ags_toggle_pattern_bit_get_type()
{
  static GType ags_type_toggle_pattern_bit = 0;

  if(!ags_type_toggle_pattern_bit){
    static const GTypeInfo ags_toggle_pattern_bit_info = {
      sizeof (AgsTogglePatternBitClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_toggle_pattern_bit_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTogglePatternBit),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_toggle_pattern_bit_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_toggle_pattern_bit_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_toggle_pattern_bit = g_type_register_static(AGS_TYPE_TASK,
							 "AgsTogglePatternBit",
							 &ags_toggle_pattern_bit_info,
							 0);

    g_type_add_interface_static(ags_type_toggle_pattern_bit,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_toggle_pattern_bit);
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

  gobject->finalize = ags_toggle_pattern_bit_finalize;

  /* properties */
  /**
   * AgsTogglePatternBit:pattern:
   *
   * The assigned #AgsPattern
   * 
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 0.7.117
>>>>>>> master
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
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 0.7.117
>>>>>>> master
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
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 0.7.117
>>>>>>> master
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
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 0.7.117
>>>>>>> master
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
<<<<<<< HEAD
   * Since: 1.0.0
=======
   * Since: 0.7.117
>>>>>>> master
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

  /* signals */
  toggle_pattern_bit_signals[REFRESH_GUI] =
    g_signal_new("refresh-gui",
		 G_TYPE_FROM_CLASS (toggle_pattern_bit),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET (AgsTogglePatternBitClass, refresh_gui),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__VOID,
		 G_TYPE_NONE, 0);

  /* task */
  task = (AgsTaskClass *) toggle_pattern_bit;

  task->launch = ags_toggle_pattern_bit_launch;

  /* toggle pattern bit */
  toggle_pattern_bit->refresh_gui = NULL;
}

void
ags_toggle_pattern_bit_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_toggle_pattern_bit_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_toggle_pattern_bit_connect;
  connectable->disconnect = ags_toggle_pattern_bit_disconnect;
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

      if(toggle_pattern_bit->pattern == (GObject *) pattern){
	return;
      }

      if(toggle_pattern_bit->pattern != NULL){
	g_object_unref(toggle_pattern_bit->pattern);
      }

      if(pattern != NULL){
	g_object_ref(pattern);
      }

      toggle_pattern_bit->pattern = (GObject *) pattern;
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
ags_toggle_pattern_bit_connect(AgsConnectable *connectable)
{
  ags_toggle_pattern_bit_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_toggle_pattern_bit_disconnect(AgsConnectable *connectable)
{
  ags_toggle_pattern_bit_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_toggle_pattern_bit_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_toggle_pattern_bit_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_toggle_pattern_bit_launch(AgsTask *task)
{
  AgsTogglePatternBit *toggle_pattern_bit;

  toggle_pattern_bit = AGS_TOGGLE_PATTERN_BIT(task);

#ifdef AGS_DEBUG
  g_message("toggle pattern");
#endif
  
  ags_pattern_toggle_bit((AgsPattern *) toggle_pattern_bit->pattern,
			 toggle_pattern_bit->index_i, toggle_pattern_bit->index_j,
			 toggle_pattern_bit->bit);

  ags_toggle_pattern_bit_refresh_gui(toggle_pattern_bit);
}

void
ags_toggle_pattern_bit_refresh_gui(AgsTogglePatternBit *toggle_pattern_bit)
{
  g_return_if_fail(AGS_IS_TOGGLE_PATTERN_BIT(toggle_pattern_bit));

  g_object_ref(G_OBJECT(toggle_pattern_bit));
  g_signal_emit(G_OBJECT(toggle_pattern_bit),
		toggle_pattern_bit_signals[REFRESH_GUI], 0);
  g_object_unref(G_OBJECT(toggle_pattern_bit));
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
 * Since: 0.4
 */
AgsTogglePatternBit*
ags_toggle_pattern_bit_new(AgsPattern *pattern,
			   guint line,
			   guint index_i, guint index_j,
			   guint bit)
{
  AgsTogglePatternBit *toggle_pattern_bit;
  
  toggle_pattern_bit = (AgsTogglePatternBit *) g_object_new(AGS_TYPE_TOGGLE_PATTERN_BIT,
							    NULL);

  toggle_pattern_bit->pattern = pattern;
  toggle_pattern_bit->line = line;

  toggle_pattern_bit->index_i = index_i;
  toggle_pattern_bit->index_j = index_j;
  toggle_pattern_bit->bit = bit;

  return(toggle_pattern_bit);
}
