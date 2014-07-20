package org.ruyunli.components.net.core.impl;

import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.service.AcceptorInterface;
import org.ruyunli.components.net.core.service.ConnectorInterface;

/**
 * Created by Roy on 2014/7/11.
 */
public abstract class AbstractNetImplFactory implements NetImplFactoryInterface
{

    @Override
    public AcceptorImplInterface createAcceptorImplInterface(String name, int port, AcceptorInterface acceptor)
            throws NetServiceException {
        return createAcceptorImplInterface(name, port, ServiceImplInterface.DEFAULT_TIMEOUT, acceptor);
    }

    @Override
    public ConnectorImplInterface createConnectorImplInterface(String name, String remoteHost, int remotePort, ConnectorInterface connector) throws NetServiceException {
        return createConnectorImplInterface(name, remoteHost, remotePort, ServiceImplInterface.DEFAULT_TIMEOUT, connector);
    }

    @Override
    public ConnectorImplInterface createConnectorImplInterface(String remoteHost, int remotePort,
                                                               ConnectorInterface connector) throws NetServiceException {
        return createConnectorImplInterface(remoteHost, remotePort, ServiceImplInterface.DEFAULT_TIMEOUT, connector);
    }

    @Override
    public ConnectorImplInterface createConnectorImplInterface(String remoteHost, int remotePort, int timeout,
                                                               ConnectorInterface connector) throws NetServiceException {
        return createConnectorImplInterface(remoteHost + ":" + remotePort, remoteHost, remotePort, timeout, connector) ;
    }
}
