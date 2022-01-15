#pragma once

#include "keyframe-track.h"

// CLIP
#define KEYFRAME_TYPE_TRACK_CLIP (keyframe_track_clip_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackClip, keyframe_track_clip, KEYFRAME, TRACK_CLIP, KeyframeTrack)

GtkWidget *keyframe_track_clip_new (void);


// FRAME
#define KEYFRAME_TYPE_TRACK_FRAME (keyframe_track_frame_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackFrame, keyframe_track_frame, KEYFRAME, TRACK_FRAME, KeyframeTrack)

GtkWidget *keyframe_track_frame_new (void);
