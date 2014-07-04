package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.SessionException;

/**
 * Created by Roy on 2014/7/1.
 */
public class SessionImplHandlerAdapter implements SessionImplHandlerInterface
{
    @Override
    public void onCreated(SessionImplInterface session) throws SessionException {

    }

    @Override
    public void onOpened(SessionImplInterface session) throws Exception {

    }

    @Override
    public void onClosed(SessionImplInterface session) throws Exception {

    }

    @Override
    public void onExceptionCaught(SessionImplInterface session, Throwable cause) throws Exception {

    }

    @Override
    public void onMsgReceived(SessionImplInterface session, Object message) throws Exception {

    }

    @Override
    public void onMsgSent(SessionImplInterface session, Object message) throws Exception {

    }
}
