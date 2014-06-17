package org.ruyunli.components.filter.values;

import org.ruyunli.components.filter.exceptions.ValueParseException;

import java.text.DateFormat;
import java.text.ParseException;
import java.util.Date;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

/**
 * Created by Roy on 2014/6/10.
 */
public class FilterValue
{
    private static final String DELIM = ";";
    private static final String EQUAL = "=";
    private static final String INVALID = ":";
    private static final String LAST_UPDATED = "lastUpdated";
    private static final DateFormat FORMATTER = DateFormat.getDateTimeInstance();

    private final ConcurrentHashMap<String, String> map = new ConcurrentHashMap<String, String>();

    public FilterValue()
    {
    }

    public void stampLastUpdate()
    {
        map.put(LAST_UPDATED, FORMATTER.format(new Date()));
    }

    public Date getLastUpdated() throws ParseException
    {
        String date = get(LAST_UPDATED);
        if (date != null)
            return FORMATTER.parse(date);
        return null;
    }

    public void putAll(Map<String, String> m)
    {
        map.putAll(m);
    }

    public String put(String key, String attr)
    {
        return map.put(key, attr);
    }

    public void put(String attrString) throws ValueParseException
    {
        putAll(parseFromString(attrString));
    }

    public String get(String key)
    {
        return map.get(key);
    }

    public Map<String, String> getAll(String[] keys)
    {
        HashMap<String, String> ret = new HashMap<String, String>();
        for (String key : keys)
        {
            ret.put(key, get(key));
        }
        return ret;
    }

    public int count()
    {
        return map.size();
    }

    public void clear()
    {
         map.clear();
    }

    public String remove(String key)
    {
        return map.remove(key);
    }

    public void removeAll(String[] keys)
    {
        for (String key : keys)
        {
            map.remove(key);
        }
    }

    private static Map<String, String> parseFromString(String value) throws ValueParseException
    {
        HashMap<String, String> m = new HashMap<String, String>();
        for (String s : value.split(DELIM))
        {
            if (!s.contains(EQUAL))
            {
                throw new ValueParseException("Invalid Attribute String , Equal is not contained");
            }
            if (s.contains(INVALID))
            {
                throw new ValueParseException("Invalid Attribute String, contains invalid characters");
            }
            String[] pair = s.split("=");
            if (pair.length ==0 || pair.length > 2 || pair[0] == null || pair[0].trim().isEmpty())
            {
                throw new ValueParseException("Invalid Attribute String, No attribute key define");
            }
            if(pair.length == 2)
            {
                if (pair[1] == null)
                {
                    pair[1] = "";
                }
                m.put(pair[0].trim(), pair[1].trim());
            }
            else
            {
                m.put(pair[0].trim(), "");
            }
        }
        if (m.size() == 0)
        {
            throw new ValueParseException("Invalid Attribute String, No attribute added");
        }
        return m;
    }
}
