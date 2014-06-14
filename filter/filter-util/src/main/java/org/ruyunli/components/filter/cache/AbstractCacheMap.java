package org.ruyunli.components.filter.cache;

import org.ruyunli.components.filter.keys.AbstractKey;
import org.ruyunli.components.filter.keys.KeyQueryInterface;

import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Created by Roy on 2014/6/11.
 */
public abstract class AbstractCacheMap<C, K extends AbstractKey<C>, V>
    implements CacheMapInterface<C, K ,V>
{
    private Map<K, V> map;
    public AbstractCacheMap(Map<K, V> m)
    {
        map = m;
        if(map == null)
        {
            map = new ConcurrentHashMap<K, V>();
        }
    }

    @Override
    public Map<K, V> query(KeyQueryInterface<C, K> queryInterface)
    {
        HashMap<K, V> ret = new HashMap<K, V>();
        for(K k : queryInterface.match(map.keySet()))
        {
            ret.put(k, map.get(k));
        }
        return ret;
    }
    @Override
    public V get(K k)
    {
        return map.get(k);
    }

    @Override
    public V put(K k, V v)
    {
        return map.put(k, v);
    }

    @Override
    public V insert(K k, V v)
    {
        return map.putIfAbsent(k, v);
    }

    @Override
    public V remove(K k)
    {
        return map.remove(k);
    }

    @Override
    public Set<K> keySet()
    {
        return map.keySet();
    }

    @Override
    public boolean contains(K k)
    {
        return map.containsKey(k);
    }

    @Override
    public void clear()
    {
        map.clear();
    }
}
