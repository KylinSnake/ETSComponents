package org.ruyunli.components.filter.test.filterUtil.functions;

import junit.framework.TestCase;
import org.junit.Test;
import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;

import java.util.HashSet;
import java.util.Set;
import java.util.Vector;

/**
 * Created by Roy on 2014/6/10.
 */
public class DefaultWildCardComparatorTestCase extends TestCase
{
    String s = "test1234OKtttxnlanglad";
    String singlePattern = "tes?1234?Ktttxnl?nglad";
    String wildcardPattern = "test123*tt*an*";
    String pattern="test1234Oktttxnlanglad";
    String mixPattern="*1234*tt?x?lan*e";
    String mixPattern_true="*1234*tt?x?lan*d";

    @Test
    public void testFunctions()
    {
        DefaultWildCardComparator value = new DefaultWildCardComparator();
        try
        {
            value.parseKeyComponents("1.x.y.z.*");
            assertNotNull("Exception should be thrown.", null);
        }
        catch (KeyParseException e)
        {
        }

        try
        {
            value.parseQueryComponents("1.x.y.z.;:");
            assertNotNull("Exception should be thrown.", null);
        }
        catch (KeyParseException e)
        {
        }

        try
        {
            Vector<String> query = value.parseQueryComponents("1.?.y.*.t?s*");
            Vector<String> key = value.parseKeyComponents("1.x.y.z.test");
            assertEquals(5, key.size());
            assertEquals("1", key.get(0));
            assertEquals("x", key.get(1));
            assertEquals("y", key.get(2));
            assertEquals("z", key.get(3));
            assertEquals("test", key.get(4));
            assertEquals(5, query.size());
            assertEquals("1", query.get(0));
            assertEquals("?", query.get(1));
            assertEquals("y", query.get(2));
            assertEquals("*", query.get(3));
            assertEquals("t?s*", query.get(4));
           assertEquals(true, value.match("1.x.y.z.test", "1.?.y.*.t?s*"));
        }
        catch (KeyParseException e)
        {
            assertNotNull("Exception should not be thrown." + e, null);
        }

        Set<String> s = new HashSet<String>();
        s.add("1.x.y.z.test");
        s.add("1.a.y.tttt.tussdfsfasd");
        s.add("1.a.y.tttt.tutsdfsfasd");
        assertEquals(2, value.match(s ,"1.?.y.*.t?s*").size());
    }

    @Test
    public void testContainWildCard()
    {
        assertEquals(true, DefaultWildCardComparator.containsAllWildCard(wildcardPattern));
        assertEquals(false, DefaultWildCardComparator.containsAllWildCard(singlePattern));
        assertEquals(false, DefaultWildCardComparator.containsAllWildCard(pattern));

        assertEquals(false, DefaultWildCardComparator.containsSingleWildCard(wildcardPattern));
        assertEquals(true, DefaultWildCardComparator.containsSingleWildCard(singlePattern));
        assertEquals(false, DefaultWildCardComparator.containsSingleWildCard(pattern));

        assertEquals(true, DefaultWildCardComparator.containsAllWildCard(mixPattern));
        assertEquals(true, DefaultWildCardComparator.containsSingleWildCard(mixPattern));

        assertEquals(true, DefaultWildCardComparator.wildMatch(s, singlePattern));
        assertEquals(false, DefaultWildCardComparator.wildMatch(s, pattern));

        assertEquals(true, DefaultWildCardComparator.wildMatch(s, wildcardPattern));
        assertEquals(true, DefaultWildCardComparator.wildMatch(s, mixPattern_true));
        assertEquals(false, DefaultWildCardComparator.wildMatch(s, mixPattern));
    }
}
