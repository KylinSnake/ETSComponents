package org.ruyunli.components.filter.index;

import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.List;
import java.util.Vector;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Created by Roy on 2014/6/12.
 */
public class IndexLeaf<C,K> implements IndexNodeInterface<C,K>
{
    private ConcurrentHashMap<C, IndexNodeInterface<C, K>> map =
            new ConcurrentHashMap<C, IndexNodeInterface<C, K>>();
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

        IndexNodeInterface<C, K> value = map.get(currentLayer);

    }

    @Override
    public K removeKey(Vector<C> components, int currentLayer)
    {
        return null;
    }

    @Override
    public void clear()
    {

    }
}
