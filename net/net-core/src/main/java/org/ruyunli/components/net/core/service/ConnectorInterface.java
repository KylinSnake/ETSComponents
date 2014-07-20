package org.ruyunli.components.net.core.service;


import org.ruyunli.components.net.core.impl.ConnectorImplInterface;

/**
 * Created by Roy on 2014/7/11.
 */
public interface ConnectorInterface extends ServiceInterface
{
    public void setImpl(ConnectorImplInterface impl);
}
