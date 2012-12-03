AUTOMAKE_OPTIONS = foreign
SUBDIRS = src

# sources
AGS_DIR = ./src/ags/
BIN_DIR = ./bin/

AGS_LIB_SRCS = $(wildcard $(AGS_DIR)lib/*.[ch])

AGS_OBJECT_SRCS = $(wildcard $(AGS_DIR)object/*.[ch])

AGS_AUDIO_SRCS = $(wildcard $(AGS_DIR)audio/*.[ch])
AGS_TASK_SRCS = $(wildcard $(AGS_DIR)audio/task/*.[ch])
AGS_TASK_RECALL_SRCS = $(wildcard $(AGS_DIR)audio/task/recall/*.[ch])
AGS_RECALL_SRCS = $(wildcard $(AGS_DIR)audio/recall/*.[ch])
AGS_AUDIO_FILE_SRCS = $(wildcard $(AGS_DIR)audio/file/*.[ch])

AGS_WIDGET_SRCS = $(wildcard $(AGS_DIR)widget/*.[ch])

AGS_X_SRCS = $(wildcard $(AGS_DIR)X/*.[ch])
AGS_EDITOR_SRCS = $(wildcard $(AGS_DIR)X/editor/*.[ch])
AGS_MACHINE_SRCS = $(wildcard $(AGS_DIR)X/machine/*.[ch])

AGS_FILE_SRCS = $(wildcard $(AGS_DIR)file/*.[ch])

# 
CFLAGS = -Isrc $(LIBASOUND2_CFLAGS) $(LIBAO_CFLAGS) $(LIBXML2_CFLAGS) $(SNDFILE_CFLAGS) $(LIBINSTPATCH_CFLAGS) $(GOBJECT_CFLAGS) $(GDKPIXBUF_CFLAGS) $(GTK_CFLAGS)
LIBS = $(LIBASOUND2_LIBS) $(LIBAO_LIBS) $(LIBXML2_LIBS) $(SNDFILE_LIBS) $(LIBINSTPATCH_LIBS) $(GOBJECT_LIBS) $(GDKPIXBUF_LIBS) $(GTK_LIBS)

ags_SRCS = $(AGS_DIR)main.c $(AGS_LIB_SRCS) $(AGS_OBJECT_SRCS) $(AGS_AUDIO_SRCS) $(AGS_TASK_SRCS) $(AGS_TASK_RECALL_SRCS) $(AGS_RECALL_SRCS) $(AGS_AUDIO_FILE_SRCS) $(AGS_WIDGET_SRCS) $(AGS_X_SRCS) $(AGS_EDITOR_SRCS) $(AGS_MACHINE_SRCS) $(AGS_FILE_SRCS)

bin_PROGRAM = ags
ags_OBJECTS = ${ags_SRCS:.c=.o}

ags: $(ags_SRCS)
	$(CC) $(CFLAGS) $(ags_SRCS) -o $(bin_PROGRAM) $(LIBS)
