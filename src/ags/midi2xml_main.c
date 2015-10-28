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

#include <midi2ags/midi/ags_midi_parser.h>

#include <stdio.h>

#include <libxml/tree.h>

int
main(int argc, char **argv)
{
  AgsMidiParser *midi_parser;

  FILE *out;
  FILE *file;
  
  xmlChar *buffer;
  gchar *filename;
  size_t length;

  if(argc == 2){

    if(!strncmp(argv[1], "--help\0", 7)){
      printf("midi2xml converts MIDI to XML\n\n\0");

      printf("Usage:\n\t%s\n\t%s\n\t%s\n\t%s\n\n",
	     "Report bugs to <jkraehemann@gmail.com>\n\0",
	     "--help              display this help and exit\0",
	     "--version           output version information and exit\0",
	     "<file>              the file to be processed\0");
      
      exit(0);
    }else if(!strncmp(argv[1], "--version\0", 10)){
      printf("midi2xml 1.0.0\n\n\0");
      
      printf("%s\n%s\n%s\n\n\0",
	     "Copyright (C) 2015 Joël Krähemann\0",
	     "This is free software; see the source for copying conditions.  There is NO\0",
	     "warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\0");
      
      printf("Written by Joël Krähemann\n\0");
      exit(0);
    }else{
      filename = argv[1];
    }
  }else{
    return(-1);
  }

  file = fopen(filename, "r\0");
  fseek(file, 0, SEEK_SET);
  midi_parser = ags_midi_parser_new(file);

  ags_midi_parser_parse_full(midi_parser);

  xmlSaveFormatFileEnc("-", midi_parser->doc, "UTF-8", 1);
  
  return(0);
}
