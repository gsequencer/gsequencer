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

#include <ags/lib/ags_turtle.h>

#include <ags/lib/ags_regex.h>

#include <gmodule.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <regex.h>

#include <sys/stat.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xlink.h>
#include <libxml/xpath.h>

#include <ags/i18n.h>

/**
 * SECTION:ags_turtle
 * @short_description: Terse RDF Triple Language
 * @title: AgsTurtle
 * @section_id:
 * @include: ags/lib/ags_turtle.h
 *
 * The #AgsTurtle object converts Terse RDF Triple Language files
 * into XML.
 */

void ags_turtle_class_init(AgsTurtleClass *turtle);
void ags_turtle_init (AgsTurtle *turtle);
void ags_turtle_set_property(GObject *gobject,
			     guint prop_id,
			     const GValue *value,
			     GParamSpec *param_spec);
void ags_turtle_get_property(GObject *gobject,
			     guint prop_id,
			     GValue *value,
			     GParamSpec *param_spec);
void ags_turtle_finalize(GObject *gobject);

gchar* ags_turtle_load_skip_comments_and_blanks(AgsTurtle *turtle,
						gchar *buffer, gsize buffer_length,
						gchar **iter);
xmlNode* ags_turtle_load_read_iriref(AgsTurtle *turtle,
				     gchar *buffer, gsize buffer_length,
				     gchar **iter);
xmlNode* ags_turtle_load_read_anon(AgsTurtle *turtle,
				   gchar *buffer, gsize buffer_length,
				   gchar **iter);
xmlNode* ags_turtle_load_read_pname_ns(AgsTurtle *turtle,
				       gchar *buffer, gsize buffer_length,
				       gchar **iter);
xmlNode* ags_turtle_load_read_pname_ln(AgsTurtle *turtle,
				       gchar *buffer, gsize buffer_length,
				       gchar **iter);
xmlNode* ags_turtle_load_read_numeric(AgsTurtle *turtle,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter);
xmlNode* ags_turtle_load_read_string(AgsTurtle *turtle,
				     gchar *buffer, gsize buffer_length,
				     gchar **iter);
xmlNode* ags_turtle_load_read_langtag(AgsTurtle *turtle,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter);
xmlNode* ags_turtle_load_read_literal(AgsTurtle *turtle,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter);
xmlNode* ags_turtle_load_read_iri(AgsTurtle *turtle,
				  gchar *buffer, gsize buffer_length,
				  gchar **iter);
xmlNode* ags_turtle_load_read_prefix_id(AgsTurtle *turtle,
					gchar *buffer, gsize buffer_length,
					gchar **iter);
xmlNode* ags_turtle_load_read_base(AgsTurtle *turtle,
				   gchar *buffer, gsize buffer_length,
				   gchar **iter);
xmlNode* ags_turtle_load_read_sparql_prefix(AgsTurtle *turtle,
					    gchar *buffer, gsize buffer_length,
					    gchar **iter);
xmlNode* ags_turtle_load_read_sparql_base(AgsTurtle *turtle,
					  gchar *buffer, gsize buffer_length,
					  gchar **iter);
xmlNode* ags_turtle_load_read_prefixed_name(AgsTurtle *turtle,
					    gchar *buffer, gsize buffer_length,
					    gchar **iter);
xmlNode* ags_turtle_load_read_blank_node(AgsTurtle *turtle,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter);
xmlNode* ags_turtle_load_read_statement(AgsTurtle *turtle,
					gchar *buffer, gsize buffer_length,
					gchar **iter);
xmlNode* ags_turtle_load_read_verb(AgsTurtle *turtle,
				   gchar *buffer, gsize buffer_length,
				   gchar **iter);
xmlNode* ags_turtle_load_read_predicate(AgsTurtle *turtle,
					gchar *buffer, gsize buffer_length,
					gchar **iter);
xmlNode* ags_turtle_load_read_object(AgsTurtle *turtle,
				     gchar *buffer, gsize buffer_length,
				     gchar **iter);
xmlNode* ags_turtle_load_read_directive(AgsTurtle *turtle,
					gchar *buffer, gsize buffer_length,
					gchar **iter);
xmlNode* ags_turtle_load_read_triple(AgsTurtle *turtle,
				     gchar *buffer, gsize buffer_length,
				     gchar **iter);
xmlNode* ags_turtle_load_read_subject(AgsTurtle *turtle,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter);
xmlNode* ags_turtle_load_read_object_list(AgsTurtle *turtle,
					  gchar *buffer, gsize buffer_length,
					  gchar **iter);
xmlNode* ags_turtle_load_read_collection(AgsTurtle *turtle,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter);
xmlNode* ags_turtle_load_read_blank_node_property_list(AgsTurtle *turtle,
						       gchar *buffer, gsize buffer_length,
						       gchar **iter);
xmlNode* ags_turtle_load_read_predicate_object_list(AgsTurtle *turtle,
						    gchar *buffer, gsize buffer_length,
						    gchar **iter);

enum{
  PROP_0,
  PROP_FILENAME,
  PROP_XML_DOC,
};

static gpointer ags_turtle_parent_class = NULL;

static GMutex regex_mutex;

GType
ags_turtle_get_type(void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_turtle = 0;

    static const GTypeInfo ags_turtle_info = {
      sizeof (AgsTurtleClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_turtle_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsTurtle),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_turtle_init,
    };

    ags_type_turtle = g_type_register_static(G_TYPE_OBJECT,
					     "AgsTurtle",
					     &ags_turtle_info,
					     0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_turtle);
  }

  return g_define_type_id__volatile;
}

void
ags_turtle_class_init(AgsTurtleClass *turtle)
{
  GObjectClass *gobject;
  GParamSpec *param_spec;
  
  ags_turtle_parent_class = g_type_class_peek_parent(turtle);

  /* GObjectClass */
  gobject = (GObjectClass *) turtle;

  gobject->set_property = ags_turtle_set_property;
  gobject->get_property = ags_turtle_get_property;
  
  gobject->finalize = ags_turtle_finalize;

  /* properties */
  /**
   * AgsTurtle:filename:
   *
   * The assigned filename.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_string("filename",
				   i18n_pspec("filename of turtle"),
				   i18n_pspec("The filename this turtle is assigned to"),
				   NULL,
				   G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_FILENAME,
				  param_spec);

  /**
   * AgsTurtle:xml-doc:
   *
   * The assigned xml-doc.
   * 
   * Since: 3.0.0
   */
  param_spec = g_param_spec_pointer("xml-doc",
				    i18n_pspec("xml document of turtle"),
				    i18n_pspec("The xml document turtle was converted to"),
				    G_PARAM_READABLE | G_PARAM_WRITABLE);
  g_object_class_install_property(gobject,
				  PROP_XML_DOC,
				  param_spec);
}

void
ags_turtle_init(AgsTurtle *turtle)
{
  turtle->flags = AGS_TURTLE_TOLOWER;

  /* add turtle mutex */
  g_rec_mutex_init(&(turtle->obj_mutex));

  /* fields */
  turtle->filename = NULL;

  turtle->doc = NULL;

  turtle->prefix_id = g_hash_table_new_full(g_str_hash, g_str_equal,
					    g_free,
					    g_free);
}

void
ags_turtle_set_property(GObject *gobject,
			guint prop_id,
			const GValue *value,
			GParamSpec *param_spec)
{
  AgsTurtle *turtle;

  GRecMutex *turtle_mutex;

  turtle = AGS_TURTLE(gobject);

  /* get turtle mutex */
  turtle_mutex = AGS_TURTLE_GET_OBJ_MUTEX(turtle);

  switch(prop_id){
  case PROP_FILENAME:
  {
    gchar *filename;

    filename = (gchar *) g_value_get_string(value);

    g_rec_mutex_lock(turtle_mutex);

    if(turtle->filename == filename){
      g_rec_mutex_unlock(turtle_mutex);

      return;
    }
      
    if(turtle->filename != NULL){
      g_free(turtle->filename);
    }

    turtle->filename = g_strdup(filename);

    g_rec_mutex_unlock(turtle_mutex);
  }
  break;
  case PROP_XML_DOC:
  {
    xmlDoc *doc;

    doc = (xmlDoc *) g_value_get_pointer(value);

    g_rec_mutex_lock(turtle_mutex);
      
    turtle->doc = doc;

    g_rec_mutex_unlock(turtle_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_turtle_get_property(GObject *gobject,
			guint prop_id,
			GValue *value,
			GParamSpec *param_spec)
{
  AgsTurtle *turtle;

  GRecMutex *turtle_mutex;

  turtle = AGS_TURTLE(gobject);

  /* get turtle mutex */
  turtle_mutex = AGS_TURTLE_GET_OBJ_MUTEX(turtle);

  switch(prop_id){
  case PROP_FILENAME:
  {
    g_rec_mutex_lock(turtle_mutex);

    g_value_set_string(value, turtle->filename);

    g_rec_mutex_unlock(turtle_mutex);
  }
  break;
  case PROP_XML_DOC:
  {
    g_rec_mutex_lock(turtle_mutex);

    g_value_set_pointer(value, turtle->doc);

    g_rec_mutex_unlock(turtle_mutex);
  }
  break;
  default:
    G_OBJECT_WARN_INVALID_PROPERTY_ID(gobject, prop_id, param_spec);
    break;
  }
}

void
ags_turtle_finalize(GObject *gobject)
{
  AgsTurtle *turtle;
  gchar **str;
  
  turtle = AGS_TURTLE(gobject);

  if(turtle->filename != NULL){
    g_free(turtle->filename);
  }

  if(turtle->doc != NULL){
    xmlFreeDoc(turtle->doc);
  }

  g_hash_table_destroy(turtle->prefix_id);
  
  /* call parent */
  G_OBJECT_CLASS(ags_turtle_parent_class)->finalize(gobject);
}

/**
 * ags_turtle_read_iriref:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read iriref value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_iriref(gchar *offset,
		       gchar *end_ptr)
{
  gchar *str;
  gchar *iriref_start_offset, *iriref_end_offset;

  str = NULL;
  
  iriref_start_offset = NULL;
  iriref_end_offset = NULL;
  
  if(ags_turtle_match_iriref(offset,
			     end_ptr,
			     &iriref_start_offset, &iriref_end_offset)){
    str = g_strndup(offset,
		    iriref_end_offset - offset);

#ifdef AGS_DEBUG
    g_message("iriref %s", str);
#endif
  }
  
  return(str);
}

/**
 * ags_turtle_read_pname_ns:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name namespace value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pname_ns(gchar *offset,
			 gchar *end_ptr)
{
  gchar *str;
  gchar *pname_ns_start_offset, *pname_ns_end_offset;

  str = NULL;
  
  pname_ns_start_offset = NULL;
  pname_ns_end_offset = NULL;
  
  if(ags_turtle_match_pname_ns(offset,
			       end_ptr,
			       &pname_ns_start_offset, &pname_ns_end_offset)){
    str = g_strndup(offset,
		    pname_ns_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pname_ln:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name localized name value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pname_ln(gchar *offset,
			 gchar *end_ptr)
{
  gchar *str;
  gchar *pname_ln_start_offset, *pname_ln_end_offset;

  str = NULL;
  
  pname_ln_start_offset = NULL;
  pname_ln_end_offset = NULL;
  
  if(ags_turtle_match_pname_ln(offset,
			       end_ptr,
			       &pname_ln_start_offset, &pname_ln_end_offset)){
    str = g_strndup(offset,
		    pname_ln_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_blank_node_label:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read blank node label value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_blank_node_label(gchar *offset,
				 gchar *end_ptr)
{
  gchar *str;
  gchar *blank_node_label_start_offset, *blank_node_label_end_offset;

  str = NULL;
  
  blank_node_label_start_offset = NULL;
  blank_node_label_end_offset = NULL;
  
  if(ags_turtle_match_blank_node_label(offset,
				       end_ptr,
				       &blank_node_label_start_offset, &blank_node_label_end_offset)){
    str = g_strndup(offset,
		    blank_node_label_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_langtag:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read langtag value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_langtag(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;
  gchar *langtag_start_offset, *langtag_end_offset;

  str = NULL;
  
  langtag_start_offset = NULL;
  langtag_end_offset = NULL;
  
  if(ags_turtle_match_langtag(offset,
			      end_ptr,
			      &langtag_start_offset, &langtag_end_offset)){
    str = g_strndup(offset,
		    langtag_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_match_iriref:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match iriref.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_iriref(gchar *offset,
			gchar *end_ptr,
			gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];
  gchar *iter;

  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;
  
  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr && offset[0] == '<'){    
    for(iter = offset + 1; !success && iter < end_ptr;){
      gchar *uchar_start_offset, *uchar_end_offset;
      
      if(iter[0] == '>'){
	match[0] = offset;
	match[1] = iter + 1;
	
	success = TRUE;
	
	break;
      }

      uchar_start_offset = NULL;
      uchar_end_offset = NULL;
      
      if(ags_turtle_match_uchar(iter,
				end_ptr,
				&uchar_start_offset, &uchar_end_offset)){
	iter += (uchar_end_offset - uchar_start_offset);
      }else{
	if(iter[0] >= 0x0 && iter[0] <= 0x20){
	  break;
	}
	
	if(iter[0] == '<' || iter[0] == '"' || iter[0] == '{' || iter[0] == '}' || iter[0] == '|' || iter[0] == '^' || iter[0] == '`' || iter[0] == '\\'){
	  break;
	}
	
	iter++;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pname_ns:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match match pname-ns.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pname_ns(gchar *offset,
			  gchar *end_ptr,
			  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];
  gchar *pn_prefix_start_offset, *pn_prefix_end_offset;

  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  pn_prefix_start_offset = NULL;
  pn_prefix_end_offset = NULL;
  
  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL){
    if(ags_turtle_match_pn_prefix(offset,
				  end_ptr,
				  &pn_prefix_start_offset, &pn_prefix_end_offset)){
      if(pn_prefix_end_offset < end_ptr &&
	 pn_prefix_end_offset[0] == ':'){
	match[0] = offset;
	match[1] = pn_prefix_end_offset + 1;
      
	success = TRUE;
      }
    }else{
      if(offset[0] == ':'){
	match[0] = offset;
	match[1] = offset + 1;
      
	success = TRUE;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pname_ln:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match pname-ln.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pname_ln(gchar *offset,
			  gchar *end_ptr,
			  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];
  gchar *pname_ns_start_offset, *pname_ns_end_offset;
  gchar *pn_local_start_offset, *pn_local_end_offset;

  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  pname_ns_start_offset = NULL;
  pname_ns_end_offset = NULL;

  pn_local_start_offset = NULL;
  pn_local_end_offset = NULL;
  
  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     ags_turtle_match_pname_ns(offset,
			       end_ptr,
			       &pname_ns_start_offset, &pname_ns_end_offset)){
    if(ags_turtle_match_pn_local(pname_ns_end_offset,
				 end_ptr,
				 &pn_local_start_offset, &pn_local_end_offset)){
      match[0] = offset;
      match[1] = pn_local_end_offset;

      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_blank_node_label:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match blank node label.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_blank_node_label(gchar *offset,
				  gchar *end_ptr,
				  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];
  gchar *label_start_offset, *label_end_offset;

  gboolean success;
  
  match[0] = NULL;
  match[1] = NULL;

  label_start_offset = NULL;
  label_end_offset = NULL;
  
  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset + 3 < end_ptr &&
     offset[0] == '_' &&
     offset[1] == ':'){
    if(ags_turtle_match_pn_chars_u(offset + 2,
				   end_ptr,
				   &label_start_offset, &label_end_offset)){
      success = TRUE;
    }else if(offset[2] >= '0' && offset[2] <= '9'){
      success = TRUE;
    }

    if(success){
      gchar *iter;
      gchar *iter_start_offset, *iter_end_offset;

      gboolean check_suffix;
      
      match[0] = offset;
      match[1] = offset + 3;

      iter = offset + 3;

      check_suffix = FALSE;
      
      while(iter < end_ptr){
	iter_start_offset = NULL;
	iter_end_offset = NULL;

	if(ags_turtle_match_pn_chars(iter,
				     end_ptr,
				     &iter_start_offset, &iter_end_offset)){
	  iter = iter_end_offset;
	}else if(iter[0] == '.'){
	  check_suffix = TRUE;
	  
	  iter++;
	}else{
	  break;
	}
      }

      if(check_suffix){
	gchar *tmp_iter;
	
	for(tmp_iter = iter - 1; tmp_iter[0] == '.'; tmp_iter--);
	
	match[1] = tmp_iter + 1;
      }else{
	match[1] = iter;
      }
    }
  }
  
  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_langtag:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match langtag.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_langtag(gchar *offset,
			 gchar *end_ptr,
			 gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset + 2 < end_ptr &&
     offset[0] == '@' &&
     ((offset[1] >= 'a' &&
       offset[1] <= 'z') ||
      (offset[1] >= 'A' &&
       offset[1] <= 'Z'))){
    gchar *iter;

    gboolean initial_dash;
    
    success = TRUE;

    iter = offset + 2;

    while(iter < end_ptr){
      if((iter[0] >= 'a' &&
	  iter[0] <= 'z') ||
	 (iter[0] >= 'A' &&
	  iter[0] <= 'Z')){
	iter++;
      }else{
	break;
      }
    }

    initial_dash = FALSE;
    
    while(iter < end_ptr){
      if((iter[0] >= 'a' &&
	  iter[0] <= 'z') ||
	 (iter[0] >= 'A' &&
	  iter[0] <= 'Z') ||
	 (iter[0] >= '0' &&
	  iter[0] <= '9')){
	initial_dash = FALSE;

	iter++;
      }else if(!initial_dash &&
	       iter[0] == '-'){
	initial_dash = TRUE;

	iter++;
      }else{
	break;
      }
    }

    if(initial_dash){
      iter--;
    }
    
    match[0] = offset;
    match[1] = iter;
  }
  
  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_read_boolean:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read boolean value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_boolean(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;
  gchar *boolean_start_offset, *boolean_end_offset;

  str = NULL;
  
  boolean_start_offset = NULL;
  boolean_end_offset = NULL;
  
  if(ags_turtle_match_boolean(offset,
			      end_ptr,
			      &boolean_start_offset, &boolean_end_offset)){
    str = g_strndup(offset,
		    boolean_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_integer:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read integer value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_integer(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;
  gchar *integer_start_offset, *integer_end_offset;

  str = NULL;
  
  integer_start_offset = NULL;
  integer_end_offset = NULL;
  
  if(ags_turtle_match_integer(offset,
			      end_ptr,
			      &integer_start_offset, &integer_end_offset)){
    str = g_strndup(offset,
		    integer_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_decimal:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read decimal value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_decimal(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;
  gchar *decimal_start_offset, *decimal_end_offset;

  str = NULL;
  
  decimal_start_offset = NULL;
  decimal_end_offset = NULL;
  
  if(ags_turtle_match_decimal(offset,
			      end_ptr,
			      &decimal_start_offset, &decimal_end_offset)){
    str = g_strndup(offset,
		    decimal_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_double:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read double value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_double(gchar *offset,
		       gchar *end_ptr)
{
  gchar *str;
  gchar *double_start_offset, *double_end_offset;

  str = NULL;
  
  double_start_offset = NULL;
  double_end_offset = NULL;
  
  if(ags_turtle_match_double(offset,
			     end_ptr,
			     &double_start_offset, &double_end_offset)){
    str = g_strndup(offset,
		    double_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_exponent:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read exponent value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_exponent(gchar *offset,
			 gchar *end_ptr)
{
  gchar *str;
  gchar *exponent_start_offset, *exponent_end_offset;

  str = NULL;
  
  exponent_start_offset = NULL;
  exponent_end_offset = NULL;
  
  if(ags_turtle_match_exponent(offset,
			       end_ptr,
			       &exponent_start_offset, &exponent_end_offset)){
    str = g_strndup(offset,
		    exponent_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_match_boolean:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match boolean.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_boolean(gchar *offset,
			 gchar *end_ptr,
			 gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    if(!strncmp(offset,
		AGS_TURTLE_BOOLEAN_LITERAL_TRUE,
		4)){
      match[0] = offset;
      match[1] = offset + 4;
      
      success = TRUE;
    }else if(!strncmp(offset,
		      AGS_TURTLE_BOOLEAN_LITERAL_FALSE,
		      5)){
      match[0] = offset;
      match[1] = offset + 5;

      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_integer:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match integer.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_integer(gchar *offset,
			 gchar *end_ptr,
			 gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;
    
    gboolean has_sign;

    iter = offset;
    
    has_sign = FALSE;
    
    if(iter[0] == '+' ||
       iter[0] == '-'){
      iter++;
      
      has_sign = TRUE;
    }

    for(; iter < end_ptr; iter++){
      if(iter[0] >= '0' &&
	 iter[0] <= '9'){
	if(!success){
	  success = TRUE;
	}
      }else{
	break;
      }
    }

    if(success){
      match[0] = offset;
      match[1] = iter;
    }
  }
    
  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}

/**
 * ags_turtle_match_decimal:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match decimal.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_decimal(gchar *offset,
			 gchar *end_ptr,
			 gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;

    gboolean has_sign;
    gboolean has_int;
    gboolean has_float;

    iter = offset;

    has_sign = FALSE;
    has_int = FALSE;
    has_float = FALSE;
    
    if(iter[0] == '+' ||
       iter[0] == '-'){
      iter++;
      
      has_sign = TRUE;
    }

    for(; iter < end_ptr; iter++){
      if(iter[0] >= '0' &&
	 iter[0] <= '9'){
	if(!has_int){
	  has_int = TRUE;
	}
      }else{
	break;
      }
    }

    if(iter[0] == '.'){
      iter++;
      
      for(; iter < end_ptr; iter++){
	if(iter[0] >= '0' &&
	   iter[0] <= '9'){
	  if(!has_float){
	    has_float = TRUE;
	    
	    success = TRUE;
	  }
	}else{
	  break;
	}
      }      
    }
    
    if(success){
      match[0] = offset;
      match[1] = iter;
    }
  }
  
  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}

/**
 * ags_turtle_match_double:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match double.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_double(gchar *offset,
			gchar *end_ptr,
			gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;
    gchar *exponent_start_offset, *exponent_end_offset;
    
    gboolean has_sign;
    gboolean has_int;
    gboolean has_float;
    gboolean has_exponent;
    
    iter = offset;

    has_sign = FALSE;
    has_int = FALSE;
    has_float = FALSE;
    has_exponent = FALSE;
    
    if(iter[0] == '+' ||
       iter[0] == '-'){
      iter++;
      
      has_sign = TRUE;
    }

    for(; iter < end_ptr; iter++){
      if(iter[0] >= '0' &&
	 iter[0] <= '9'){
	if(!has_int){
	  has_int = TRUE;
	}
      }else{
	break;
      }
    }
    
    if(iter[0] == '.'){
      iter++;
      
      for(; iter < end_ptr; iter++){
	if(iter[0] >= '0' &&
	   iter[0] <= '9'){
	  if(!has_float){
	    has_float = TRUE;
	  }
	}else{
	  break;
	}
      }      
    }

    if(ags_turtle_match_exponent(iter,
				 end_ptr,
				 &exponent_start_offset, &exponent_end_offset)){
      iter = exponent_end_offset;

      has_exponent = TRUE;
    }

    if((has_int && has_float && has_exponent) ||
       (has_float && has_exponent) ||
       (has_int && has_exponent)){
      success = TRUE;
    }
    
    if(success){
      match[0] = offset;
      match[1] = iter;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_exponent:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match exponent.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_exponent(gchar *offset,
			  gchar *end_ptr,
			  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;

    gboolean has_e;
    gboolean has_exponent_sign;
    gboolean has_exponent;
    
    iter = offset;

    has_e = FALSE;
    has_exponent_sign = FALSE;
    has_exponent = FALSE;

    if(iter[0] == 'e' ||
       iter[0] == 'E'){
      iter++;

      has_e = TRUE;

      if(iter[0] == '+' ||
	 iter[0] == '-'){
	iter++;
      
	has_exponent_sign = TRUE;
      }
      
      for(; iter < end_ptr; iter++){
	if(iter[0] >= '0' &&
	   iter[0] <= '9'){
	  if(!has_exponent){
	    has_exponent = TRUE;
	  }
	}else{
	  break;
	}
      }      
    }

    success = has_exponent;

    if(success){
      match[0] = offset;
      match[1] = iter;
    }
  }
  
  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}

/**
 * ags_turtle_read_string:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_string(gchar *offset,
		       gchar *end_ptr)
{
  gchar *str;
  gchar *string_start_offset, *string_end_offset;

  str = NULL;
  
  string_start_offset = NULL;
  string_end_offset = NULL;
  
  if(ags_turtle_match_string(offset,
			     end_ptr,
			     &string_start_offset, &string_end_offset)){
    str = g_strndup(offset,
		    string_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_match_string:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match string.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_string(gchar *offset,
			gchar *end_ptr,
			gchar **start_offset, gchar **end_offset)
{
  gchar *string_literal_long_quote_start_offset, *string_literal_long_quote_end_offset;
  gchar *string_literal_long_single_quote_start_offset, *string_literal_long_single_quote_end_offset;
  gchar *string_literal_quote_start_offset, *string_literal_quote_end_offset;
  gchar *string_literal_single_quote_start_offset, *string_literal_single_quote_end_offset;
  gchar* match[2];

  gboolean success;

  string_literal_long_quote_start_offset = NULL;
  string_literal_long_quote_end_offset = NULL;
  
  string_literal_long_single_quote_start_offset = NULL;
  string_literal_long_single_quote_end_offset = NULL;

  string_literal_quote_start_offset = NULL;
  string_literal_quote_end_offset = NULL;

  string_literal_single_quote_start_offset = NULL;
  string_literal_single_quote_end_offset = NULL;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL){
    if(ags_turtle_match_string_literal_long_quote(offset,
						  end_ptr,
						  &string_literal_long_quote_start_offset, &string_literal_long_quote_end_offset)){ /* match string literal long quote */
      match[0] = offset;
      match[1] = string_literal_long_quote_end_offset;
      
      success = TRUE;
    }else if(ags_turtle_match_string_literal_long_single_quote(offset, 
							       end_ptr,
							       &string_literal_long_single_quote_start_offset, &string_literal_long_single_quote_end_offset)){ /* match string literal long single quote */
      match[0] = offset;
      match[1] = string_literal_long_single_quote_end_offset;
      
      success = TRUE;
    }else if(ags_turtle_match_string_literal_quote(offset,
						   end_ptr,
						   &string_literal_quote_start_offset, &string_literal_quote_end_offset)){ /* match string literal quote */
      match[0] = offset;
      match[1] = string_literal_quote_end_offset;
      
      success = TRUE;
    }else if(ags_turtle_match_string_literal_single_quote(offset,
							  end_ptr,
							  &string_literal_single_quote_start_offset, &string_literal_single_quote_end_offset)){ /* match string literal single quote */
      match[0] = offset;
      match[1] = string_literal_single_quote_end_offset;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_read_string_literal_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal quote value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_string_literal_quote(gchar *offset,
				     gchar *end_ptr)
{
  gchar *str;
  gchar *string_literal_quote_start_offset, *string_literal_quote_end_offset;

  str = NULL;
  
  string_literal_quote_start_offset = NULL;
  string_literal_quote_end_offset = NULL;
  
  if(ags_turtle_match_string_literal_quote(offset,
					   end_ptr,
					   &string_literal_quote_start_offset, &string_literal_quote_end_offset)){
    str = g_strndup(offset,
		    string_literal_quote_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_string_literal_single_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal single quote value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_string_literal_single_quote(gchar *offset,
					    gchar *end_ptr)
{
  gchar *str;
  gchar *string_literal_single_quote_start_offset, *string_literal_single_quote_end_offset;

  str = NULL;
  
  string_literal_single_quote_start_offset = NULL;
  string_literal_single_quote_end_offset = NULL;
  
  if(ags_turtle_match_string_literal_single_quote(offset,
						  end_ptr,
						  &string_literal_single_quote_start_offset, &string_literal_single_quote_end_offset)){
    str = g_strndup(offset,
		    string_literal_single_quote_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_string_literal_long_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal long quote value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_string_literal_long_quote(gchar *offset,
					  gchar *end_ptr)
{
  gchar *str;
  gchar *string_literal_long_quote_start_offset, *string_literal_long_quote_end_offset;

  str = NULL;
  
  string_literal_long_quote_start_offset = NULL;
  string_literal_long_quote_end_offset = NULL;
  
  if(ags_turtle_match_string_literal_long_quote(offset,
						end_ptr,
						&string_literal_long_quote_start_offset, &string_literal_long_quote_end_offset)){
    str = g_strndup(offset,
		    string_literal_long_quote_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_string_literal_long_single_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read string literal long single quote value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_string_literal_long_single_quote(gchar *offset,
						 gchar *end_ptr)
{
  gchar *str;
  gchar *string_literal_long_single_quote_start_offset, *string_literal_long_single_quote_end_offset;

  str = NULL;
  
  string_literal_long_single_quote_start_offset = NULL;
  string_literal_long_single_quote_end_offset = NULL;
  
  if(ags_turtle_match_string_literal_long_single_quote(offset,
						       end_ptr,
						       &string_literal_long_single_quote_start_offset, &string_literal_long_single_quote_end_offset)){
    str = g_strndup(offset,
		    string_literal_long_single_quote_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_match_string_literal_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match string literal quote.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_string_literal_quote(gchar *offset,
				      gchar *end_ptr,
				      gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;

    iter = offset;

    if(iter[0] == '"'){
      iter++;
      
      while(iter < end_ptr){
	gchar *uchar_start_offset, *uchar_end_offset;
	gchar *echar_start_offset, *echar_end_offset;
	
	if(iter[0] == '"'){
	  match[0] = offset;
	  match[1] = iter + 1;
	  
	  success = TRUE;

	  break;
	}

	uchar_start_offset = NULL;
	uchar_end_offset = NULL;

	echar_start_offset = NULL;
	echar_end_offset = NULL;

	if(ags_turtle_match_echar(iter,
				  end_ptr,
				  &echar_start_offset, &echar_end_offset)){
	  iter += (echar_end_offset - echar_start_offset);
	  
	  continue;
	}

	if(ags_turtle_match_uchar(iter,
				  end_ptr,
				  &uchar_start_offset, &uchar_end_offset)){
	  iter += (uchar_end_offset - uchar_start_offset);
	  
	  continue;
	}
	
	if(iter[0] == '\\' || iter[0] == '\n' || iter[0] == '\r'){
	  break;
	}

	iter++;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}

/**
 * ags_turtle_match_string_literal_single_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match string literal single quote.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_string_literal_single_quote(gchar *offset,
					     gchar *end_ptr,
					     gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;

    iter = offset;

    if(iter[0] == '\''){
      iter++;
      
      while(iter < end_ptr){
	gchar *uchar_start_offset, *uchar_end_offset;
	gchar *echar_start_offset, *echar_end_offset;
	
	if(iter[0] == '\''){
	  match[0] = offset;
	  match[1] = iter + 1;
	  
	  success = TRUE;

	  break;
	}

	uchar_start_offset = NULL;
	uchar_end_offset = NULL;

	echar_start_offset = NULL;
	echar_end_offset = NULL;

	if(ags_turtle_match_echar(iter,
				  end_ptr,
				  &echar_start_offset, &echar_end_offset)){
	  iter += (echar_end_offset - echar_start_offset);
	  
	  continue;
	}

	if(ags_turtle_match_uchar(iter,
				  end_ptr,
				  &uchar_start_offset, &uchar_end_offset)){
	  iter += (uchar_end_offset - uchar_start_offset);
	  
	  continue;
	}

	if(iter[0] == '\\' || iter[0] == '\n' || iter[0] == '\r'){
	  break;
	}

	iter++;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}

/**
 * ags_turtle_match_string_literal_long_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match string literal long quote.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_string_literal_long_quote(gchar *offset,
					   gchar *end_ptr,
					   gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  guint n_match;
  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset + 6 < end_ptr){
    gchar *iter;

    iter = offset;

    if(iter[0] == '"' &&
       iter[1] == '"' &&
       iter[2] == '"'){
      iter += 3;
      
      for(n_match = 0; iter < end_ptr;){
	gchar *uchar_start_offset, *uchar_end_offset;
	gchar *echar_start_offset, *echar_end_offset;
	
	if(iter[0] == '"'){
	  n_match++;

	  if(n_match == 3){
	    match[0] = offset;
	    match[1] = iter + 1;
	    
	    success = TRUE;
	    
	    break;
	  }

	  iter++;

	  continue;
	}
	
	n_match = 0;

	uchar_start_offset = NULL;
	uchar_end_offset = NULL;

	echar_start_offset = NULL;
	echar_end_offset = NULL;

	if(ags_turtle_match_echar(iter,
				  end_ptr,
				  &echar_start_offset, &echar_end_offset)){
	  iter += (echar_end_offset - echar_start_offset);
	  
	  continue;
	}

	if(ags_turtle_match_uchar(iter,
				  end_ptr,
				  &uchar_start_offset, &uchar_end_offset)){
	  iter += (uchar_end_offset - uchar_start_offset);
	  
	  continue;
	}
	
	if(iter[0] == '\\'){
	  break;
	}

	iter++;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}

/**
 * ags_turtle_match_string_literal_long_single_quote:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match string literal long single quote.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_string_literal_long_single_quote(gchar *offset,
						  gchar *end_ptr,
						  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  guint n_match;
  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;

    iter = offset;

    if(iter[0] == '\'' &&
       iter[1] == '\'' &&
       iter[2] == '\''){
      iter += 3;
      
      for(n_match = 0; iter < end_ptr;){
	gchar *uchar_start_offset, *uchar_end_offset;
	gchar *echar_start_offset, *echar_end_offset;
	
	if(iter[0] == '\''){
	  n_match++;

	  if(n_match == 3){
	    match[0] = offset;
	    match[1] = iter + 1;
	    
	    success = TRUE;
	    
	    break;
	  }

	  iter++;

	  continue;
	}
	
	n_match = 0;

	uchar_start_offset = NULL;
	uchar_end_offset = NULL;

	echar_start_offset = NULL;
	echar_end_offset = NULL;

	if(ags_turtle_match_echar(iter,
				  end_ptr,
				  &echar_start_offset, &echar_end_offset)){
	  iter += (echar_end_offset - echar_start_offset);
	  
	  continue;
	}

	if(ags_turtle_match_uchar(iter,
				  end_ptr,
				  &uchar_start_offset, &uchar_end_offset)){
	  iter += (uchar_end_offset - uchar_start_offset);
	  
	  continue;
	}

	if(iter[0] == '\\'){
	  break;
	}
	
	iter++;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }

  return(success);
}


/**
 * ags_turtle_read_uchar:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read uchar value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_uchar(gchar *offset,
		      gchar *end_ptr)
{
  gchar *str;
  gchar *uchar_start_offset, *uchar_end_offset;

  str = NULL;
  
  uchar_start_offset = NULL;
  uchar_end_offset = NULL;
  
  if(ags_turtle_match_uchar(offset,
			    end_ptr,
			    &uchar_start_offset, &uchar_end_offset)){
    str = g_strndup(offset,
		    uchar_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_echar:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read echar value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_echar(gchar *offset,
		      gchar *end_ptr)
{
  gchar *str;
  gchar *echar_start_offset, *echar_end_offset;

  str = NULL;
  
  echar_start_offset = NULL;
  echar_end_offset = NULL;
  
  if(ags_turtle_match_echar(offset,
			    end_ptr,
			    &echar_start_offset, &echar_end_offset)){
    str = g_strndup(offset,
		    echar_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_ws:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read ws value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_ws(gchar *offset,
		   gchar *end_ptr)
{
  gchar *str;
  gchar *ws_start_offset, *ws_end_offset;

  str = NULL;
  
  ws_start_offset = NULL;
  ws_end_offset = NULL;
  
  if(ags_turtle_match_ws(offset,
			 end_ptr,
			 &ws_start_offset, &ws_end_offset)){
    str = g_strndup(offset,
		    ws_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_anon:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read anon value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_anon(gchar *offset,
		     gchar *end_ptr)
{
  gchar *str;
  gchar *anon_start_offset, *anon_end_offset;

  str = NULL;
  
  anon_start_offset = NULL;
  anon_end_offset = NULL;
  
  if(ags_turtle_match_anon(offset,
			   end_ptr,
			   &anon_start_offset, &anon_end_offset)){
    str = g_strndup(offset,
		    anon_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_chars_base:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name chars base value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pn_chars_base(gchar *offset,
			      gchar *end_ptr)
{
  gchar *str;
  gchar *pn_chars_base_start_offset, *pn_chars_base_end_offset;

  str = NULL;
  
  pn_chars_base_start_offset = NULL;
  pn_chars_base_end_offset = NULL;
  
  if(ags_turtle_match_pn_chars_base(offset,
				    end_ptr,
				    &pn_chars_base_start_offset, &pn_chars_base_end_offset)){
    str = g_strndup(offset,
		    pn_chars_base_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_chars_u:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name chars underscore value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pn_chars_u(gchar *offset,
			   gchar *end_ptr)
{  
  gchar *str;
  gchar *pn_chars_u_start_offset, *pn_chars_u_end_offset;

  str = NULL;
  
  pn_chars_u_start_offset = NULL;
  pn_chars_u_end_offset = NULL;
  
  if(ags_turtle_match_pn_chars_u(offset,
				 end_ptr,
				 &pn_chars_u_start_offset, &pn_chars_u_end_offset)){
    str = g_strndup(offset,
		    pn_chars_u_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_chars:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name chars value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pn_chars(gchar *offset,
			 gchar *end_ptr)
{
  gchar *str;
  gchar *pn_chars_start_offset, *pn_chars_end_offset;

  str = NULL;
  
  pn_chars_start_offset = NULL;
  pn_chars_end_offset = NULL;
  
  if(ags_turtle_match_pn_chars(offset,
			       end_ptr,
			       &pn_chars_start_offset, &pn_chars_end_offset)){
    str = g_strndup(offset,
		    pn_chars_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_prefix:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixe-name prefix value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pn_prefix(gchar *offset,
			  gchar *end_ptr)
{
  gchar *str;
  gchar *pn_prefix_start_offset, *pn_prefix_end_offset;

  str = NULL;
  
  pn_prefix_start_offset = NULL;
  pn_prefix_end_offset = NULL;
  
  if(ags_turtle_match_pn_prefix(offset,
				end_ptr,
				&pn_prefix_start_offset, &pn_prefix_end_offset)){
    str = g_strndup(offset,
		    pn_prefix_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_local:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed-name local value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pn_local(gchar *offset,
			 gchar *end_ptr)
{
  gchar *str;
  gchar *pn_local_start_offset, *pn_local_end_offset;

  str = NULL;
  
  pn_local_start_offset = NULL;
  pn_local_end_offset = NULL;
  
  if(ags_turtle_match_pn_local(offset,
			       end_ptr,
			       &pn_local_start_offset, &pn_local_end_offset)){
    str = g_strndup(offset,
		    pn_local_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_plx:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read plx value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_plx(gchar *offset,
		    gchar *end_ptr)
{
  gchar *str;
  gchar *plx_start_offset, *plx_end_offset;

  str = NULL;
  
  plx_start_offset = NULL;
  plx_end_offset = NULL;
  
  if(ags_turtle_match_plx(offset,
			  end_ptr,
			  &plx_start_offset, &plx_end_offset)){
    str = g_strndup(offset,
		    plx_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_percent:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read percent value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_percent(gchar *offset,
			gchar *end_ptr)
{
  gchar *str;
  gchar *percent_start_offset, *percent_end_offset;

  str = NULL;
  
  percent_start_offset = NULL;
  percent_end_offset = NULL;
  
  if(ags_turtle_match_percent(offset,
			      end_ptr,
			      &percent_start_offset, &percent_end_offset)){
    str = g_strndup(offset,
		    percent_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_hex:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read hex value.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_hex(gchar *offset,
		    gchar *end_ptr)
{
  gchar *str;
  gchar *hex_start_offset, *hex_end_offset;

  str = NULL;
  
  hex_start_offset = NULL;
  hex_end_offset = NULL;
  
  if(ags_turtle_match_hex(offset,
			  end_ptr,
			  &hex_start_offset, &hex_end_offset)){
    str = g_strndup(offset,
		    hex_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_read_pn_local_esc:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 *
 * Read prefixed name local escapes.
 *
 * Returns: (transfer full): a string on success otherwise %NULL
 * 
 * Since: 3.0.0
 */
gchar*
ags_turtle_read_pn_local_esc(gchar *offset,
			     gchar *end_ptr)
{
  gchar *str;
  gchar *pn_local_esc_start_offset, *pn_local_esc_end_offset;

  str = NULL;
  
  pn_local_esc_start_offset = NULL;
  pn_local_esc_end_offset = NULL;
  
  if(ags_turtle_match_pn_local_esc(offset,
				   end_ptr,
				   &pn_local_esc_start_offset, &pn_local_esc_end_offset)){
    str = g_strndup(offset,
		    pn_local_esc_end_offset - offset);
  }
  
  return(str);
}

/**
 * ags_turtle_match_uchar:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match uchar.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_uchar(gchar *offset,
		       gchar *end_ptr,
		       gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset + 1 < end_ptr){
    if(offset[0] == '\\' &&
       offset[1] == 'u' &&
       offset + 5 < end_ptr){
      if(g_ascii_isxdigit(offset[2]) &&
	 g_ascii_isxdigit(offset[3]) &&
	 g_ascii_isxdigit(offset[4]) &&
	 g_ascii_isxdigit(offset[5])){
	match[0] = offset;
	match[1] = offset + 6;
	
	success = TRUE;
      }
    }else if(offset[0] == '\\' &&
	     offset[1] == 'U' &&
	     offset + 9 < end_ptr){
      if(g_ascii_isxdigit(offset[2]) &&
	 g_ascii_isxdigit(offset[3]) &&
	 g_ascii_isxdigit(offset[4]) &&
	 g_ascii_isxdigit(offset[5]) &&
	 g_ascii_isxdigit(offset[6]) &&
	 g_ascii_isxdigit(offset[7]) &&
	 g_ascii_isxdigit(offset[8]) &&
	 g_ascii_isxdigit(offset[9])){
	match[0] = offset;
	match[1] = offset + 10;
	
	success = TRUE;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_echar:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match echar.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_echar(gchar *offset,
		       gchar *end_ptr,
		       gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL){
    if(offset + 1 < end_ptr &&
       offset[0] == '\\' &&
       (offset[1] == 't' ||
	offset[1] == 'b' ||
	offset[1] == 'n' ||
	offset[1] == 'r' ||
	offset[1] == 'f' ||
	offset[1] == '"' ||
	offset[1] == '\'' ||
	offset[1] == '\\')){
      match[0] = offset;
      match[1] = offset + 2;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_ws:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match ws.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_ws(gchar *offset,
		    gchar *end_ptr,
		    gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    if(offset[0] == ' ' || offset[0] == '\t' || offset[0] == '\r' || offset[0] == '\n'){
      match[0] = offset;
      match[1] = offset + 1;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_anon:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match anon.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_anon(gchar *offset,
		      gchar *end_ptr,
		      gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;
    gchar *ws_start_offset, *ws_end_offset;
    
    iter = offset;

    if(iter[0] == '['){
      iter++;
      
      for(; iter < end_ptr;){
	if(iter[0] == ']'){
	  match[0] = offset;
	  match[1] = iter + 1;
	  
	  success = TRUE;

	  break;
	}

	ws_start_offset = NULL;
	ws_end_offset = NULL;
	
	if(ags_turtle_match_ws(iter,
			       end_ptr,
			       &ws_start_offset, &ws_end_offset)){
	  iter += (ws_end_offset - ws_start_offset);
	}else{
	  break;
	}
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pn_chars_base:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match pn chars base.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pn_chars_base(gchar *offset,
			       gchar *end_ptr,
			       gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;

    iter = offset;

    if((iter[0] >= 'a' && iter[0] <= 'z') ||
       (iter[0] >= 'A' && iter[0] <= 'Z')){
      match[0] = offset;
      match[1] = offset + 1;
      
      success = TRUE;
    }else{
      gunichar c;

      c = g_utf8_get_char(iter);

      if((c >= 0x00c0 && c <= 0x00d6) ||
	 (c >= 0x00d8 && c <= 0x00f6) ||
	 (c >= 0x00f8 && c <= 0x02ff) ||
	 (c >= 0x0370 && c <= 0x037d) ||
	 (c >= 0x037f && c <= 0x1fff) ||
	 (c >= 0x200c && c <= 0x200d) ||
	 (c >= 0x2070 && c <= 0x218f) ||
	 (c >= 0x2c00 && c <= 0x2fef) ||
	 (c >= 0x3001 && c <= 0xd7ff) ||
	 (c >= 0xf900 && c <= 0xfdcf) ||
	 (c >= 0xfdf0 && c <= 0xfffd) ||
	 (c >= 0x10000 && c <= 0xeffff)){
	match[0] = offset;
	match[1] = offset + g_unichar_to_utf8(c,
					      NULL);
	
	success = TRUE;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pn_chars_u:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match pn chars u.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pn_chars_u(gchar *offset,
			    gchar *end_ptr,
			    gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *pn_chars_base_start_offset, *pn_chars_base_end_offset;

    pn_chars_base_start_offset = NULL;
    pn_chars_base_end_offset = NULL;

    if(ags_turtle_match_pn_chars_base(offset,
				      end_ptr,
				      &pn_chars_base_start_offset, &pn_chars_base_end_offset)){
      match[0] = offset;
      match[1] = pn_chars_base_end_offset;
      
      success = TRUE;
    }else if(offset[0] == '_'){
      match[0] = offset;
      match[1] = offset + 1;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pn_chars:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match pn chars.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pn_chars(gchar *offset,
			  gchar *end_ptr,
			  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *pn_chars_u_start_offset, *pn_chars_u_end_offset;

    pn_chars_u_start_offset = NULL;
    pn_chars_u_end_offset = NULL;

    if(ags_turtle_match_pn_chars_u(offset,
				   end_ptr,
				   &pn_chars_u_start_offset, &pn_chars_u_end_offset)){
      match[0] = offset;
      match[1] = pn_chars_u_end_offset;
      
      success = TRUE;
    }else if(offset[0] == '-'){
      match[0] = offset;
      match[1] = offset + 1;
      
      success = TRUE;
    }else if(offset[0] >= '0' && offset[0] <= '9'){
      match[0] = offset;
      match[1] = offset + 1;
      
      success = TRUE;
    }else{
      gunichar c;

      c = g_utf8_get_char(offset);

      if((c == 0x00b7) ||
	 (c >= 0x0300 && c <= 0x036f) ||
	 (c >= 0x203f && c <= 0x2040)){
	match[0] = offset;
	match[1] = offset + g_unichar_to_utf8(c,
					      NULL);
	
	success = TRUE;
      }
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pn_prefix:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match pn prefix.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pn_prefix(gchar *offset,
			   gchar *end_ptr,
			   gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;
    gchar *pn_chars_base_start_offset, *pn_chars_base_end_offset;
    gchar *check_point_start_offset;
    
    gboolean check_point;
    
    pn_chars_base_start_offset = NULL;
    pn_chars_base_end_offset = NULL;
    
    iter = offset;

    check_point_start_offset = NULL;
    
    check_point = FALSE;
    
    if(ags_turtle_match_pn_chars_base(iter,
				      end_ptr,
				      &pn_chars_base_start_offset, &pn_chars_base_end_offset)){
      iter += (pn_chars_base_end_offset - pn_chars_base_start_offset);

      check_point_start_offset = iter;
      
      while(iter < end_ptr){
	gchar *pn_chars_start_offset, *pn_chars_end_offset;
	
	pn_chars_start_offset = NULL;
	pn_chars_end_offset = NULL;
    
	if(ags_turtle_match_pn_chars(iter,
				     end_ptr,
				     &pn_chars_start_offset, &pn_chars_end_offset)){
	  iter += (pn_chars_end_offset - pn_chars_start_offset);

	  check_point = FALSE;
	}else if(iter[0] == '.'){
	  iter++;

	  check_point = TRUE;
	}else{
	  break;
	}
      }
    }

    if(check_point){
      for(iter -= 1; iter >= check_point_start_offset && iter[0] == '.'; iter--);
    }

    match[0] = offset;
    match[1] = iter;
    
    success = TRUE;
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pn_local:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match uchar.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pn_local(gchar *offset,
			  gchar *end_ptr,
			  gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *iter;
    gchar *pn_chars_u_start_offset, *pn_chars_u_end_offset;
    gchar *plx_start_offset, *plx_end_offset;
    gchar *check_point_start_offset;
    
    gboolean check_point;
    
    iter = offset;

    pn_chars_u_start_offset = NULL;
    pn_chars_u_end_offset = NULL;

    plx_start_offset = NULL;
    plx_end_offset = NULL;
    
    if(ags_turtle_match_pn_chars_u(iter,
				   end_ptr,
				   &pn_chars_u_start_offset, &pn_chars_u_end_offset)){
      iter += (pn_chars_u_end_offset - pn_chars_u_start_offset);
    }else if(iter[0] == ':'){
      iter++;
    }else if(iter[0] >= '0' &&
	     iter[0] <= '9'){
      iter++;
    }else if(ags_turtle_match_plx(iter,
				  end_ptr,
				  &plx_start_offset, &plx_end_offset)){
      iter += (plx_end_offset - plx_start_offset);
    }else{
      goto ags_turtle_match_pn_local_END;
    }

    check_point_start_offset = iter;
    
    check_point = FALSE;
    
    while(iter < end_ptr){
      gchar *pn_chars_start_offset, *pn_chars_end_offset;
      gchar *plx_start_offset, *plx_end_offset;

      pn_chars_start_offset = NULL;
      pn_chars_end_offset = NULL;

      plx_start_offset = NULL;
      plx_end_offset = NULL;
      
      if(ags_turtle_match_pn_chars(iter,
				   end_ptr,
				   &pn_chars_start_offset, &pn_chars_end_offset)){
	iter += (pn_chars_end_offset - pn_chars_start_offset);

	check_point = FALSE;
      }else if(iter[0] == '.'){
	iter++;
	
	check_point = TRUE;
      }else if(iter[0] == ':'){
	iter++;

	check_point = FALSE;
      }else if(ags_turtle_match_plx(iter,
				    end_ptr,
				    &plx_start_offset, &plx_end_offset)){
	iter += (plx_end_offset - plx_start_offset);

	check_point = FALSE;
      }else{
	break;
      }
    }

    if(check_point){
      for(iter -= 1; iter >= check_point_start_offset && iter[0] == '.'; iter--);
    }

    match[0] = offset;
    match[1] = iter;
    
    success = TRUE;
  }

ags_turtle_match_pn_local_END:
  
  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_plx:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match plx.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_plx(gchar *offset,
		     gchar *end_ptr,
		     gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    gchar *percent_start_offset, *percent_end_offset;
    gchar *pn_local_esc_start_offset, *pn_local_esc_end_offset;

    percent_start_offset = NULL;
    percent_end_offset = NULL;

    pn_local_esc_start_offset = NULL;
    pn_local_esc_end_offset = NULL;
    
    if(ags_turtle_match_percent(offset,
				end_ptr,
				&percent_start_offset, &percent_end_offset)){
      match[0] = offset;
      match[1] = percent_end_offset;
      
      success = TRUE;
    }else if(ags_turtle_match_pn_local_esc(offset,
					   end_ptr,
					   &pn_local_esc_start_offset, &pn_local_esc_end_offset)){
      match[0] = offset;
      match[1] = pn_local_esc_end_offset;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_percent:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match percent.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_percent(gchar *offset,
			 gchar *end_ptr,
			 gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset + 2 < end_ptr){
    if(offset[0] == '%' &&
       g_ascii_isxdigit(offset[1]) &&
       g_ascii_isxdigit(offset[2])){
      match[0] = offset;
      match[1] = offset + 3;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_hex:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match hex.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_hex(gchar *offset,
		     gchar *end_ptr,
		     gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset < end_ptr){
    if(g_ascii_isxdigit(offset[0])){
      match[0] = offset;
      match[1] = offset + 1;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_match_pn_local_es:
 * @offset: the string pointer
 * @end_ptr: the end of @offset
 * @start_offset: (out) (transfer none): points to start offset of matched, otherwise %NULL
 * @end_offset: (out) (transfer none): points to end offset of matched, otherwise %NULL
 *
 * Match pn local esc.
 *
 * Returns: %TRUE on success, otherwise %FALSE
 * 
 * Since: 3.2.12
 */
gboolean
ags_turtle_match_pn_local_esc(gchar *offset,
			      gchar *end_ptr,
			      gchar **start_offset, gchar **end_offset)
{
  gchar* match[2];

  gboolean success;

  match[0] = NULL;
  match[1] = NULL;

  success = FALSE;

  if(offset != NULL &&
     end_ptr != NULL &&
     offset + 1 < end_ptr){
    if(offset[0] == '\\' &&
       (offset[1] == '_' ||
	offset[1] == '~' ||
	offset[1] == '.' ||
	offset[1] == '-' ||
	offset[1] == '!' ||
	offset[1] == '$' ||
	offset[1] == '&' ||
	offset[1] == '\'' ||
	offset[1] == '(' ||
	offset[1] == ')' ||
	offset[1] == '*' ||
	offset[1] == '+' ||
	offset[1] == ',' ||
	offset[1] == ';' ||
	offset[1] == '=' ||
	offset[1] == '/' ||
	offset[1] == '?' ||
	offset[1] == '#' ||
	offset[1] == '@' ||
	offset[1] == '%')){
      match[0] = offset;
      match[1] = offset + 2;
      
      success = TRUE;
    }
  }

  if(start_offset != NULL){
    start_offset[0] = match[0];
  }
  
  if(end_offset != NULL){
    end_offset[0] = match[1];
  }
  
  return(success);
}

/**
 * ags_turtle_find_xpath:
 * @turtle: the #AgsTurtle
 * @xpath: a XPath expression as string
 *
 * Lookup XPath expression withing @turtle.
 *
 * Returns: (element-type libxml2.Node) (transfer container): a #GList-struct containing xmlNode
 *
 * Since: 3.0.0
 */
GList*
ags_turtle_find_xpath(AgsTurtle *turtle,
		      gchar *xpath)
{
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode **node;
  GList *list;

  guint i;
  
  xpath_context = xmlXPathNewContext(turtle->doc);
  xpath_object = xmlXPathEval((xmlChar *) xpath,
			      xpath_context);
  
  list = NULL;

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	list = g_list_prepend(list,
			      node[i]);
      }
    }
  }

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  list = g_list_reverse(list);
  
  return(list);
}

/**
 * ags_turtle_find_xpath_with_context_node:
 * @turtle: the #AgsTurtle
 * @xpath: a XPath expression as string
 * @context_node: a #xmlNode-struct
 *
 * Lookup XPath expression from @context_node withing @turtle.
 *
 * Returns: (element-type libxml2.Node) (transfer container): a #GList-struct containing xmlNode
 *
 * Since: 3.0.0
 */
GList*
ags_turtle_find_xpath_with_context_node(AgsTurtle *turtle,
					gchar *xpath,
					xmlNode *context_node)
{
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode **node;
  GList *list;

  guint i;

  if(!AGS_IS_TURTLE(turtle) ||
     turtle->doc == NULL ||
     xpath == NULL ||
     context_node == NULL){
    return(NULL);
  }
  
  xpath_context = xmlXPathNewContext(turtle->doc);
  //xpath_context->node = context_node;
  xpath_object = xmlXPathNodeEval(context_node,
				  (xmlChar *) xpath,
				  xpath_context);
  
  list = NULL;

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	list = g_list_prepend(list,
			      node[i]);
      }
    }
  }

  xmlXPathFreeObject(xpath_object);
  xmlXPathFreeContext(xpath_context);

  if(list != NULL){
    list = g_list_reverse(list);
  }
  
  return(list);
}

gchar*
ags_turtle_string_convert(AgsTurtle *turtle, gchar *str)
{
  if((AGS_TURTLE_TOLOWER & (turtle->flags)) != 0){
    str = g_ascii_strdown(str,
			  -1);
  }
  
  return(str);
}

gchar*
ags_turtle_load_skip_comments_and_blanks(AgsTurtle *turtle,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter)
{
  gchar *look_ahead;
  gchar *next;
    
  if(iter == NULL){
    return(NULL);
  }

  look_ahead = *iter;

  if(look_ahead == NULL){
    return(NULL);
  }
    
  /* skip whitespaces and comments */
  for(; (look_ahead < &(buffer[buffer_length])) && *look_ahead != '\0';){
    /* skip comments */
    if(buffer == look_ahead){
      if(*buffer == '#'){
	next = strchr(look_ahead, '\n');

	if(next != NULL){
	  look_ahead = next + 1;
	}else{
	  look_ahead = &(buffer[buffer_length]);

	  break;
	}
	  
	continue;
      }
    }else if(buffer[look_ahead - buffer - 1] == '\n' && *look_ahead == '#'){
      next = strchr(look_ahead, '\n');

      if(next != NULL){
	look_ahead = next + 1;
      }else{
	look_ahead = &(buffer[buffer_length]);

	break;
      }
	
      continue;
    }

    /* spaces */
    if(!(*look_ahead == ' ' || *look_ahead == '\t' || *look_ahead == '\n')){
      break;
    }

    look_ahead++;
  }

  return(look_ahead);
}

xmlNode*
ags_turtle_load_read_iriref(AgsTurtle *turtle,
			    gchar *buffer, gsize buffer_length,
			    gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);
  
  /* read iriref */
  str = ags_turtle_read_iriref(look_ahead,
			       &(buffer[buffer_length]));

  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-iriref");
    xmlNodeSetContent(node,
		      str);
    //      xmlNodeSetContent(node,
    //		g_ascii_strdown(str,
    //				-1));

#ifdef AGS_DEBUG
    g_message("iriref = %s", str);
#endif
      
    *iter = look_ahead + strlen(str);
    g_free(str);
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_anon(AgsTurtle *turtle,
			  gchar *buffer, gsize buffer_length,
			  gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* read anon */
  str = ags_turtle_read_anon(look_ahead,
			     &(buffer[buffer_length]));

  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-anon");
    xmlNodeSetContent(node,
		      g_ascii_strdown(str,
				      -1));

#ifdef AGS_DEBUG
    g_message("anon - %s", str);
#endif
            
    *iter = look_ahead + strlen(str);
    g_free(str);
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_pname_ns(AgsTurtle *turtle,
			      gchar *buffer, gsize buffer_length,
			      gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* read pname-ns */
  str = ags_turtle_read_pname_ns(look_ahead,
				 &(buffer[buffer_length]));
    
  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-pname-ns");
    xmlNodeSetContent(node,
		      g_ascii_strdown(str,
				      -1));

#ifdef AGS_DEBUG
    g_message("pname-ns - %s", str);
#endif
      
    *iter = look_ahead + strlen(str);
    g_free(str);      
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_pname_ln(AgsTurtle *turtle,
			      gchar *buffer, gsize buffer_length,
			      gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* read pname-ln */
  str = ags_turtle_read_pname_ln(look_ahead,
				 &(buffer[buffer_length]));
    
  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-pname-ln");
    xmlNodeSetContent(node,
		      g_ascii_strdown(str,
				      -1));

#ifdef AGS_DEBUG
    g_message("pname-ln - %s", str);
#endif
      
    *iter = look_ahead + strlen(str);
    g_free(str);      
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_numeric(AgsTurtle *turtle,
			     gchar *buffer, gsize buffer_length,
			     gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* read numeric */
  str = NULL;
    
  if(str == NULL){
    str = ags_turtle_read_decimal(look_ahead,
				  &(buffer[buffer_length]));
  }

  if(str == NULL){
    str = ags_turtle_read_double(look_ahead,
				 &(buffer[buffer_length]));      
  }

  if(str == NULL){
    str = ags_turtle_read_integer(look_ahead,
				  &(buffer[buffer_length]));
  }

  if(str == NULL){
    str = ags_turtle_read_boolean(look_ahead,
				  &(buffer[buffer_length]));      
  }
    
  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-numeric");
    xmlNodeSetContent(node,
		      g_ascii_strdown(str,
				      -1));

#ifdef AGS_DEBUG
    g_message("numeric - %s", str);
#endif
            
    *iter = look_ahead + strlen(str);
    g_free(str);
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_string(AgsTurtle *turtle,
			    gchar *buffer, gsize buffer_length,
			    gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
  gchar *encoded_str;  
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* read pname-ns */
  str = ags_turtle_read_string(look_ahead,
			       &(buffer[buffer_length]));
    
  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-string");

    encoded_str = xmlEncodeSpecialChars(turtle->doc,
					str);
      
    xmlNodeSetContent(node,
		      encoded_str);

#ifdef AGS_DEBUG
    g_message("string - %s", str);
#endif

    g_free(encoded_str);
      
    *iter = look_ahead + strlen(str);
    g_free(str);
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_langtag(AgsTurtle *turtle,
			     gchar *buffer, gsize buffer_length,
			     gchar **iter)
{
  xmlNode *node;

  gchar *look_ahead;
  gchar *str;
    
  node = NULL;
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* read langtag */
  str = ags_turtle_read_langtag(look_ahead,
				&(buffer[buffer_length]));
    
  if(str != NULL){
    node = xmlNewNode(NULL,
		      "rdf-langtag");
    xmlNodeSetContent(node,
		      g_ascii_strdown(str,
				      -1));

#ifdef AGS_DEBUG
    g_message("langtag - %s", str);
#endif
            
    *iter = look_ahead + strlen(str);
    g_free(str);
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_literal(AgsTurtle *turtle,
			     gchar *buffer, gsize buffer_length,
			     gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_string_node, *rdf_langtag_node, *rdf_iri_node;
    
  gchar *look_ahead;
    
  node = NULL;
  look_ahead = *iter;

  /* read string */
  rdf_string_node = ags_turtle_load_read_string(turtle,
						buffer, buffer_length,
						&look_ahead);

  if(rdf_string_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-literal");
    xmlAddChild(node,
		rdf_string_node);		  

    *iter = look_ahead;

    /* read optional langtag */
    rdf_langtag_node = ags_turtle_load_read_langtag(turtle,
						    buffer, buffer_length,
						    &look_ahead);

    if(rdf_langtag_node != NULL){
      xmlAddChild(node,
		  rdf_langtag_node);

      *iter = look_ahead;
    }else{
      /* skip blanks and comments */
      look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							    buffer, buffer_length,
							    &look_ahead);

      if(look_ahead + 2 < &(buffer[buffer_length]) &&
	 look_ahead[0] == '^' &&
	 look_ahead[1] == '^'){
	/* alternate r2ead optional iri */
	rdf_iri_node = ags_turtle_load_read_iri(turtle,
						buffer, buffer_length,
						&look_ahead);
	
	if(rdf_iri_node != NULL){
	  xmlAddChild(node,
		      rdf_iri_node);
	  
	  *iter = look_ahead;
	}
      }
    }
  }else{
    xmlNode *rdf_numeric_node;

    rdf_numeric_node = ags_turtle_load_read_numeric(turtle,
						    buffer, buffer_length,
						    &look_ahead);

    if(rdf_numeric_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-literal");
      xmlAddChild(node,
		  rdf_numeric_node);

      *iter = look_ahead;
    }      
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_iri(AgsTurtle *turtle,
			 gchar *buffer, gsize buffer_length,
			 gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_iriref_node, *rdf_prefixed_name_node;
    
  gchar *look_ahead;
    
  node = NULL;
  look_ahead = *iter;

  /* read iriref */
  rdf_iriref_node = ags_turtle_load_read_iriref(turtle,
						buffer, buffer_length,
						&look_ahead);

  if(rdf_iriref_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-iri");
    xmlAddChild(node,
		rdf_iriref_node);
      
    *iter = look_ahead;
  }else{
    /* alternate read prefixed name */
    rdf_prefixed_name_node = ags_turtle_load_read_prefixed_name(turtle,
								buffer, buffer_length,
								&look_ahead);

    if(rdf_prefixed_name_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-iri");
      xmlAddChild(node,
		  rdf_prefixed_name_node);
      
      *iter = look_ahead;
    }
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_prefix_id(AgsTurtle *turtle,
			       gchar *buffer, gsize buffer_length,
			       gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_pname_ns_node, *rdf_iriref_node;

  regmatch_t match_arr[1];

  gchar *look_ahead;
    
  static regex_t prefix_id_regex;

  static gboolean regex_compiled = FALSE;

  static const char *prefix_id_pattern = "^(@prefix)";
    
  static const size_t max_matches = 1;

  node = NULL;
  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&prefix_id_regex, prefix_id_pattern, REG_EXTENDED);
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* match @prefix */
  if(ags_regexec(&prefix_id_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
    rdf_pname_ns_node = ags_turtle_load_read_pname_ns(turtle,
						      buffer, buffer_length,
						      &look_ahead);
      
    rdf_iriref_node = ags_turtle_load_read_iriref(turtle,
						  buffer, buffer_length,
						  &look_ahead);

    /* create node if complete prefix id */
    if(rdf_pname_ns_node != NULL &&
       rdf_iriref_node != NULL){
      gchar *str;
	
      node = xmlNewNode(NULL,
			"rdf-prefix-id");
      xmlAddChild(node,
		  rdf_pname_ns_node);
      xmlAddChild(node,
		  rdf_iriref_node);

      str = xmlNodeGetContent(rdf_iriref_node);

      if(str != NULL &&
	 strlen(str) > 2){
	gchar *iriref;

	iriref = g_strndup(str + 1,
			   strlen(str) - 2);
	  
	g_hash_table_insert(turtle->prefix_id,
			    iriref, g_strdup(xmlNodeGetContent(rdf_pname_ns_node)));
      }
	
      *iter = look_ahead;
    }
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_base(AgsTurtle *turtle,
			  gchar *buffer, gsize buffer_length,
			  gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_iriref_node;

  regmatch_t match_arr[1];

  gchar *look_ahead;
    
  static regex_t base_regex;

  static gboolean regex_compiled = FALSE;

  static const char *base_pattern = "^(@base)";
    
  static const size_t max_matches = 1;

  node = NULL;
  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&base_regex, base_pattern, REG_EXTENDED);      
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* match @base */
  if(ags_regexec(&base_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
    rdf_iriref_node = ags_turtle_load_read_iriref(turtle,
						  buffer, buffer_length,
						  &look_ahead);

    /* create node if complete base */
    if(rdf_iriref_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-base");
      xmlAddChild(node,
		  rdf_iriref_node);
	
      *iter = look_ahead;
    }
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_sparql_prefix(AgsTurtle *turtle,
				   gchar *buffer, gsize buffer_length,
				   gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_pname_ns_node, *rdf_iriref_node;

  regmatch_t match_arr[1];

  gchar *look_ahead;
    
  static regex_t sparql_prefix_regex;

  static gboolean regex_compiled = FALSE;

  static const char *sparql_prefix_pattern = "^(\"PREFIX\")";
    
  static const size_t max_matches = 1;

  node = NULL;
  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&sparql_prefix_regex, sparql_prefix_pattern, REG_EXTENDED);
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* match @prefix */
  if(ags_regexec(&sparql_prefix_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
    rdf_pname_ns_node = ags_turtle_load_read_pname_ns(turtle,
						      buffer, buffer_length,
						      &look_ahead);
      
    rdf_iriref_node = ags_turtle_load_read_iriref(turtle,
						  buffer, buffer_length,
						  &look_ahead);

    /* create node if complete sparql prefix */
    if(rdf_pname_ns_node != NULL &&
       rdf_iriref_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-sparql-prefix");
      xmlAddChild(node,
		  rdf_pname_ns_node);
      xmlAddChild(node,
		  rdf_iriref_node);
	
      *iter = look_ahead;
    }
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_sparql_base(AgsTurtle *turtle,
				 gchar *buffer, gsize buffer_length,
				 gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_iriref_node;

  regmatch_t match_arr[1];

  gchar *look_ahead;
    
  static regex_t sparql_base_regex;

  static gboolean regex_compiled = FALSE;

  static const char *sparql_base_pattern = "^(\"BASE\")";
    
  static const size_t max_matches = 1;

  node = NULL;
  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&sparql_base_regex, sparql_base_pattern, REG_EXTENDED);      
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* match @base */
  if(ags_regexec(&sparql_base_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
    rdf_iriref_node = ags_turtle_load_read_iriref(turtle,
						  buffer, buffer_length,
						  &look_ahead);

    /* create node if complete sparqle base */
    if(rdf_iriref_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-sparql-base");
      xmlAddChild(node,
		  rdf_iriref_node);
	
      *iter = look_ahead;
    }
  }

  return(node);
}
  
xmlNode*
ags_turtle_load_read_prefixed_name(AgsTurtle *turtle,
				   gchar *buffer, gsize buffer_length,
				   gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_pname_ns_node, *rdf_pname_ln_node;

  gchar *look_ahead;

  node = NULL;
  look_ahead = *iter;

  rdf_pname_ln_node = ags_turtle_load_read_pname_ln(turtle,
						    buffer, buffer_length,
						    &look_ahead);

  if(rdf_pname_ln_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-prefixed-name");
    xmlAddChild(node,
		rdf_pname_ln_node);		  

    *iter = look_ahead;
  }else{
    rdf_pname_ns_node = ags_turtle_load_read_pname_ns(turtle,
						      buffer, buffer_length,
						      &look_ahead);

    if(rdf_pname_ns_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-prefixed-name");
      xmlAddChild(node,
		  rdf_pname_ns_node);

      *iter = look_ahead;
    }
  }

  return(node);
}

xmlNode*
ags_turtle_load_read_blank_node(AgsTurtle *turtle,
				gchar *buffer, gsize buffer_length,
				gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_anon_node;

  gchar *rdf_blank_node_label;
  gchar *look_ahead;
    
  node = NULL;
  look_ahead = *iter;    

  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  rdf_blank_node_label = ags_turtle_read_blank_node_label(look_ahead,
							  &(buffer[buffer_length]));
    
  if(rdf_blank_node_label != NULL){
    node = xmlNewNode(NULL,
		      "rdf-blank-node");
    xmlNodeSetContent(node,
		      g_ascii_strdown(rdf_blank_node_label,
				      -1));

    g_free(rdf_blank_node_label);
      
    *iter = look_ahead + strlen(rdf_blank_node_label);
  }else{
    rdf_anon_node = ags_turtle_load_read_anon(turtle,
					      buffer, buffer_length,
					      &look_ahead);

    if(rdf_anon_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-blank-node");
      xmlAddChild(node,
		  rdf_anon_node);

      *iter = look_ahead;
    }
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_statement(AgsTurtle *turtle,
			       gchar *buffer, gsize buffer_length,
			       gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_directive_node, *rdf_triple_node;
    
  gchar *look_ahead;
  gchar *next;
    
  node = NULL;
  look_ahead = *iter;    

  rdf_directive_node = ags_turtle_load_read_directive(turtle,
						      buffer, buffer_length,
						      &look_ahead);

  if(rdf_directive_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-statement");
    xmlAddChild(node,
		rdf_directive_node);		  

#ifdef AGS_DEBUG
    g_message("====");
#endif
      
    next = strchr(look_ahead,
		  '.');

    if(next != NULL){
      *iter = next + 1;
    }else{
      *iter = &(buffer[buffer_length]);
	
      g_critical("malformed RDF Turtle");
    }
  }else{
    rdf_triple_node = ags_turtle_load_read_triple(turtle,
						  buffer, buffer_length,
						  &look_ahead);

    if(rdf_triple_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-statement");
      xmlAddChild(node,
		  rdf_triple_node);

#ifdef AGS_DEBUG
      g_message("-----");
#endif
	
      next = strchr(look_ahead,
		    '.');

      if(next != NULL){
	*iter = next + 1;
      }else{
	*iter = &(buffer[buffer_length]);
	
	g_critical("malformed RDF Turtle");
      }
    }
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_verb(AgsTurtle *turtle,
			  gchar *buffer, gsize buffer_length,
			  gchar **iter)
{
  xmlNode *node;
  xmlNode *predicate_node;
    
  gchar *look_ahead;

  node = NULL;
  look_ahead = *iter;

  /* predicate */
  predicate_node = ags_turtle_load_read_predicate(turtle,
						  buffer, buffer_length,
						  &look_ahead);
	  
  if(predicate_node != NULL){
#ifdef AGS_DEBUG
    g_message("read rdf-verb");
#endif
      
    node = xmlNewNode(NULL,
		      "rdf-verb");
	  
    xmlAddChild(node,
		predicate_node);
	
    *iter = look_ahead;
  }else{
    /* skip blanks and comments */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							  buffer, buffer_length,
							  &look_ahead);
      
    if(*look_ahead == 'a'){
#ifdef AGS_DEBUG
      g_message("read - verb: a");
#endif
	
      node = xmlNewNode(NULL,
			"rdf-verb");
      
      /* verb - a */
      xmlNewProp(node,
		 "verb",
		 "a");

      look_ahead += 1;
      *iter = look_ahead;
    }
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_predicate(AgsTurtle *turtle,
			       gchar *buffer, gsize buffer_length,
			       gchar **iter)
{
  xmlNode *node;
  xmlNode *iri_node;

  gchar *look_ahead;
    
  node = NULL;
  look_ahead = *iter;

  /* iri - IRIREF */
  iri_node = ags_turtle_load_read_iri(turtle,
				      buffer, buffer_length,
				      &look_ahead);
    
  if(iri_node != NULL){
#ifdef AGS_DEBUG
    g_message("read rdf-predicate");
#endif
      
    node = xmlNewNode(NULL,
		      "rdf-predicate");
    xmlAddChild(node,
		iri_node);

    *iter = look_ahead;      
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_object(AgsTurtle *turtle,
			    gchar *buffer, gsize buffer_length,
			    gchar **iter)
{
  xmlNode *node;
  xmlNode *iri_node;
  xmlNode *blank_node;
  xmlNode *collection_node;
  xmlNode *blank_node_property_list_node;
  xmlNode *literal_node;
    
  gchar *look_ahead;
    
  node = NULL;
  iri_node = NULL;
  blank_node = NULL;
  collection_node = NULL;
  blank_node_property_list_node = NULL;
  literal_node = NULL;
    
  look_ahead = *iter;

  /* iri - IRIREF */
  iri_node = ags_turtle_load_read_iri(turtle,
				      buffer, buffer_length,
				      &look_ahead);

  if(iri_node != NULL){
    goto ags_turtle_load_read_object_CREATE_NODE;
  }

  /* read blank node */
  blank_node = ags_turtle_load_read_blank_node(turtle,
					       buffer, buffer_length,
					       &look_ahead);

  if(blank_node != NULL){
    goto ags_turtle_load_read_object_CREATE_NODE;
  }

  /* skip blanks and comments */
  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  /* collection */
  if(*look_ahead == '('){
    collection_node = ags_turtle_load_read_collection(turtle,
						      buffer, buffer_length,
						      &look_ahead);
	
    goto ags_turtle_load_read_object_CREATE_NODE;
  }

  /* blank node property listimplemented ags_turtle_load_read_object() */
  if(*look_ahead == '['){
    blank_node_property_list_node = ags_turtle_load_read_blank_node_property_list(turtle,
										  buffer, buffer_length,
										  &look_ahead);
	
    goto ags_turtle_load_read_object_CREATE_NODE;
  }

  /* literal */
#ifdef AGS_DEBUG
  g_message("*** PASS ***");
#endif
    
  literal_node = ags_turtle_load_read_literal(turtle,
					      buffer, buffer_length,
					      &look_ahead);
    
  if(literal_node != NULL){
    goto ags_turtle_load_read_object_CREATE_NODE;
  }

  /* create node */
ags_turtle_load_read_object_CREATE_NODE:

  if(iri_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-object");
    xmlAddChild(node,
		iri_node);

    *iter = look_ahead;
  }else if(blank_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-object");
    xmlAddChild(node,
		blank_node);

    *iter = look_ahead;
  }else if(collection_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-object");
    xmlAddChild(node,
		collection_node);

    *iter = look_ahead;
  }else if(blank_node_property_list_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-object");
    xmlAddChild(node,
		blank_node_property_list_node);

    *iter = look_ahead;
  }else if(literal_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-object");
    xmlAddChild(node,
		literal_node);

    *iter = look_ahead;
  }

#ifdef AGS_DEBUG
  if(node != NULL){
    g_message("read rdf-object");
  }
#endif
    
  return(node);
}

xmlNode*
ags_turtle_load_read_directive(AgsTurtle *turtle,
			       gchar *buffer, gsize buffer_length,
			       gchar **iter)
{
  xmlNode *node;
  xmlNode *rdf_prefix_id_node, *rdf_base_node, *rdf_sparql_prefix_node, *rdf_sparql_base_node;
    
  gchar *look_ahead, *current;

  node = NULL;
  look_ahead = *iter;
    
  rdf_prefix_id_node = ags_turtle_load_read_prefix_id(turtle,
						      buffer, buffer_length,
						      &look_ahead);

  if(rdf_prefix_id_node != NULL){
    goto ags_turtle_load_read_directive_CREATE_NODE;
  }
    
  rdf_base_node = ags_turtle_load_read_base(turtle,
					    buffer, buffer_length,
					    &look_ahead);

  if(rdf_base_node != NULL){
    goto ags_turtle_load_read_directive_CREATE_NODE;
  }
    
  rdf_sparql_prefix_node = ags_turtle_load_read_sparql_prefix(turtle,
							      buffer, buffer_length,
							      &look_ahead);

  if(rdf_sparql_prefix_node != NULL){
    goto ags_turtle_load_read_directive_CREATE_NODE;
  }
    
  rdf_sparql_base_node = ags_turtle_load_read_sparql_base(turtle,
							  buffer, buffer_length,
							  &look_ahead);

  if(rdf_sparql_base_node != NULL){
    goto ags_turtle_load_read_directive_CREATE_NODE;
  }
    
  /* create node */
ags_turtle_load_read_directive_CREATE_NODE:
    
  if(rdf_prefix_id_node != NULL){
#ifdef AGS_DEBUG
    g_message("read - rdf-directive");
#endif
      
    node = xmlNewNode(NULL,
		      "rdf-directive");

    xmlAddChild(node,
		rdf_prefix_id_node);

    *iter = look_ahead;
  }else if(rdf_base_node != NULL){
#ifdef AGS_DEBUG
    g_message("read - rdf-directive");
#endif
      
    node = xmlNewNode(NULL,
		      "rdf-directive");

    xmlAddChild(node,
		rdf_base_node);

    *iter = look_ahead;
  }else if(rdf_sparql_prefix_node != NULL){
#ifdef AGS_DEBUG
    g_message("read - rdf-directive");
#endif
      
    node = xmlNewNode(NULL,
		      "rdf-directive");

    xmlAddChild(node,
		rdf_sparql_prefix_node);

    *iter = look_ahead;
  }else if(rdf_sparql_base_node != NULL){
#ifdef AGS_DEBUG
    g_message("read - rdf-directive");
#endif
      
    node = xmlNewNode(NULL,
		      "rdf-directive");

    xmlAddChild(node,
		rdf_sparql_base_node);

    *iter = look_ahead;
  }
    
  return(node);
}

xmlNode*
ags_turtle_load_read_triple(AgsTurtle *turtle,
			    gchar *buffer, gsize buffer_length,
			    gchar **iter)
{
  xmlNode *node;
  xmlNode *subject_node,  *predicate_object_list_node_0, *blank_node_object_list_node,  *predicate_object_list_node_1;
    
  gchar *look_ahead, *current;

  node = NULL;
  look_ahead = *iter;
	
  /* subject */
  subject_node = ags_turtle_load_read_subject(turtle,
					      buffer, buffer_length,
					      &look_ahead);

  /* create triple node */
  if(subject_node != NULL){
    /* first predicate object list */
    predicate_object_list_node_0 = ags_turtle_load_read_predicate_object_list(turtle,
									      buffer, buffer_length,
									      &look_ahead);

    if(predicate_object_list_node_0 != NULL){
      node = xmlNewNode(NULL,
			"rdf-triple");
	
      xmlAddChild(node,
		  subject_node);
      xmlAddChild(node,
		  predicate_object_list_node_0);

      /* second predicate object list */
      predicate_object_list_node_1 = ags_turtle_load_read_predicate_object_list(turtle,
										buffer, buffer_length,
										&look_ahead);
	
      if(predicate_object_list_node_1 != NULL){
	xmlAddChild(node,
		    predicate_object_list_node_1);
      }
	
#ifdef AGS_DEBUG
      g_message("read rdf-triple");
#endif
	
      *iter = look_ahead;
    }else{
      /* alternate first blank node object list */
      blank_node_object_list_node = ags_turtle_load_read_blank_node_property_list(turtle,
										  buffer, buffer_length,
										  &look_ahead);

      if(blank_node_object_list_node != NULL){
	node = xmlNewNode(NULL,
			  "rdf-triple");
	  
	xmlAddChild(node,
		    blank_node_object_list_node);
	  
	/* predicate object list */
	predicate_object_list_node_1 = ags_turtle_load_read_predicate_object_list(turtle,
										  buffer, buffer_length,
										  &look_ahead);
	
	if(predicate_object_list_node_1 != NULL){
	  xmlAddChild(node,
		      predicate_object_list_node_1);
	}
	  
#ifdef AGS_DEBUG
	g_message("read rdf-triple");
#endif
	  
	*iter = look_ahead;
      }
    }
  }

  return(node);
}

xmlNode*
ags_turtle_load_read_subject(AgsTurtle *turtle,
			     gchar *buffer, gsize buffer_length,
			     gchar **iter)
{
  xmlNode *node;
  xmlNode *iri_node;
  xmlNode *blank_node;
  xmlNode *collection_node;

  gchar *look_ahead;
    
  node = NULL;

  iri_node =  NULL;
  blank_node = NULL;
  collection_node = NULL;
    
  look_ahead = *iter;

  /* iri - IRIREF */
  iri_node = ags_turtle_load_read_iri(turtle,
				      buffer, buffer_length,
				      &look_ahead);

  if(iri_node != NULL){
    goto ags_turtle_load_read_subject_CREATE_NODE;
  }

  /* read blank node */
  blank_node = ags_turtle_load_read_blank_node(turtle,
					       buffer, buffer_length,
					       &look_ahead);

  if(blank_node != NULL){
    goto ags_turtle_load_read_subject_CREATE_NODE;
  }
      
  /* collection */
  if(*look_ahead == '('){
    collection_node = ags_turtle_load_read_collection(turtle,
						      buffer, buffer_length,
						      &look_ahead);
	
    goto ags_turtle_load_read_subject_CREATE_NODE;
  }

  /* create node */
ags_turtle_load_read_subject_CREATE_NODE:

  if(iri_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-subject");
    xmlAddChild(node,
		iri_node);

    *iter = look_ahead;
  }else if(blank_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-subject");
    xmlAddChild(node,
		blank_node);

    *iter = look_ahead;
  }else if(collection_node != NULL){
    node = xmlNewNode(NULL,
		      "rdf-subject");
    xmlAddChild(node,
		collection_node);

    *iter = look_ahead;
  }

#ifdef AGS_DEBUG
  if(node != NULL){
    g_message("read rdf-subject");
  }
#endif
    
  return(node);
}

xmlNode*
ags_turtle_load_read_object_list(AgsTurtle *turtle,
				 gchar *buffer, gsize buffer_length,
				 gchar **iter)
{
  xmlNode *node;
  xmlNode *object_node;
    
  gchar *look_ahead;
  gchar *start_ptr, *end_ptr;
    
  node = NULL;
  look_ahead = *iter;

  /* read first object */
  object_node = ags_turtle_load_read_object(turtle,
					    buffer, buffer_length,
					    &look_ahead);
    
  if(object_node != NULL){
    /* create node */
    node = xmlNewNode(NULL,
		      "rdf-object-list");
    xmlAddChild(node,
		object_node);

    /* iterate */
    look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							  buffer, buffer_length,
							  &look_ahead);
	
    while(*look_ahead == ','){
      look_ahead++;
	
      object_node = ags_turtle_load_read_object(turtle,
						buffer, buffer_length,
						&look_ahead);

      if(object_node != NULL){
	xmlAddChild(node,
		    object_node);

	look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							      buffer, buffer_length,
							      &look_ahead);
      }
    }
      
    *iter = look_ahead;
  }

#ifdef AGS_DEBUG
  if(node != NULL){
    g_message("read rdf-object-list");
  }
#endif
    
  return(node);
}  
  
xmlNode*
ags_turtle_load_read_collection(AgsTurtle *turtle,
				gchar *buffer, gsize buffer_length,
				gchar **iter)
{
  xmlNode *node;
  xmlNode *object_node;
    
  gchar *look_ahead;
  gchar *start_ptr, *end_ptr;
    
  node = NULL;
  look_ahead = *iter;

  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);
    
  if(*look_ahead == '\0'){
    return(NULL);
  }

  if(*look_ahead == '('){
    start_ptr = look_ahead;
    look_ahead++;

    /* create node */
    node = xmlNewNode(NULL,
		      "rdf-collection");
      
    /* read objects */
    while((object_node = ags_turtle_load_read_object(turtle,
						     buffer, buffer_length,
						     &look_ahead)) != NULL){
#ifdef AGS_DEBUG
      g_message("read rdf-collection");
#endif
	
      xmlAddChild(node,
		  object_node);
    }

    end_ptr = strchr(look_ahead,
		     ')');

    if(end_ptr != NULL){
      *iter = end_ptr + 1;
    }else{
      *iter = &(buffer[buffer_length]);
    }
  }

  return(node);
}

xmlNode*
ags_turtle_load_read_predicate_object_list(AgsTurtle *turtle,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter)
{
  xmlNode *node;
  xmlNode *current_verb_node, *current_object_list_node;

  gchar *look_ahead;
    
  node = NULL;
  look_ahead = *iter;

  current_verb_node = ags_turtle_load_read_verb(turtle,
						buffer, buffer_length,
						&look_ahead);

  if(current_verb_node != NULL){
    current_object_list_node = ags_turtle_load_read_object_list(turtle,
								buffer, buffer_length,
								&look_ahead);

    if(current_object_list_node != NULL){
      node = xmlNewNode(NULL,
			"rdf-predicate-object-list");

      xmlAddChild(node,
		  current_verb_node);
      xmlAddChild(node,
		  current_object_list_node);

      /* iterate */	
      look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							    buffer, buffer_length,
							    &look_ahead);
	
      while(*look_ahead == ';'){
	look_ahead++;
	current_verb_node = ags_turtle_load_read_verb(turtle,
						      buffer, buffer_length,
						      &look_ahead);

	if(current_verb_node != NULL){
	  current_object_list_node = ags_turtle_load_read_object_list(turtle,
								      buffer, buffer_length,
								      &look_ahead);
      
	  if(current_object_list_node != NULL){
	    xmlAddChild(node,
			current_verb_node);
	    xmlAddChild(node,
			current_object_list_node);
	      
	    look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
								  buffer, buffer_length,
								  &look_ahead);
	  }else{
	    break;
	  }
	}else{
	  break;
	}
	  
      }

      *iter = look_ahead;
    }
  }
    
  return(node);
}
  
xmlNode*
ags_turtle_load_read_blank_node_property_list(AgsTurtle *turtle,
					      gchar *buffer, gsize buffer_length,
					      gchar **iter)
{
  xmlNode *node;
  xmlNode *predicate_object_list_node;

  gchar *look_ahead;
  gchar *start_ptr, *end_ptr;
    
  node = NULL;
  look_ahead = *iter;

  look_ahead = ags_turtle_load_skip_comments_and_blanks(turtle,
							buffer, buffer_length,
							&look_ahead);

  if(*look_ahead == '['){
    start_ptr = look_ahead;
    look_ahead++;
      
    predicate_object_list_node = ags_turtle_load_read_predicate_object_list(turtle,
									    buffer, buffer_length,
									    &look_ahead);

    if(predicate_object_list_node != NULL){
#ifdef AGS_DEBUG
      g_message("read rdf-blank-node-property-list");
#endif
	
      node = xmlNewNode(NULL,
			"rdf-blank-node-property-list");

      xmlAddChild(node,
		  predicate_object_list_node);

      end_ptr = strchr(look_ahead,
		       ']');

      if(end_ptr != NULL){
	*iter = end_ptr + 1;
      }else{
	*iter = &(buffer[buffer_length]);
      }
    }
  }
    
  return(node);
}  

/**
 * ags_turtle_load:
 * @turtle: the #AgsTurtle
 * @error: a pointer to a #GError
 *
 * Loads a RDF triple file into an XML Document.
 *
 * Returns: (transfer none): a #xmlDoc pointer
 *
 * Since: 3.0.0
 */
xmlDoc*
ags_turtle_load(AgsTurtle *turtle,
		GError **error)
{
  xmlDoc *doc;
  xmlNode *root_node, *rdf_statement_node;
  FILE *file;

  struct stat *sb;
  
  gchar *buffer, *iter;

  size_t n_read;
  
#ifdef AGS_DEBUG
  g_message("file: %s", turtle->filename);
#endif
  
  /* entry point - open file and read it */
  sb = (struct stat *) g_malloc(sizeof(struct stat));
  stat(turtle->filename,
       sb);
  file = fopen(turtle->filename,
	       "r");
  
  if(file == NULL ||
     sb->st_size <= 0){
    g_free(sb);
    
    return(NULL);
  }

  buffer = (gchar *) g_malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    g_free(sb);

    return(NULL);
  }
  
  n_read = fread(buffer, sizeof(gchar), sb->st_size, file);

  if(n_read != sb->st_size){
    g_critical("number of read bytes doesn't match buffer size");
  }
  
  buffer[sb->st_size] = '\0';
  fclose(file);

  /* alloc document */
  doc =
    turtle->doc = xmlNewDoc("1.0");
  root_node = xmlNewNode(NULL, "rdf-turtle-doc");
  xmlDocSetRootElement(doc, root_node);
  xmlNewProp(root_node,
	     "version",
	     AGS_TURTLE_DEFAULT_VERSION);
  
  iter = buffer;

  do{
    /* skip blanks and comments */
    iter = ags_turtle_load_skip_comments_and_blanks(turtle,
						    buffer, sb->st_size,
						    &iter);

    if(iter >= &(buffer[sb->st_size])){
      break;
    }
    
    rdf_statement_node = ags_turtle_load_read_statement(turtle,
							buffer, sb->st_size,
							&iter);

    if(rdf_statement_node != NULL){
      xmlAddChild(root_node,
		  rdf_statement_node);
    }

    if(rdf_statement_node == NULL){
      iter++;
    }
  }while(iter < &(buffer[sb->st_size]));
  
  g_free(sb);
  g_free(buffer);
  
//  xmlCleanupParser();
//  xmlMemoryDump();
  
  return(doc);
}

/**
 * ags_turtle_new:
 * @filename: (nullable): the filename as string
 *
 * Instantiate a new #AgsTurtle.
 *
 * Returns: the new instance
 *
 * Since: 3.0.0
 */
AgsTurtle*
ags_turtle_new(gchar *filename)
{
  AgsTurtle *turtle;
  
  turtle = g_object_new(AGS_TYPE_TURTLE,
			"filename", filename,
			NULL);

  return(turtle);
}
