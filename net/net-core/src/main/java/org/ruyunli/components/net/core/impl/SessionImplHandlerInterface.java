package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.SessionException;

/**
 * Created by Roy on 2014/7/1.
 */
public interface SessionImplHandlerInterface
{
    public void onCreated(SessionImplInterface session) throws SessionException;
    public void onOpened(SessionImplInterface session) throws Exception;
    public void onClosed(SessionImplInterface session) throws Exception;
    public void onExceptionCaught(SessionImplInterface session, Throwable cause) throws Exception;
    public void onMsgReceived(SessionImplInterface session, Object message) throws Exception;
    public void onMsgSent(SessionImplInterface session, Object message) throws Exception;
}
