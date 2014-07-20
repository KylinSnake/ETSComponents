package org.ruyunli.components.net.core.sessionManagers;

/**
 * Created by Roy on 2014/7/6.
 */
public final class SessionManager
{
    private static SessionManager instance;

    private SessionManagerImplInterface impl;

    static
    {
        instance = new SessionManager();
    }

    private SessionManager()
    {
    }

    void setImpl(SessionManagerImplInterface impl)
    {
        this.impl = impl;
    }

    SessionManagerImplInterface getImpl()
    {
        return impl;
    }
}
