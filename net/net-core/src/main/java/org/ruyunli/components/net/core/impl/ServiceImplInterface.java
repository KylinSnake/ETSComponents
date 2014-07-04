package org.ruyunli.components.net.core.impl;

/**
 * Created by Roy on 2014/7/1.
 */
public interface ServiceImplInterface
{
    public static final int DEFAULT_TIMEOUT=3000;
    public int getTimeout();
    public void setTimeout(int timeout);
    public void setHandler(SessionImplHandlerInterface handler);
    public SessionImplHandlerInterface getHandler();
    public SessionImplInterface createDecoratedSessionImpl(SessionImplInterface session);
    public String getName();
}
