#pragma once

#include <gtk/gtk.h>

#include "keyframe-track.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TRACK_CLIP (keyframe_track_clip_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackClip, keyframe_track_clip, KEYFRAME, TRACK_CLIP, KeyframeTrack)

GtkWidget *keyframe_track_clip_new (void);

G_END_DECLS
