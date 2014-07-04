package org.ruyunli.components.net.core.session;


import java.util.*;

/**
 * Created by Roy on 2014/6/25.
 */
public class AbstractSession
{
    private String name;
    private Set<SessionAsyncEventHandlerInterface> handlers;

    public AbstractSession(String name)
    {
        this.name = name;
        handlers = Collections.synchronizedSet(new HashSet<SessionAsyncEventHandlerInterface>());
    }

    public void addHandler(SessionAsyncEventHandlerInterface in)
    {
        handlers.add(in);
    }

    public void removeHandler(SessionAsyncEventHandlerInterface in)
    {
        handlers.remove(in);
    }

    public void clearHandlers()
    {
        handlers.clear();
    }

    public String getName()
    {
        return name;
    }
}
