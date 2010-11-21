#include <ags/audio/task/ags_apply_synth.h>

#include <ags/object/ags_connectable.h>

#include <ags/audio/ags_audio.h>
#include <ags/audio/ags_audio_signal.h>
#include <ags/audio/ags_synths.h>

#include <math.h>

void ags_apply_synth_class_init(AgsApplySynthClass *apply_synth);
void ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable);
void ags_apply_synth_init(AgsApplySynth *apply_synth);
void ags_apply_synth_connect(AgsConnectable *connectable);
void ags_apply_synth_disconnect(AgsConnectable *connectable);
void ags_apply_synth_finalize(GObject *gobject);

void ags_apply_synth_launch(AgsTask *task);

static gpointer ags_apply_synth_parent_class = NULL;
static AgsConnectableInterface *ags_apply_synth_parent_connectable_interface;

GType
ags_apply_synth_get_type()
{
  static GType ags_type_apply_synth = 0;

  if(!ags_type_apply_synth){
    static const GTypeInfo ags_apply_synth_info = {
      sizeof (AgsApplySynthClass),
      NULL, /* base_init */
      NULL, /* base_finalize */
      (GClassInitFunc) ags_apply_synth_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsApplySynth),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_apply_synth_init,
    };

    static const GInterfaceInfo ags_connectable_interface_info = {
      (GInterfaceInitFunc) ags_apply_synth_connectable_interface_init,
      NULL, /* interface_finalize */
      NULL, /* interface_data */
    };

    ags_type_apply_synth = g_type_register_static(AGS_TYPE_TASK,
						  "AgsApplySynth\0",
						  &ags_apply_synth_info,
						  0);

    g_type_add_interface_static(ags_type_apply_synth,
				AGS_TYPE_CONNECTABLE,
				&ags_connectable_interface_info);
  }
  
  return (ags_type_apply_synth);
}

void
ags_apply_synth_class_init(AgsApplySynthClass *apply_synth)
{
  GObjectClass *gobject;
  AgsTaskClass *task;

  ags_apply_synth_parent_class = g_type_class_peek_parent(apply_synth);

  /* GObjectClass */
  gobject = (GObjectClass *) apply_synth;

  gobject->finalize = ags_apply_synth_finalize;

  /* AgsTaskClass */
  task = (AgsTaskClass *) apply_synth;

  task->launch = ags_apply_synth_launch;
}

void
ags_apply_synth_connectable_interface_init(AgsConnectableInterface *connectable)
{
  ags_apply_synth_parent_connectable_interface = g_type_interface_peek_parent(connectable);

  connectable->connect = ags_apply_synth_connect;
  connectable->disconnect = ags_apply_synth_disconnect;
}

void
ags_apply_synth_init(AgsApplySynth *apply_synth)
{
  apply_synth->start_channel = NULL;
  apply_synth->count = 0;

  apply_synth->wave = AGS_APPLY_SYNTH_INVALID;
  apply_synth->attack = 0;
  apply_synth->frame_count = 0;
  apply_synth->phase = 0;
  apply_synth->start = 0;
  apply_synth->volume = 1.0;
}

void
ags_apply_synth_connect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->connect(connectable);

  /* empty */
}

void
ags_apply_synth_disconnect(AgsConnectable *connectable)
{
  ags_apply_synth_parent_connectable_interface->disconnect(connectable);

  /* empty */
}

void
ags_apply_synth_finalize(GObject *gobject)
{
  G_OBJECT_CLASS(ags_apply_synth_parent_class)->finalize(gobject);

  /* empty */
}

void
ags_apply_synth_launch(AgsTask *task)
{
  AgsDevout *devout;
  AgsApplySynth *apply_synth;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  GList *stream;
  gint wave;
  guint attack, frame_count, stop, phase, frequency;
  double volume;
  guint stream_start, last_frame_count, current_frequency, current_phase[2];
  guint i, j;
  void ags_apply_synth_launch_write(guint offset,
				    guint frequency, guint phase, guint frame_count,
				    double volume){
    switch(wave){
    case AGS_APPLY_SYNTH_SIN:
      ags_synth_sin(devout, (short *) stream->data,
		    offset, frequency, phase, frame_count,
		    volume);
      break;
    case AGS_APPLY_SYNTH_SAW:
      ags_synth_saw(devout, (short *) stream->data,
		    offset, frequency, phase, frame_count,
		    volume);
      break;
    case AGS_APPLY_SYNTH_SQUARE:
      ags_synth_square(devout, (short *) stream->data, offset, frequency, phase, frame_count, volume);
      break;
    case AGS_APPLY_SYNTH_TRIANGLE:
      ags_synth_triangle(devout, (short *) stream->data, offset, frequency, phase, frame_count, volume);
      break;
    default:
      printf("ags_apply_synth_launch_write: warning no wave selected\n\0");
    }
  }

  apply_synth = AGS_APPLY_SYNTH(task);
  channel = apply_synth->start_channel;
  devout = AGS_DEVOUT(AGS_AUDIO(channel->audio)->devout);
  
  wave = (gint) apply_synth->wave;
  fprintf(stdout, "wave = %d\n\0", wave);

  attack = apply_synth->attack;
  frame_count = apply_synth->frame_count;
  stop = (guint) ceil((double)(attack + frame_count) / (double)devout->buffer_size);
  phase = apply_synth->phase;
  frequency = apply_synth->frequency;
  volume = (double) apply_synth->volume;

  stream_start = (guint) floor((double)attack / (double)devout->buffer_size);
  attack = attack % (guint) devout->buffer_size;
  
  last_frame_count = (frame_count - devout->buffer_size - attack) % devout->buffer_size;
  current_phase[0] = (phase + (devout->buffer_size - attack) + i * devout->buffer_size) % frequency;


  for(i = 0; channel != NULL; i++){
    current_frequency = (guint) ((double) frequency * exp2((double)((apply_synth->start * -1.0) + (double)i) / 12.0));
    current_phase[0] = (guint) ((double) phase * ((double) frequency / (double) current_frequency));
    
    audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);
    
    ags_audio_signal_stream_resize(audio_signal, stop);
    
    stream = g_list_nth(audio_signal->stream_beginning, stream_start);
    
    if(stream->next != NULL){
      fprintf(stdout, "freq = %u, phase = %u\n\0", current_frequency, current_phase[0]);
      ags_apply_synth_launch_write(attack,
				   current_frequency, current_phase[0], AGS_DEVOUT(audio_signal->devout)->buffer_size - attack,
				   volume);
    }else{
      ags_apply_synth_launch_write(attack,
				   current_frequency, current_phase[0], frame_count,
				   volume);
      
      channel = channel->next;
      continue;
    }
    
    stream = stream->next;
    
    
    for(j = 1; stream->next != NULL; j++){
      current_phase[1] = (j * AGS_DEVOUT(audio_signal->devout)->buffer_size + current_phase[0]) % (devout->frequency / current_frequency);
      fprintf(stdout, "freq = %u, phase = %u\n\0", current_frequency, current_phase[1]);
      
      ags_apply_synth_launch_write(0,
				   frequency, current_phase[1], AGS_DEVOUT(audio_signal->devout)->buffer_size,
				   volume);
      
      stream = stream->next;
    }
    
    current_phase[1] = (current_phase[0] + (AGS_DEVOUT(audio_signal->devout)->buffer_size - attack) + j * AGS_DEVOUT(audio_signal->devout)->buffer_size) % current_frequency;
    ags_apply_synth_launch_write(0,
				 current_frequency, current_phase[1], last_frame_count,
				 volume);
    
    
    channel = channel->next;
  }
}

AgsApplySynth*
ags_apply_synth_new(AgsChannel *start_channel, guint count,
		    guint wave,
		    guint attack, guint frame_count,
		    guint frequency, guint phase, guint start,
		    gdouble volume)
{
  AgsApplySynth *apply_synth;

  apply_synth = (AgsApplySynth *) g_object_new(AGS_TYPE_APPLY_SYNTH,
					       NULL);

  apply_synth->start_channel = start_channel;
  apply_synth->count = count;
  apply_synth->wave = wave;
  apply_synth->attack = attack;
  apply_synth->frame_count = frame_count;
  apply_synth->frequency = frequency;
  apply_synth->phase = phase;
  apply_synth->start = start;
  apply_synth->volume = volume;
  
  return(apply_synth);
}
