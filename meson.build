project('rdmtracker', 'cpp', 'c', default_options: ['cpp_std=c++20'])

rdm4001_proj = subproject('rdm4001')
rdm4001_dep = rdm4001_proj.get_variable('rdm4001_dep')

emu2413 = static_library('emu2413',
                         'emu2413.h',
                         'emu2413.c',
              )

executable('rdmtracker',
	'main.cpp',
	'tracker.hpp',
	'tracker.cpp',
	'opl.hpp',
	'opl.cpp',
        'emu2413.h',
        'emu2413.c',
	dependencies: rdm4001_dep, link_with: emu2413
)
