package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.service.IoAcceptor;
import org.apache.mina.core.service.IoHandler;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;
import org.ruyunli.components.net.core.impl.AcceptorImplInterface;
import org.ruyunli.components.net.core.impl.SessionImplInterface;
import org.ruyunli.components.net.core.service.AcceptorInterface;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.util.concurrent.atomic.AtomicBoolean;

/**
 * Created by Roy on 2014/7/1.
 */
public class DefaultAcceptorImpl extends DefaultAbstractServiceImpl
        implements AcceptorImplInterface, IoHandler
{
    private int port;
    private IoAcceptor acceptor;
    private AtomicBoolean isStarted = new AtomicBoolean(false);

    DefaultAcceptorImpl(String name, int port, int timeout, IoAcceptor acceptor, AcceptorInterface server)
    {
        super(name,timeout,server);
        this.port = port;
        // TODO: Here acceptor may need inject from factory, as we want to share with the same thread pool
        this.acceptor = acceptor;
    }
    @Override
    public int getPort() {
        return port;
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

    @Override
    protected String getSessionName(IoSession session)
    {
        assert(session != null);
        String name = (String)session.getAttribute(SessionImplInterface.NAME_ATTR);
        if(name == null)
        {
            InetSocketAddress address = (InetSocketAddress)session.getRemoteAddress();
            assert(address != null);
            name = getName() + "-" + address.getHostString() + ":" + address.getPort();
            session.setAttribute(SessionImplInterface.NAME_ATTR, name);
        }
        return name;
    }
}


