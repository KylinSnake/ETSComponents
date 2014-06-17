package org.ruyunli.components.filter.test.filterUtil.functions;

import junit.framework.TestCase;
import org.junit.Test;
import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.exceptions.ValueParseException;
import org.ruyunli.components.filter.keys.DefaultStringKeyQuery;
import org.ruyunli.components.filter.keys.FilterKey;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;
import org.ruyunli.components.filter.values.FilterValue;

import java.util.*;

/**
 * Created by Roy on 2014/6/16.
 */
public class FilterKeyAndValueTestCase extends TestCase
{
    @Test
    public void testFilterKeyMethods()
    {
        DefaultWildCardComparator comparator = new DefaultWildCardComparator();
        try
        {
            FilterKey.parseFromStringToKey("1.x.y.*");
            assertNotNull("Exception should be thrown out for key creation: ", null);
        }
        catch (KeyParseException e)
        {
        }
        try
        {
            FilterKey key = FilterKey.parseFromStringToKey("1.0005:HK:HKD.7400ABP.pkhk");
            FilterKey key2 = (FilterKey)key.clone();
            assertEquals(true, key.equals(key2));
            assertEquals(true, key.match(key2));
            Vector<String> vec = new Vector<String>();
            vec.add("1");
            vec.add("0005:HK:HKD");
            vec.add("7400ABP");
            vec.add("pkhk");
            assertEquals(vec, key.getComponents());
            DefaultStringKeyQuery<FilterKey> query = DefaultStringKeyQuery.parseFromString("1.*.7400ABP.pk*");
            assertEquals(true,query.match(key));
            Set<FilterKey> keys = new HashSet<FilterKey>();
            keys.add(key);
            keys.add(key2);
            assertEquals(1, keys.size());
            keys.add(FilterKey.parseFromStringToKey("1.2330:TW:TWD.7400ABP.pktw"));
            assertEquals(1, key.match(keys).size());
            assertEquals(2, query.match(keys).size());
        }
        catch (KeyParseException e)
        {
            assertNotNull("Exception should not be thrown out for key creation: ", null);
        }
    }
    @Test
    public void testFilterValueMethods()
    {
        FilterValue value = new FilterValue();
        try
        {
            value.put("op=0.0;tps=100.0;oos=200;ool=300");
            value.put("op", String.valueOf(-200.0));
            HashMap<String, String> tests = new HashMap<String, String>();
            tests.put("tpl", "-300");
            tests.put("unsettled", "-1000");
            tests.put("oos", "800");
            value.putAll(tests);

            assertEquals(-200.0, Double.valueOf(value.get("op")));
            assertEquals(100.0, Double.valueOf(value.get("tps")));
            assertEquals(-300.0, Double.valueOf(value.get("tpl")));
            assertEquals(800.0, Double.valueOf(value.get("oos")));
            assertEquals(300.0, Double.valueOf(value.get("ool")));
            assertEquals(-1000.0, Double.valueOf(value.get("unsettled")));
            assertEquals(6, value.count());
            Map map = value.getAll(new String[]{"ool", "tpl"});
            assertEquals(2, map.size());
            assertEquals(-300.0, Double.valueOf(map.get("tpl").toString()));
            assertEquals(300.0, Double.valueOf(map.get("ool").toString()));
        }
        catch (ValueParseException e)
        {
            assertNotNull("put failed: " + e, null);
        }


        value.stampLastUpdate();
        assertEquals(true, value.get("lastUpdated").trim().length() > 0);


        int size = value.count();
        value.remove("tpl");
        assertEquals(null, value.get("tpl"));
        assertEquals(size - 1, value.count());
        size = size - 1;
        value.removeAll(new String[]{"ool", "oos", "nokey"});
        assertEquals(size - 2, value.count());
        assertEquals(null, value.get("ool"));
        assertEquals(null, value.get("oos"));

        try
        {
            value.put("op=;tps= ; tpl =  ");
            assertEquals(true, value.get("op").isEmpty());
            assertEquals(true, value.get("tps").isEmpty());
            assertEquals(true, value.get("tpl").isEmpty());
        }
        catch (ValueParseException e)
        {
            assertNotNull("put failed: " + e, null);
        }

        value.clear();
        assertEquals(0, value.count());
    }
}
