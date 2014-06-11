package org.ruyunli.components.filter.keys;

import org.ruyunli.components.filter.exceptions.KeyParseException;

import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public class FilterKey extends AbstractKey<String>
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

    public static FilterKey parseFromStringToKey(String s) throws KeyParseException
    {
        Vector<String> v = new Vector<String>();
        for(String com : s.split(KEY_DELIM))
        {
            if(com == null || com.trim().isEmpty() || com.matches(INVALID_REG))
            {
                throw new KeyParseException(s);
            }
            v.add(com);
        }
        if(v.size() == 0)
        {
            throw new KeyParseException(s);
        }
        return new FilterKey(v);
    }
}
