keyframe_sources = [
	'main.c',
	'keyframe-window.c',
	'keyframe-application.c',
	'keyframe-canvas.c',
	'keyframe-renderer.c',
	'keyframe-composition-manager.c',
	'keyframe-export-dialog.c',
	'keyframe-layer-list.c',
	'keyframe-layer-properties-dialog.c',
	'keyframe-control-point-dialog.c',
	'timeline/keyframe-timeline.c',
	'timeline/keyframe-timeline-menu.c',
	'timeline/keyframe-timeline-column-view.c',
	'timeline/keyframe-timeline-property.c',
	'timeline/keyframe-timeline-header.c',
	'timeline/keyframe-timeline-channel.c',
	'timeline/tracks/keyframe-track.c',
	'timeline/tracks/keyframe-track-clip.c',
	'timeline/tracks/keyframe-track-clip-layout.c',
	'timeline/tracks/keyframe-track-clip-widget.c',
	'timeline/tracks/keyframe-track-frame.c',
	'timeline/tracks/keyframe-track-frame-layout.c',
	'timeline/tracks/keyframe-track-frame-point.c',
	'model/keyframe-layer.c',
	'model/keyframe-layer-geometry.c',
	'model/keyframe-layer-text.c',
	'model/keyframe-layer-cool.c',
	'model/keyframe-composition.c',
	'model/keyframe-value-float.c',
	'export/export-gstreamer.c'
]

keyframe_deps = [
	dependency('gtk4', version : '>=4.4'),
	dependency('libadwaita-1'),
    dependency('libpanel-1'),
    dependency('gstreamer-1.0'),
    dependency('gstreamer-app-1.0')
]

# Move to data dir?
subdir('res')

executable('keyframe', keyframe_sources,
	dependencies: keyframe_deps,
	install: true,
)
