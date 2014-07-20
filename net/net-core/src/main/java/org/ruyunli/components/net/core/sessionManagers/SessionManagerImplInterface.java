package org.ruyunli.components.net.core.sessionManagers;

import org.ruyunli.components.net.core.exception.SessionException;
import org.ruyunli.components.net.core.impl.SessionImplInterface;

/**
 * Created by Roy on 2014/6/25.
 */
interface SessionManagerImplInterface
{
    public boolean registerSessionImpl(SessionImplInterface session, Class SessionClassType) throws SessionException;
    public boolean registerSessionImpl(SessionImplInterface session, String SessionClassName) throws SessionException;
    public void unregisterSessionImpl(String name) throws SessionException;
    public void onSessionOpen(String name) throws SessionException;
}
