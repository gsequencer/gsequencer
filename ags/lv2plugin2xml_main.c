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

#include <glib.h>
#include <glib-object.h>

#include <ags/lib/ags_turtle.h>

#include <ags/audio/midi/ags_midi_parser.h>

#include <stdio.h>

#include <libxml/tree.h>

void ags_lv2_turtle_to_xml(gchar *turtle_path, gchar *output_path);
void ags_xml_to_fastxml(xmlDoc *lv2_doc, gchar *filename);

void
ags_lv2_turtle_to_xml(gchar *turtle_path, gchar *output_path)
{
  AgsTurtle *turtle, *manifest;

  xmlDoc *doc;
  
  FILE *out;

  GList *ttl_list;
  
  xmlChar *buffer;
  int size;

  gchar *manifest_filename, *plugin_filename, *fastxml_filename;
  gchar *str;
  gchar *tmp;

  /* read manifest */
  manifest = g_strdup_printf("%s/manifest.ttl\0",
			     plugin_path);
  
  turtle = ags_turtle_new(manifest_filename);
  manifest = ags_turtle_load(manifest_filename,
			     NULL);
  
  /* persist XML */
  xmlDocDumpFormatMemoryEnc(manifest->doc, &buffer, &size, "UTF-8\0", TRUE);
  
  out = fopen(g_strdup_printf("%s/manifest.ttl.xml\0", output_path), "w+\0");
  
  fwrite(buffer, size, sizeof(xmlChar), out);
  fflush(out);
      
  /* read turtle from manifest */
  ttl_list = ags_turtle_find_xpath(manifest,
				   "//rdf-triple//rdf-verb[//rdf-pname-ln[substring(text(), string-length(text()) - string-length(':binary') + 1) = ':binary']]/following-sibling::rdf-object-list//rdf-iriref[substring(text(), string-length(text()) - string-length('.ttl>') + 1) = '.ttl>']\0");
  
  /* load */
  while(ttl_list != NULL){
    /* read filename */
    plugin_filename = xmlNodeGetContent((xmlNode *) ttl_list->data);
    
    if(turtle_filename == NULL){
      ttl_list = ttl_list->next;

      continue;
    }
    
    turtle_filename = g_strndup(&(turtle_filename[1]),
				strlen(turtle_filename) - 2);
    
    if(!g_ascii_strncasecmp(turtle_filename,
			    "http://\0",
			    7)){
      ttl_list = ttl_list->next;

      continue;
    }

    /* load turtle doc */
    g_message("%s", plugin_filename);

    turtle = ags_turtle_new(g_strdup_printf("%s/%s\0",
					    turtle_path,
					    plugin_filename));
    ags_turtle_load(turtle,
		    NULL);

    /* persist XML */
    xmlDocDumpFormatMemoryEnc(turtle->doc, &buffer, &size, "UTF-8\0", TRUE);

    out = fopen(g_strdup_printf("%s/%s.xml\0", turtle_path, plugin_filename), "w+\0");

    fwrite(buffer, size, sizeof(xmlChar), out);
    fflush(out);

    /* generate fast XML */
    tmp = g_strndup(plugin_filename,
		    rindex(plugin_filename, '.') - plugin_filename);
    fastxml_filename = g_strdup_printf("%s/%s.xml\0",
				       output_path,
				       tmp);
    ags_xml_to_fastxml(turtle->doc, fastxml_filename);
    
    /* iterate */	
    ttl_list = ttl_list->next;
  }
}

void
ags_xml_to_fastxml(xmlDoc *lv2_doc, gchar *filename)
{
  xmlNode *root_node;
  xmlNode *lv2_list_node, *lv2_node;
  xmlNode *port_list_node, *port_node;
  xmlNode *child;
  
  xmlXPathContext *xpath_context;
  xmlXPathObject *xpath_object;
  xmlNode **node;

  xmlChar *xpath;

  xpath = "//rdf-triple//rdf-verb//rdf-pname-ln[substring(text(), string-length(text()) - string-length('doap:name') + 1) = 'doap:name']/ancestor::*[self::rdf-verb][1]/following-sibling::rdf-object-list[1]//rdf-string[text()]\0";

  xpath_context = xmlXPathNewContext(lv2_doc);
  xpath_object = xmlXPathEval((xmlChar *) xpath,
			      xpath_context);

  if(xpath_object->nodesetval != NULL){
    node = xpath_object->nodesetval->nodeTab;

    for(i = 0; i < xpath_object->nodesetval->nodeNr; i++){
      if(node[i]->type == XML_ELEMENT_NODE){
	node[i];
      }
    }
  }
}

int
main(int argc, char **argv)
{
  GDir *dir;

  gchar *path, *plugin_path, *output_path;

  static const gchar *lv2_default_path = "/usr/lib/lv2\0";
  static const gchar *xml_base_path = "/usr/share/gsequencer/lv2";

  /* turtle to xml - read default directory */
  error = NULL;
  dir = g_dir_open(lv2_default_path,
		   0,
		   &error);

  if(error != NULL){
    g_warning(error->message);
  }

  while((path = g_dir_read_name(dir)) != NULL){
    if(!g_ascii_strncasecmp(path,
			    "..\0",
			    3) ||
       !g_ascii_strncasecmp(path,
			    ".\0",
			    2)){
      continue;
    }

    plugin_path = g_strdup_printf("%s/%s\0",
				  ags_lv2_default_path,
				  path);

    if(g_file_test(plugin_path,
		   G_FILE_TEST_IS_DIR)){
      output_path = g_strdup_printf("%s/%s\0",
				    xml_base_path,
				    path);

      if(!g_file_test(output_path,
		      G_FILE_TEST_IS_DIR)){
	g_mkdir(output_path,
		0x755);
      }
      
      ags_lv2_turtle_to_xml(plugin_path,
			    output_path);
    }    
  }
  
  return(0);
}

