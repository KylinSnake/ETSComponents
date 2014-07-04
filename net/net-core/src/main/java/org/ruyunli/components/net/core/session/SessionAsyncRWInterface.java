package org.ruyunli.components.net.core.session;

import org.ruyunli.components.net.core.future.ReadFutureInterface;
import org.ruyunli.components.net.core.future.WriteFutureInterface;

/**
 * Created by Roy on 2014/6/25.
 */
public interface SessionAsyncRWInterface
{
    public WriteFutureInterface write(String message);
    public ReadFutureInterface read();
}
