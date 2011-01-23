#include <ags/audio/ags_input.h>

void ags_input_class_init (AgsInputClass *input_class);
void ags_input_init (AgsInput *input);
void ags_input_finalize (GObject *gobject);

extern void ags_file_write_input(AgsFile *file, AgsChannel *channel);

static gpointer ags_input_parent_class = NULL;

GType
ags_input_get_type (void)
{
  static GType ags_type_input = 0;

  if(!ags_type_input){
    static const GTypeInfo ags_input_info = {
      sizeof (AgsInputClass),
      (GBaseInitFunc) NULL, /* base_init */
      (GBaseFinalizeFunc) NULL, /* base_finalize */
      (GClassInitFunc) ags_input_class_init,
      NULL, /* class_finalize */
      NULL, /* class_data */
      sizeof (AgsInput),
      0,    /* n_preallocs */
      (GInstanceInitFunc) ags_input_init,
    };

    ags_type_input = g_type_register_static(AGS_TYPE_CHANNEL,
					    "AgsInput\0",
					    &ags_input_info,
					    0);
  }

  return (ags_type_input);
}

void
ags_input_class_init(AgsInputClass *input)
{
  GObjectClass *gobject;
  AgsChannelClass *channel;

  ags_input_parent_class = g_type_class_peek_parent(input);
  
  gobject = (GObjectClass *) input;
  gobject->finalize = ags_input_finalize;
  
  channel = (AgsChannelClass *) input;
  channel->write_file = ags_file_write_input;
}

void
ags_input_init(AgsInput *input)
{
  input->file = NULL;
}

void
ags_input_finalize(GObject *gobject)
{
  AgsInput *input;

  input = AGS_INPUT(gobject);

  if(input->file != NULL)
    g_object_unref(input->file);

  G_OBJECT_CLASS(ags_input_parent_class)->finalize(gobject);
}

void
ags_input_connect(AgsInput *input)
{
}

AgsInput*
ags_input_new()
{
  AgsInput *input;

  input = (AgsInput *) g_object_new(AGS_TYPE_INPUT, NULL);

  return(input);
}
