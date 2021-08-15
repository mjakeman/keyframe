#pragma once

#include <glib-2.0/glib-object.h>

#include "keyframe-renderer.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_LAYER (keyframe_layer_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeLayer, keyframe_layer, KEYFRAME, LAYER, GObject)

struct _KeyframeLayerClass
{
  GObjectClass parent_class;
  void (*fill_command_buffer)(KeyframeLayer *self, KeyframeRenderer *renderer);
};

void keyframe_layer_fill_command_buffer (KeyframeLayer *self, KeyframeRenderer *renderer);

G_END_DECLS
