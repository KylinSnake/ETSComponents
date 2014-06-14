package org.ruyunli.components.filter.keys;

import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;
import org.ruyunli.components.filter.util.KeyParserInterface;
import org.ruyunli.components.filter.util.MatcherInterface;

import java.util.*;

/**
 * Created by Roy on 2014/6/11.
 */
public class DefaultStringAbstractKeyQuery<K extends AbstractKey<String>>
        implements KeyQueryInterface<String,K>
{
    private Vector<String> components;
    private MatcherInterface<String> matcher;

    protected DefaultStringAbstractKeyQuery(Vector<String> e,
                                    MatcherInterface<String> s)
    {
        components = e;
        matcher = s;
        if(matcher == null)
        {
            matcher = new DefaultWildCardComparator();
        }
    }
    @Override
    public Vector<String> getComponents()
    {
        return components;
    }

    @Override
    public boolean match(K key)
    {
        assert(matcher != null);
        if(key.getComponents().size() != components.size())
        {
            return false;
        }
        int size = components.size();
        for(int i = 0; i < size ;++i)
        {
            if(!matcher.match(key.getComponents().get(i),components.get(i)))
            {
                return false;
            }
        }
        return true;
    }

    private List<K> iterateMatch(Iterator<K> it)
    {
        ArrayList<K> ret = new ArrayList<K>();
        while(it.hasNext())
        {
            K k = it.next();
            if(match(k))
            {
                ret.add(k);
            }
        }
        return ret;
    }

    @Override
    public List<K> match(Set<K> keys)
    {
        return iterateMatch(keys.iterator());
    }

    @Override
    public List<K> match(List<K> keys)
    {
        return iterateMatch(keys.iterator());
    }

    public static DefaultStringAbstractKeyQuery parseFromString(String s)
            throws KeyParseException
    {
        return parseFromString(s, null, null);
    }

    public static DefaultStringAbstractKeyQuery parseFromString(String s,
                                                        MatcherInterface<String> e,
                                                        KeyParserInterface<String, String> p)
            throws KeyParseException
    {
        if(p == null)
        {
            p = new DefaultWildCardComparator();
        }
        return new DefaultStringAbstractKeyQuery(p.parseQueryComponents(s), e);
    }
}
