package org.ruyunli.components.filter.test.filterUtil.performance;

import junit.framework.TestCase;
import org.ruyunli.components.filter.util.DefaultWildCardComparator;

/**
 * Created by Roy on 2014/6/17.
 */
public class DefaultWildCardComparatorPerformance extends TestCase
{
    String s = "test1234OKtttxnlanglad";
    String singlePattern = "tes?1234?Ktttxnl?nglad";
    String wildcardPattern = "test123*tt*an*";
    String pattern="test1234Oktttxnlanglad";
    String mixPattern="*1234*tt?x?lan*e";
    String mixPattern_true="*1234*tt?x?lan*d";

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
