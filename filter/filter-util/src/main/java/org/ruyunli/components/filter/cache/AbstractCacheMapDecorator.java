package org.ruyunli.components.filter.cache;

import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.keys.AbstractKey;
import org.ruyunli.components.filter.keys.KeyQueryInterface;

import java.util.Map;
import java.util.Set;

/**
 * Created by Roy on 2014/6/11.
 */
public abstract class AbstractCacheMapDecorator<C, K extends AbstractKey<C>, V>
        implements CacheMapInterface<C, K, V>
{
    private CacheMapInterface<C,K,V> impl;

    public AbstractCacheMapDecorator(CacheMapInterface<C,K,V> i)
    {
        assert(i != null);
        impl = i;
    }

    public CacheMapInterface<C, K ,V> getImpl()
    {
        return impl;
    }

    @Override
    public Map<K, V> query(KeyQueryInterface<C, K> queryInterface)
    {
        return impl.query(queryInterface);
    }

    @Override
    public V get(K k)
    {
        return impl.get(k);
    }

    @Override
    public V put(K k, V v)
    {
        return impl.put(k,v);
    }

    @Override
    public V insert(K k, V v)
    {
        return impl.insert(k,v);
    }

    @Override
    public V remove(K k)
    {
        return impl.remove(k);
    }

    @Override
    public Set<K> keySet()
    {
        return impl.keySet();
    }

    @Override
    public boolean contains(K k)
    {
        return impl.contains(k);
    }

    @Override
    public void clear()
    {
        impl.clear();
    }

    @Override
    public K generateKeyFromString(String s) throws KeyParseException
    {
        return impl.generateKeyFromString(s);
    }
}
