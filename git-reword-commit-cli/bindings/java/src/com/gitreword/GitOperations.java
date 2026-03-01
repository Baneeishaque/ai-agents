package com.gitreword;

/**
 * Git operations wrapper.
 */
public class GitOperations implements AutoCloseable {
    private long nativeHandle;
    
    static {
        NativeLibrary.load();
    }
    
    public GitOperations() {
        this(".");
    }
    
    public GitOperations(String repoPath) {
        nativeHandle = nativeCreate(repoPath);
    }
    
    public boolean isGitAvailable() {
        checkHandle();
        return nativeIsGitAvailable(nativeHandle);
    }
    
    public boolean isGitRepository() {
        checkHandle();
        return nativeIsGitRepository(nativeHandle);
    }
    
    public boolean isWorkingDirectoryClean() {
        checkHandle();
        return nativeIsWorkingDirectoryClean(nativeHandle);
    }
    
    public String getHeadCommit() {
        checkHandle();
        return nativeGetHeadCommit(nativeHandle);
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
            throw new IllegalStateException("GitOperations has been closed");
        }
    }
    
    // Native methods
    private static native long nativeCreate(String repoPath);
    private static native void nativeDestroy(long handle);
    private static native boolean nativeIsGitAvailable(long handle);
    private static native boolean nativeIsGitRepository(long handle);
    private static native boolean nativeIsWorkingDirectoryClean(long handle);
    private static native String nativeGetHeadCommit(long handle);
}
