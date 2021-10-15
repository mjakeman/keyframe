#include "keyframe-control-point-dialog.h"

struct _KeyframeControlPointDialog
{
    GtkDialog parent_instance;

    GtkSpinButton *time_spinner;
    GtkSpinButton *value_spinner;

    float old_time;
};

G_DEFINE_FINAL_TYPE (KeyframeControlPointDialog, keyframe_control_point_dialog, GTK_TYPE_DIALOG)

enum {
    PROP_0,
    PROP_OLD_TIMESTAMP,
    PROP_TIMESTAMP,
    PROP_VALUE,
    N_PROPS
};

static GParamSpec *properties [N_PROPS];

GtkWidget *
keyframe_control_point_dialog_new (float timestamp, float value)
{
    return g_object_new (KEYFRAME_TYPE_CONTROL_POINT_DIALOG,
                         "old-timestamp", timestamp,
                         "timestamp", timestamp,
                         "value", value,
                         NULL);
}

static void
keyframe_control_point_dialog_finalize (GObject *object)
{
    KeyframeControlPointDialog *self = (KeyframeControlPointDialog *)object;

    G_OBJECT_CLASS (keyframe_control_point_dialog_parent_class)->finalize (object);
}

static void
keyframe_control_point_dialog_get_property (GObject    *object,
                                            guint       prop_id,
                                            GValue     *value,
                                            GParamSpec *pspec)
{
    KeyframeControlPointDialog *self = KEYFRAME_CONTROL_POINT_DIALOG (object);

    switch (prop_id)
    {
    case PROP_OLD_TIMESTAMP:
        g_value_set_float (value, self->old_time);
        break;
    case PROP_TIMESTAMP:
        g_value_set_float (value, gtk_spin_button_get_value (self->time_spinner));
        break;
    case PROP_VALUE:
        g_value_set_float (value, gtk_spin_button_get_value (self->value_spinner));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_control_point_dialog_set_property (GObject      *object,
                                            guint         prop_id,
                                            const GValue *value,
                                            GParamSpec   *pspec)
{
    KeyframeControlPointDialog *self = KEYFRAME_CONTROL_POINT_DIALOG (object);

    switch (prop_id)
    {
    case PROP_OLD_TIMESTAMP:
        self->old_time = g_value_get_float (value);
        break;
    case PROP_TIMESTAMP:
        gtk_spin_button_set_value (self->time_spinner, g_value_get_float (value));
        break;
    case PROP_VALUE:
        gtk_spin_button_set_value (self->value_spinner, g_value_get_float (value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
    }
}

static void
keyframe_control_point_dialog_class_init (KeyframeControlPointDialogClass *klass)
{
    GObjectClass *object_class = G_OBJECT_CLASS (klass);

    object_class->finalize = keyframe_control_point_dialog_finalize;
    object_class->get_property = keyframe_control_point_dialog_get_property;
    object_class->set_property = keyframe_control_point_dialog_set_property;

    properties [PROP_OLD_TIMESTAMP]
        = g_param_spec_float ("old-timestamp",
                              "Old Timestamp",
                              "Old Timestamp",
                              0, G_MAXFLOAT, 0,
                              G_PARAM_READWRITE|G_PARAM_CONSTRUCT_ONLY);

    properties [PROP_TIMESTAMP]
        = g_param_spec_float ("timestamp",
                              "Timestamp",
                              "Timestamp",
                              0, G_MAXFLOAT, 0,
                              G_PARAM_READWRITE);

    properties [PROP_VALUE]
        = g_param_spec_float ("value",
                              "Value",
                              "Value",
                              -(G_MAXFLOAT/2), (G_MAXFLOAT/2)-1, 0,
                              G_PARAM_READWRITE);

    g_object_class_install_properties (object_class, N_PROPS, properties);

    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

    gtk_widget_class_set_template_from_resource (widget_class, "/com/mattjakeman/Keyframe/control-point-dialog.ui");
    gtk_widget_class_bind_template_child (widget_class, KeyframeControlPointDialog, time_spinner);
    gtk_widget_class_bind_template_child (widget_class, KeyframeControlPointDialog, value_spinner);
    /*gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, header_bar);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, grid);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, dock);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, timeline);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, render_btn);
    gtk_widget_class_bind_template_child (widget_class, KeyframeWindow, layer_list);
    gtk_widget_class_bind_template_callback (widget_class, create_frame_cb);*/
}

static void
keyframe_control_point_dialog_init (KeyframeControlPointDialog *self)
{
    gtk_widget_init_template (GTK_WIDGET (self));

    gtk_spin_button_set_range (self->time_spinner, 0, G_MAXFLOAT);
    gtk_spin_button_set_increments (self->time_spinner, 1, 10);

    gtk_spin_button_set_range (self->value_spinner, 0, G_MAXFLOAT);
    gtk_spin_button_set_increments (self->value_spinner, 1, 10);
}
