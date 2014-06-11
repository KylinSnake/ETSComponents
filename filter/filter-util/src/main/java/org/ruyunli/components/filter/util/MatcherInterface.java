package org.ruyunli.components.filter.util;

/**
 * Created by Roy on 2014/6/10.
 */
public interface MatcherInterface<K,T>
{
    boolean match(K value, T pattern);
}
