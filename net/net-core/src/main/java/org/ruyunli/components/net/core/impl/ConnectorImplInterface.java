package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.future.CloseFutureInterface;
import org.ruyunli.components.net.core.future.ConnectFutureInterface;

/**
 * Created by Roy on 2014/7/1.
 */
public interface ConnectorImplInterface extends ServiceImplInterface
{
    public String getRemoteHost();
    public int getRemotePort();
    public ConnectFutureInterface connect() throws NetServiceException;
    public CloseFutureInterface disconnect();
}
