package org.ruyunli.components.filter.index;

import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.List;
import java.util.Vector;

/**
 * Created by Roy on 2014/6/12.
 */
public interface IndexNodeInterface<C, K>
{
    public void getAllMatchedKeys(Vector<C> components, int currentLayer, MatcherInterface<C> matcher, List<K> ret);
    public void insertKey(Vector<C> components, int currentLayer, K key);
    public K removeKey(Vector<C> components, int currentLayer);
    public void clear();
    public void addRef();
    public void releaseRef();
    public boolean canBeRemoved();
}
