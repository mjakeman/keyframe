#pragma once

#include <glib-2.0/glib-object.h>

#include "keyframe-layer.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_COMPOSITION (keyframe_composition_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeComposition, keyframe_composition, KEYFRAME, COMPOSITION, GObject)

struct _KeyframeCompositionClass
{
  GObjectClass parent_class;
};

KeyframeComposition *
keyframe_composition_new (const char *title,
                          int         width,
                          int         height,
                          float       framerate);

void
keyframe_composition_push_layer (KeyframeComposition *self,
                                 KeyframeLayer       *layer);

GSList *
keyframe_composition_get_layers (KeyframeComposition *self);

G_END_DECLS
