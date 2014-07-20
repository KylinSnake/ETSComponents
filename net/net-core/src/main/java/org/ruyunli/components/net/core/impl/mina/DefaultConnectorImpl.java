package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.future.ConnectFuture;
import org.apache.mina.core.service.IoConnector;
import org.apache.mina.core.service.IoHandler;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;
import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.future.CloseFutureInterface;
import org.ruyunli.components.net.core.future.ConnectFutureInterface;
import org.ruyunli.components.net.core.impl.ConnectorImplInterface;
import org.ruyunli.components.net.core.service.ConnectorInterface;

import java.net.InetSocketAddress;
import java.util.concurrent.Future;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by Roy on 2014/7/1.
 */
public class DefaultConnectorImpl extends DefaultAbstractServiceImpl
        implements ConnectorImplInterface
{
    private IoConnector connector;
    private int remotePort;
    private String remoteHost;
    private String sessionName;
    private AtomicBoolean isActive;
    private IoSession session;

    DefaultConnectorImpl(String name, String remoteHost, int remotePort,
                         int timeout, IoConnector connector, ConnectorInterface server)
    {
        super(name, timeout, server);
        this.remoteHost = remoteHost;
        this.remotePort = remotePort;
        this.connector = connector;
        isActive = new AtomicBoolean(this.connector.isActive());
        sessionName = name + "-" + remoteHost + ":" + remotePort;
        session = null;
    }

    @Override
    public String getRemoteHost() {
        return remoteHost;
    }

    @Override
    public int getRemotePort() {
        return remotePort;
    }

    @Override
    public ConnectFutureInterface connect() throws NetServiceException {
        assert(connector != null);
        return new  DefaultConnectFuture(connector.connect(new InetSocketAddress(remoteHost, remotePort)));
    }

    @Override
    public CloseFutureInterface disconnect()
    {
        if (isActive.compareAndSet(true, false))
        {
            if(session != null)
            {
                DefaultCloseFuture result =  new DefaultCloseFuture(session.close(true));
                session = null;
                return result;
            }
        }
        return DefaultCloseFuture.CLOSED;
    }

    @Override
    public void sessionClosed(IoSession session) throws Exception
    {
        super.sessionClosed(session);
        if (isActive.compareAndSet(true, false))
        {
            if(this.session != null && session == this.session)
            {
                this.session = null;
            }
        }
    }

    @Override
    public void sessionCreated(IoSession session) throws Exception
    {
        super.sessionCreated(session);
        if (isActive.compareAndSet(false, true))
        {
            if(session != null)
            {
                this.session = session;
            }
        }
    }

    @Override
    protected String getSessionName(IoSession s) {
        return sessionName;
    }
}
