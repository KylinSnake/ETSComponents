package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.service.IoAcceptor;
import org.apache.mina.transport.socket.nio.NioSocketAcceptor;
import org.ruyunli.components.net.core.impl.AcceptorImplInterface;
import org.ruyunli.components.net.core.impl.SessionImplHandlerAdapter;
import org.ruyunli.components.net.core.impl.SessionImplHandlerInterface;
import org.ruyunli.components.net.core.impl.SessionImplInterface;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by Roy on 2014/7/1.
 */
public class DefaultAcceptorImpl implements AcceptorImplInterface
{
    private int port;
    private int timeout;
    private String name;
    private SessionImplHandlerInterface handler;
    private IoAcceptor acceptor;
    private AtomicBoolean isStarted = new AtomicBoolean(false);

    DefaultAcceptorImpl(String name, int port, int timeout, IoAcceptor acceptor, SessionImplHandlerInterface handler)
    {
        this.name = name;
        this.port = port;
        this.timeout = timeout;
        this.handler = handler == null ? new SessionImplHandlerAdapter(): handler;

        // TODO: Here acceptor may need inject from factory, as we want to share with the same thread pool
        this.acceptor = acceptor;
    }
    @Override
    public int getPort() {
        return port;
    }

    @Override
    public int getTimeout() {
        return timeout;
    }

    @Override
    public void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    @Override
    public void setHandler(SessionImplHandlerInterface handler)
    {
        assert(handler != null);
        this.handler = handler;
    }

    @Override
    public SessionImplHandlerInterface getHandler() {
        return handler;
    }


    // TODO: for now, we only return session without any decoration
    // in future, we will add the decoration creation for PTCP session, MessageQueue sesson and so on
    @Override
    public SessionImplInterface createDecoratedSessionImpl(SessionImplInterface session) {
        return session;
    }

    @Override
    public String getName() {
        return name;
    }

    @Override
    public void start() throws IOException
    {
        assert(acceptor != null);
        if(isStarted.compareAndSet(false, true))
        {
            try {
                acceptor.bind(new InetSocketAddress(port));
            }
            catch(IOException e)
            {
                isStarted.set(false);
                throw e;
            }
        }
    }

    @Override
    public void stop()
    {
        assert(acceptor != null);
        if(isStarted.compareAndSet(true, false))
        {
                acceptor.unbind();
        }
    }
}


