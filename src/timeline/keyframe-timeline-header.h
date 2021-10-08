#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE_HEADER (keyframe_timeline_header_get_type())

G_DECLARE_FINAL_TYPE (KeyframeTimelineHeader, keyframe_timeline_header, KEYFRAME, TIMELINE_HEADER, GtkWidget)

GtkWidget *keyframe_timeline_header_new (void);

G_END_DECLS
