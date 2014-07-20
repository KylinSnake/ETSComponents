package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.SessionException;
import org.ruyunli.components.net.core.future.CloseFutureInterface;
import org.ruyunli.components.net.core.future.ReadFutureInterface;
import org.ruyunli.components.net.core.future.WriteFutureInterface;

/**
 * Created by Roy on 2014/7/1.
 */
public interface SessionImplInterface
{
    public static final String NAME_ATTR = "sessionName";
    public String getSessionName();
    public CloseFutureInterface close(boolean immediate);
    public void write(String message, int timeout) throws SessionException;
    public String read(int timeout) throws SessionException;
    public WriteFutureInterface asyncWrite(String message);
    public ReadFutureInterface asyncRead();
}
