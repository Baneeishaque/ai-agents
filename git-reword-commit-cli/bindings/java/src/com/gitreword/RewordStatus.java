package com.gitreword;

/**
 * Status codes for reword operations.
 */
public enum RewordStatus {
    SUCCESS(0),
    GIT_NOT_AVAILABLE(1),
    GIT_LFS_NOT_AVAILABLE(2),
    NOT_A_GIT_REPOSITORY(3),
    WORKING_DIRECTORY_DIRTY(4),
    REBASE_IN_PROGRESS(5),
    COMMIT_NOT_FOUND(6),
    REBASE_FAILED(7),
    AMEND_FAILED(8),
    ROLLBACK_FAILED(9),
    UNKNOWN_ERROR(10);
    
    private final int value;
    
    RewordStatus(int value) {
        this.value = value;
    }
    
    public int getValue() {
        return value;
    }
    
    public static RewordStatus fromValue(int value) {
        for (RewordStatus status : values()) {
            if (status.value == value) {
                return status;
            }
        }
        return UNKNOWN_ERROR;
    }
}
