APPINFO_JSON = {u'sdkVersion': u'3', u'projectType': u'native', u'uuid': u'7675abce-0f4f-4e40-91b5-576a420f2a3e', u'appKeys': {u'KEY_CONDITIONS': 1, u'KEY_TEMPERATURE': 0, u'KEY_MINUTES_TO': 3, u'KEY_SECONDS_STYLE': 4, u'KEY_HOURS_TO': 2}, u'companyName': u'martin.zoerfuss@kabelplus.at', u'enableMultiJS': True, u'targetPlatforms': [u'basalt', u'aplite'], u'capabilities': [u'location', u'configurable'], u'versionLabel': u'1.0', u'longName': u'Watchface', u'shortName': u'Watchface', u'watchapp': {u'watchface': True}, u'resources': {u'media': [{u'type': u'png', u'menuIcon': True, u'targetPlatforms': None, u'name': u'IMAGE_PREVIEW', u'file': u'images/PreviewImage'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_POPPINS_24', u'file': u'fonts/DateFont'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_POPPINS_8', u'file': u'fonts/ConnectionFont'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_ORBITRON_44', u'file': u'fonts/Orbitron-Regular.ttf'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_POPPINS_20', u'file': u'fonts/WeatherFont'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_ORBITRON_20', u'file': u'fonts/Orbitron-Regular.ttf'}]}}
AR = 'arm-none-eabi-gcc-ar'
ARFLAGS = 'rcs'
AS = 'arm-none-eabi-gcc'
BINDIR = '/usr/local/bin'
BUILD_DIR = 'aplite'
CC = ['arm-none-eabi-gcc']
CCLNK_SRC_F = []
CCLNK_TGT_F = ['-o']
CC_NAME = 'gcc'
CC_SRC_F = []
CC_TGT_F = ['-c', '-o']
CC_VERSION = ('4', '7', '2')
CFLAGS = ['-std=c99', '-mcpu=cortex-m3', '-mthumb', '-ffunction-sections', '-fdata-sections', '-g', '-fPIE', '-Os', '-D_TIME_H_', '-Wall', '-Wextra', '-Werror', '-Wno-unused-parameter', '-Wno-error=unused-function', '-Wno-error=unused-variable']
CFLAGS_MACBUNDLE = ['-fPIC']
CFLAGS_cshlib = ['-fPIC']
CPPPATH_ST = '-I%s'
DEFINES = ['RELEASE', 'PBL_PLATFORM_APLITE', 'PBL_BW', 'PBL_RECT', 'PBL_SDK_3']
DEFINES_ST = '-D%s'
DEST_BINFMT = 'elf'
DEST_CPU = 'arm'
DEST_OS = 'darwin'
INCLUDES = ['aplite']
LD = 'arm-none-eabi-ld'
LIBDIR = '/usr/local/lib'
LIBPATH_ST = '-L%s'
LIB_ST = '-l%s'
LINKFLAGS = ['-mcpu=cortex-m3', '-mthumb', '-Wl,--gc-sections', '-Wl,--warn-common', '-fPIE', '-Os']
LINKFLAGS_MACBUNDLE = ['-bundle', '-undefined', 'dynamic_lookup']
LINKFLAGS_cshlib = ['-shared']
LINKFLAGS_cstlib = ['-Wl,-Bstatic']
LINK_CC = ['arm-none-eabi-gcc']
PBW_BIN_DIR = 'aplite'
PBW_NAME = 'watchface.pbw'
PEBBLE_SDK_COMMON = '/Users/Martin/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/common'
PEBBLE_SDK_PLATFORM = '/Users/Martin/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble/aplite'
PEBBLE_SDK_ROOT = '/Users/Martin/Library/Application Support/Pebble SDK/SDKs/current/sdk-core/pebble'
PLATFORM = {'PBW_BIN_DIR': 'aplite', 'TAGS': ['aplite', 'bw', 'rect'], 'ADDITIONAL_TEXT_LINES_FOR_PEBBLE_H': [], 'MAX_APP_BINARY_SIZE': 65536, 'MAX_RESOURCES_SIZE': 524288, 'MAX_APP_MEMORY_SIZE': 24576, 'MAX_WORKER_MEMORY_SIZE': 10240, 'NAME': 'aplite', 'BUILD_DIR': 'aplite', 'MAX_RESOURCES_SIZE_APPSTORE_2_X': 98304, 'MAX_RESOURCES_SIZE_APPSTORE': 128000, 'DEFINES': ['PBL_PLATFORM_APLITE', 'PBL_BW', 'PBL_RECT']}
PLATFORM_NAME = 'aplite'
PREFIX = '/usr/local'
RESOURCES_JSON = [{u'type': u'png', u'menuIcon': True, u'targetPlatforms': None, u'name': u'IMAGE_PREVIEW', u'file': u'images/PreviewImage'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_POPPINS_24', u'file': u'fonts/DateFont'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_POPPINS_8', u'file': u'fonts/ConnectionFont'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_ORBITRON_44', u'file': u'fonts/Orbitron-Regular.ttf'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_POPPINS_20', u'file': u'fonts/WeatherFont'}, {u'targetPlatforms': None, u'type': u'font', u'name': u'FONT_ORBITRON_20', u'file': u'fonts/Orbitron-Regular.ttf'}]
RPATH_ST = '-Wl,-rpath,%s'
SDK_VERSION_MAJOR = 5
SDK_VERSION_MINOR = 78
SHLIB_MARKER = None
SIZE = 'arm-none-eabi-size'
SONAME_ST = '-Wl,-h,%s'
STLIBPATH_ST = '-L%s'
STLIB_MARKER = None
STLIB_ST = '-l%s'
TARGET_PLATFORMS = ['basalt', 'aplite']
TIMESTAMP = 1465210327
cprogram_PATTERN = '%s'
cshlib_PATTERN = 'lib%s.so'
cstlib_PATTERN = 'lib%s.a'
macbundle_PATTERN = '%s.bundle'
