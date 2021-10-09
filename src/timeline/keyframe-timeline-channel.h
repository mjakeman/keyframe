#pragma once

#include <gtk/gtk.h>

#include "keyframe-timeline-track.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_CHANNEL (keyframe_timeline_channel_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineChannel, keyframe_timeline_channel, KEYFRAME, TIMELINE_CHANNEL, GtkWidget)

GtkWidget *keyframe_timeline_channel_new (void);
GtkWidget *keyframe_timeline_channel_get_info_box (KeyframeTimelineChannel *self);

void
keyframe_timeline_channel_set_track (KeyframeTimelineChannel *self,
                                     KeyframeTimelineTrack   *track);

KeyframeTimelineTrack *
keyframe_timeline_channel_get_track (KeyframeTimelineChannel *self);

G_END_DECLS
