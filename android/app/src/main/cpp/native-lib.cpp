#include "idriver.h"

#include <jni.h>
#include <string>

IDriver* driver = nullptr;

extern "C" JNIEXPORT void JNICALL
Java_com_davidjwalling_argo_MainActivity_start(JNIEnv* env, jobject)
{
    (void)env;
    driver = TheDriverPtr();
    if (driver) {
        driver->Start(0, NULL);
    }
}

extern "C" JNIEXPORT void JNICALL
Java_com_davidjwalling_argo_MainActivity_stop(JNIEnv* env, jobject)
{
    (void)env;
    if (driver) {
        driver->Stop();
    }
}