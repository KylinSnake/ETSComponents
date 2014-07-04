package org.ruyunli.components.net.core.impl;

import java.io.IOException;

/**
 * Created by Roy on 2014/7/1.
 */
public interface AcceptorImplInterface extends ServiceImplInterface
{
    public int getPort();
    public void start() throws IOException;
    public void stop();
}
