#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TRACK (keyframe_track_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeTrack, keyframe_track, KEYFRAME, TRACK, GtkWidget)

struct _KeyframeTrackClass
{
  GtkWidgetClass parent_class;
  void (*adjustment_changed)(KeyframeTrack *self, GtkAdjustment *adj);
};

GtkWidget *keyframe_track_new (void);

float
keyframe_track_get_start_pos (KeyframeTrack *self);

G_END_DECLS

#include "keyframe-tracks.h"
