package com.gitreword;

/**
 * Main engine for rewording git commits.
 */
public class RewordEngine implements AutoCloseable {
    private long nativeHandle;
    
    static {
        NativeLibrary.load();
    }
    
    public RewordEngine() {
        this(".");
    }
    
    public RewordEngine(String repoPath) {
        nativeHandle = nativeCreate(repoPath);
    }
    
    public RewordResult preflightChecks() {
        checkHandle();
        return nativePreflightChecks(nativeHandle);
    }
    
    public RewordResult rewordCommit(String commitHash, String newMessage) {
        return rewordCommit(commitHash, newMessage, false);
    }
    
    public RewordResult rewordCommit(String commitHash, String newMessage, boolean keepBackup) {
        checkHandle();
        return nativeRewordCommit(nativeHandle, commitHash, newMessage, keepBackup);
    }
    
    public RewordResult rollback(String backupTag) {
        checkHandle();
        return nativeRollback(nativeHandle, backupTag);
    }
    
    public static String getVersion() {
        return nativeGetVersion();
    }
    
    @Override
    public void close() {
        if (nativeHandle != 0) {
            nativeDestroy(nativeHandle);
            nativeHandle = 0;
        }
    }
    
    private void checkHandle() {
        if (nativeHandle == 0) {
            throw new IllegalStateException("RewordEngine has been closed");
        }
    }
    
    // Native methods
    private static native long nativeCreate(String repoPath);
    private static native void nativeDestroy(long handle);
    private static native String nativeGetVersion();
    private static native RewordResult nativePreflightChecks(long handle);
    private static native RewordResult nativeRewordCommit(long handle, String commitHash, String newMessage, boolean keepBackup);
    private static native RewordResult nativeRollback(long handle, String backupTag);
}
