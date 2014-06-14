package org.ruyunli.components.filter.test.util;

import junit.framework.TestSuite;
import junit.textui.TestRunner;

/**
 * Created by Roy on 2014/6/10.
 */
public class UtilTestSuite extends TestSuite
{
    public static void main(String[] argv)
    {
        UtilTestSuite suite =  new UtilTestSuite();
        suite.addTest(new DefaultWildCardComparatorTestCase());

        TestRunner.run(suite);
    }
}
