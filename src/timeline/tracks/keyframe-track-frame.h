#pragma once

#include <gtk/gtk.h>
#include "keyframe-track.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TRACK_FRAME (keyframe_track_frame_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackFrame, keyframe_track_frame, KEYFRAME, TRACK_FRAME, KeyframeTrack)

GtkWidget *keyframe_track_frame_new (void);

G_END_DECLS
