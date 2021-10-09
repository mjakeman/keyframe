#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_TRACK (keyframe_timeline_track_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeTimelineTrack, keyframe_timeline_track, KEYFRAME, TIMELINE_TRACK, GtkWidget)

struct _KeyframeTimelineTrackClass
{
  GtkWidgetClass parent_class;
};

GtkWidget *keyframe_timeline_track_new (void);

G_END_DECLS

#include "keyframe-timeline-tracks.h"
