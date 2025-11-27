package com.gitreword;

/**
 * Platform detection utilities for git-reword.
 */
public class Platform {
    static {
        NativeLibrary.load();
    }
    
    public static native boolean isWindows();
    public static native boolean isMacOS();
    public static native boolean isLinux();
    public static native boolean isAndroid();
    public static native boolean isIOS();
    public static native boolean isTermux();
    public static native String getPlatformName();
}
