package org.ruyunli.components.filter.keys;

import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.util.MatcherInterface;
import org.ruyunli.components.filter.util.WildCardComparator;

import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public class DefaultStringAbstractKeyQuery<K extends AbstractKey<String>>
        implements KeyQueryInterface<String,K>
{
    private static final String KEY_DILIM=".";
    private static final String INVALID_REG = "\\.;:";
    private Vector<String> components;
    private MatcherInterface<String> matcher;

    protected DefaultStringAbstractKeyQuery(Vector<String> e,
                                    MatcherInterface<String> s)
    {
        components = e;
        matcher = s;
        if(matcher == null)
        {
            matcher = new WildCardComparator();
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

    @Override
    public MatcherInterface<String> getMatcher()
    {
        return matcher;
    }

    @Override
    public void setMatcher(MatcherInterface<String> s)
    {
        if(s != null)
        {
            matcher = s;
        }
    }

    public static DefaultStringAbstractKeyQuery parseFromString(String s)
            throws KeyParseException
    {
        return parseFromString(s, null);
    }

    public static DefaultStringAbstractKeyQuery parseFromString(String s,
                                                        MatcherInterface<String> e) throws KeyParseException
    {
        Vector<String> v = new Vector<String>();
        for(String com : s.split(KEY_DILIM))
        {
            if(com == null || com.isEmpty() || com.matches(INVALID_REG))
            {
                throw new KeyParseException(s);
            }
            v.add(com.trim());
        }
        if(v.size() == 0)
        {
            throw new KeyParseException(s);
        }
        return new DefaultStringAbstractKeyQuery(v, e);
    }
}
