package org.ruyunli.components.filter.keys;

import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public abstract class AbstractKey<C>
{
    private Vector<C> components;
    protected AbstractKey(Vector<C> com)
    {
        components = com;
    }

    public Vector<C> getComponents()
    {
        return components;
    }

    @Override
    public boolean equals(Object o)
    {
        if(this == o)
        {
            return true;
        }
        if(o instanceof AbstractKey)
        {
            return ((AbstractKey) o).components.equals(components);
        }
        return false;
    }

    @Override
    public int hashCode()
    {
        return components.hashCode();
    }

}
