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

#include <ags/audio/osc/ags_osc_util.h>

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
xmlNode* ags_osc_parser_real_parse_bytes(AgsOscParser *osc_parser,
					 unsigned char *osc_buffer,
					 guint buffer_length);

xmlNode* ags_osc_parser_real_packet(AgsOscParser *osc_parser);

xmlNode* ags_osc_parser_real_bundle(AgsOscParser *osc_parser);

xmlNode* ags_osc_parser_real_message(AgsOscParser *osc_parser);

xmlNode* ags_osc_parser_real_value(AgsOscParser *osc_parser,
				   guint v_type);

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
  osc_parser->parse_bytes = ags_osc_parser_real_parse_bytes;

  osc_parser->packet = ags_osc_parser_real_packet;

  osc_parser->bundle = ags_osc_parser_real_bundle;

  osc_parser->packet = ags_osc_parser_real_packet;

  osc_parser->value = ags_osc_parser_real_value;

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

  /**
   * AgsOscParser::parse-full:
   * @osc_parser: the parser
   *
   * The ::parse-full signal is emited during parsing of osc file.
   *
   * Returns: The XML doc
   *
   * Since: 2.1.0
   */
  osc_parser_signals[PARSE_FULL] =
    g_signal_new("parse-full",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, parse_full),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsOscParser::parse-bytes:
   * @osc_parser: the parser
   * @buffer: the OSC data
   * @buffer_length: the buffer's length
   *
   * The ::parse-bytes signal is emited during parsing of bytes.
   *
   * Returns: The XML node representing the event
   *
   * Since: 2.1.0
   */
  osc_parser_signals[PARSE_BYTES] =
    g_signal_new("parse-bytes",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, parse_bytes),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__POINTER_UINT,
		 G_TYPE_POINTER, 2,
		 G_TYPE_POINTER,
		 G_TYPE_UINT);
  
  /**
   * AgsOscParser::packet:
   * @osc_parser: the parser
   *
   * The ::packet signal is emited during parsing.
   *
   * Returns: The XML node representing packet
   *
   * Since: 2.1.0
   */
  osc_parser_signals[PACKET] =
    g_signal_new("packet",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, packet),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsOscParser::bundle:
   * @osc_parser: the parser
   *
   * The ::bundle signal is emited during parsing.
   *
   * Returns: The XML node representing bundle
   *
   * Since: 2.1.0
   */
  osc_parser_signals[BUNDLE] =
    g_signal_new("bundle",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, bundle),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsOscParser::message:
   * @osc_parser: the parser
   *
   * The ::message signal is emited during parsing.
   *
   * Returns: The XML node representing message
   *
   * Since: 2.1.0
   */
  osc_parser_signals[MESSAGE] =
    g_signal_new("message",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, message),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__VOID,
		 G_TYPE_POINTER, 0);

  /**
   * AgsOscParser::value:
   * @osc_parser: the parser
   *
   * The ::value signal is emited during parsing.
   *
   * Returns: The XML node representing value
   *
   * Since: 2.1.0
   */
  osc_parser_signals[VALUE] =
    g_signal_new("value",
		 G_TYPE_FROM_CLASS(osc_parser),
		 G_SIGNAL_RUN_LAST,
		 G_STRUCT_OFFSET(AgsOscParserClass, value),
		 NULL, NULL,
		 ags_cclosure_marshal_POINTER__UINT,
		 G_TYPE_POINTER, 1,
		 G_TYPE_UINT);
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

/**
 * ags_osc_parser_read_gint32:
 * @osc_parser: the #AgsOscParser
 * 
 * Read 32 bit integer.
 * 
 * Returns: the gint32 read
 * 
 * Since: 2.1.0
 */
gint32
ags_osc_parser_read_gint32(AgsOscParser *osc_parser)
{
  char str[4];
  gint32 value;
  
  str[0] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[1] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[2] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[3] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);

  value = (str[0] & 0xff);
  value = (value<<8) + (str[1] & 0xff);
  value = (value<<8) + (str[2] & 0xff);
  value = (value<<8) + (str[3] & 0xff);
  
  return(value);
}

/**
 * ags_osc_parser_read_gint64:
 * @osc_parser: the #AgsOscParser
 * 
 * Read 64 bit integer.
 * 
 * Returns: the gint64 read
 * 
 * Since: 2.1.0
 */
gint64
ags_osc_parser_read_gint64(AgsOscParser *osc_parser)
{
  char str[8];
  gint64 value;
  
  str[0] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[1] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[2] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[3] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[4] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[5] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[6] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[7] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);

  value = (str[0] & 0xff);
  value = (value << 8) + (str[1] & 0xff);
  value = (value << 8) + (str[2] & 0xff);
  value = (value << 8) + (str[3] & 0xff);
  value = (value << 8) + (str[4] & 0xff);
  value = (value << 8) + (str[5] & 0xff);
  value = (value << 8) + (str[6] & 0xff);
  value = (value << 8) + (str[7] & 0xff);
  
  return(value);
}

/**
 * ags_osc_parser_read_gfloat:
 * @osc_parser: the #AgsOscParser
 * 
 * Read floating point value.
 * 
 * Returns: the gfloat read
 * 
 * Since: 2.1.0
 */
gfloat
ags_osc_parser_read_gfloat(AgsOscParser *osc_parser)
{
  char str[4];
  union{
    guint32 val;
    GFloatIEEE754 ieee_float;
  }data;
  
  str[0] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[1] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[2] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[3] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);

  data.val = (str[0] & 0xff);
  data.val = (data.val<<8) + (str[1] & 0xff);
  data.val = (data.val<<8) + (str[2] & 0xff);
  data.val = (data.val<<8) + (str[3] & 0xff);
  
  return(data.ieee_float.v_float);
}

/**
 * ags_osc_parser_read_gdouble:
 * @osc_parser: the #AgsOscParser
 * 
 * Read double precision floating point value.
 * 
 * Returns: the gdouble read
 * 
 * Since: 2.1.0
 */
gdouble
ags_osc_parser_read_gdouble(AgsOscParser *osc_parser)
{
  char str[8];
  union{
    guint64 val;
    GDoubleIEEE754 ieee_double;
  }data;
  
  str[0] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[1] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[2] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[3] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[4] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[5] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[6] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
  str[7] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);

  data.val = (str[0] & 0xff);
  data.val = (data.val << 8) + (str[1] & 0xff);
  data.val = (data.val << 8) + (str[2] & 0xff);
  data.val = (data.val << 8) + (str[3] & 0xff);
  data.val = (data.val << 8) + (str[4] & 0xff);
  data.val = (data.val << 8) + (str[5] & 0xff);
  data.val = (data.val << 8) + (str[6] & 0xff);
  data.val = (data.val << 8) + (str[7] & 0xff);
  
  return(data.ieee_double.v_double);
}

/**
 * ags_osc_parser_read_text:
 * @osc_parser: the #AgsOscParser
 * @length: the length
 * 
 * Read text.
 * 
 * Returns: the text read as string
 * 
 * Since: 2.1.0
 */
gchar*
ags_osc_parser_read_text(AgsOscParser *osc_parser,
			 gint length)
{
  gchar text[AGS_OSC_PARSER_MAX_TEXT_LENGTH + 1];

  gchar c;
  guint i;
  
  memset(text, 0, AGS_OSC_PARSER_MAX_TEXT_LENGTH * sizeof(char));
  i = 0;

  while((length <= 0 ||
	 i < length) &&
	(AGS_OSC_PARSER_EOF & (osc_parser->flags)) == 0 &&
	i < AGS_OSC_PARSER_MAX_TEXT_LENGTH){
    (c = (char) 0xff & (ags_osc_parser_osc_getc(osc_parser)));
    
    if(c == '\0'){
      break;
    }

    text[i] = c;
    i++;
  }

  text[i] = '\0';
    
  return(g_strdup(text));
}

int
ags_osc_parser_real_osc_getc(AgsOscParser *osc_parser)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_parser_osc_getc:
 * @osc_parser: the #AgsOscParser
 * 
 * Read byte.
 * 
 * Returns: the byte read
 * 
 * Since: 2.1.0
 */
int
ags_osc_parser_osc_getc(AgsOscParser *osc_parser)
{
  int c;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), '\0');
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[OSC_GETC], 0,
		&c);
  g_object_unref((GObject *) osc_parser);

  return(c);
}

void
ags_osc_parser_real_on_error(AgsOscParser *osc_parser,
			     GError **error)
{
  //TODO:JK: implement me
}

/**
 * ags_osc_parser_on_error:
 * @osc_parser: the #AgsOscParser
 * @error: the #GError-struct return location
 * 
 * On error event.
 * 
 * Since: 2.1.0
 */
void
ags_osc_parser_on_error(AgsOscParser *osc_parser,
			GError **error)
{
  g_return_if_fail(AGS_IS_OSC_PARSER(osc_parser));
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[ON_ERROR], 0,
		error);
  g_object_unref((GObject *) osc_parser);
}

xmlDoc*
ags_osc_parser_real_parse_full(AgsOscParser *osc_parser)
{
  xmlDoc *doc;
  xmlNode *root_node;
  xmlNode *packets_node;
  xmlNode *current;

  /* create xmlDoc and set root node */
  osc_parser->doc = 
    doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "osc");
  xmlDocSetRootElement(doc, root_node);

  /* create packets node */
  packets_node = xmlNewNode(NULL, "osc-packets");

  /* parse packets */
  xmlAddChild(root_node,
	      packets_node);

  osc_parser->offset = 0;
  
  while(((AGS_OSC_PARSER_EOF & (osc_parser->flags))) == 0){
    current = ags_osc_parser_packet(osc_parser);
    
    if(current != NULL){
      xmlAddChild(packets_node,
		  current);
#ifdef AGS_DEBUG
      g_message("parsed packet");
#endif
    }else{
      g_warning("skipped input");
    }
  }

  return(doc);
}

/**
 * ags_osc_parser_parse_full:
 * @osc_parser: the #AgsOscParser
 * 
 * Parse full document.
 * 
 * Returns: the parsed XML doc
 * 
 * Since: 2.1.0
 */
xmlDoc*
ags_osc_parser_parse_full(AgsOscParser *osc_parser)
{
  xmlDoc *doc;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), NULL);
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[PARSE_FULL], 0,
		&doc);
  g_object_unref((GObject *) osc_parser);

  return(doc);
}

xmlNode*
ags_osc_parser_real_parse_bytes(AgsOscParser *osc_parser,
				unsigned char *osc_buffer,
				guint buffer_length)
{
  xmlNode *node;

  node = NULL;
  
  //TODO:JK: implement me

  return(node);
}

/**
 * ags_osc_parser_parse_bytes:
 * @osc_parser: the #AgsOscParser
 * @osc_buffer: the data buffer
 * @buffer_length: the length of data buffer
 * 
 * Parse bytes.
 * 
 * Returns: the parsed XML node
 * 
 * Since: 2.1.0
 */
xmlNode*
ags_osc_parser_parse_bytes(AgsOscParser *osc_parser,
			   unsigned char *osc_buffer,
			   guint buffer_length)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), NULL);
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[PARSE_BYTES], 0,
		&node);
  g_object_unref((GObject *) osc_parser);

  return(node);
}

xmlNode*
ags_osc_parser_real_packet(AgsOscParser *osc_parser)
{
  xmlNode *node, *current;

  gsize start_offset;
  gint32 packet_size;
  gchar current_byte;
  
  node = xmlNewNode(NULL, "osc-packet");

  packet_size =
    osc_parser->packet_size = ags_osc_parser_read_gint32(osc_parser);
  xmlNewProp(node,
	     "packet-size",
	     g_strdup_printf("%u", packet_size));

  current_byte = ags_osc_parser_osc_getc(osc_parser);

  osc_parser->offset += 4;

  start_offset = 
    osc_parser->start_offset = osc_parser->offset;
  
  while(osc_parser->offset < start_offset + packet_size){
    if(current_byte == '#'){
      current = ags_osc_parser_bundle(osc_parser);
    }else if(current_byte == '/'){
      current = ags_osc_parser_message(osc_parser);
    }else{
      current = NULL;

      g_warning("failed to read bundle or message");
    }

    if(current != NULL){
      xmlAddChild(node,
		  current);
    }
  }

  return(node);
}

/**
 * ags_osc_parser_packet:
 * @osc_parser: the #AgsOscParser
 * 
 * Parse OSC packet.
 * 
 * Returns: the parsed XML node
 * 
 * Since: 2.1.0
 */
xmlNode*
ags_osc_parser_packet(AgsOscParser *osc_parser)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), NULL);
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[PACKET], 0,
		&node);
  g_object_unref((GObject *) osc_parser);

  return(node);
}

xmlNode*
ags_osc_parser_real_bundle(AgsOscParser *osc_parser)
{
  xmlNode *node, *current;

  gint32 tv_secs;
  gint32 tv_fraction;
  gboolean immediately;
  guint i;
  gchar current_byte;

  static gchar bundle[] = "#bundle";

  for(i = 0; i < 7; i++){
    gchar c;
    
    c = ags_osc_parser_osc_getc(osc_parser);

    if(c != bundle[i + 1]){
      g_warning("bad byte");
      
      return(NULL);
    }
  }

  node = xmlNewNode(NULL,
		    "osc-bundle");

  /* read time tag */
  tv_secs = ags_osc_parser_read_gint32(osc_parser);
  tv_fraction = ags_osc_parser_read_gint32(osc_parser);

  immediately = FALSE;
  
  if((0x1 & (tv_fraction)) != 0){
    immediately = TRUE;
  }

  /* add prop */
  xmlNewProp(node,
	     "tv-secs",
	     g_strdup_printf("%u", tv_secs));

  xmlNewProp(node,
	     "tv-fraction",
	     g_strdup_printf("%u", tv_fraction));

  xmlNewProp(node,
	     "immediately",
	     g_strdup_printf("%s", ((immediately) ? "true": "false")));

  while(osc_parser->offset < osc_parser->start_offset + osc_parser->packet_size){
    current_byte = ags_osc_parser_osc_getc(osc_parser);

    if(current_byte == '#'){
      current = ags_osc_parser_bundle(osc_parser);
    }else if(current_byte == '/'){
      current = ags_osc_parser_message(osc_parser);
    }else{
      current = NULL;

      g_warning("failed to read bundle or message");
    }

    if(current != NULL){
      xmlAddChild(node,
		  current);
    }
  }
  
  return(node);
}

/**
 * ags_osc_parser_bundle:
 * @osc_parser: the #AgsOscParser
 * 
 * Parse OSC bundle.
 * 
 * Returns: the parsed XML node
 * 
 * Since: 2.1.0
 */
xmlNode*
ags_osc_parser_bundle(AgsOscParser *osc_parser)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), NULL);
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[BUNDLE], 0,
		&node);
  g_object_unref((GObject *) osc_parser);

  return(node);
}

xmlNode*
ags_osc_parser_real_message(AgsOscParser *osc_parser)
{
  xmlNode *node, *current;
  
  gchar *address_pattern;
  gchar *type_tag;
  gchar *str;
  gchar current_byte;

  node = xmlNewNode(NULL,
		    "osc-message");

  str = ags_osc_parser_read_text(osc_parser,
				 -1);

  address_pattern = g_strdup_printf("/%s", str);
  g_free(str);
  
  xmlNewProp(node,
	     "address-pattern",
	     address_pattern);

  current_byte = ags_osc_parser_osc_getc(osc_parser);

  if(current_byte != ','){
    unsigned char *blob;
    
    char str[4];
    gint32 value;
    guint i;
    
    /* no type tag - provide blob */
    str[0] = (char) 0xff & current_byte;
    str[1] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
    str[2] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
    str[3] = (char) 0xff & ags_osc_parser_osc_getc(osc_parser);
    
    value = (str[0] & 0xff);
    value = (value<<8) + (str[1] & 0xff);
    value = (value<<8) + (str[2] & 0xff);
    value = (value<<8) + (str[3] & 0xff);
    
    current = xmlNewNode(NULL,
		      "osc-value");

    blob = (unsigned char *) malloc(value * sizeof(unsigned char));
    
    for(i = 0; i < value; i++){
      blob[i] = ags_osc_parser_osc_getc(osc_parser);
    }

    xmlNodeSetContent(current,
		      g_base64_encode(blob,
				      value));

    free(blob);
    
    xmlAddChild(node,
		current);
  }else{
    gchar *iter;
    
    /* has got type tag */
    str = ags_osc_parser_read_text(osc_parser,
				   -1);

    type_tag = g_strdup_printf(",%s", str);
    g_free(str);
    
    xmlNewProp(node,
	       "type-tag",
	       type_tag);

    for(iter = type_tag + 1; iter[0] != '\0'; iter++){
      switch(iter[0]){
      case AGS_OSC_UTIL_TYPE_TAG_STRING_INT32:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_FLOAT:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_STRING:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_BLOB:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_INT64:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_TIMETAG:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_DOUBLE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_SYMBOL:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_CHAR:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_RGBA:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_MIDI:
	{
	  current = ags_osc_parser_value(osc_parser,
					 iter[0]);
	  
	  if(current != NULL){
	    xmlAddChild(node,
			current);
	  }
	}
	break;
      case AGS_OSC_UTIL_TYPE_TAG_STRING_TRUE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_FALSE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_INFINITE:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_NIL:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_ARRAY_START:
      case AGS_OSC_UTIL_TYPE_TAG_STRING_ARRAY_END:
	break;
      default:
	{
	  g_warning("unknown type");
	}
      }
    }
  }

  return(node);
}

/**
 * ags_osc_parser_message:
 * @osc_parser: the #AgsOscParser
 * 
 * Parse OSC message.
 * 
 * Returns: the parsed XML node
 * 
 * Since: 2.1.0
 */
xmlNode*
ags_osc_parser_message(AgsOscParser *osc_parser)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), NULL);
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[MESSAGE], 0,
		&node);
  g_object_unref((GObject *) osc_parser);

  return(node);
}

xmlNode*
ags_osc_parser_real_value(AgsOscParser *osc_parser,
			  guint v_type)
{
  xmlNode *node;
  
  node = xmlNewNode(NULL,
		    "osc-value");

  switch(v_type){
  case AGS_OSC_UTIL_TYPE_TAG_STRING_INT32:
    {
      xmlNewProp(node,
		 "int32",
		 g_strdup_printf("%d", ags_osc_parser_read_gint32(osc_parser)));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_FLOAT:
    {
      xmlNewProp(node,
		 "float",
		 g_strdup_printf("%f", ags_osc_parser_read_gfloat(osc_parser)));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_STRING:
    {
      xmlNewProp(node,
		 "text",
		 ags_osc_parser_read_text(osc_parser,
					  -1));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_BLOB:
    {
      unsigned char *blob;
      
      gint32 data_size;
      guint i;
      
      data_size = ags_osc_parser_read_gint32(osc_parser);

      blob = (unsigned char *) malloc(data_size * sizeof(unsigned char));
      
      for(i = 0; i < data_size; i++){
	blob[i] = ags_osc_parser_osc_getc(osc_parser);
      }
      
      xmlNodeSetContent(node,
			g_base64_encode(blob,
					data_size));

      free(blob);
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_INT64:
    {
      xmlNewProp(node,
		 "int64",
		 g_strdup_printf("%d", ags_osc_parser_read_gint64(osc_parser)));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_TIMETAG:
    {
      gint32 tv_secs;
      gint32 tv_fraction;
      gboolean immediately;

      tv_secs = ags_osc_parser_read_gint32(osc_parser);
      tv_fraction = ags_osc_parser_read_gint32(osc_parser);

      immediately = (0x1 & (tv_fraction)) ? TRUE: FALSE;

      xmlNewProp(node,
		 "tv-secs",
		 g_strdup_printf("%d", tv_secs));

      xmlNewProp(node,
		 "tv-fraction",
		 g_strdup_printf("%d", tv_fraction));

      xmlNewProp(node,
		 "immediately",
		 g_strdup_printf("%s", ((immediately) ? "true": "false")));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_DOUBLE:
    {
      xmlNewProp(node,
		 "double",
		 g_strdup_printf("%f", ags_osc_parser_read_gdouble(osc_parser)));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_SYMBOL:
    {
      xmlNewProp(node,
		 "symbol",
		 ags_osc_parser_read_text(osc_parser,
					  -1));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_CHAR:
    {
      gint32 c;

      c = ags_osc_parser_read_gint32(osc_parser);

      xmlNewProp(node,
		 "char",
		 g_strdup_printf("%c", c));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_RGBA:
    {
      guint8 r, g, b, a;

      r = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      g = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      b = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      a = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      
      xmlNewProp(node,
		 "red",
		 g_strdup_printf("%u", r));

      xmlNewProp(node,
		 "green",
		 g_strdup_printf("%u", g));

      xmlNewProp(node,
		 "blue",
		 g_strdup_printf("%u", b));

      xmlNewProp(node,
		 "alpha",
		 g_strdup_printf("%u", a));
    }
    break;
  case AGS_OSC_UTIL_TYPE_TAG_STRING_MIDI:
    {
      guint8 port;
      guint8 status_byte;
      guint8 data0, data1;

      port = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      status_byte = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      data0 = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      data1 = 0xff & (ags_osc_parser_osc_getc(osc_parser));
      
      xmlNewProp(node,
		 "port",
		 g_strdup_printf("%u", port));

      xmlNewProp(node,
		 "status-byte",
		 g_strdup_printf("%u", status_byte));

      xmlNewProp(node,
		 "data0",
		 g_strdup_printf("%u", data0));

      xmlNewProp(node,
		 "data1",
		 g_strdup_printf("%u", data1));
    }
    break;
  }

  return(node);
}

/**
 * ags_osc_parser_value:
 * @osc_parser: the #AgsOscParser
 * 
 * Parse OSC value.
 * 
 * Returns: the parsed XML node
 * 
 * Since: 2.1.0
 */
xmlNode*
ags_osc_parser_value(AgsOscParser *osc_parser,
		     guint v_type)
{
  xmlNode *node;
  
  g_return_val_if_fail(AGS_IS_OSC_PARSER(osc_parser), NULL);
  
  g_object_ref((GObject *) osc_parser);
  g_signal_emit(G_OBJECT(osc_parser),
		osc_parser_signals[VALUE], 0,
		v_type,
		&node);
  g_object_unref((GObject *) osc_parser);

  return(node);
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
