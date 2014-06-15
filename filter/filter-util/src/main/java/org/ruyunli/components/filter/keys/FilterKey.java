package org.ruyunli.components.filter.keys;

import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;
import org.ruyunli.components.filter.util.KeyParserInterface;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;
import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public class FilterKey extends AbstractKey<String> implements KeyQueryInterface<String, FilterKey>
{
    private static final String KEY_DELIM = ".";
    private static final String INVALID_REG = "[\\*\\.\\?;:]";
    protected FilterKey(Vector<String> com)
    {
        super(com);
    }

    @Override
    public Object clone()
    {
        return new FilterKey((Vector<String>)(getComponents().clone()));
    }

    public static FilterKey parseFromStringToKey(String s)
            throws KeyParseException
    {
        return parseFromStringToKey(s, null);
    }


    public static FilterKey parseFromStringToKey(String s,
                                                 KeyParserInterface<String, String> p)
            throws KeyParseException
    {
        if(p == null)
        {
           p = new DefaultWildCardComparator();
        }
        return new FilterKey(p.parseKeyComponents(s));
    }

    @Override
    public boolean match(FilterKey key)
    {
        return equals(key);
    }

    @Override
    public List<FilterKey> match(Set<FilterKey> keys)
    {
        ArrayList<FilterKey> ret = new ArrayList<FilterKey>();
        if(keys.contains(this))
        {
            ret.add(this);
        }
        return ret;
    }
}
