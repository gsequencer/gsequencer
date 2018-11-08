/* GSequencer - Advanced GTK Sequencer
 * Copyright (C) 2005-2018 Joël Krähemann
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

#include <ags/audio/osc/ags_osc_parser.h>

#include <ags/libags.h>

#include <string.h>

void ags_osc_parser_class_init(AgsOscParserClass *osc_parser);
void ags_osc_parser_init(AgsOscParser *osc_parser);
void ags_osc_parser_set_property(GObject *gobject,
				 guint prop_id,
				 const GValue *value,
				 GParamSpec *param_spec);
void ags_osc_parser_get_property(GObject *gobject,
				 guint prop_id,
				 GValue *value,
				 GParamSpec *param_spec);
void ags_osc_parser_finalize(GObject *gobject);

int ags_osc_parser_real_osc_getc(AgsOscParser *osc_parser);
void ags_osc_parser_real_on_error(AgsOscParser *osc_parser,
				  GError **error);

xmlDoc* ags_osc_parser_real_parse_full(AgsOscParser *osc_parser);

/**
 * SECTION:ags_osc_parser
 * @short_description: the menu bar.
 * @title: AgsOscParser
 * @section_id:
 * @include: ags/audio/osc/ags_osc_parser.h
 *
 * #AgsOscParser reads your osc parsers.
 */

enum{
  PROP_0,
};

enum{
  OSC_GETC,
  ON_ERROR,
  PARSE_FULL,
  PARSE_BYTES,
  PACKET,
  BUNDLE,
  MESSAGE,
  VALUE,
  LAST_SIGNAL,
};

static gpointer ags_osc_parser_parent_class = NULL;
static guint osc_parser_signals[LAST_SIGNAL];

static pthread_mutex_t ags_osc_parser_class_mutex = PTHREAD_MUTEX_INITIALIZER;

GType
ags_osc_parser_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_osc_parser = 0;

    static const GTypeInfo ags_osc_parser_info = {
      sizeof (AgsOscParserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_osc_parser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsOscParser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_osc_parser_init,
    };

    ags_type_osc_parser = g_type_register_static(G_TYPE_OBJECT,
						 "AgsOscParser", &ags_osc_parser_info,
						 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_osc_parser);
  }

  return g_define_type_id__volatile;
}

void
ags_osc_parser_class_init(AgsOscParserClass *osc_parser)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_osc_parser_parent_class = g_type_class_peek_parent(osc_parser);

  /* GObjectClass */
  gobject = (GObjectClass *) osc_parser;

  gobject->set_property = ags_osc_parser_set_property;
  gobject->get_property = ags_osc_parser_get_property;
  
  gobject->finalize = ags_osc_parser_finalize;

  /* AgsOscParser */
  osc_parser->osc_getc = ags_osc_parser_real_osc_getc;
  osc_parser->on_error = ags_osc_parser_real_on_error;

  osc_parser->parse_full = ags_osc_parser_real_parse_full;

  /* signals */
  /**
   * AgsOscParser::osc-getc:
   * @osc_parser: the parser
   *
   * The ::osc-getc signal is emited during parsing of event.
   *
   * Returns: The character read
   *
   * Since: 2.1.0
   */
  osc_parser_signals[OSC_GETC] =
    g_signal_new("osc-getc",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, osc_getc),
		 NULL, NULL,
		 ags_cclosure_marshal_INT__VOID,
		 G_TYPE_INT, 0);

  /**
   * AgsOscParser::on-error:
   * @osc_parser: the parser
   *
   * The ::on-error signal is emited as error occurs.
   *
   * Since: 2.1.0
   */
  osc_parser_signals[ON_ERROR] =
    g_signal_new("on-error",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, on_error),
		 NULL, NULL,
		 g_cclosure_marshal_VOID__POINTER,
		 G_TYPE_NONE, 1,
		 G_TYPE_POINTER);
}

void
ags_osc_parser_init(AgsOscParser *osc_parser)
{
  osc_parser->flags = 0;
  
  /* osc parser mutex */
  osc_parser->obj_mutexattr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t));
  pthread_mutexattr_init(osc_parser->obj_mutexattr);
  pthread_mutexattr_settype(osc_parser->obj_mutexattr,
			    PTHREAD_MUTEX_RECURSIVE);

#ifdef __linux__
  pthread_mutexattr_setprotocol(osc_parser->obj_mutexattr,
				PTHREAD_PRIO_INHERIT);
#endif

  osc_parser->obj_mutex =  (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(osc_parser->obj_mutex,
		     osc_parser->obj_mutexattr);

  osc_parser->offset = 0;

  osc_parser->doc = NULL;
}

void
ags_osc_parser_set_property(GObject *gobject,
			    guint prop_id,
			    const GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscParser *osc_parser;

  pthread_mutex_t *osc_parser_mutex;

  osc_parser = AGS_OSC_PARSER(gobject);

  /* get osc parser mutex */
  pthread_mutex_lock(ags_osc_parser_get_class_mutex());
  
  osc_parser_mutex = osc_parser->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_parser_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_parser_get_property(GObject *gobject,
			    guint prop_id,
			    GValue *value,
			    GParamSpec *param_spec)
{
  AgsOscParser *osc_parser;

  pthread_mutex_t *osc_parser_mutex;

  osc_parser = AGS_OSC_PARSER(gobject);

  /* get osc parser mutex */
  pthread_mutex_lock(ags_osc_parser_get_class_mutex());
  
  osc_parser_mutex = osc_parser->obj_mutex;
  
  pthread_mutex_unlock(ags_osc_parser_get_class_mutex());
  
  switch(prop_id){
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_osc_parser_finalize(GObject *gobject)
{
  AgsOscParser *osc_parser;
    
  osc_parser = (AgsOscParser *) gobject;

  pthread_mutex_destroy(osc_parser->obj_mutex);
  free(osc_parser->obj_mutex);

  pthread_mutexattr_destroy(osc_parser->obj_mutexattr);
  free(osc_parser->obj_mutexattr);

  /* call parent */
  G_OBJECT_CLASS(ags_osc_parser_parent_class)->finalize(gobject);
}

/**
 * ags_osc_parser_get_class_mutex:
 * 
 * Use this function's returned mutex to access mutex fields.
 *
 * Returns: the class mutex
 * 
 * Since: 2.1.0
 */
pthread_mutex_t*
ags_osc_parser_get_class_mutex()
{
  return(&ags_osc_parser_class_mutex);
}

int
ags_osc_parser_real_osc_getc(AgsOscParser *osc_parser)
{
  //TODO:JK: implement me
}

void
ags_osc_parser_real_on_error(AgsOscParser *osc_parser,
			     GError **error)
{
  //TODO:JK: implement me
}

xmlDoc*
ags_osc_parser_real_parse_full(AgsOscParser *osc_parser)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_parser_new:
 * 
 * Creates a new instance of #AgsOscParser
 *
 * Returns: the new #AgsOscParser
 * 
 * Since: 2.1.0
 */
AgsOscParser*
ags_osc_parser_new()
{
  AgsOscParser *osc_parser;
   
  osc_parser = (AgsOscParser *) g_object_new(AGS_TYPE_OSC_PARSER,
					     NULL);
  
  return(osc_parser);
}
