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

#include <ags/audio/midi/ags_midi_parser.h>

void ags_midi_parser_class_init(AgsMidiParserClass *midi_parser);
void ags_midi_parser_init(AgsMidiParser *midi_parser);
void ags_midi_parser_finalize(GObject *gobject);

#define AGS_MIDI_PARSE_MSB16(x) ((x[0] << 8) | (x[1]))
#define AGS_MIDI_PARSE_MSB24(x) ((x[0] << 16) | (x[1] << 8) | (x[2]))
#define AGS_MIDI_PARSE_MSB32(x) ((x[0] << 24) | (x[1] << 16) | (x[2] << 8) | (x[3]))
#define AGS_MIDI_PARSE_MSB64(x) ((x[0] << 56) | (x[1] << 48) | (x[2] << 40) | (x[3] << 32) | (x[4] << 24) | (x[5] << 16) | (x[6] << 8) | (x[7]))

#define AGS_MIDI_EVENT "event\0"

/**
 * SECTION:ags_midi_parser
 * @short_description: the menu bar.
 * @title: AgsMidiParser
 * @section_id:
 * @include: ags/X/ags_midi_parser.h
 *
 * #AgsMidiParser reads your midi parsers.
 */

static gpointer ags_midi_parser_parent_class = NULL;

GType
ags_midi_parser_get_type(void)
{
  static GType ags_type_midi_parser = 0;

  if(!ags_type_midi_parser){
    static const GTypeInfo ags_midi_parser_info = {
      sizeof (AgsMidiParserClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_midi_parser_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsMidiParser),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_midi_parser_init,
    };

    ags_type_midi_parser = g_type_register_static(G_TYPE_OBJECT,
						  "AgsMidiParser\0", &ags_midi_parser_info,
						  0);
  }

  return(ags_type_midi_parser);
}

void
ags_midi_parser_class_init(AgsMidiParserClass *midi_parser)
{
  GObjectClass *gobject;

  ags_midi_parser_parent_class = g_type_class_peek_parent(midi_parser);

  /* GObjectClass */
  gobject = (GObjectClass *) midi_parser;
  
  gobject->finalize = ags_midi_parser_finalize;
}

void
ags_midi_parser_init(AgsMidiParser *midi_parser)
{
  midi_parser->fd = -1;
  midi_parser->nth_chunk = 0;
}

void
ags_midi_parser_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_midi_parser_parent_class)->finalize(gobject);
}
}

char*
ags_midi_parser_read_chunk(AgsMidiParser *midi_parser,
			   guint *message_type, guint *message_length,
			   GError **error)
{
  char chunk_type[4];
  char chunk_length[4];
  char *data;

  guint midi_type;
  guint data_length;
  
  read(midi_parser->fd, chunk_type, 4 * sizeof(char));

  if(!g_strncmp(AGS_MIDI_PARSER_MTHD,
		chunk_type,
		4)){
    midi_type = AGS_MIDI_CHUNK_HEADER;
  }else if(!g_strncmp(AGS_MIDI_PARSER_MTCK,
		      chunk_type,
		      4)){
    midi_type = AGS_MIDI_CHUNK_TRACK;
  }else{
    midi_type = AGS_MIDI_CHUNK_UNKNOWN;
  }
  
  read(midi_parser->fd, chunk_length, 4 * sizeof(char));
  data_length = AGS_MIDI_PARSE_MSB32(chunk_length);

  data = (char *) malloc(data_length * sizeof(char));
  read(midi_parser->fd, data, data_length * sizeof(char));

  midi_parser->nth_chunk += 1;
  
  if(message_type != NULL){
    *message_type = midi_type;
  }

  if(message_length != NULL){
    *message_length = data_length;
  }
  
  return(data);
}

void
ags_midi_parser_write_chunk(AgsMidiParser *midi_parser,
			    char *chunk, size_t length)
{
  write(midi_parser->fd, chunk, length);
}

void
ags_midi_parser_seek(AgsMidiParser *midi_parser, guint n_chunks, gint whence)
{
  char chunk_head[8];
  guint start_chunk;
  guint data_length;

  switch(whence){
  case SEEK_CUR:
    start_chunk = midi_parser->nth_chunk;

    while(midi_parser->nth_chunk - start_chunk < n_chunks){
      read(midi_parser->fd, chunk_head, 8 * sizeof(char));
      data_length = AGS_MIDI_PARSE_MSB32(&(chunk_head[4]));
      
      seek(midi_parser->fd, SEEK_CUR, data_length);
      
      midi_parser->nth_chunk += 1;
    }

    break;
  case SEEK_SET:
    midi_parser->nth_chunk = 0;
    seek(midi_parser->fd, SEEK_SET, 0);

    while(midi_parser->nth_chunk < n_chunks){
      read(midi_parser->fd, chunk_head, 8 * sizeof(char));
      data_length = AGS_MIDI_PARSE_MSB32(&(chunk_head[4]));
      
      seek(midi_parser->fd, SEEK_CUR, data_length);

      midi_parser->nth_chunk += 1;
    }

    break;
  case SEEK_END:
    //TODO:JK: implement me
    break;
  }
}

void
ags_midi_parser_flush(AgsMidiParser *midi_parser)
{
  flush(midi_parser->fd);
}

xmlNode*
ags_midi_parser_parse_header(AgsMidiParser *parser,
			     char *chunk_data,
			     guint chunk_type,
			     guint chunk_length)
{
  xmlNode *node;

  char chunk_format[2];
  char chunk_tracks[2];
  char chunk_division[2];
  
  guint track_format;
  guint track_count;
  guint track_division;
  
  node = xmlNewNode(NULL,
		    "midi-header\0");

  memcpy(chunk_format, data, 2 * sizeof(char));
  track_format = AGS_MIDI_PARSE_MSB16(chunk_format);

  switch(track_format){
  case 0:
    {
      xmlNewProp(node,
		 "track-format\0",
		 "single\0");
    }
    break;
  case 1:
    {
      xmlNewProp(node,
		 "track-format\0",
		 "simoultaneous\0");
    }
    break;
  case 2
    {
      xmlNewProp(node,
		 "track-format\0",
		 "independant\0");
    }
  break;
  }
  
  memcpy(chunk_tracks, &(data[2]), 2 * sizeof(char));
  track_count = AGS_MIDI_PARSE_MSB16(chunk_tracks);

  xmlNewProp(node,
	     "track-count\0",
	     g_strdup_printf("%d\0", track_count));
  
  memcpy(chunk_division, &(data[4]), 2 * sizeof(char));
  track_division = AGS_MIDI_PARSE_MSB16(chunk_division);

  if((track_division & (1 << 15)) != 0){
    xmlNewProp(node,
	       "smtpe\0",
	       "false\0");
    
    xmlNewProp(node,
	       "delta-time\0",
	       g_strdup_printf("%d\0",
			       (track_division & (~(1 << 15)))));
  }else{
    xmlNewProp(node,
	       "smtpe\0",
	       "true\0");

    xmlNewProp(node,
	       "delta-time\0",
	       g_strdup_printf("%d\0",
			       (track_division & (~(0xff)))));

    xmlNewProp(node,
	       "smtpe-fps\0",
	       g_strdup_printf("%d\0",
			       (track_division & (~(0x7f00)))));
  }
  
  return(node);
}

xmlNode*
ags_midi_parser_parse_track(AgsMidiParser *parser,
			    char *chunk_data,
			    guint chunk_type,
			    guint chunk_length)
{
  xmlNode *node, *message_node;

  char chunk_delta_time[8];
  char voice_message[4];
  guint64 delta_time;
  guint imask;
  guint key, control, value, program, pressure, pitch;
  guint velocity;
  guint offset;
  guint i;
  
  node = xmlNewNode(NULL,
		    "midi-track\0");

  /* delta time */
  for(offset = 0; offset < 8; offset++){
    if((&(chunk_data[offset]) & (1 << 7)) != 0){
      break;
    }
  }

  for(i = 0; i < offset + 1; i++){
    chunk_delta_time[i] = chunk_data[offset - i];
  }
  
  delta_time = AGS_MIDI_PARSE_MSB64(chunk_delta_time);
  offset++;
  
  xmlNewProp(node,
	     "delta-time\0",
	     g_strdup_printf("%d\0", delta_time));

  for(; offset < chunk_length;){
    message_node = xmlNewNode(NULL,
			      "midi-message\0");
    
    if((chunk_data[offset] & (0x80)) != 0 ||
       (chunk_data[offset] & (0x90)) != 0 ||
       (chunk_data[offset] & (0xa0)) != 0){
      /* definitely voice message */
      imask = 0x0f;

      xmlNewProp(message_node,
		 "type\0",
		 "voice-message\0");

      key = chunk_data[offset + 1];
      velocity = chunk_data[offset + 2];

      if((~imask) & chunk_data[offset] == 0x80){
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "note-off\0");
      }else if((~imask) & chunk_data[offset] == 0x90){
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "note-on\0");
      }else{
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "polyphonic\0");
      }

      xmlNewProp(message_node,
		 "key\0",
		 g_strdup_printf("%d\0", key));

      xmlNewProp(message_node,
		 "velocity\0",
		 g_strdup_printf("%d\0", velocity));

      offset += 3;
    }else if(chunk_data[offset] == 0xb0){
      /* voice or channel mode message */
      imask = 0x0f;

      control = chunk_data[offset + 1];
      value = chunk_data[offset + 2];

      if(control < 120){
	xmlNewProp(message_node,
		   "type\0",
		   "voice-message\0");

	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "control-change\0");

	xmlNewProp(message_node,
		   "control\0",
		   g_strdup_printf("%d\0", control));

	xmlNewProp(message_node,
		   "value\0",
		   g_strdup_printf("%d\0", value));
      }else{
	xmlNewProp(message_node,
		   "type\0",
		   "mode-message\0");

	switch(control){
	case 120:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "all-sound-off\0");
	  }
	  break;
	case 121:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "reset-control-all\0");
	    
	    xmlNewProp(message_node,
		       "value\0",
		       g_strdup_printf("%d\0", value));
	  }
	  break;
	case 122:
	  {
	    if(value == 0){
	      xmlNewProp(message_node,
			 "mode\0",
			 "local-control-off\0");
	    }else{
	      xmlNewProp(message_node,
			 "mode\0",
			 "local-control-on\0");
	    }
	  }
	  break;
	case 123:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "all-notes-off");
	  }
	  break;
	case 124:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "omni-mode-on");
	  }
	  break;
	case 125:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "omni-mode-off\0");
	  }
	  break;
	case 126:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "mono-mode-on\0");

	    xmlNewProp(message_node,
		       "omni-off-channels\0",
		       g_strdup_printf("%d\0", value));
	  }
	  break;
	case 127:
	  {
	    xmlNewProp(message_node,
		       "mode\0",
		       "poly-mode-on\0");
	  }
	  break;
	}  
      }
      
      offset += 3;
    }else if((chunk_data[offset] & (0xc0)) != 0 ||
	     (chunk_data[offset] & (0xd0)) != 0 ||
	     (chunk_data[offset] & (0xe0)) != 0){
      imask = 0x0f;

      xmlNewProp(message_node,
		 "type\0",
		 "voice-message\0");
      
      key = chunk_data[offset + 1];
      velocity = chunk_data[offset + 2];

      /* definitely voice message */
      if((~imask) & chunk_data[offset] == 0xc0){
	program = chunk_data[offset + 1];
  
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "program-change\0");

	xmlNewProp(message_node,
		   "program\0",
		   g_strdup_printf("%d\0", program));
		
	offset += 2;
      }else if((~imask) & chunk_data[offset] == 0xd0){
	pressure = chunk_data[offset + 1];
	  	  
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "channel-pressure\0");

	xmlNewProp(message_node,
		   "pressure\0",
		   g_strdup_printf("%d\0", pressure));

	offset += 2;
      }else{
	pitch = AGS_MIDI_PARSE_MSB16(chunk_data[offset + 1]);

	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "pitch-bend\0");

	xmlNewProp(message_node,
		   "pitch\0",
		   g_strdup_printf("%d\0", pitch));	  

	offset += 3;
      }

    }else if((chunk_data[offset] & (0xf)) &&
	     !(chunk_data[offset] & (0x08))){
      /* system common message */
      xmlNewProp(message_node,
		 "type\0",
		 "system-common\0");
      
      if(chunk_data[offset] == 0xf0){
	guint manufacturer;
	guint start_value, end_value;
	char *reserved;

	/* system exclusive */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "sysex\0");

	if(chunck_length > 6){
	  manufacturer = AGS_MIDI_PARSE_MSB24(chunk_data);
	  offset += 3;
	}else{
	  manufacturer = chunk_data[0];
	  offset++;
	}
	
	xmlNewProp(message_node,
		   "manufacturer\0"
		   g_strdup_printf("%d\0", manufacturer));

	start_value = chunk_data[offset];
	xmlNewProp(message_node,
		   "start-value\0"
		   g_strdup_printf("%d\0", start_value));

	end_value = chunk_data[offset + 3];
	xmlNewProp(message_node,
		   "end-value\0"
		   g_strdup_printf("%d\0", end_value));

	reserverd = AGS_MIDI_PARSE_MSB16(&(chunk_data[1]));
	xmlNewProp(message_node,
		   "reserved\0"
		   g_strdup_printf("%x\0", reserved));
      }else if(chunk_data[offset] == 0xf1){
	guint frame_type;
	guint frame_value;

	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "quarter-frame\0");
	
	/* time code quarter frame */
	frame_type = (0xf0 & (&(chunk_data[1]))) >> 4;
	
	xmlNewProp(message_node,
		   "message-type\0",
		   g_strdup_printf("%x\0", frame_type));

	frame_type = (0x0f & (&(chunk_data[1])));

	xmlNewProp(message_node,
		   "message-value\0",
		   g_strdup_printf("%d\0", frame_value));
      }else if(chunk_data[offset] == 0xf2){
	guint beats_count;

	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "position-pointer\0");
	
	/* song position pointer */
	beats_count = (((0x3f & (chunk_data[0])) << 7) | (0x7f & (chunk_data[1])));
	
	xmlNewProp(message_node,
		   "beats-count\0",
		   g_strdup_printf("%d\0", beats_count));
      }else if(chunk_data[offset] == 0xf3){
	guint sequence;

	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "song-select\0");
	
	/* song select */
	sequence = chunk_data[0];
	
	xmlNewProp(message_node,
		   "sequence\0",
		   g_strdup_printf("%d\0", sequence);
      }else if(chunk_data[offset] == 0xf4){
	g_warning("undefined system common message\0");
      }else if(chunk_data[offset] == 0xf5){
	g_warning("undefined system common message\0");
      }else if(chunk_data[offset] == 0xf6){
	/* tune request to all analog synths */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "tune-on\0");
      }else if(chunk_data[offset] == 0xf7){
	/* end of sysex */
      }
    }else{
      /* real-time message */
      xmlNewProp(message_node,
		 "type\0",
		 "system-realtime\0");
      
      if(chunk_data[offset] == 0xf8){
	/* timing clock */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "clock\0");
      }else if(chunk_data[offset] == 0xf9){
	g_warning("undefined system realtime message\0");
      }else if(chunk_data[offset] == 0xfa){
	/* start */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "start\0");
      }else if(chunk_data[offset] == 0xfb){
	/* continue */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "continue\0");
      }else if(chunk_data[offset] == 0xfc){
	/* stop */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "stop\0");
      }else if(chunk_data[offset] == 0xfd){
	g_warning("undefined system realtime message\0");
      }else if(chunk_data[offset] == 0xfe){
	/* active sensinge */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "active-sense\0");
      }else if(chunk_data[offset] == 0xff){
	/* reset */
	xmlNewProp(message_node,
		   AGS_MIDI_EVENT,
		   "reset\0");
      }
    }
  }

  xmlAddChild(node,
	      message_node);
}
  
  //TODO:JK: implement me
  
  return(node);
}

AgsMidiParser*
ags_midi_parser_new()
{
  AgsMidiParser *midi_parser;

  midi_parser = (AgsMidiParser *) g_object_new(AGS_TYPE_MIDI_PARSER,
					       NULL);

  return(midi_parser);
}

