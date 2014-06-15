package org.ruyunli.components.filter.map;

import org.ruyunli.components.filter.index.IndexNode;
import org.ruyunli.components.filter.keys.AbstractKey;
import org.ruyunli.components.filter.keys.KeyQueryInterface;
import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

/**
 * Created by Roy on 2014/6/15.
 */
public class IndexCacheMap <C, K extends AbstractKey<C>, V>
        extends AbstractCacheMapDecorator<C, K, V>
{
    private IndexNode<C, K> root;
    private MatcherInterface<C> matcher;

    public IndexCacheMap(CacheMapInterface<C, K, V> i, MatcherInterface<C> t)
    {
        super(i);
        matcher = t;
        root = new IndexNode<C, K>();
    }

    public void reloadIndex()
    {
        root.clear();
        for(K k : getImpl().keySet())
        {
            root.insertKey(k.getComponents(), 0 ,k);
        }
    }

    @Override
    public Map<K, V> query(KeyQueryInterface<C, K> queryInterface)
    {
        ArrayList<K> keys = new ArrayList<K>();
        HashMap<K, V> ret = new HashMap<K, V>();
        root.getAllMatchedKeys(queryInterface.getComponents(), 0, matcher,keys);
        for(K key : keys)
        {
            if(key != null)
            {
                V value = getImpl().get(key);
                if(value != null)
                {
                    ret.put(key, value);
                }
            }
        }
        return ret;
    }


    @Override
    public V put(K k, V v)
    {
        if(!getImpl().contains(k))
        {
            root.insertKey(k.getComponents(), 0, k);
        }
        return getImpl().put(k, v);
    }

    @Override
    public V insert(K k, V v)
    {
        if(!getImpl().contains(k))
        {
            root.insertKey(k.getComponents(), 0, k);
        }
        return getImpl().insert(k, v);
    }

    @Override
    public V remove(K k)
    {
        if(!getImpl().contains(k))
        {
            root.removeKey(k.getComponents(), 0);
        }
        return getImpl().remove(k);
    }

    @Override
    public void clear()
    {
        getImpl().clear();
        root.clear();
    }
}
