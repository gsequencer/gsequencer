#include "ags_apply_synth.h"

#include "../../object/ags_connectable.h"
#include "../ags_audio.h"

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
  AgsApplySynth *apply_synth;
  AgsDevout *devout;
  AgsChannel *channel;
  AgsAudioSignal *audio_signal;
  GList *stream;
  short *buffer;
  guint real_frame_count, last_frame_count, real_length;
  guint start, length, stop;
  guint current_attack, current_length;
  guint current_frequency, current_phase[2];
  guint i, j;
  void ags_apply_synth_launch_write(AgsDevout *devout, short *buffer, guint offset,
				    guint frequency, guint phase, guint frame_count,
				    double volume){
    printf("buffer_size = %u; offset = %u; frequency = %u; phase = %u; frame_count = %u; volume = %f\n\0", devout->buffer_size, offset, frequency, phase, frame_count, volume);

    switch(apply_synth->wave){
    case AGS_APPLY_SYNTH_SIN:
      ags_synth_sin(devout, buffer, offset,
		    frequency, phase, length,
		    volume);
      break;
    case AGS_APPLY_SYNTH_SAW:
      ags_synth_saw(devout, buffer, offset,
		    frequency, phase, length,
		    volume);
      break;
    case AGS_APPLY_SYNTH_SQUARE:
      ags_synth_square(devout, buffer, offset,
		       frequency, phase, length,
		       volume);
      break;
    case AGS_APPLY_SYNTH_TRIANGLE:
      ags_synth_triangle(devout, buffer, offset,
			 frequency, phase, length,
			 volume);
      break;
    default:
      printf("ags_apply_synth_launch: invalid wave\n\0");
      break;
    }
  }

  apply_synth = AGS_APPLY_SYNTH(task);

  channel = apply_synth->start_channel;
  devout = AGS_DEVOUT(AGS_AUDIO(apply_synth->start_channel->audio)->devout);

  start = (guint) floor((double)apply_synth->attack / (double)devout->buffer_size);

  real_frame_count = apply_synth->attack + apply_synth->frame_count;
  real_length = (guint) ceil((double)real_frame_count / (double)devout->buffer_size);

  length = real_length - (guint) floor((double)apply_synth->attack / (double)devout->buffer_size);

  current_attack = apply_synth->attack % (guint) devout->buffer_size;

  last_frame_count = (apply_synth->frame_count - current_attack) % devout->buffer_size;

  for(i = 0; channel != NULL && i < apply_synth->count; i++){
    current_frequency = (guint) ((double) apply_synth->frequency *
				 exp2((double)((apply_synth->start * -1.0) + (double)i) / 12.0));

    current_phase[0] = (guint) ((double) apply_synth->phase *
				((double) apply_synth->frequency / (double) current_frequency));

    audio_signal = ags_audio_signal_get_template(channel->first_recycling->audio_signal);

    if(audio_signal->length < real_length)
      ags_audio_signal_stream_resize(audio_signal, real_length);

    stream = g_list_nth(audio_signal->stream_beginning, start);


    if(length == 1){
      buffer = (short *) stream->data;

      ags_apply_synth_launch_write(devout, buffer, current_attack,
				   current_frequency, current_phase[0], apply_synth->frame_count,
				   apply_synth->volume);

      channel = channel->next;
      continue;
    }else{
      buffer = (short *) stream->data;

      ags_apply_synth_launch_write(devout, buffer, current_attack,
				   current_frequency, current_phase[0], devout->buffer_size - current_attack,
				   apply_synth->volume);

      stream = stream->next;
    }

    for(j = 1; j < length - 1; j++){
      current_phase[1] = (current_phase[0] + (devout->buffer_size - current_attack) + (j - 1) * devout->buffer_size) % current_frequency;

      //      current_phase[1] = (j * devout->buffer_size + current_phase[0]) % (devout->frequency / current_frequency);
      buffer = (short *) stream->data;
      
      ags_apply_synth_launch_write(devout, buffer, 0,
				   current_frequency, current_phase[1], devout->buffer_size,
				   apply_synth->volume);

      stream = stream->next;
    }

    current_phase[1] = (current_phase[0] + (devout->buffer_size - current_attack) + (j - 1) * devout->buffer_size) % current_frequency;
    buffer = (short *) stream->data;

    ags_apply_synth_launch_write(devout, buffer, 0,
				 current_frequency, current_phase[1], last_frame_count,
				 apply_synth->volume);

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
