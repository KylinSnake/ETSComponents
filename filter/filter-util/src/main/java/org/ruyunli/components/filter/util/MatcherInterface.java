package org.ruyunli.components.filter.util;

/**
 * Created by Roy on 2014/6/10.
 */
public interface MatcherInterface<C>
{
    boolean match(C value, C pattern);
}
