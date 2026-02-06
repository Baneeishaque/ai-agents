package com.gitreword

/**
 * Kotlin extensions for git-reword library.
 * Uses the same JNI bindings as Java.
 */

/**
 * DSL for performing reword operations.
 */
fun rewordCommit(repoPath: String = ".", block: RewordBuilder.() -> Unit): RewordResult {
    val builder = RewordBuilder()
    builder.block()
    
    return RewordEngine(repoPath).use { engine ->
        engine.rewordCommit(
            builder.commitHash ?: throw IllegalArgumentException("commitHash is required"),
            builder.newMessage ?: throw IllegalArgumentException("newMessage is required"),
            builder.keepBackup
        )
    }
}

class RewordBuilder {
    var commitHash: String? = null
    var newMessage: String? = null
    var keepBackup: Boolean = false
}

/**
 * Extension property to check if result was successful.
 */
val RewordResult.isSuccessful: Boolean
    get() = this.status == RewordStatus.SUCCESS

/**
 * Kotlin wrapper for RewordEngine with resource management.
 */
inline fun <R> withRewordEngine(repoPath: String = ".", block: (RewordEngine) -> R): R {
    return RewordEngine(repoPath).use(block)
}

/**
 * Kotlin wrapper for GitOperations with resource management.
 */
inline fun <R> withGitOperations(repoPath: String = ".", block: (GitOperations) -> R): R {
    return GitOperations(repoPath).use(block)
}

/**
 * Platform utilities as Kotlin object.
 */
object PlatformInfo {
    val isWindows: Boolean get() = Platform.isWindows()
    val isMacOS: Boolean get() = Platform.isMacOS()
    val isLinux: Boolean get() = Platform.isLinux()
    val isAndroid: Boolean get() = Platform.isAndroid()
    val isIOS: Boolean get() = Platform.isIOS()
    val isTermux: Boolean get() = Platform.isTermux()
    val platformName: String get() = Platform.getPlatformName()
}
