#pragma once

#include <gtk/gtk.h>

#include "model/keyframe-composition.h"

G_BEGIN_DECLS

#define KEYFRAME_TYPE_EXPORT_DIALOG (keyframe_export_dialog_get_type())

G_DECLARE_DERIVABLE_TYPE (KeyframeExportDialog, keyframe_export_dialog, KEYFRAME, EXPORT_DIALOG, GtkDialog)

struct _KeyframeExportDialogClass
{
  GtkDialogClass parent_class;
};

GtkWidget *keyframe_export_dialog_new (KeyframeComposition *composition);

G_END_DECLS
