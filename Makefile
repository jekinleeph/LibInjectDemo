ROOT=$(HOME)/PATH/TO/YOUR/DIRECTOR

ANDROID_NDK_ROOT=android-ndk-r7

PREFIX=$(ROOT)/$(ANDROID_NDK_ROOT)/toolchains/arm-linux-androideabi-4.4.3/prebuilt/linux-x86/bin/arm-linux-androideabi-

SYSROOT=--sysroot=$(ROOT)/$(ANDROID_NDK_ROOT)/platforms/android-9/arch-arm/

CFLAGS := -g -Wall -c

LDFLAGS := -g -Wall

CC := $(PREFIX)gcc $(SYSROOT)

###################################
INJECTSO :=	inject

INJECT_DIR := Injectso

INJECT_SOURCES := $(wildcard $(INJECT_DIR)/*.c)

INJECT_OBJECTS := $(patsubst %.c,%.o,$(INJECT_SOURCES))

####################################

LIBTEST := libtest.so

LIBTEST_DIR := testLibrary

LIBTEST_SOURCES := $(wildcard $(LIBTEST_DIR)/*.c)

LIBTEST_OBJECTS := $(patsubst %.c,%.o,$(LIBTEST_SOURCES))

###################################

all:$(INJECTSO) $(LIBTEST)

$(INJECTSO):$(INJECT_OBJECTS)
	$(CC) $(LDFLAGS) $(INJECT_DIR)/shellcode.s $(INJECT_OBJECTS) -o $@

$(INJECT_OBJECTS):%.o:%.c
	$(CC) $(CFLAGS) $< -I$(INJECT_DIR) -o $@

$(LIBTEST):$(LIBTEST_OBJECTS)
	$(CC) $(LDFLAGS) -shared -llog -fPIC $(LIBTEST_OBJECTS) -o $@
	
$(LIBTEST_OBJECTS):%.o:%.c
	$(CC) $(CFLAGS) $< -I$(LIBTEST_DIR) -o $@
	
.PHONY:clean pushobj

clean:
	rm -f $(INJECT_DIR)/*.o
	rm -f $(LIBTEST_DIR)/*.o
	rm -f $(LIBTEST)
	rm -f $(INJECTSO)

pushobj:
	adb push $(INJECTSO) /data/data/
	adb push $(LIBTEST) /data/data/
