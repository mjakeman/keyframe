keyframe_sources = [
	'main.c',
	'keyframe-window.c',
	'keyframe-application.c',
	'keyframe-timeline.c',
	'keyframe-layer.c',
	'keyframe-layer-geometry.c',
	'keyframe-canvas.c',
	'keyframe-renderer.c'
]

keyframe_deps = [
	dependency('gtk4', version : '>=4.3'),
	dependency('libadwaita-1', fallback : ['libadwaita', 'libadwaita_dep']),
    dependency('libpanel', fallback : ['libpanel', 'libpanel_dep'])
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
