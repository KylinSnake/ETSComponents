package org.ruyunli.components.net.core.session;

import org.ruyunli.components.net.core.exception.SessionException;

/**
 * Created by Roy on 2014/6/25.
 */
public interface SessionSyncRWInterface
{
    public void write(String message, int timeout, int retry) throws SessionException;
    public void write(String message, int timeout) throws SessionException;
    public void write(String message) throws SessionException;
    public String read(int timeout, int retry) throws SessionException;
    public String read(int timeout) throws SessionException;
    public String read() throws SessionException;
}
