package org.ruyunli.components.filter.keys;

import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.List;
import java.util.Set;
import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public interface KeyQueryInterface<C, K extends AbstractKey<C>>
{
    Vector<C> getComponents();
    boolean match(K key);
    List<K> match(Set<K> keys);
    List<K> match(List<K> keys);
}
