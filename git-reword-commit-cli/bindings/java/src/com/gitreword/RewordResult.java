package com.gitreword;

/**
 * Result of a reword operation.
 */
public class RewordResult {
    private final RewordStatus status;
    private final String message;
    private final String oldCommitHash;
    private final String newCommitHash;
    private final String backupTag;
    
    public RewordResult(int statusValue, String message, String oldCommitHash, 
                        String newCommitHash, String backupTag) {
        this.status = RewordStatus.fromValue(statusValue);
        this.message = message;
        this.oldCommitHash = oldCommitHash;
        this.newCommitHash = newCommitHash;
        this.backupTag = backupTag;
    }
    
    public RewordStatus getStatus() {
        return status;
    }
    
    public String getMessage() {
        return message;
    }
    
    public String getOldCommitHash() {
        return oldCommitHash;
    }
    
    public String getNewCommitHash() {
        return newCommitHash;
    }
    
    public String getBackupTag() {
        return backupTag;
    }
    
    public boolean isSuccess() {
        return status == RewordStatus.SUCCESS;
    }
    
    @Override
    public String toString() {
        return "RewordResult{" +
                "status=" + status +
                ", message='" + message + '\'' +
                ", oldCommitHash='" + oldCommitHash + '\'' +
                ", newCommitHash='" + newCommitHash + '\'' +
                ", backupTag='" + backupTag + '\'' +
                '}';
    }
}
