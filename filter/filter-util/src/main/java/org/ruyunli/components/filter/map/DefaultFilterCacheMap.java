package org.ruyunli.components.filter.map;

import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.keys.DefaultStringKeyQuery;
import org.ruyunli.components.filter.keys.FilterKey;
import org.ruyunli.components.filter.keys.KeyQueryInterface;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;
import org.ruyunli.components.filter.values.FilterValue;

import java.util.Map;

/**
 * Created by Roy on 2014/6/16.
 */
public class DefaultFilterCacheMap extends AbstractCacheMap<String, FilterKey, FilterValue>
{
    public DefaultFilterCacheMap(Map<FilterKey, FilterValue> m )
    {
        super(m);
    }

    public DefaultFilterCacheMap()
    {
        this(null);
    }

    @Override
    public FilterKey generateKeyFromString(String s) throws KeyParseException
    {
        return FilterKey.parseFromStringToKey(s, new DefaultWildCardComparator());
    }

    @Override
    public KeyQueryInterface<String, FilterKey> generateQueryFromString(String s) throws KeyParseException
    {
        DefaultWildCardComparator parser = new DefaultWildCardComparator();
        try
        {
            return FilterKey.parseFromStringToKey(s, parser);
        }
        catch(KeyParseException e)
        {
             return DefaultStringKeyQuery.parseFromString(s, parser, parser);
        }
    }
}
