package org.ruyunli.components.net.core.session;

import org.apache.mina.core.service.IoServiceListener;
import org.apache.mina.core.session.IoSession;
import org.apache.mina.transport.socket.DatagramAcceptor;
import org.apache.mina.transport.socket.DefaultDatagramSessionConfig;
import org.apache.mina.transport.socket.SocketAcceptor;
import org.apache.mina.transport.socket.nio.NioSession;

import java.nio.channels.Channel;

/**
 * Created by Roy on 2014/6/4.
 */
public class SessionBase
{
    private IoSession session;
    private String name;
    public SessionBase (IoSession s, String n)
    {
        session = s;
        name = n;
    }

    IoSession getSession()
    {
        return session;
    }

    public String getName()
    {
        return name;
    }
}
