#pragma once

#include <gtk/gtk.h>
#include "keyframe-tracks.h"

#include "../../model/keyframe-value-float.h"

struct _KeyframeTrackFrame
{
    KeyframeTrack parent_instance;

    KeyframeValueFloat *float_value;

    gboolean drag_active;
    float drag_start_x;
    float drag_current_x;

    float start_position;
};

// Layout
#define KEYFRAME_TYPE_TRACK_FRAME_LAYOUT (keyframe_track_frame_layout_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTrackFrameLayout, keyframe_track_frame_layout, KEYFRAME, TRACK_FRAME_LAYOUT, GtkLayoutManager)
