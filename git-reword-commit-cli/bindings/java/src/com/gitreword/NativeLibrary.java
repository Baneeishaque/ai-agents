package com.gitreword;

/**
 * Helper class for loading the native library.
 */
public class NativeLibrary {
    private static boolean loaded = false;
    
    public static synchronized void load() {
        if (loaded) {
            return;
        }
        
        try {
            // Try to load from java.library.path
            System.loadLibrary("git_reword_jni");
            loaded = true;
        } catch (UnsatisfiedLinkError e) {
            // Try platform-specific names
            String os = System.getProperty("os.name").toLowerCase();
            String arch = System.getProperty("os.arch").toLowerCase();
            String libName;
            
            if (os.contains("win")) {
                libName = "git_reword_jni.dll";
            } else if (os.contains("mac")) {
                libName = "libgit_reword_jni.dylib";
            } else {
                libName = "libgit_reword_jni.so";
            }
            
            try {
                System.load(System.getProperty("user.dir") + "/" + libName);
                loaded = true;
            } catch (UnsatisfiedLinkError e2) {
                throw new RuntimeException("Failed to load native library: " + libName, e2);
            }
        }
    }
}
