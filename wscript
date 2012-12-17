#!/usr/bin/env python

top = '.'
out = '.waf_build_directory'

from waflib.Errors import ConfigurationError
from waflib import Configure, Logs
import os, sys
from subprocess import check_output

libdir = "lib"
clientsdir = "clients"
toolsdir = "tools"
daemondir = "daemon"
subdirs = os.listdir('./' + libdir)

for s in subdirs:
	subdirs[subdirs.index(s)] = os.getcwd() + "/" + libdir + "/" + s
	
subdirs += [os.getcwd() + "/" + toolsdir]
subdirs += [os.getcwd() + "/" + clientsdir]
subdirs += [os.getcwd() + "/" + daemondir]

def options(opt):

	opt.load('compiler_c')
	opt.load('compiler_cxx')

	opt.add_option('--optimize', action='store_true', default=False,
		help='Compile with -O3 rather than -g')	

	opt.add_option('--x11', action='store_true', default=False,
		help='Compile with support for X11 capture')
		
	opt.add_option('--wayland', action='store_true', default=False,
		help='Compile with support for Wayland output')		

def check_way(way):
	if way.options.wayland != False:
		print("Enabling Wayland!")
		way.env['KRAD_USE_WAYLAND'] = "yes"
		way.env.append_unique('CFLAGS', ['-DKRAD_USE_WAYLAND'])

def check_x11(x11):
	if x11.options.x11 != False:
		print("Enabling X11..")
		x11.env['KRAD_USE_X11'] = "yes"
		x11.env.append_unique('CFLAGS', ['-DKRAD_USE_X11'])
		
def get_git_ver(conf):
	try:
		gitver = check_output(["git", "describe", "--tags"])
		flag = '-DGIT_VERSION="' + gitver.decode('utf8').strip() + '"'
		conf.env.append_unique('CFLAGS', [flag])
		conf.env.append_unique('CXXLAGS', [flag])
	except OSError as e:
		print("Unable to get git tag/commit hash")

def configure(conf):

	platform = sys.platform
	conf.env['IS_MACOSX'] = platform == 'darwin'
	conf.env['IS_LINUX'] = platform == 'linux' or platform == 'linux2'

	if conf.env['IS_LINUX']:
		print("Linux detected :D")
		conf.env.append_unique('CFLAGS', ['-DIS_LINUX'])
		conf.env.append_unique('CXXFLAGS', ['-DIS_LINUX'])

	if conf.env['IS_MACOSX']:
		print("MacOS X detected :(")
		conf.env.append_unique('CFLAGS', ['-DIS_MACOSX'])
		conf.env.append_unique('CXXFLAGS', ['-DIS_MACOSX'])

	check_x11(conf)
	check_way(conf)
	get_git_ver(conf)

	conf.load('compiler_c')	
	conf.load('compiler_cxx')

	conf.link_add_flags

	conf.env.append_unique('CFLAGS', ['-fPIC'])

	if conf.options.optimize == False:
		conf.env.append_unique('CXXFLAGS', ['-g', '-Wall', '-Wno-write-strings'])
		conf.env.append_unique('CFLAGS', ['-g', '-Wall'])
	else:
		#conf.env.append_unique('CXXFLAGS', ['-O3', '-Wno-write-strings', '-march=core-avx-i', '-ffast-math', '-ftree-vectorizer-verbose=5'])
		#conf.env.append_unique('CFLAGS', ['-O3', '-march=core-avx-i', '-ffast-math', '-ftree-vectorizer-verbose=5'])
		conf.env.append_unique('CXXFLAGS', ['-O3', '-Wno-write-strings'])
		conf.env.append_unique('CFLAGS', ['-O3'])

	conf.recurse(subdirs, mandatory = False)
	
def build(bld):
	check_x11(bld)
	check_way(bld)
	bld.recurse(subdirs, mandatory = False)
	bld.add_post_fun(post)

def post(pst):
	if pst.cmd == 'install' and pst.env['IS_LINUX'] == True: 
		pst.exec_command('/sbin/ldconfig')
