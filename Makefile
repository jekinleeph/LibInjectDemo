ROOT=C:/Progra~1/Android

ANDROID_NDK_ROOT=android-ndk-r10c

PREFIX=$(ROOT)/$(ANDROID_NDK_ROOT)/toolchains/arm-linux-androideabi-4.6/prebuilt/windows-x86_64/bin/arm-linux-androideabi-

SYSROOT=--sysroot=$(ROOT)/$(ANDROID_NDK_ROOT)/platforms/android-9/arch-arm/

CFLAGS = -g -Wall -c

LDFLAGS = -g -Wall

CC = $(PREFIX)gcc.exe $(SYSROOT)


###################################
INJECTSO =	inject

INJECT_DIR = Injectso

INJECT_SOURCES = $(wildcard $(INJECT_DIR)/*.c)

INJECT_OBJECTS = $(patsubst %.c,%.o,$(INJECT_SOURCES))

####################################

LIBTEST = libtest.so

LIBTEST_DIR = testLibrary

LIBTEST_SOURCES = $(wildcard $(LIBTEST_DIR)/*.c)

LIBTEST_OBJECTS = $(patsubst %.c,%.o,$(LIBTEST_SOURCES))

###################################

all:$(LIBTEST) $(INJECTSO)

$(INJECTSO):$(INJECT_OBJECTS)
	$(CC) $(LDFLAGS) -llog $(INJECT_DIR)/shellcode.s $(INJECT_OBJECTS) -o $@

$(INJECT_OBJECTS):%.o:%.c
	$(CC) $(CFLAGS) $< -I$(INJECT_DIR) -o $@

$(LIBTEST):$(LIBTEST_OBJECTS)
	$(CC) $(LDFLAGS) -shared -llog -fPIC $(LIBTEST_OBJECTS) -o $@
	
$(LIBTEST_OBJECTS):%.o:%.c
	$(CC) $(CFLAGS) $< -I$(LIBTEST_DIR) -o $@
	
.PHONY:all clean push

clean:
	-del $(INJECT_DIR)\*.o
	-del $(LIBTEST_DIR)\*.o
	-del $(LIBTEST)
	-del $(INJECTSO)

push:
	adb push $(INJECTSO) /data/local/tmp
	adb push $(LIBTEST) /data/local/tmp
