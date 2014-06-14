package org.ruyunli.components.filter.test.util;

import junit.framework.TestCase;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;

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
    public void testPerformance()
    {
        for(int i = 0; i < 1000000; ++i)
        {
            DefaultWildCardComparator.wildMatch(s, wildcardPattern);
            DefaultWildCardComparator.wildMatch(s, mixPattern_true);
            DefaultWildCardComparator.wildMatch(s, mixPattern);
        }
    }
}
