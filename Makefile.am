AUTOMAKE_OPTIONS = foreign

# sources
AGS_DIR = ./src/ags/
BIN_DIR = ./bin/

AGS_LIB_SRCS = $(wildcard $(AGS_DIR)lib/*.c)

AGS_OBJECT_SRCS = $(wildcard $(AGS_DIR)object/*.c)

AGS_AUDIO_SRCS = $(wildcard $(AGS_DIR)audio/*.c)
AGS_TASK_SRCS = $(wildcard $(AGS_DIR)audio/task/*.c)
AGS_RECALL_SRCS = $(wildcard $(AGS_DIR)audio/recall/*.c)
AGS_AUDIO_FILE_SRCS = $(wildcard $(AGS_DIR)audio/file/*.c)

AGS_WIDGET_SRCS = $(wildcard $(AGS_DIR)widget/*.c)

AGS_X_SRCS = $(wildcard $(AGS_DIR)X/*.c)
AGS_EDITOR_SRCS = $(wildcard $(AGS_DIR)X/editor/*.c)
AGS_MACHINE_SRCS = $(wildcard $(AGS_DIR)X/machine/*.c)

AGS_FILE_SRCS = $(wildcard $(AGS_DIR)file/*.c)

# 
CFLAGS = -Isrc $(LIBASOUND2_CFLAGS) $(LIBAO_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(GTK_CFLAGS)
LIBS = $(LIBASOUND2_LIBS) $(LIBAO_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(GTK_LIBS)

ags_SRCS = $(AGS_DIR)main.c $(AGS_LIB_SRCS) $(AGS_OBJECT_SRCS) $(AGS_AUDIO_SRCS) $(AGS_TASK_SRCS) $(AGS_RECALL_SRCS) $(AGS_AUDIO_FILE_SRCS) $(AGS_WIDGET_SRCS) $(AGS_X_SRCS) $(AGS_EDITOR_SRCS) $(AGS_MACHINE_SRCS) $(AGS_FILE_SRCS)

bin_PROGRAM = ags
ags_OBJECTS = ${ags_SRCS:.c=.o}

ags: $(ags_OBJECTS)
	mv $(ags_OBJECTS) $(BIN_DIR)
	$(CC) $(ags_OBJECTS) -o ../$(bin_PROGRAM) $(LIBS)

$@: $(ags_SRCS)
	$(CC) -ggdb -c $? -o $@ $(CFLAGS)
