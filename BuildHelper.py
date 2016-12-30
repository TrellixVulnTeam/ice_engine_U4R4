import subprocess, sys, os
import platform
import multiprocessing

from SCons.Script import *

### Establish our system
isLinux = platform.system() == 'Linux'
isWindows = os.name == 'nt'
isMac = platform.system() == 'Darwin'

buildFlags = {}
buildFlags['debug'] = True # True by default (at least for now)
buildFlags['release'] = False
buildFlags['clean'] = False
buildFlags['build'] = 'debug'
buildFlags['compiler'] = 'default'
buildFlags['num_jobs'] = multiprocessing.cpu_count()

dependenciesDirectory = 'deps/'

cpp_paths = []
cpp_defines = []
cpp_flags = []
link_flags = []
library_paths = []
libraries = []



def setup(ARGUMENTS):	
	### Error check our platform type
	if (not isLinux and not isWindows and not isMac):
		print("Sorry, but it appears your platform is not recognized.")
		sys.exit(1)
	
	AddOption('--build', dest='build', type='string', nargs=1, action='store', help='Set the build to compile:  release, debug (default), and release-with-debug')
	AddOption('--compiler', dest='compiler', type='string', nargs=1, action='store', help='Set the compiler to use.')
	
	global buildFlags
	
	### Set and error check our build flags
	if (GetOption('clean') is True):
		buildFlags['clean'] = True
	if (GetOption('build') is not None):
		buildFlags['build'] = GetOption('build')
		if (buildFlags['build'] != 'release' and buildFlags['build'] != 'debug' and buildFlags['build'] != 'release-with-debug'):
			print( "Invalid build: '{0}'".format(buildFlags['build']) )
			sys.exit(1)
	if (GetOption('compiler') is not None):
		buildFlags['compiler'] = GetOption('compiler')

	# TODO: num_jobs seems to default to 1 - how can I know if the user is actually setting this value?
	if (GetOption('num_jobs') > 1):
		buildFlags['num_jobs'] = GetOption('num_jobs')
	
	# Override - useful for limiting the number of cpus used for CI builds (i.e. travis ci)
	if (os.environ.get('SCONS_NUM_JOBS')):
		buildFlags['num_jobs'] = os.environ.get('SCONS_NUM_JOBS')
	
	if (buildFlags['build'] == 'debug' or buildFlags['build'] == 'release-with-debug'):
		pass
		#cpp_defines.append('DEBUG')
	else:
		buildFlags['debug'] = False
	
	if (buildFlags['build'] == 'release' or buildFlags['build'] == 'release-with-debug'):
		buildFlags['release'] = True
	
	if (buildFlags['compiler'] is None or buildFlags['compiler'] == ''):
		buildFlags['compiler'] = 'default'
	if (buildFlags['compiler'] == 'gcc' and isWindows):
		buildFlags['compiler'] = 'mingw'
	if (buildFlags['compiler'] == 'msvc' and isWindows):
		buildFlags['compiler'] = 'default'
	
	### Error check compiler
	if (buildFlags['compiler'] == 'msvc' and not isWindows):
		print( "Cannot use msvc in this environment!" )
		sys.exit(1)
	
	### Set our compiler variables
	# TODO: Properly include header files

	includeDirectories = []
	includeDirectories.append('freeimage/include')
	includeDirectories.append('boost/include')
	includeDirectories.append('bullet/include')
	includeDirectories.append('sdl/include')
	includeDirectories.append('assimp/include')
	includeDirectories.append('glm/include')
	includeDirectories.append('glew/include')
	#includeDirectories.append('sfml/include')
	includeDirectories.append('angelscript/include')
	includeDirectories.append('entityx/include')
	includeDirectories.append('threadpool11/include')
	includeDirectories.append('sqlite3/include')
	
	# For dark horizon compilation
	cpp_paths.append('include')
	cpp_paths.append('src')
	for d in includeDirectories:
		cpp_paths.append(dependenciesDirectory + d)
	
	### Set our OS specific compiler variables
	if (not isWindows):
		if (buildFlags['compiler'] == 'gcc' or (buildFlags['compiler'] == 'default' and isLinux)):
			if (buildFlags['debug']):
				cpp_flags.append('-g')
				cpp_flags.append('-pg') # profiler
				if (not buildFlags['release']):
					cpp_flags.append('-O0') # optimization level 0
					cpp_defines.append('DEBUG')
			
			if (buildFlags['release']):
				cpp_flags.append('-O3') # optimization level 3
				cpp_defines.append('NDEBUG')
			
			cpp_flags.append('-std=c++11')
			cpp_flags.append('-pedantic-errors')
			#cpp_flags.append('-Wall')
			#cpp_flags.append('-Werror')
			
		# Dynamically link to boost log
		cpp_defines.append('BOOST_LOG_DYN_LINK')
		
		# For some reason, on windows we need to use boost::phoenix version 3 with boost::log
		cpp_defines.append('BOOST_SPIRIT_USE_PHOENIX_V3')
	else:
		if isWindows:
			if (buildFlags['compiler'] == 'default'):
				cpp_flags.append('/w') # disables warnings (Windows)
				cpp_flags.append('/wd4350') # disables the specific warning C4350
				cpp_flags.append('/EHsc') # Enable 'unwind semantics' for exception handling (Windows)
				cpp_flags.append('/MD')
				
				if (buildFlags['debug']):
					cpp_flags.append('/Zi') # Produces a program database (PDB) that contains type information and symbolic debugging information for use with the debugger.
					cpp_flags.append('/FS') # Allows multiple cl.exe processes to write to the same .pdb file
					link_flags.append('/DEBUG') # Enable debug during linking
					if (not buildFlags['release']):
						cpp_flags.append('/Od') # Disables optimization
						cpp_defines.append('DEBUG')
				
				if (buildFlags['release']):
					cpp_flags.append('/Ox') # Full optimization
					cpp_defines.append('NDEBUG')
			elif (buildFlags['compiler'] == 'mingw'):
				if (buildFlags['debug']):
					cpp_flags.append('-g')
					cpp_flags.append('-pg') # profiler
					if (not buildFlags['release']):
						cpp_flags.append('-O0') # optimization level 0
						cpp_defines.append('DEBUG')
				
				if (buildFlags['release']):
					cpp_flags.append('-O3') # optimization level 3
					cpp_defines.append('NDEBUG')
				
				cpp_flags.append('-std=c++11')
				cpp_flags.append('-pedantic-errors')
				#cpp_flags.append('-Wall')
				#cpp_flags.append('-Werror')
			
			# For some reason, on windows we need to use boost::phoenix version 3 with boost::log
			cpp_defines.append('BOOST_SPIRIT_USE_PHOENIX_V3')
		
def clear():
	if (isWindows):
		os.system('cls')
	else:
		os.system('clear')

def exitOnError(returnCode):
	if ( returnCode != 0):
		print( "Script halted due to error(s)!" )
		sys.exit(1)
