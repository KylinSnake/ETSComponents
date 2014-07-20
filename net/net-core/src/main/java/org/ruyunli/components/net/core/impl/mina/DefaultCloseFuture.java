package org.ruyunli.components.net.core.impl.mina;

import org.apache.mina.core.future.CloseFuture;
import org.ruyunli.components.net.core.future.CloseFutureInterface;

/**
 * Created by Roy on 2014/7/12.
 */
public class DefaultCloseFuture implements CloseFutureInterface
{
    public final static CloseFutureInterface CLOSED = new CloseFutureInterface(){

    };
    private CloseFuture future;
    public DefaultCloseFuture(CloseFuture f)
    {
        this.future = f;
    }
}
