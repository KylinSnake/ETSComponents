package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.NetServiceException;

/**
 * Created by Roy on 2014/7/1.
 */
public interface ConnectorImplInterface extends ServiceImplInterface
{
    public String getRemoteHost();
    public int getRemotePort();
    public boolean connect() throws NetServiceException;
    public void disconnect() throws NetServiceException;
}
