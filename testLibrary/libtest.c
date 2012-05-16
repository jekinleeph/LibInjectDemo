
#include <android/log.h>

void test(const char* s) {
	__android_log_print(ANDROID_LOG_VERBOSE, "injectso", s);
}
