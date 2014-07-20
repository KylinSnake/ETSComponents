package org.ruyunli.components.net.core.service;

import org.ruyunli.components.net.core.impl.AcceptorImplInterface;

/**
 * Created by Roy on 2014/7/11.
 */
public interface AcceptorInterface extends ServiceInterface
{
    public void setImpl(AcceptorImplInterface impl);
}
