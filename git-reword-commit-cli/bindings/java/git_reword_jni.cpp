/**
 * @file git_reword_jni.cpp
 * @brief JNI bindings for git_reword library (Java/Kotlin)
 */

#include <jni.h>
#include <string>
#include "git_reword/reword_engine.hpp"
#include "git_reword/git_operations.hpp"
#include "git_reword/platform.hpp"

// Helper to convert Java string to C++ string
std::string jstring_to_string(JNIEnv* env, jstring jstr) {
    if (jstr == nullptr) return "";
    const char* chars = env->GetStringUTFChars(jstr, nullptr);
    std::string result(chars);
    env->ReleaseStringUTFChars(jstr, chars);
    return result;
}

// Helper to convert C++ string to Java string
jstring string_to_jstring(JNIEnv* env, const std::string& str) {
    return env->NewStringUTF(str.c_str());
}

extern "C" {

// Platform functions
JNIEXPORT jboolean JNICALL Java_com_gitreword_Platform_isWindows(JNIEnv*, jclass) {
    return git_reword::platform::is_windows();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_Platform_isMacOS(JNIEnv*, jclass) {
    return git_reword::platform::is_macos();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_Platform_isLinux(JNIEnv*, jclass) {
    return git_reword::platform::is_linux();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_Platform_isAndroid(JNIEnv*, jclass) {
    return git_reword::platform::is_android();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_Platform_isIOS(JNIEnv*, jclass) {
    return git_reword::platform::is_ios();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_Platform_isTermux(JNIEnv*, jclass) {
    return git_reword::platform::is_termux();
}

JNIEXPORT jstring JNICALL Java_com_gitreword_Platform_getPlatformName(JNIEnv* env, jclass) {
    return string_to_jstring(env, git_reword::platform::get_platform_name());
}

// RewordEngine class
JNIEXPORT jlong JNICALL Java_com_gitreword_RewordEngine_nativeCreate(JNIEnv* env, jclass, jstring repoPath) {
    std::string path = jstring_to_string(env, repoPath);
    auto* engine = new git_reword::RewordEngine(path);
    return reinterpret_cast<jlong>(engine);
}

JNIEXPORT void JNICALL Java_com_gitreword_RewordEngine_nativeDestroy(JNIEnv*, jclass, jlong handle) {
    auto* engine = reinterpret_cast<git_reword::RewordEngine*>(handle);
    delete engine;
}

JNIEXPORT jstring JNICALL Java_com_gitreword_RewordEngine_getVersion(JNIEnv* env, jclass) {
    return string_to_jstring(env, git_reword::RewordEngine::get_version());
}

JNIEXPORT jobject JNICALL Java_com_gitreword_RewordEngine_nativePreflightChecks(JNIEnv* env, jclass, jlong handle) {
    auto* engine = reinterpret_cast<git_reword::RewordEngine*>(handle);
    auto result = engine->preflight_checks();
    
    // Create RewordResult object
    jclass resultClass = env->FindClass("com/gitreword/RewordResult");
    jmethodID constructor = env->GetMethodID(resultClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    
    return env->NewObject(resultClass, constructor,
        static_cast<jint>(result.status),
        string_to_jstring(env, result.message),
        string_to_jstring(env, result.old_commit_hash),
        string_to_jstring(env, result.new_commit_hash),
        string_to_jstring(env, result.backup_tag));
}

JNIEXPORT jobject JNICALL Java_com_gitreword_RewordEngine_nativeRewordCommit(
    JNIEnv* env, jclass, jlong handle, jstring commitHash, jstring newMessage, jboolean keepBackup) {
    
    auto* engine = reinterpret_cast<git_reword::RewordEngine*>(handle);
    auto result = engine->reword_commit(
        jstring_to_string(env, commitHash),
        jstring_to_string(env, newMessage),
        keepBackup
    );
    
    // Create RewordResult object
    jclass resultClass = env->FindClass("com/gitreword/RewordResult");
    jmethodID constructor = env->GetMethodID(resultClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    
    return env->NewObject(resultClass, constructor,
        static_cast<jint>(result.status),
        string_to_jstring(env, result.message),
        string_to_jstring(env, result.old_commit_hash),
        string_to_jstring(env, result.new_commit_hash),
        string_to_jstring(env, result.backup_tag));
}

JNIEXPORT jobject JNICALL Java_com_gitreword_RewordEngine_nativeRollback(JNIEnv* env, jclass, jlong handle, jstring backupTag) {
    auto* engine = reinterpret_cast<git_reword::RewordEngine*>(handle);
    auto result = engine->rollback(jstring_to_string(env, backupTag));
    
    // Create RewordResult object
    jclass resultClass = env->FindClass("com/gitreword/RewordResult");
    jmethodID constructor = env->GetMethodID(resultClass, "<init>", "(ILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    
    return env->NewObject(resultClass, constructor,
        static_cast<jint>(result.status),
        string_to_jstring(env, result.message),
        string_to_jstring(env, result.old_commit_hash),
        string_to_jstring(env, result.new_commit_hash),
        string_to_jstring(env, result.backup_tag));
}

// GitOperations class
JNIEXPORT jlong JNICALL Java_com_gitreword_GitOperations_nativeCreate(JNIEnv* env, jclass, jstring repoPath) {
    std::string path = jstring_to_string(env, repoPath);
    auto* ops = new git_reword::GitOperations(path);
    return reinterpret_cast<jlong>(ops);
}

JNIEXPORT void JNICALL Java_com_gitreword_GitOperations_nativeDestroy(JNIEnv*, jclass, jlong handle) {
    auto* ops = reinterpret_cast<git_reword::GitOperations*>(handle);
    delete ops;
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_GitOperations_nativeIsGitAvailable(JNIEnv*, jclass, jlong handle) {
    auto* ops = reinterpret_cast<git_reword::GitOperations*>(handle);
    return ops->is_git_available();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_GitOperations_nativeIsGitRepository(JNIEnv*, jclass, jlong handle) {
    auto* ops = reinterpret_cast<git_reword::GitOperations*>(handle);
    return ops->is_git_repository();
}

JNIEXPORT jboolean JNICALL Java_com_gitreword_GitOperations_nativeIsWorkingDirectoryClean(JNIEnv*, jclass, jlong handle) {
    auto* ops = reinterpret_cast<git_reword::GitOperations*>(handle);
    return ops->is_working_directory_clean();
}

JNIEXPORT jstring JNICALL Java_com_gitreword_GitOperations_nativeGetHeadCommit(JNIEnv* env, jclass, jlong handle) {
    auto* ops = reinterpret_cast<git_reword::GitOperations*>(handle);
    auto head = ops->get_head_commit();
    if (head) {
        return string_to_jstring(env, *head);
    }
    return nullptr;
}

} // extern "C"
