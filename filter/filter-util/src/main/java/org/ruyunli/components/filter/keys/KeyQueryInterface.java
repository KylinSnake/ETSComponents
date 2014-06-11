package org.ruyunli.components.filter.keys;

import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public interface KeyQueryInterface<C, K extends AbstractKey<C>>
{
    Vector<C> getComponents();
    boolean match(K key);
    MatcherInterface<C> getMatcher();
    void setMatcher(MatcherInterface<C> s);
}
