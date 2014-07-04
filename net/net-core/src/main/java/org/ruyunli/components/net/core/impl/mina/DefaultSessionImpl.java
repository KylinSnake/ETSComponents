package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.session.IoSession;
import org.ruyunli.components.net.core.exception.SessionException;
import org.ruyunli.components.net.core.future.CloseFutureInterface;
import org.ruyunli.components.net.core.future.ReadFutureInterface;
import org.ruyunli.components.net.core.future.WriteFutureInterface;
import org.ruyunli.components.net.core.impl.SessionImplInterface;

/**
 * Created by Roy on 2014/7/1.
 */
public class DefaultSessionImpl implements SessionImplInterface
{
    private String name;
    private IoSession session;

    public DefaultSessionImpl(String name, IoSession session)
    {
        this.name = name;
        this.session = session;
    }

    public IoSession getSessionInternal()
    {
        return session;
    }

    @Override
    public String getSessionName() {
        return name;
    }

    @Override
    public CloseFutureInterface close(boolean immediate) {
        return null;
    }

    @Override
    public void write(String message, int timeout) throws SessionException {
    }

    @Override
    public String read(int timeout) throws SessionException {
        return null;
    }

    @Override
    public WriteFutureInterface asyncWrite(String message) {
        return null;
    }

    @Override
    public ReadFutureInterface asyncRead() {
        return null;
    }
}
