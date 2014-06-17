package org.ruyunli.components.filter.test.filterUtil.functions;

import junit.framework.TestSuite;
import junit.textui.TestRunner;

/**
 * Created by Roy on 2014/6/10.
 */
public class UtilTestSuite extends TestSuite
{
    public static void main(String[] argv)
    {
        Class[] classes = new Class[] {
                FilterKeyAndValueTestCase.class,
                DefaultWildCardComparatorTestCase.class,
                DefaultCacheMapTestCase.class
        };
        for(Class s : classes)
        {
            TestRunner.run(s);
        }
    }
}
