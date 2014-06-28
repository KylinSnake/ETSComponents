package org.ruyunli.components.filter.index;

import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.List;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.atomic.AtomicLong;

/**
 * Created by Roy on 2014/6/12.
 */
public class IndexNode<C,K> implements IndexNodeInterface<C,K>
{
    private ConcurrentHashMap<C, IndexNodeInterface<C, K>> map;

    private AtomicLong referenceNumber;

    public IndexNode()
    {
        map = new ConcurrentHashMap<C, IndexNodeInterface<C, K>>();
        referenceNumber = new AtomicLong(0);
    }


    @Override
    public void getAllMatchedKeys(Vector<C> components, int currentLayer, MatcherInterface<C> matcher, List<K> ret)
    {
        assert(components.size() > currentLayer);
        for(C c : matcher.match(map.keySet(),components.get(currentLayer)))
        {
            IndexNodeInterface<C, K> value = map.get(c);
            if(value != null)
            {
                value.getAllMatchedKeys(components, currentLayer+1, matcher, ret);
            }
        }
    }

    @Override
    public void insertKey(Vector<C> components, int currentLayer, K key)
    {
        assert(components.size() > currentLayer);
        C component = components.get(currentLayer);

        IndexNodeInterface<C, K> newNode = null;
        if(currentLayer == components.size() - 1)
        {
            newNode = new IndexLeaf<C, K>();
        }
        else
        {
            newNode = new IndexNode<C, K>();
        }
        synchronized (map.putIfAbsent(component, newNode))
        {
            newNode = map.get(component);
            newNode.addRef();
        }

        newNode.insertKey(components, currentLayer + 1, key);
        newNode.releaseRef();
    }

    @Override
    public K removeKey(Vector<C> components, int currentLayer)
    {
        assert(components.size() > currentLayer);
        C component = components.get(currentLayer);
        IndexNodeInterface<C, K> node = map.get(component);

        if(node != null)
        {
            K ret = node.removeKey(components, currentLayer + 1);
            synchronized (map.get(component))
            {
                node = map.get(component);
                if(node.canBeRemoved())
                {
                    map.remove(component);
                }
            }
            return ret;
        }
        return null;
    }

    @Override
    public void clear()
    {
        map.clear();
    }

    @Override
    public void addRef()
    {
        referenceNumber.incrementAndGet();
    }

    @Override
    public void releaseRef()
    {
        referenceNumber.decrementAndGet();
    }

    @Override
    public boolean canBeRemoved()
    {
        assert(referenceNumber.get() >= 0);
        return referenceNumber.get() == 0 && map.size() == 0;
    }
}
