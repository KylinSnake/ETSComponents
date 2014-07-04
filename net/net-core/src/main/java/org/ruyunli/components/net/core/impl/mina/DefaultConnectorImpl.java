package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.service.IoConnector;
import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.impl.ConnectorImplInterface;
import org.ruyunli.components.net.core.impl.SessionImplHandlerAdapter;
import org.ruyunli.components.net.core.impl.SessionImplHandlerInterface;
import org.ruyunli.components.net.core.impl.SessionImplInterface;

import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by Roy on 2014/7/1.
 */
public class DefaultConnectorImpl implements ConnectorImplInterface
{
    private IoConnector connector;
    private int port;
    private String host;
    private int timeout;
    private String name;
    private SessionImplHandlerInterface handler;
    private String sessionName;
    private AtomicBoolean isConnected = new AtomicBoolean(false);

    DefaultConnectorImpl(String name, String host, int port, int timeout, IoConnector connector, SessionImplHandlerInterface handler)
    {
        this.name = name;
        this.host = host;
        this.port = port;
        this.timeout = timeout;
        this.handler = handler == null ? new SessionImplHandlerAdapter(): handler;

        // TODO: Here connector may need inject from factory, as we want to share with the same thread pool
        this.connector = connector;
    }

    @Override
    public String getRemoteHost() {
        return host;
    }

    @Override
    public int getRemotePort() {
        return port;
    }

    @Override
    public boolean connect() throws NetServiceException {
        assert(connector != null);
        if (isConnected.compareAndSet(false, true))
        {

        }

        return false;
    }

    @Override
    public void disconnect() throws NetServiceException {
        if (isConnected.compareAndSet(true, false)) {
            // use the session name to get the session, and close it
        }
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
    public void setHandler(SessionImplHandlerInterface handler) {
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
}
