package org.ruyunli.components.filter.test.util;

import junit.framework.TestCase;
import org.ruyunli.components.filter.util.WildCardComparator;

/**
 * Created by Roy on 2014/6/10.
 */
public class WildCardComparatorTestCase extends TestCase
{
    String s = "test1234OKtttxnlanglad";
    String singlePattern = "tes?1234?Ktttxnl?nglad";
    String wildcardPattern = "test123*tt*an*";
    String pattern="test1234Oktttxnlanglad";
    String mixPattern="*1234*tt?x?lan*e";
    String mixPattern_true="*1234*tt?x?lan*d";

    public void testContainWildCard()
    {
        assertEquals(true, WildCardComparator.containsAllWildCard(wildcardPattern));
        assertEquals(false,WildCardComparator.containsAllWildCard(singlePattern));
        assertEquals(false,WildCardComparator.containsAllWildCard(pattern));

        assertEquals(false, WildCardComparator.containsSingleWildCard(wildcardPattern));
        assertEquals(true,WildCardComparator.containsSingleWildCard(singlePattern));
        assertEquals(false,WildCardComparator.containsSingleWildCard(pattern));

        assertEquals(true, WildCardComparator.containsAllWildCard(mixPattern));
        assertEquals(true,WildCardComparator.containsSingleWildCard(mixPattern));

        assertEquals(true,WildCardComparator.wildMatch(s, singlePattern));
        assertEquals(false,WildCardComparator.wildMatch(s, pattern));

        assertEquals(true, WildCardComparator.wildMatch(s,wildcardPattern));
        assertEquals(true, WildCardComparator.wildMatch(s,mixPattern_true));
        assertEquals(false, WildCardComparator.wildMatch(s,mixPattern));
    }
    public void testPerformance()
    {
        for(int i = 0; i < 1000000; ++i)
        {
            WildCardComparator.wildMatch(s,wildcardPattern);
            WildCardComparator.wildMatch(s,mixPattern_true);
            WildCardComparator.wildMatch(s,mixPattern);
        }
    }
}
