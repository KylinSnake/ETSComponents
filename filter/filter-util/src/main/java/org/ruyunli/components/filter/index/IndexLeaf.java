package org.ruyunli.components.filter.index;

import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.List;
import java.util.Vector;
import java.util.concurrent.atomic.AtomicLong;

/**
 * Created by Roy on 2014/6/15.
 */
public class IndexLeaf<C, K> implements IndexNodeInterface<C,K>
{
    private K value;

    private AtomicLong referenceNumber;

    public IndexLeaf()
    {
        value = null;
        referenceNumber = new AtomicLong(0);
    }
    @Override
    public void getAllMatchedKeys(Vector<C> components, int currentLayer, MatcherInterface<C> matcher, List<K> ret)
    {
        assert(components.size() == currentLayer);
        ret.add(value);

    }

    @Override
    public void insertKey(Vector<C> components, int currentLayer, K key)
    {
        assert(components.size() == currentLayer);
        value = key;
    }

    @Override
    public K removeKey(Vector<C> components, int currentLayer)
    {
        assert(components.size() == currentLayer);
        K ret = value;
        value = null;
        return ret;
    }

    @Override
    public void clear()
    {
        value = null;
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
        return referenceNumber.get() == 0 && value == null;
    }
}
