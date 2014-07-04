package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.service.IoHandler;
import org.apache.mina.core.session.IdleStatus;
import org.apache.mina.core.session.IoSession;
import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.impl.ServiceImplInterface;
import org.ruyunli.components.net.core.impl.SessionImplInterface;

import java.net.InetSocketAddress;
import java.net.SocketAddress;

/**
 * Created by Roy on 2014/7/1.
 */
class DefaultIoHandler implements IoHandler
{
    private static final String NAME_ATTR = "sessionName";

    private ServiceImplInterface outer;
    public DefaultIoHandler(ServiceImplInterface outer)
    {
        this.outer = outer;
    }

    private String getSessionName(IoSession session)
    {
        SocketAddress address = session.getRemoteAddress();
        if (address instanceof InetSocketAddress)
        {
            InetSocketAddress socketAddress = (InetSocketAddress)address;
            return outer.getName() + "-" + socketAddress.getHostString()
                    + ":" + socketAddress.getPort();
        }
        return null;
    }


    @Override
    public void sessionCreated(IoSession session) throws Exception
    {
        assert(outer != null && outer.getHandler() != null);
        String sessionName = getSessionName(session);
        if (sessionName == null)
        {
            throw  new NetServiceException("Failed to get the session name for " + session.getId());
        }
        session.setAttribute(NAME_ATTR,sessionName);
        SessionImplInterface sessionImpl = outer.createDecoratedSessionImpl(new DefaultSessionImpl(sessionName, session));

        //TODO: Register the session to SessionMgr. If the sessionImpl is not there, we use the new one
        // Otherwise, we set the IoSession to to the sessionimpl. Of course, the setIoSession is the method in defaultSessionImpl
        outer.getHandler().onCreated(sessionImpl);
    }

    @Override
    public void sessionOpened(IoSession session) throws Exception
    {
        assert(outer != null && outer.getHandler() != null);
        String sessionName = (String) session.getAttribute(NAME_ATTR);
        if(sessionName == null || sessionName.isEmpty())
        {
            throw  new NetServiceException("Failed to get the session name for " + session.getId());
        }
        // TODO: in future, we will ask the Session for the session, instead of creating again
        outer.getHandler().onOpened(new DefaultSessionImpl(sessionName, session));
    }

    @Override
    public void sessionClosed(IoSession session) throws Exception
    {
        assert(outer != null && outer.getHandler() != null);
        String sessionName = (String) session.getAttribute(NAME_ATTR);
        if(sessionName == null || sessionName.isEmpty())
        {
            throw  new NetServiceException("Failed to get the session name for " + session.getId());
        }
        // TODO: in future, we will ask the Session for the session, instead of creating again
        outer.getHandler().onClosed(new DefaultSessionImpl(sessionName, session));
    }

    @Override
    public void sessionIdle(IoSession session, IdleStatus status) throws Exception {

    }

    @Override
    public void exceptionCaught(IoSession session, Throwable cause) throws Exception
    {
        assert(outer != null && outer.getHandler() != null);
        String sessionName = (String) session.getAttribute(NAME_ATTR);
        if(sessionName == null || sessionName.isEmpty())
        {
            throw  new NetServiceException("Failed to get the session name for " + session.getId());
        }
        // TODO: in future, we will ask the Session for the session, instead of creating again
        outer.getHandler().onExceptionCaught(new DefaultSessionImpl(sessionName, session), cause);
    }

    @Override
    public void messageReceived(IoSession session, Object message) throws Exception
    {
        assert(outer != null && outer.getHandler() != null);
        String sessionName = (String) session.getAttribute(NAME_ATTR);
        if(sessionName == null || sessionName.isEmpty())
        {
            throw  new NetServiceException("Failed to get the session name for " + session.getId());
        }
        // TODO: in future, we will ask the Session for the session, instead of creating again
        outer.getHandler().onMsgReceived(new DefaultSessionImpl(sessionName, session), message);
    }

    @Override
    public void messageSent(IoSession session, Object message) throws Exception
    {
        assert(outer != null && outer.getHandler() != null);
        String sessionName = (String) session.getAttribute(NAME_ATTR);
        if(sessionName == null || sessionName.isEmpty())
        {
            throw  new NetServiceException("Failed to get the session name for " + session.getId());
        }
        // TODO: in future, we will ask the Session for the session, instead of creating again
        outer.getHandler().onMsgSent(new DefaultSessionImpl(sessionName, session), message);
    }
}
