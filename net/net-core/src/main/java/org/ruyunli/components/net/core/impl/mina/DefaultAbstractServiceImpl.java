package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.service.IoHandler;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;
import org.ruyunli.components.net.core.impl.ServiceImplInterface;
import org.ruyunli.components.net.core.service.ServiceInterface;

/**
 * Created by Roy on 2014/7/11.
 */
public abstract class DefaultAbstractServiceImpl implements ServiceImplInterface, IoHandler
{
    private String name;
    private int timeout;
    private ServiceInterface server;
    public DefaultAbstractServiceImpl(String name, int timeout, ServiceInterface wrapper)
    {
        assert(wrapper != null);
        this.name = name;
        this.timeout = timeout;
        this.server = wrapper;
    }

    @Override
    public final int getTimeout() {
        return timeout;
    }

    @Override
    public final void setTimeout(int timeout) {
        this.timeout = timeout;
    }

    @Override
    public final String getName() {
        return name;
    }

    @Override
    public ServiceInterface getService() {
        return server;
    }

    @Override
    public void sessionCreated(IoSession session) throws Exception
    {
        server.onSessionCreated(new DefaultSessionImpl(getSessionName(session), session));
    }

    @Override
    public void sessionOpened(IoSession session) throws Exception
    {
        server.onSessionOpened(getSessionName(session));
    }

    @Override
    public void sessionClosed(IoSession session) throws Exception
    {
        server.onSessionClosed(getSessionName(session));
    }

    @Override
    public void sessionIdle(IoSession session, IdleStatus status) throws Exception
    {
    }

    @Override
    public void exceptionCaught(IoSession session, Throwable cause) throws Exception {

    }

    @Override
    public void messageReceived(IoSession session, Object message) throws Exception
    {
        server.onMsgReceived(getSessionName(session), message.toString());
    }

    @Override
    public void messageSent(IoSession session, Object message) throws Exception
    {
        server.onMsgSent(getSessionName(session),message.toString());
    }

    protected abstract String getSessionName(IoSession s);
}
