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

#include <ags/plugin/ags_lv2_turtle_scanner.h>

#include <ags/plugin/ags_lv2_manager.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <regex.h>

#include <sys/stat.h>
#include <unistd.h>

#include <ags/i18n.h>

void ags_lv2_turtle_scanner_class_init(AgsLv2TurtleScannerClass *lv2_turtle_scanner);
void ags_lv2_turtle_scanner_init (AgsLv2TurtleScanner *lv2_turtle_scanner);
void ags_lv2_turtle_scanner_set_property(GObject *gobject,
					 guint prop_id,
					 const GValue *value,
					 GParamSpec *param_spec);
void ags_lv2_turtle_scanner_get_property(GObject *gobject,
					 guint prop_id,
					 GValue *value,
					 GParamSpec *param_spec);
void ags_lv2_turtle_scanner_dispose(GObject *gobject);
void ags_lv2_turtle_scanner_finalize(GObject *gobject);

gchar* ags_lv2_turtle_scanner_load_skip_comments_and_blanks(AgsLv2TurtleScanner *lv2_turtle_scanner,
							    gchar *buffer, gsize buffer_length,
							    gchar **iter);
void ags_lv2_turtle_scanner_load_read_iriref(AgsLv2TurtleScanner *lv2_turtle_scanner,
					     gchar *buffer, gsize buffer_length,
					     gchar **iter);
void ags_lv2_turtle_scanner_load_read_anon(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter);
void ags_lv2_turtle_scanner_load_read_pname_ns(AgsLv2TurtleScanner *lv2_turtle_scanner,
					       gchar *buffer, gsize buffer_length,
					       gchar **iter);
void ags_lv2_turtle_scanner_load_read_pname_ln(AgsLv2TurtleScanner *lv2_turtle_scanner,
					       gchar *buffer, gsize buffer_length,
					       gchar **iter);
void ags_lv2_turtle_scanner_load_read_numeric(AgsLv2TurtleScanner *lv2_turtle_scanner,
					      gchar *buffer, gsize buffer_length,
					      gchar **iter);
void ags_lv2_turtle_scanner_load_read_string(AgsLv2TurtleScanner *lv2_turtle_scanner,
					     gchar *buffer, gsize buffer_length,
					     gchar **iter);
void ags_lv2_turtle_scanner_load_read_langtag(AgsLv2TurtleScanner *lv2_turtle_scanner,
					      gchar *buffer, gsize buffer_length,
					      gchar **iter);
void ags_lv2_turtle_scanner_load_read_literal(AgsLv2TurtleScanner *lv2_turtle_scanner,
					      gchar *buffer, gsize buffer_length,
					      gchar **iter);
void ags_lv2_turtle_scanner_load_read_iri(AgsLv2TurtleScanner *lv2_turtle_scanner,
					  gchar *buffer, gsize buffer_length,
					  gchar **iter);
void ags_lv2_turtle_scanner_load_read_prefix_id(AgsLv2TurtleScanner *lv2_turtle_scanner,
						gchar *buffer, gsize buffer_length,
						gchar **iter);
void ags_lv2_turtle_scanner_load_read_base(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter);
void ags_lv2_turtle_scanner_load_read_sparql_prefix(AgsLv2TurtleScanner *lv2_turtle_scanner,
						    gchar *buffer, gsize buffer_length,
						    gchar **iter);
void ags_lv2_turtle_scanner_load_read_sparql_base(AgsLv2TurtleScanner *lv2_turtle_scanner,
						  gchar *buffer, gsize buffer_length,
						  gchar **iter);
void ags_lv2_turtle_scanner_load_read_prefixed_name(AgsLv2TurtleScanner *lv2_turtle_scanner,
						    gchar *buffer, gsize buffer_length,
						    gchar **iter);
void ags_lv2_turtle_scanner_load_read_blank_node(AgsLv2TurtleScanner *lv2_turtle_scanner,
						 gchar *buffer, gsize buffer_length,
						 gchar **iter);
void ags_lv2_turtle_scanner_load_read_statement(AgsLv2TurtleScanner *lv2_turtle_scanner,
						gchar *buffer, gsize buffer_length,
						gchar **iter);
void ags_lv2_turtle_scanner_load_read_verb(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter);
void ags_lv2_turtle_scanner_load_read_predicate(AgsLv2TurtleScanner *lv2_turtle_scanner,
						gchar *buffer, gsize buffer_length,
						gchar **iter);
void ags_lv2_turtle_scanner_load_read_object(AgsLv2TurtleScanner *lv2_turtle_scanner,
					     gchar *buffer, gsize buffer_length,
					     gchar **iter);
void ags_lv2_turtle_scanner_load_read_directive(AgsLv2TurtleScanner *lv2_turtle_scanner,
						gchar *buffer, gsize buffer_length,
						gchar **iter);
void ags_lv2_turtle_scanner_load_read_triple(AgsLv2TurtleScanner *lv2_turtle_scanner,
					     gchar *buffer, gsize buffer_length,
					     gchar **iter);
void ags_lv2_turtle_scanner_load_read_subject(AgsLv2TurtleScanner *lv2_turtle_scanner,
					      gchar *buffer, gsize buffer_length,
					      gchar **iter);
void ags_lv2_turtle_scanner_load_read_object_list(AgsLv2TurtleScanner *lv2_turtle_scanner,
						  gchar *buffer, gsize buffer_length,
						  gchar **iter);
void ags_lv2_turtle_scanner_load_read_collection(AgsLv2TurtleScanner *lv2_turtle_scanner,
						 gchar *buffer, gsize buffer_length,
						 gchar **iter);
void ags_lv2_turtle_scanner_load_read_blank_node_property_list(AgsLv2TurtleScanner *lv2_turtle_scanner,
							       gchar *buffer, gsize buffer_length,
							       gchar **iter);
void ags_lv2_turtle_scanner_load_read_predicate_object_list(AgsLv2TurtleScanner *lv2_turtle_scanner,
							    gchar *buffer, gsize buffer_length,
							    gchar **iter);

/**
 * SECTION:ags_lv2_turtle_scanner
 * @short_description: The lv2 turtle scanner class
 * @title: AgsLv2TurtleScanner
 * @section_id:
 * @include: ags/plugin/ags_lv2_turtle_scanner.h
 *
 * The #AgsLv2TurtleScanner parses RDF Turtle files.
 */

enum{
  PROP_0,
};

static gpointer ags_lv2_turtle_scanner_parent_class = NULL;

static GMutex regex_mutex;

GType
ags_lv2_turtle_scanner_get_type (void)
{
  static volatile gsize g_define_type_id__volatile = 0;

  if(g_once_init_enter (&g_define_type_id__volatile)){
    GType ags_type_lv2_turtle_scanner = 0;

    static const GTypeInfo ags_lv2_turtle_scanner_info = {
      sizeof(AgsLv2TurtleScannerClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_lv2_turtle_scanner_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof(AgsLv2TurtleScanner),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_lv2_turtle_scanner_init,
    };

    ags_type_lv2_turtle_scanner = g_type_register_static(G_TYPE_OBJECT,
							 "AgsLv2TurtleScanner",
							 &ags_lv2_turtle_scanner_info,
							 0);

    g_once_init_leave(&g_define_type_id__volatile, ags_type_lv2_turtle_scanner);
  }

  return g_define_type_id__volatile;
}

GType
ags_lv2_turtle_scanner_status_flags_get_type()
{
  static volatile gsize g_flags_type_id__volatile;

  if(g_once_init_enter (&g_flags_type_id__volatile)){
    static const GFlagsValue values[] = {
      { AGS_LV2_TURTLE_SCANNER_A, "AGS_LV2_TURTLE_SCANNER_IS_SYNTHESIZER", "lv2-turtle-scanner-a" },
      { AGS_LV2_TURTLE_SCANNER_SEEALSO, "AGS_LV2_TURTLE_SCANNER_IS_SYNTHESIZER", "lv2-turtle-scanner-seealso" },
      { AGS_LV2_TURTLE_SCANNER_BINARY, "AGS_LV2_TURTLE_SCANNER_IS_SYNTHESIZER", "lv2-turtle-scanner-binary" },
      { AGS_LV2_TURTLE_SCANNER_NAME, "AGS_LV2_TURTLE_SCANNER_NAME", "lv2-turtle-scanner-name" },
      { 0, NULL, NULL }
    };

    GType g_flags_type_id = g_flags_register_static(g_intern_static_string("AgsLv2TurtleScannerStatusFlags"), values);

    g_once_init_leave (&g_flags_type_id__volatile, g_flags_type_id);
  }
  
  return g_flags_type_id__volatile;
}

void
ags_lv2_turtle_scanner_class_init(AgsLv2TurtleScannerClass *lv2_turtle_scanner)
{
  GObjectClass *gobject;

  GParamSpec *param_spec;
  
  ags_lv2_turtle_scanner_parent_class = g_type_class_peek_parent(lv2_turtle_scanner);

  /* GObjectClass */
  gobject = (GObjectClass *) lv2_turtle_scanner;

  gobject->dispose = ags_lv2_turtle_scanner_dispose;
  gobject->finalize = ags_lv2_turtle_scanner_finalize;
}

void
ags_lv2_turtle_scanner_init(AgsLv2TurtleScanner *lv2_turtle_scanner)
{
  lv2_turtle_scanner->flags = 0;

  lv2_turtle_scanner->status_flags = 0;

  /* add base plugin mutex */
  g_rec_mutex_init(&(lv2_turtle_scanner->obj_mutex));

  lv2_turtle_scanner->current_subject_iriref = NULL;

  lv2_turtle_scanner->plugin_position = -1;
  
  /*  */
  lv2_turtle_scanner->cache_turtle = NULL;
}

void
ags_lv2_turtle_scanner_dispose(GObject *gobject)
{
  AgsLv2TurtleScanner *lv2_turtle_scanner;

  lv2_turtle_scanner = AGS_LV2_TURTLE_SCANNER(gobject);

  /* call parent */
  G_OBJECT_CLASS(ags_lv2_turtle_scanner_parent_class)->dispose(gobject);
}

void
ags_lv2_turtle_scanner_finalize(GObject *gobject)
{
  AgsLv2TurtleScanner *lv2_turtle_scanner;

  lv2_turtle_scanner = AGS_LV2_TURTLE_SCANNER(gobject);

  g_list_free_full(lv2_turtle_scanner->cache_turtle,
		   (GDestroyNotify) ags_lv2_cache_turtle_free);

  g_free(lv2_turtle_scanner->current_subject_iriref);
  
  /* call parent */
  G_OBJECT_CLASS(ags_lv2_turtle_scanner_parent_class)->finalize(gobject);
}

/**
 * ags_lv2_cache_turtle_alloc:
 * @parent: the parent #AgsLv2CacheTurtle-struct
 * @turtle_filename: the .ttl filename
 *
 * Allocate an #AgsLv2CacheTurtle-struct
 *
 * Returns: a new #AgsLv2CacheTurtle-struct
 *
 * Since: 3.2.7
 */
AgsLv2CacheTurtle*
ags_lv2_cache_turtle_alloc(AgsLv2CacheTurtle *parent,
			   gchar *turtle_filename)
{
  AgsLv2CacheTurtle *lv2_cache_turtle;

  lv2_cache_turtle = (AgsLv2CacheTurtle *) g_malloc(sizeof(AgsLv2CacheTurtle));

  lv2_cache_turtle->parent = parent;

  lv2_cache_turtle->turtle_filename = g_strdup(turtle_filename);

  lv2_cache_turtle->prefix_id = g_hash_table_new_full(g_str_hash, g_str_equal,
						      g_free,
						      g_free);

  lv2_cache_turtle->plugin_filename = g_hash_table_new_full(g_str_hash, g_str_equal,
							    g_free,
							    g_free);
  lv2_cache_turtle->plugin_effect = g_hash_table_new_full(g_str_hash, g_str_equal,
							  g_free,
							  g_free);

  lv2_cache_turtle->is_plugin = g_hash_table_new_full(g_str_hash, g_str_equal,
						      g_free,
						      NULL);
  lv2_cache_turtle->is_instrument = g_hash_table_new_full(g_str_hash, g_str_equal,
							  g_free,
							  NULL);

  lv2_cache_turtle->plugin_count = 0;
  
  lv2_cache_turtle->see_also = NULL;
  
  return(lv2_cache_turtle);
}

/**
 * ags_lv2_cache_turtle_free:
 * @lv2_cache_turtle: the #AgsLv2CacheTurtle-struct
 *
 * Free an #AgsLv2CacheTurtle-struct
 *
 * Since: 3.2.7
 */
void
ags_lv2_cache_turtle_free(AgsLv2CacheTurtle *lv2_cache_turtle)
{
  if(lv2_cache_turtle == NULL){
    return;
  }

  g_free(lv2_cache_turtle->turtle_filename);

  g_hash_table_destroy(lv2_cache_turtle->prefix_id);

  g_hash_table_destroy(lv2_cache_turtle->plugin_filename);
  g_hash_table_destroy(lv2_cache_turtle->plugin_effect);

  g_hash_table_destroy(lv2_cache_turtle->is_plugin);
  g_hash_table_destroy(lv2_cache_turtle->is_instrument);

  g_strfreev(lv2_cache_turtle->see_also);
  
  g_free(lv2_cache_turtle);
}

/**
 * ags_lv2_cache_turtle_find:
 * @lv2_cache_turtle: a #GList-struct containing #AgsLv2CacheTurtle-struct
 * @turtle_filename: the .ttl filename
 * 
 * Find @lv2_cache_turtle matching @turtle_filename.
 * 
 * Returns: the next matching #GList-struct
 * 
 * Since: 3.2.11
 */
GList*
ags_lv2_cache_turtle_find(GList *lv2_cache_turtle,
			  gchar *turtle_filename)
{
  if(turtle_filename == NULL){
    return(NULL);
  }
  
  while(lv2_cache_turtle != NULL){
    if(AGS_LV2_CACHE_TURTLE(lv2_cache_turtle)->turtle_filename != NULL &&
       !g_strcmp0(AGS_LV2_CACHE_TURTLE(lv2_cache_turtle)->turtle_filename,
		  turtle_filename)){
      return(lv2_cache_turtle);
    }

    lv2_cache_turtle = lv2_cache_turtle->next;
  }
  
  return(NULL);
}

gchar*
ags_lv2_turtle_scanner_load_skip_comments_and_blanks(AgsLv2TurtleScanner *lv2_turtle_scanner,
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
    if(!(look_ahead[0] == ' ' || look_ahead[0] == '\t' || look_ahead[0] == '\n')){
      break;
    }

    look_ahead++;
  }

  return(look_ahead);
}

void
ags_lv2_turtle_scanner_load_read_iriref(AgsLv2TurtleScanner *lv2_turtle_scanner,
					gchar *buffer, gsize buffer_length,
					gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
  
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);
  
  /* match iriref */
  start_offset = NULL;
  end_offset = NULL;
  
  if(ags_turtle_match_iriref(look_ahead,
			     &(buffer[buffer_length]),
			     &start_offset, &end_offset)){
    iter[0] = end_offset;
  }
}

void
ags_lv2_turtle_scanner_load_read_anon(AgsLv2TurtleScanner *lv2_turtle_scanner,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match anon */
  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_anon(look_ahead,
			     &(buffer[buffer_length]),
			     &start_offset, &end_offset)){
    iter[0] = end_offset;
  }
}

void
ags_lv2_turtle_scanner_load_read_pname_ns(AgsLv2TurtleScanner *lv2_turtle_scanner,
					  gchar *buffer, gsize buffer_length,
					  gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* read pname-ns */
  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_pname_ns(look_ahead,
			       &(buffer[buffer_length]),
			       &start_offset, &end_offset)){
    iter[0] = end_offset;
  }
}

void
ags_lv2_turtle_scanner_load_read_pname_ln(AgsLv2TurtleScanner *lv2_turtle_scanner,
					  gchar *buffer, gsize buffer_length,
					  gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match pname-ln */
  start_offset = NULL;
  end_offset = NULL;
  
  if(ags_turtle_match_pname_ln(look_ahead,
			       &(buffer[buffer_length]),
			       &start_offset, &end_offset)){
    iter[0] = end_offset;
  }
}

void
ags_lv2_turtle_scanner_load_read_numeric(AgsLv2TurtleScanner *lv2_turtle_scanner,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* read numeric */
  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_decimal(look_ahead,
			      &(buffer[buffer_length]),
			      &start_offset, &end_offset)){
    iter[0] = end_offset;

    return;
  }

  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_double(look_ahead,
			     &(buffer[buffer_length]),
			     &start_offset, &end_offset)){
    iter[0] = end_offset;

    return;
  }

  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_integer(look_ahead,
			      &(buffer[buffer_length]),
			      &start_offset, &end_offset)){
    iter[0] = end_offset;

    return;
  }

  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_boolean(look_ahead,
			      &(buffer[buffer_length]),
			      &start_offset, &end_offset)){
    iter[0] = end_offset;

    return;
  }
}

void
ags_lv2_turtle_scanner_load_read_string(AgsLv2TurtleScanner *lv2_turtle_scanner,
					gchar *buffer, gsize buffer_length,
					gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match string */
  start_offset = NULL;
  end_offset = NULL;
  
  if(ags_turtle_match_string(look_ahead,
			     &(buffer[buffer_length]),
			     &start_offset, &end_offset)){
    iter[0] = end_offset;
  }
}

void
ags_lv2_turtle_scanner_load_read_langtag(AgsLv2TurtleScanner *lv2_turtle_scanner,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;
    
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match langtag */
  start_offset = NULL;
  end_offset = NULL;
  
  if(ags_turtle_match_langtag(look_ahead,
			      &(buffer[buffer_length]),
			      &start_offset, &end_offset)){
    iter[0] = end_offset;
  }
}

void
ags_lv2_turtle_scanner_load_read_literal(AgsLv2TurtleScanner *lv2_turtle_scanner,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter)
{
  gchar *look_ahead;
  gchar *str;
    
  look_ahead = *iter;

  /* read string */
  str = look_ahead;
  
  ags_lv2_turtle_scanner_load_read_string(lv2_turtle_scanner,
					  buffer, buffer_length,
					  &look_ahead);

  if(look_ahead != str){
    *iter = look_ahead;

    /* read optional langtag */
    str = look_ahead;
    
    ags_lv2_turtle_scanner_load_read_langtag(lv2_turtle_scanner,
					     buffer, buffer_length,
					     &look_ahead);
    
    if(look_ahead != str){
      *iter = look_ahead;
    }else{
      /* skip blanks and comments */
      ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
							   buffer, buffer_length,
							   &look_ahead);

      if(look_ahead + 2 < &(buffer[buffer_length]) &&
	 look_ahead[0] == '^' &&
	 look_ahead[1] == '^'){
	/* alternate read optional iri */
	str = look_ahead;
    
	ags_lv2_turtle_scanner_load_read_iri(lv2_turtle_scanner,
					     buffer, buffer_length,
					     &look_ahead);
	
	if(look_ahead != str){
	  *iter = look_ahead;
	}
      }
    }
  }else{
    ags_lv2_turtle_scanner_load_read_numeric(lv2_turtle_scanner,
					     buffer, buffer_length,
					     &look_ahead);

    if(look_ahead != str){
      *iter = look_ahead;
    }      
  }
}

void
ags_lv2_turtle_scanner_load_read_iri(AgsLv2TurtleScanner *lv2_turtle_scanner,
				     gchar *buffer, gsize buffer_length,
				     gchar **iter)
{
  gchar *look_ahead;
  gchar *str;
  
  look_ahead = *iter;

  /* read iriref */
  str = look_ahead;
  
  ags_lv2_turtle_scanner_load_read_iriref(lv2_turtle_scanner,
					  buffer, buffer_length,
					  &look_ahead);

  if(look_ahead != str){
    *iter = look_ahead;
  }else{
    /* alternate read prefixed name */
    ags_lv2_turtle_scanner_load_read_prefixed_name(lv2_turtle_scanner,
						   buffer, buffer_length,
						   &look_ahead);
    
    if(look_ahead != str){
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_prefix_id(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter)
{
  regmatch_t match_arr[1];

  gchar *look_ahead;
  gchar *rdf_pname_ns_str;
  gchar *rdf_iriref_str;
  
  static regex_t prefix_id_regex;

  static gboolean regex_compiled = FALSE;

  static const char *prefix_id_pattern = "^(@prefix)";
    
  static const size_t max_matches = 1;

  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&prefix_id_regex, prefix_id_pattern, REG_EXTENDED);
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match @prefix */
  if(ags_regexec(&prefix_id_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);
      
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
    rdf_pname_ns_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_pname_ns(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);
    
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
    rdf_iriref_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_iriref(lv2_turtle_scanner,
					    buffer, buffer_length,
					    &look_ahead);
		       
    /* create node if complete prefix id */
    if(look_ahead != rdf_pname_ns_str &&
       look_ahead != rdf_iriref_str){
      gchar *iriref;
      gchar *pname_ns;

      /* iriref */
      rdf_iriref_str = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									    buffer, buffer_length,
									    &rdf_iriref_str);

      iriref = ags_turtle_read_iriref(rdf_iriref_str,
				      &(buffer[buffer_length]));
      
      /* pname */
      rdf_pname_ns_str = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									      buffer, buffer_length,
									      &rdf_pname_ns_str);
	
      pname_ns = ags_turtle_read_pname_ns(rdf_pname_ns_str,
					  &(buffer[buffer_length]));

      /* insert prefix */
      g_hash_table_insert(AGS_LV2_CACHE_TURTLE(lv2_turtle_scanner->cache_turtle->data)->prefix_id,
			  g_strndup(iriref + 1, (gsize) (strlen(iriref) - 2)),
			  pname_ns);

      g_free(iriref);
    }
	
    *iter = look_ahead;
  }
}

void
ags_lv2_turtle_scanner_load_read_base(AgsLv2TurtleScanner *lv2_turtle_scanner,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter)
{
  regmatch_t match_arr[1];

  gchar *look_ahead;
  gchar *str;
  
  static regex_t base_regex;

  static gboolean regex_compiled = FALSE;

  static const char *base_pattern = "^(@base)";
    
  static const size_t max_matches = 1;

  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&base_regex, base_pattern, REG_EXTENDED);      
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match @base */
  if(ags_regexec(&base_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);

    str = look_ahead;
    ags_lv2_turtle_scanner_load_read_iriref(lv2_turtle_scanner,
					    buffer, buffer_length,
					    &look_ahead);

    /* create node if complete base */
    if(look_ahead != str){
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_sparql_prefix(AgsLv2TurtleScanner *lv2_turtle_scanner,
					       gchar *buffer, gsize buffer_length,
					       gchar **iter)
{
  regmatch_t match_arr[1];

  gchar *look_ahead;
  gchar *rdf_pname_ns_str, *rdf_iriref_str;
    
  static regex_t sparql_prefix_regex;

  static gboolean regex_compiled = FALSE;

  static const char *sparql_prefix_pattern = "^(\"PREFIX\")";
    
  static const size_t max_matches = 1;

  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&sparql_prefix_regex, sparql_prefix_pattern, REG_EXTENDED);
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match @prefix */
  if(ags_regexec(&sparql_prefix_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);

    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
    rdf_pname_ns_str = look_ahead;
    
    ags_lv2_turtle_scanner_load_read_pname_ns(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);
      
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
    rdf_iriref_str = look_ahead;
    
    ags_lv2_turtle_scanner_load_read_iriref(lv2_turtle_scanner,
					    buffer, buffer_length,
					    &look_ahead);
    
    /* create node if complete sparql prefix */
    if(look_ahead != rdf_pname_ns_str &&
       look_ahead != rdf_iriref_str){	
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_sparql_base(AgsLv2TurtleScanner *lv2_turtle_scanner,
					     gchar *buffer, gsize buffer_length,
					     gchar **iter)
{
  regmatch_t match_arr[1];

  gchar *look_ahead;
  gchar *str;
    
  static regex_t sparql_base_regex;

  static gboolean regex_compiled = FALSE;

  static const char *sparql_base_pattern = "^(\"BASE\")";
    
  static const size_t max_matches = 1;

  look_ahead = *iter;

  g_mutex_lock(&regex_mutex);

  if(!regex_compiled){
    regex_compiled = TRUE;
      
    ags_regcomp(&sparql_base_regex, sparql_base_pattern, REG_EXTENDED);      
  }

  g_mutex_unlock(&regex_mutex);

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* match @base */
  if(ags_regexec(&sparql_base_regex, look_ahead, max_matches, match_arr, 0) == 0){
    look_ahead += (match_arr[0].rm_eo - match_arr[0].rm_so);

    str = look_ahead;
    ags_lv2_turtle_scanner_load_read_iriref(lv2_turtle_scanner,
					    buffer, buffer_length,
					    &look_ahead);
    
    /* create node if complete sparqle base */
    if(look_ahead != str){
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_prefixed_name(AgsLv2TurtleScanner *lv2_turtle_scanner,
					       gchar *buffer, gsize buffer_length,
					       gchar **iter)
{
  gchar *look_ahead;
  gchar *str;
  
  look_ahead = *iter;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  str = look_ahead;
  
  ags_lv2_turtle_scanner_load_read_pname_ln(lv2_turtle_scanner,
					    buffer, buffer_length,
					    &look_ahead);
  
  if(look_ahead != str){
    *iter = look_ahead;
  }else{
    ags_lv2_turtle_scanner_load_read_pname_ns(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);

    if(look_ahead != str){
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_blank_node(AgsLv2TurtleScanner *lv2_turtle_scanner,
					    gchar *buffer, gsize buffer_length,
					    gchar **iter)
{
  gchar *look_ahead;
  gchar *start_offset, *end_offset;
    
  look_ahead = *iter;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);
  
  start_offset = NULL;
  end_offset = NULL;

  if(ags_turtle_match_blank_node_label(look_ahead,
				       &(buffer[buffer_length]),
				       &start_offset, &end_offset)){
    iter[0] = end_offset;
  }else{
    gchar *str;

    str = look_ahead;
    
    ags_lv2_turtle_scanner_load_read_anon(lv2_turtle_scanner,
					  buffer, buffer_length,
					  &look_ahead);
    
    if(look_ahead != str){
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_statement(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter)
{
  gchar *look_ahead;
  gchar *next;
  gchar *rdf_directive_str, *rdf_triple_str;
    
  look_ahead = *iter;    
  
  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  rdf_directive_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_directive(lv2_turtle_scanner,
					     buffer, buffer_length,
					     &look_ahead);
  
  if(look_ahead != rdf_directive_str){      
    next = strchr(look_ahead,
		  '.');

    if(next != NULL){
      *iter = next + 1;
    }else{
      *iter = &(buffer[buffer_length]);
	
      g_critical("malformed RDF Turtle");
    }
  }else{
    rdf_triple_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_triple(lv2_turtle_scanner,
					    buffer, buffer_length,
					    &look_ahead);
    
    if(look_ahead != rdf_triple_str){	
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
}

void
ags_lv2_turtle_scanner_load_read_verb(AgsLv2TurtleScanner *lv2_turtle_scanner,
				      gchar *buffer, gsize buffer_length,
				      gchar **iter)
{
  AgsLv2CacheTurtle *cache_turtle;
  AgsLv2CacheTurtle *cache_turtle_iter;
  
  gchar *look_ahead;
  gchar *str;
  
  gchar *prefix_id_doap;
  gchar *prefix_id_rdfs;
  gchar *prefix_id_lv2_core;

  cache_turtle = lv2_turtle_scanner->cache_turtle->data;
  
  prefix_id_doap = NULL;
  prefix_id_rdfs = NULL;
  prefix_id_lv2_core = NULL;

  for(cache_turtle_iter = cache_turtle; cache_turtle_iter != NULL;){
    gchar *str;

    if(prefix_id_doap == NULL &&
       (str = g_hash_table_lookup(cache_turtle_iter->prefix_id,
				  "http://usefulinc.com/ns/doap#")) != NULL){
      prefix_id_doap = str;
    }

    if(prefix_id_rdfs == NULL &&
       (str = g_hash_table_lookup(cache_turtle_iter->prefix_id,
				  "http://www.w3.org/2000/01/rdf-schema#")) != NULL){
      prefix_id_rdfs = str;
    }

    if(prefix_id_lv2_core == NULL &&
       (str = g_hash_table_lookup(cache_turtle_iter->prefix_id,
				  "http://lv2plug.in/ns/lv2core#")) != NULL){
      prefix_id_lv2_core = str;
    }

    cache_turtle_iter = cache_turtle_iter->parent;
  }

  /*  */
  look_ahead = *iter;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* predicate */
  str = look_ahead;
  
  ags_lv2_turtle_scanner_load_read_predicate(lv2_turtle_scanner,
					     buffer, buffer_length,
					     &look_ahead);
	  
  if(look_ahead != str){
    /* seealso */
    if(!g_ascii_strncasecmp(str,
			    "<http://www.w3.org/2000/01/rdf-schema#seealso>",
			    47)){
      lv2_turtle_scanner->status_flags |= AGS_LV2_TURTLE_SCANNER_SEEALSO;
    }else{
      gchar *prefix_id;
      
      prefix_id = g_strdup_printf("%sseealso",
				  prefix_id_rdfs);

      if(!g_ascii_strncasecmp(str,
			      prefix_id,
			      (gsize) strlen(prefix_id))){
	lv2_turtle_scanner->status_flags |= AGS_LV2_TURTLE_SCANNER_SEEALSO;
      }
      
      g_free(prefix_id);	
    }
    
    /* binary */
    if(!g_ascii_strncasecmp(str,
			    "<http://lv2plug.in/ns/lv2core#binary>",
			    38)){
      lv2_turtle_scanner->status_flags |= AGS_LV2_TURTLE_SCANNER_BINARY;
    }else{
      gchar *prefix_id;
      
      prefix_id = g_strdup_printf("%sbinary",
				  prefix_id_lv2_core);

      if(!g_ascii_strncasecmp(str,
			      prefix_id,
			      (gsize) strlen(prefix_id))){
	lv2_turtle_scanner->status_flags |= AGS_LV2_TURTLE_SCANNER_BINARY;
      }
      
      g_free(prefix_id);	
    }
			    
    /* name */
    if(!g_ascii_strncasecmp(str,
			    "<http://usefulinc.com/ns/doap#name>",
			    36)){
    }else{
      gchar *prefix_id;
      
      prefix_id = g_strdup_printf("%sname",
				  prefix_id_doap);

      if(!g_ascii_strncasecmp(str,
			      prefix_id,
			      (gsize) strlen(prefix_id))){
	lv2_turtle_scanner->status_flags |= AGS_LV2_TURTLE_SCANNER_NAME;
      }
      
      g_free(prefix_id);	
    }
    
    *iter = look_ahead;
  }else{
    /* skip blanks and comments */
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
      
    if(*look_ahead == 'a'){
      lv2_turtle_scanner->status_flags |= AGS_LV2_TURTLE_SCANNER_A;
      
      look_ahead += 1;
      
      *iter = look_ahead;
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_predicate(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter)
{
  gchar *look_ahead;
  gchar *iri_str;
  
  look_ahead = *iter;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* iri - IRIREF */
  iri_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_iri(lv2_turtle_scanner,
				       buffer, buffer_length,
				       &look_ahead);
    
  if(look_ahead != iri_str){
    *iter = look_ahead;      
  }
}

void
ags_lv2_turtle_scanner_load_read_object(AgsLv2TurtleScanner *lv2_turtle_scanner,
					gchar *buffer, gsize buffer_length,
					gchar **iter)
{    
  gchar *look_ahead;
  gchar *str;
  gchar *iri_str;
  gchar *blank_str;
  gchar *collection_str;
  gchar *blank_node_property_list_str;
  gchar *literal_str;
  
  look_ahead = *iter;

  str = NULL;
  iri_str = NULL;
  blank_str = NULL;
  collection_str = NULL;
  blank_node_property_list_str = NULL;
  literal_str = NULL;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* iri - IRIREF */
  iri_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_iri(lv2_turtle_scanner,
				       buffer, buffer_length,
				       &look_ahead);

  if(look_ahead != iri_str){
    goto ags_lv2_turtle_scanner_load_read_object_CREATE_STR;
  }

  /* read blank node */
  blank_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_blank_node(lv2_turtle_scanner,
					       buffer, buffer_length,
					       &look_ahead);

  if(look_ahead != blank_str){
    goto ags_lv2_turtle_scanner_load_read_object_CREATE_STR;
  }

  /* collection */
  if(*look_ahead == '('){
    collection_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_collection(lv2_turtle_scanner,
						buffer, buffer_length,
						&look_ahead);
    
    goto ags_lv2_turtle_scanner_load_read_object_CREATE_STR;
  }

  /* blank node property listimplemented ags_lv2_turtle_scanner_load_read_object() */
  if(*look_ahead == '['){
    blank_node_property_list_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_blank_node_property_list(lv2_turtle_scanner,
							      buffer, buffer_length,
							      &look_ahead);
    
    goto ags_lv2_turtle_scanner_load_read_object_CREATE_STR;
  }

  /* literal */
#ifdef AGS_DEBUG
  g_message("*** PASS ***");
#endif
    
  literal_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_literal(lv2_turtle_scanner,
					   buffer, buffer_length,
					   &look_ahead);
    
  if(look_ahead != literal_str){
    goto ags_lv2_turtle_scanner_load_read_object_CREATE_STR;
  }

  /* create str */
ags_lv2_turtle_scanner_load_read_object_CREATE_STR:

  if(iri_str != NULL){
    *iter = look_ahead;
  }else if(blank_str != NULL){
    *iter = look_ahead;
  }else if(collection_str != NULL){
    *iter = look_ahead;
  }else if(blank_node_property_list_str != NULL){
    *iter = look_ahead;
  }else if(literal_str != NULL){
    *iter = look_ahead;
  }
}

void
ags_lv2_turtle_scanner_load_read_directive(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   gchar *buffer, gsize buffer_length,
					   gchar **iter)
{    
  gchar *look_ahead, *current;
  gchar *str;
  gchar *rdf_prefix_id_str, *rdf_base_str, *rdf_sparql_prefix_str, *rdf_sparql_base_str;

  str = NULL;

  rdf_prefix_id_str = NULL;
  rdf_base_str = NULL;
  rdf_sparql_prefix_str = NULL;
  rdf_sparql_base_str = NULL;
  
  look_ahead = *iter;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  rdf_prefix_id_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_prefix_id(lv2_turtle_scanner,
					     buffer, buffer_length,
					     &look_ahead);

  if(look_ahead != rdf_prefix_id_str){
    goto ags_lv2_turtle_scanner_load_read_directive_CREATE_STR;
  }
    
  rdf_base_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_base(lv2_turtle_scanner,
					buffer, buffer_length,
					&look_ahead);

  if(look_ahead != rdf_base_str){
    goto ags_lv2_turtle_scanner_load_read_directive_CREATE_STR;
  }
    
  rdf_sparql_prefix_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_sparql_prefix(lv2_turtle_scanner,
						 buffer, buffer_length,
						 &look_ahead);
  
  if(look_ahead != rdf_sparql_prefix_str){
    goto ags_lv2_turtle_scanner_load_read_directive_CREATE_STR;
  }
    
  rdf_sparql_base_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_sparql_base(lv2_turtle_scanner,
					       buffer, buffer_length,
					       &look_ahead);

  if(look_ahead != rdf_sparql_base_str){
    goto ags_lv2_turtle_scanner_load_read_directive_CREATE_STR;
  }
    
  /* create str */
ags_lv2_turtle_scanner_load_read_directive_CREATE_STR:
    
  if(look_ahead != rdf_prefix_id_str){
    *iter = look_ahead;
  }else if(look_ahead != rdf_base_str){
    *iter = look_ahead;
  }else if(look_ahead != rdf_sparql_prefix_str){
    *iter = look_ahead;
  }else if(look_ahead != rdf_sparql_base_str){
    *iter = look_ahead;
  }
}

void
ags_lv2_turtle_scanner_load_read_triple(AgsLv2TurtleScanner *lv2_turtle_scanner,
					gchar *buffer, gsize buffer_length,
					gchar **iter)
{
  gchar *look_ahead, *current;
  gchar *str;
  gchar *subject_str,  *predicate_object_list_str_0, *blank_node_object_list_str,  *predicate_object_list_str_1;
  
  str = NULL;
  look_ahead = *iter;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);
	
  /* subject */
  subject_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_subject(lv2_turtle_scanner,
					   buffer, buffer_length,
					   &look_ahead);

  /* create triple str */
  if(look_ahead != subject_str){
    /* skip blanks and comments */
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);

    /* first predicate object list */
    predicate_object_list_str_0 = look_ahead;

    ags_lv2_turtle_scanner_load_read_predicate_object_list(lv2_turtle_scanner,
							   buffer, buffer_length,
							   &look_ahead);

    if(look_ahead != predicate_object_list_str_0){
      /* second predicate object list */
      ags_lv2_turtle_scanner_load_read_predicate_object_list(lv2_turtle_scanner,
							     buffer, buffer_length,
							     &look_ahead);
      
      *iter = look_ahead;
    }else{
      /* alternate first blank node object list */
      blank_node_object_list_str = look_ahead;

      ags_lv2_turtle_scanner_load_read_blank_node_property_list(lv2_turtle_scanner,
								buffer, buffer_length,
								&look_ahead);
      
      if(look_ahead != blank_node_object_list_str){
	/* predicate object list */
	ags_lv2_turtle_scanner_load_read_predicate_object_list(lv2_turtle_scanner,
							       buffer, buffer_length,
							       &look_ahead);
	  
	*iter = look_ahead;
      }
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_subject(AgsLv2TurtleScanner *lv2_turtle_scanner,
					 gchar *buffer, gsize buffer_length,
					 gchar **iter)
{
  gchar *look_ahead;
  gchar *str;
  gchar *iri_str;
  gchar *blank_str;
  gchar *collection_str;
  gchar *iriref;
  gchar *pname;
    
  str = NULL;

  iri_str =  NULL;
  blank_str = NULL;
  collection_str = NULL;
    
  look_ahead = *iter;

  g_free(lv2_turtle_scanner->current_subject_iriref);
  lv2_turtle_scanner->current_subject_iriref = NULL;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* iri - IRIREF */
  iri_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_iri(lv2_turtle_scanner,
				       buffer, buffer_length,
				       &look_ahead);

  if(iri_str != look_ahead){
    iriref = ags_turtle_read_iriref(iri_str,
				    &(buffer[buffer_length]));

    if(iriref != NULL){
      lv2_turtle_scanner->current_subject_iriref = iriref;
    }else{
      pname = ags_turtle_read_pname_ln(iri_str,
				       &(buffer[buffer_length]));
    
      if(pname == NULL){
	pname = ags_turtle_read_pname_ns(iri_str,
					 &(buffer[buffer_length]));	
      }

      if(pname != NULL){
	AgsLv2CacheTurtle *cache_turtle;
	AgsLv2CacheTurtle *cache_turtle_iter;
  
	cache_turtle = lv2_turtle_scanner->cache_turtle->data;

	for(cache_turtle_iter = cache_turtle; cache_turtle_iter != NULL;){
	  GList *start_list, *list;

	  gchar *str;
	  gchar *prefix, *suffix;
	
	  prefix = NULL;
	  suffix = strchr(pname, ':');

	  if(suffix != NULL){
	    suffix += 1;
	    
	    prefix = g_strndup(pname,
			       suffix - pname);

	    if(suffix != '\0'){
	      suffix = g_strdup(suffix);
	    }else{
	      suffix = NULL;
	    }
	  }

	  list =
	    start_list = g_hash_table_get_keys(cache_turtle_iter->prefix_id);

	  str = NULL;

	  while(list != NULL){
	    gchar *value;		  

	    value = g_hash_table_lookup(cache_turtle_iter->prefix_id,
					list->data);

	    if(!g_ascii_strcasecmp(prefix,
				   value)){
	      str = list->data;

	      break;
	    }
		  
	    list = list->next;
	  }

	  g_list_free(start_list);

	  if(str != NULL &&
	     suffix != NULL){		  
	    iriref = g_strdup_printf("%s%s",
				     str,
				     suffix);

	    lv2_turtle_scanner->current_subject_iriref = iriref;
	  }else if(str != NULL){
	    iriref = g_strdup(str);

	    lv2_turtle_scanner->current_subject_iriref = iriref;
	  }

	  g_free(prefix);
	  g_free(suffix);

	  if(iriref != NULL){
	    break;
	  }
	  
	  cache_turtle_iter = cache_turtle_iter->parent;
	}
	
      }
    }

    goto ags_lv2_turtle_scanner_load_read_subject_CREATE_STR;
  }

  /* read blank str */
  blank_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_blank_node(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);

  if(blank_str != look_ahead){
    goto ags_lv2_turtle_scanner_load_read_subject_CREATE_STR;
  }
      
  /* collection */
  if(*look_ahead == '('){
    collection_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_collection(lv2_turtle_scanner,
						buffer, buffer_length,
						&look_ahead);
    
    goto ags_lv2_turtle_scanner_load_read_subject_CREATE_STR;
  }

  /* create str */
ags_lv2_turtle_scanner_load_read_subject_CREATE_STR:
  *iter = look_ahead;
}

void
ags_lv2_turtle_scanner_load_read_object_list(AgsLv2TurtleScanner *lv2_turtle_scanner,
					     gchar *buffer, gsize buffer_length,
					     gchar **iter)
{
  AgsLv2CacheTurtle *manifest_cache_turtle;
  AgsLv2CacheTurtle *cache_turtle;
  AgsLv2CacheTurtle *cache_turtle_iter;

  gchar *look_ahead;
  gchar *start_ptr, *end_ptr;
  gchar *str;
  gchar *object_str;    
    
  gchar *prefix_id_lv2_core;

  guint plugin_count;
  gboolean is_plugin;
  gboolean is_instrument;

  manifest_cache_turtle = NULL;
  cache_turtle = lv2_turtle_scanner->cache_turtle->data;

  prefix_id_lv2_core = NULL;

  for(cache_turtle_iter = cache_turtle; cache_turtle_iter != NULL;){
    gchar *str;

    if(prefix_id_lv2_core == NULL &&
       (str = g_hash_table_lookup(cache_turtle_iter->prefix_id,
				  "http://lv2plug.in/ns/lv2core#")) != NULL){
      prefix_id_lv2_core = str;
    }

    if(cache_turtle_iter->parent == NULL){
      manifest_cache_turtle = cache_turtle_iter;
    }
    
    cache_turtle_iter = cache_turtle_iter->parent;
  }

  str = NULL;
  look_ahead = *iter;

  is_plugin = FALSE;

  is_instrument = FALSE;

  /* skip blanks and comments */
  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  /* read first object */
  object_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_object(lv2_turtle_scanner,
					  buffer, buffer_length,
					  &look_ahead);
  
  if(look_ahead != object_str){
    /* a */
    if((AGS_LV2_TURTLE_SCANNER_A & (lv2_turtle_scanner->status_flags)) != 0){
      /* plugin */
      if(!g_ascii_strncasecmp(object_str,
			      "<http://lv2plug.in/ns/lv2core#plugin>",
			      38)){
	is_plugin = TRUE;
      }else{
	gchar *prefix_id;
      
	prefix_id = g_strdup_printf("%s%s",
				    prefix_id_lv2_core,
				    "plugin");
	
	if(!g_ascii_strncasecmp(object_str,
				prefix_id,
				(gsize) strlen(prefix_id))){
	  is_plugin = TRUE;
	}
      
	g_free(prefix_id);	
      }

      /* instrument plugin */
      if(!g_ascii_strncasecmp(object_str,
			      "<http://lv2plug.in/ns/lv2core#instrumentplugin>",
			      48)){
	is_instrument = TRUE;
      }else{
	gchar *prefix_id;
      
	prefix_id = g_strdup_printf("%sinstrumentplugin",
				    prefix_id_lv2_core);

	if(!g_ascii_strncasecmp(object_str,
				prefix_id,
				(gsize) strlen(prefix_id))){
	  is_instrument = TRUE;
	}
      
	g_free(prefix_id);	
      }
    }

    /* see also */
    if((AGS_LV2_TURTLE_SCANNER_SEEALSO & (lv2_turtle_scanner->status_flags)) != 0){
      gchar *iriref;
      
      iriref = ags_turtle_read_iriref(object_str,
				      &(buffer[buffer_length]));

      if(cache_turtle->see_also == NULL ||
	 !g_strv_contains(cache_turtle->see_also, iriref)){
	guint length;
	gboolean is_available;

	is_available = FALSE;
	
	if(cache_turtle->see_also == NULL){
	  length = 0;
	  
	  cache_turtle->see_also = (gchar **) g_malloc(2 * sizeof(gchar *));
	}else{
	  if(!g_strv_contains(cache_turtle->see_also,
			      iriref)){
	    length = g_strv_length(cache_turtle->see_also);
	    
	    cache_turtle->see_also = (gchar **) g_realloc(cache_turtle->see_also,
							  (length + 2) * sizeof(gchar *));
	  }else{
	    is_available = TRUE;
	  }
	}

	if(!is_available){
	  cache_turtle->see_also[length] = iriref;
	  cache_turtle->see_also[length + 1] = NULL;
	}
      }
    }

    /* binary */
    if((AGS_LV2_TURTLE_SCANNER_BINARY & (lv2_turtle_scanner->status_flags)) != 0){
      gchar *iriref;
      gchar *path;
      
      guint length;
      
      iriref = ags_turtle_read_iriref(object_str,
				      &(buffer[buffer_length]));

      path = g_path_get_dirname(cache_turtle->turtle_filename);

      if(lv2_turtle_scanner->current_subject_iriref != NULL){
	g_hash_table_insert(manifest_cache_turtle->plugin_filename,
			    g_strdup(lv2_turtle_scanner->current_subject_iriref), g_strdup_printf("%s%c%.*s",
												  path,
												  G_DIR_SEPARATOR,
												  (gint) (strlen(iriref) - 2),
												  iriref + 1));
      }
      
      g_free(path);
      g_free(iriref);
    }

    /* effect */
    if((AGS_LV2_TURTLE_SCANNER_NAME & (lv2_turtle_scanner->status_flags)) != 0){
      gchar *name;

      guint length;
      
      name = ags_turtle_read_string(object_str,
				    &(buffer[buffer_length]));

      if(!g_ascii_strncasecmp(name, "\"\"\"", 3)){
	gchar *tmp;

	tmp = g_strndup(name + 3,
			(gsize) (strlen(name) - 6));

	g_free(name);
	
	name = tmp;
      }else if(!g_ascii_strncasecmp(name, "'''", 3)){
	gchar *tmp;

	tmp = g_strndup(name + 3,
			(gsize) (strlen(name) - 6));

	g_free(name);
	
	name = tmp;
      }else if(name[0] == '"'){
	gchar *tmp;

	tmp = g_strndup(name + 1,
			(gsize) (strlen(name) - 2));

	g_free(name);
	
	name = tmp;
      }else if(name[0] == '\''){
	gchar *tmp;

	tmp = g_strndup(name + 1,
			(gsize) (strlen(name) - 2));

	g_free(name);
	
	name = tmp;
      }
      
      if(lv2_turtle_scanner->current_subject_iriref != NULL){
	g_hash_table_insert(manifest_cache_turtle->plugin_effect,
			    g_strdup(lv2_turtle_scanner->current_subject_iriref), name);
      }
    }
    
    /* iterate */
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
	
    while(*look_ahead == ','){
      look_ahead++;
	
      /* skip blanks and comments */
      look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									buffer, buffer_length,
									&look_ahead);
      
      object_str = look_ahead;

      ags_lv2_turtle_scanner_load_read_object(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);

      if(look_ahead != object_str){
	/* a */
	if((AGS_LV2_TURTLE_SCANNER_A & (lv2_turtle_scanner->status_flags)) != 0){
	  /* plugin */
	  if(!g_ascii_strncasecmp(object_str,
				  "<http://lv2plug.in/ns/lv2core#plugin>",
				  38)){
	    is_plugin = TRUE;
	  }else{
	    gchar *prefix_id;
	    
	    prefix_id = g_strdup_printf("%s%s",
					prefix_id_lv2_core,
					"plugin");

	    if(!g_ascii_strncasecmp(object_str,
				    prefix_id,
				    (gsize) strlen(prefix_id))){
	      is_plugin = TRUE;
	    }
      
	    g_free(prefix_id);	
	  }

	  /* instrument plugin */
	  if(!g_ascii_strncasecmp(object_str,
				  "<http://lv2plug.in/ns/lv2core#instrumentplugin>",
				  48)){
	    is_instrument = TRUE;
	  }else{
	    gchar *prefix_id;
      
	    prefix_id = g_strdup_printf("%sinstrumentplugin",
					prefix_id_lv2_core);

	    if(!g_ascii_strncasecmp(object_str,
				    prefix_id,
				    (gsize) strlen(prefix_id))){
	      is_instrument = TRUE;
	    }
      
	    g_free(prefix_id);	
	  }
	}
	
	look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									  buffer, buffer_length,
									  &look_ahead);
      }
    }
      
    *iter = look_ahead;
  }

  if(is_plugin){
    if(lv2_turtle_scanner->current_subject_iriref != NULL){
      g_hash_table_insert(manifest_cache_turtle->is_plugin,
			  g_strdup(lv2_turtle_scanner->current_subject_iriref), GINT_TO_POINTER(TRUE));
    }
  }
      
  if(is_instrument){
    if(lv2_turtle_scanner->current_subject_iriref != NULL){
      g_hash_table_insert(manifest_cache_turtle->is_instrument,
			  g_strdup(lv2_turtle_scanner->current_subject_iriref), GINT_TO_POINTER(TRUE));
    }
  }

  lv2_turtle_scanner->status_flags = 0;
}

void
ags_lv2_turtle_scanner_load_read_collection(AgsLv2TurtleScanner *lv2_turtle_scanner,
					    gchar *buffer, gsize buffer_length,
					    gchar **iter)
{    
  gchar *look_ahead;
  gchar *start_ptr, *end_ptr;
  gchar *str;
  gchar *object_str;
    
  str = NULL;
  look_ahead = *iter;

  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);
    
  if(*look_ahead == '\0'){
    return;
  }

  if(*look_ahead == '('){
    start_ptr = look_ahead;
    look_ahead++;
      
    /* read objects */
    do{
      look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									buffer, buffer_length,
									&look_ahead);

      object_str = look_ahead;

      ags_lv2_turtle_scanner_load_read_object(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);
    }while(object_str != look_ahead);

    end_ptr = strchr(look_ahead,
		     ')');

    if(end_ptr != NULL){
      *iter = end_ptr + 1;
    }else{
      *iter = &(buffer[buffer_length]);
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_blank_node_property_list(AgsLv2TurtleScanner *lv2_turtle_scanner,
							  gchar *buffer, gsize buffer_length,
							  gchar **iter)
{
  gchar *look_ahead;
  gchar *start_ptr, *end_ptr;
  gchar *str;
  gchar *predicate_object_list_str;
    
  str = NULL;
  look_ahead = *iter;

  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  if(*look_ahead == '['){
    start_ptr = look_ahead;
    look_ahead++;
      
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
    predicate_object_list_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_predicate_object_list(lv2_turtle_scanner,
							   buffer, buffer_length,
							   &look_ahead);

    if(look_ahead != predicate_object_list_str){
      end_ptr = strchr(look_ahead,
		       ']');

      if(end_ptr != NULL){
	*iter = end_ptr + 1;
      }else{
	*iter = &(buffer[buffer_length]);
      }
    }
  }
}

void
ags_lv2_turtle_scanner_load_read_predicate_object_list(AgsLv2TurtleScanner *lv2_turtle_scanner,
						       gchar *buffer, gsize buffer_length,
						       gchar **iter)
{
  gchar *look_ahead;
  gchar *str;
  gchar *current_verb_str, *current_object_list_str;
  
  str = NULL;
  look_ahead = *iter;

  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								    buffer, buffer_length,
								    &look_ahead);

  current_verb_str = look_ahead;

  ags_lv2_turtle_scanner_load_read_verb(lv2_turtle_scanner,
					buffer, buffer_length,
					&look_ahead);

  if(look_ahead != current_verb_str){
    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								      buffer, buffer_length,
								      &look_ahead);
    
    current_object_list_str = look_ahead;

    ags_lv2_turtle_scanner_load_read_object_list(lv2_turtle_scanner,
						 buffer, buffer_length,
						 &look_ahead);
    
    if(look_ahead != current_object_list_str){
      /* iterate */	
      look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									buffer, buffer_length,
									&look_ahead);
	
      while(*look_ahead == ';'){
	look_ahead++;
	look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									  buffer, buffer_length,
									  &look_ahead);

	current_verb_str = look_ahead;

	ags_lv2_turtle_scanner_load_read_verb(lv2_turtle_scanner,
					      buffer, buffer_length,
					      &look_ahead);

	if(look_ahead != current_verb_str){
	  look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
									    buffer, buffer_length,
									    &look_ahead);
	  
	  current_object_list_str = look_ahead;

	  ags_lv2_turtle_scanner_load_read_object_list(lv2_turtle_scanner,
						       buffer, buffer_length,
						       &look_ahead);
      
	  if(look_ahead != current_object_list_str){
	    look_ahead = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
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
}

void
ags_lv2_turtle_scanner_quick_scan_see_also(AgsLv2TurtleScanner *lv2_turtle_scanner,
					   AgsLv2CacheTurtle *parent,
					   gchar *turtle_filename)
{
  AgsLv2CacheTurtle *lv2_cache_turtle;
  
  FILE *file;

  struct stat *sb;

  gchar **see_also;
  
  gchar *buffer, *iter;
  
  size_t n_read;
  gboolean is_available;
  
  GRecMutex *lv2_turtle_scanner_mutex;

  if(!AGS_IS_LV2_TURTLE_SCANNER(lv2_turtle_scanner) ||
     turtle_filename == NULL){
    return;
  }

  lv2_turtle_scanner_mutex = AGS_LV2_TURTLE_SCANNER_GET_OBJ_MUTEX(lv2_turtle_scanner);

  /* check if turtle yet available */
  g_rec_mutex_lock(lv2_turtle_scanner_mutex);

  is_available = (ags_lv2_cache_turtle_find(lv2_turtle_scanner->cache_turtle, turtle_filename) != NULL) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(lv2_turtle_scanner_mutex);
  
  if(is_available){
    return;
  }
  
  /* entry point - open file and read it */
  sb = (struct stat *) g_malloc(sizeof(struct stat));
  stat(turtle_filename,
       sb);
  file = fopen(turtle_filename,
	       "r");
  
  if(file == NULL ||
     sb->st_size <= 0){
    g_free(sb);
    
    return;
  }

  buffer = (gchar *) g_malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    g_free(sb);

    return;
  }
  
  n_read = fread(buffer, sizeof(gchar), sb->st_size, file);

  if(n_read != sb->st_size){
    g_critical("number of read bytes doesn't match buffer size");
  }
  
  buffer[sb->st_size] = '\0';
  fclose(file);

  lv2_cache_turtle = ags_lv2_cache_turtle_alloc(parent,
						g_strdup(turtle_filename));
  
  lv2_turtle_scanner->cache_turtle = g_list_prepend(lv2_turtle_scanner->cache_turtle,
						    lv2_cache_turtle);

  g_message("scanning %s", turtle_filename);
  
  iter = buffer;

  do{
    gchar *str;
    
    /* skip blanks and comments */
    iter = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								buffer, sb->st_size,
								&iter);

    if(iter >= &(buffer[sb->st_size])){
      break;
    }

    str = iter;
    ags_lv2_turtle_scanner_load_read_statement(lv2_turtle_scanner,
					       buffer, sb->st_size,
					       &iter);
    
    if(iter == str){
      iter++;
    }
  }while(iter < &(buffer[sb->st_size]));
  
  g_free(sb);
  g_free(buffer);

  /* see also */
  if(lv2_cache_turtle->see_also != NULL){
    for(see_also = lv2_cache_turtle->see_also; see_also[0] != NULL; see_also++){
      gchar *filename;
      gchar *path;

      path = g_path_get_dirname(turtle_filename);
      filename = g_strdup_printf("%s%c%.*s",
				 path,
				 G_DIR_SEPARATOR,
				 (int) (strlen(see_also[0]) - 2),
				 see_also[0] + 1);
      
      if(g_str_has_suffix(filename, ".ttl")){
	ags_lv2_turtle_scanner_quick_scan_see_also(lv2_turtle_scanner,
						   lv2_cache_turtle,
						   filename);
      }
      
      g_free(path);
      g_free(filename);
    }
  }
}

/**
 * ags_lv2_turtle_scanner_quick_scan:
 * @lv2_turtle_scanner: the #AgsLv2TurtleScanner
 * @manifest_filename: the manifest filename
 * 
 * Quick scan to detect available plugins. 
 * 
 * Since: 3.2.7
 */
void
ags_lv2_turtle_scanner_quick_scan(AgsLv2TurtleScanner *lv2_turtle_scanner,
				  gchar *manifest_filename)
{
  AgsLv2CacheTurtle *lv2_cache_turtle;
  
  GFile *manifest_file;
	
  FILE *file;

  struct stat *sb;

  gchar **see_also;
  
  gchar *buffer, *iter;
  gchar *manifest_path;
  
  size_t n_read;
  gboolean is_available;
  
  GRecMutex *lv2_turtle_scanner_mutex;
  
  if(!AGS_IS_LV2_TURTLE_SCANNER(lv2_turtle_scanner) ||
     manifest_filename == NULL){
    return;
  }

  lv2_turtle_scanner_mutex = AGS_LV2_TURTLE_SCANNER_GET_OBJ_MUTEX(lv2_turtle_scanner);

  /* check if turtle yet available */
  g_rec_mutex_lock(lv2_turtle_scanner_mutex);

  is_available = (ags_lv2_cache_turtle_find(lv2_turtle_scanner->cache_turtle, manifest_filename) != NULL) ? TRUE: FALSE;
  
  g_rec_mutex_unlock(lv2_turtle_scanner_mutex);
  
  if(is_available){
    return;
  }
  
  /* entry point - open file and read it */
  sb = (struct stat *) g_malloc(sizeof(struct stat));
  stat(manifest_filename,
       sb);
  file = fopen(manifest_filename,
	       "r");
  
  if(file == NULL ||
     sb->st_size <= 0){
    g_free(sb);
    
    return;
  }

  buffer = (gchar *) g_malloc((sb->st_size + 1) * sizeof(gchar));

  if(buffer == NULL){
    g_free(sb);

    return;
  }
  
  n_read = fread(buffer, sizeof(gchar), sb->st_size, file);

  if(n_read != sb->st_size){
    g_critical("number of read bytes doesn't match buffer size");
  }
  
  buffer[sb->st_size] = '\0';
  fclose(file);

  lv2_cache_turtle = ags_lv2_cache_turtle_alloc(NULL,
						g_strdup(manifest_filename));

  lv2_turtle_scanner->cache_turtle = g_list_prepend(lv2_turtle_scanner->cache_turtle,
						    lv2_cache_turtle);
  
  iter = buffer;

  do{
    gchar *str;
    
    /* skip blanks and comments */
    iter = ags_lv2_turtle_scanner_load_skip_comments_and_blanks(lv2_turtle_scanner,
								buffer, sb->st_size,
								&iter);

    if(iter >= &(buffer[sb->st_size])){
      break;
    }

    str = iter;
    ags_lv2_turtle_scanner_load_read_statement(lv2_turtle_scanner,
					       buffer, sb->st_size,
					       &iter);
    
    if(iter == str){
      iter++;
    }
  }while(iter < &(buffer[sb->st_size]));
  
  g_free(sb);
  g_free(buffer);

  /* see also */
  manifest_file = g_file_new_for_path(manifest_filename);

  manifest_path = g_file_get_path(manifest_file);
  
  if(lv2_cache_turtle->see_also != NULL){
    for(see_also = lv2_cache_turtle->see_also; see_also[0] != NULL; see_also++){
      gchar *filename;
      gchar *path;

      path = g_path_get_dirname(manifest_filename);
      filename = g_strdup_printf("%s%c%.*s",
				 path,
				 G_DIR_SEPARATOR,
				 (int) (strlen(see_also[0]) - 2),
				 see_also[0] + 1);

      if(g_str_has_suffix(filename, ".ttl")){
	GFile *current_file;

	gchar *current_path;
	
	current_file = g_file_new_for_path(filename);

	current_path = g_file_get_path(current_file);

	if((!g_strcmp0(manifest_path,
		       current_path)) == FALSE){
	  ags_lv2_turtle_scanner_quick_scan_see_also(lv2_turtle_scanner,
						     lv2_cache_turtle,
						     filename);
	}

	g_free(current_path);

	if(current_file != NULL){
	  g_object_unref(current_file);
	}
      }
      
      g_free(path);
      g_free(filename);
    }
  }

  g_free(manifest_path);

  if(manifest_file != NULL){
    g_object_unref(manifest_file);
  }
}

/**
 * ags_lv2_turtle_scanner_new:
 *
 * Creates an #AgsLv2TurtleScanner
 *
 * Returns: a new #AgsLv2TurtleScanner
 *
 * Since: 3.2.7
 */
AgsLv2TurtleScanner*
ags_lv2_turtle_scanner_new()
{
  AgsLv2TurtleScanner *lv2_turtle_scanner;

  lv2_turtle_scanner = (AgsLv2TurtleScanner *) g_object_new(AGS_TYPE_LV2_TURTLE_SCANNER,
							    NULL);

  return(lv2_turtle_scanner);
}
