package org.ruyunli.components.net.core.service;

import org.ruyunli.components.net.core.impl.SessionImplInterface;

/**
 * Created by Roy on 2014/7/11.
 */
public interface ServiceInterface
{
    public void onSessionCreated(SessionImplInterface impl);
    public void onSessionOpened(String name);
    public void onSessionClosed(String name);
    public void onMsgReceived(String name, String message);
    public void onMsgSent(String name, String message);
}
