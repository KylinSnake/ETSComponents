package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.service.ServiceInterface;

/**
 * Created by Roy on 2014/7/1.
 */
public interface ServiceImplInterface
{
    public static final int DEFAULT_TIMEOUT=3000;
    public int getTimeout();
    public void setTimeout(int timeout);
    public String getName();
    public ServiceInterface getService();
}
