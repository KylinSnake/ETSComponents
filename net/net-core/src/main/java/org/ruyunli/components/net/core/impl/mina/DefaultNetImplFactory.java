package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.service.IoProcessor;
import org.apache.mina.filter.codec.ProtocolCodecFilter;
import org.apache.mina.filter.codec.textline.TextLineCodecFactory;
import org.apache.mina.transport.socket.nio.NioProcessor;
import org.apache.mina.transport.socket.nio.NioSession;
import org.apache.mina.transport.socket.nio.NioSocketAcceptor;
import org.apache.mina.transport.socket.nio.NioSocketConnector;
import org.ruyunli.components.net.core.exception.NetServiceException;
import org.ruyunli.components.net.core.impl.AbstractNetImplFactory;
import org.ruyunli.components.net.core.impl.AcceptorImplInterface;
import org.ruyunli.components.net.core.impl.ConnectorImplInterface;
import org.ruyunli.components.net.core.service.AcceptorInterface;
import org.ruyunli.components.net.core.service.ConnectorInterface;

import java.nio.charset.Charset;
import java.util.concurrent.Executor;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by Roy on 2014/7/11.
 */
public class DefaultNetImplFactory extends AbstractNetImplFactory
{
    private Executor executor;
    private IoProcessor<NioSession> processor;
    private String charFormat = "ASCII";

    public void setCharFormat(String s)
    {
        charFormat = s;
    }

    public String getCharFormat()
    {
        return charFormat;
    }

    public DefaultNetImplFactory()
    {
        this(0);
    }

    public DefaultNetImplFactory(int threadNum)
    {
        if(threadNum <= 0)
        {
            executor = Executors.newCachedThreadPool();
        }
        else
        {
            executor = Executors.newFixedThreadPool(threadNum);
        }
        processor = new NioProcessor(executor);
    }

    @Override
    public AcceptorImplInterface createAcceptorImplInterface(String name, int port, int timeout,
                                                             AcceptorInterface acceptor) throws NetServiceException
    {
        NioSocketAcceptor acp = new NioSocketAcceptor(executor, processor);
        DefaultAcceptorImpl result = new DefaultAcceptorImpl(name, port, timeout, acp, acceptor);
        acceptor.setImpl(result);
        acp.setHandler(result);
        acp.getFilterChain().addLast( "codec", new ProtocolCodecFilter( new TextLineCodecFactory( Charset.forName(charFormat))));
        return result;
    }

    @Override
    public ConnectorImplInterface createConnectorImplInterface(String name, String remoteHost,
                                                               int remotePort, int timeout, ConnectorInterface connector) throws NetServiceException
    {
        NioSocketConnector conn = new NioSocketConnector(executor, processor);
        DefaultConnectorImpl result = new DefaultConnectorImpl(name, remoteHost, remotePort,timeout, conn, connector);
        connector.setImpl(result);
        conn.setHandler(result);
        conn.getFilterChain().addLast( "codec", new ProtocolCodecFilter( new TextLineCodecFactory( Charset.forName(charFormat))));
        return result;
    }
}
