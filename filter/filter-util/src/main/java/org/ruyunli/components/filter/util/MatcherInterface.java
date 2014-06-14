package org.ruyunli.components.filter.util;

import java.util.List;
import java.util.Set;

/**
 * Created by Roy on 2014/6/10.
 */
public interface MatcherInterface<C>
{
    boolean match(C value, C pattern);
    List<C> match(Set<C> values, C pattern);
    List<C> match(List<C> values, C pattern);
}
