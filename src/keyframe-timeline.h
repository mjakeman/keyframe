#pragma once

#include <gtk/gtk.h>

#include "keyframe-layer.h"
#include "keyframe-layer-geometry.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_TIMELINE (keyframe_timeline_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeTimeline, keyframe_timeline, KEYFRAME, TIMELINE, GtkWidget)

struct _KeyframeTimelineClass
{
  GtkWidgetClass parent_class;
};

KeyframeTimeline *keyframe_timeline_new (void);

G_END_DECLS
