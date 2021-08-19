keyframe_sources = [
	'main.c',
	'keyframe-window.c',
	'keyframe-application.c',
	'keyframe-timeline.c',
	'keyframe-canvas.c',
	'keyframe-renderer.c',
	'keyframe-composition-manager.c',
	'keyframe-export-dialog.c',
	'keyframe-layer-list.c',
	'keyframe-layer-properties-dialog.c',
	'model/keyframe-layer.c',
	'model/keyframe-layer-geometry.c',
	'model/keyframe-layer-text.c',
	'model/keyframe-layer-cool.c',
	'model/keyframe-composition.c',
	'model/keyframe-value-float.c',
	'export/export-gstreamer.c'
]

keyframe_deps = [
	dependency('gtk4', version : '>=4.3'),
	dependency('libadwaita-1', fallback : ['libadwaita', 'libadwaita_dep']),
    dependency('libpanel', fallback : ['libpanel', 'libpanel_dep']),
    dependency('gstreamer-1.0'),
    dependency('gstreamer-app-1.0')
]

gnome = import('gnome')

keyframe_sources += gnome.compile_resources('keyframe-resources',
	'keyframe.gresource.xml',
	c_name: 'keyframe'
)

executable('keyframe', keyframe_sources,
	dependencies: keyframe_deps,
	install: true,
)
