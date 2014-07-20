package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.service.AcceptorInterface;
import org.ruyunli.components.net.core.service.ConnectorInterface;

/**
 * Created by Roy on 2014/7/6.
 */
public interface NetImplFactoryInterface
{
    AcceptorImplInterface createAcceptorImplInterface(String name, int port, AcceptorInterface acceptor) throws NetServiceException;
    AcceptorImplInterface createAcceptorImplInterface(String name, int port, int timeout, AcceptorInterface acceptor) throws NetServiceException;
    ConnectorImplInterface createConnectorImplInterface(String name, String remoteHost, int remotePort, ConnectorInterface connector) throws NetServiceException;
    ConnectorImplInterface createConnectorImplInterface(String remoteHost, int remotePort, ConnectorInterface connector) throws NetServiceException;
    ConnectorImplInterface createConnectorImplInterface(String name, String remoteHost, int remotePort, int timeout, ConnectorInterface connector) throws NetServiceException;
    ConnectorImplInterface createConnectorImplInterface(String remoteHost, int remotePort, int timeout, ConnectorInterface connector) throws NetServiceException;
}
