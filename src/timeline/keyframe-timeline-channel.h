#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_CHANNEL (keyframe_timeline_channel_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineChannel, keyframe_timeline_channel, KEYFRAME, TIMELINE_CHANNEL, GtkBox)

GtkWidget *keyframe_timeline_channel_new (void);

G_END_DECLS
