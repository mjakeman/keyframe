#pragma once

#include <gtk/gtk.h>

#include "keyframe-timeline-track.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_TRACK_CLIP (keyframe_timeline_track_clip_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineTrackClip, keyframe_timeline_track_clip, KEYFRAME, TIMELINE_TRACK_CLIP, KeyframeTimelineTrack)

GtkWidget *keyframe_timeline_track_clip_new (void);

G_END_DECLS
