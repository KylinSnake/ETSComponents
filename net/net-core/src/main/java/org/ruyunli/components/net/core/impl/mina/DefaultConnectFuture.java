package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.future.ConnectFuture;
import org.ruyunli.components.net.core.future.ConnectFutureInterface;

/**
 * Created by Roy on 2014/7/12.
 */
public class DefaultConnectFuture implements ConnectFutureInterface
{
    private ConnectFuture future;
    public DefaultConnectFuture(ConnectFuture f)
    {
        future = f;
    }
}
