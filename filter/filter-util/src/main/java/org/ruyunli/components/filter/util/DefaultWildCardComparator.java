package org.ruyunli.components.filter.util;

import org.ruyunli.components.filter.exceptions.KeyParseException;

import java.util.*;

/**
 * Created by Roy on 2014/6/10.
 */
public class DefaultWildCardComparator
        implements MatcherInterface<String>, KeyParserInterface<String, String>
{
    private static final char ALL_WILD = '*';
    private static final char SINGLE_WILD = '?';
    private static final String KEY_DELIM = ".";
    private static final String INVALID_REG = "\\.;:";

    private static void MatchWithAllWildcardWithOverlay(
            char[] s, char[] pattern, int[] overlay_value, int sBegin, int pBegin,
            int sEnd, int pEnd, List<Integer> ret
    )
    {
        if (sEnd <= sBegin)
        {
            return;
        }
        int target_length = sEnd - sBegin;
        int pattern_length = pEnd - pBegin;
        int pattern_index = 0;
        int target_index = 0;
        while (pattern_index < pattern_length && target_index < target_length)
        {
            if (s[sBegin + target_index] == pattern[pBegin + pattern_index]
                    || pattern[pBegin + pattern_index] == SINGLE_WILD)
            {
                ++target_index;
                ++pattern_index;
            }
            else if (pattern_index == 0)
            {
                ++target_index;
            }
            else
            {
                pattern_index = overlay_value[pattern_index - 1] + 1;
            }
        }
        if (pattern_index == pattern_length)
        {
            ret.add(target_index - pattern_index + sBegin);
            MatchWithAllWildcardWithOverlay(s, pattern, overlay_value,
                    target_index - pattern_index + sBegin + 1, pBegin, sEnd,
                    pEnd, ret);
        }
    }


    private static void AllPatternMatchWithAllWildcard(
            char[] s, char[] pattern, int sBegin, int pBegin,
            int pEnd, List<Integer> ret
    )
    {
        int sEnd = s.length;
        if (sBegin >= sEnd)
        {
            return;
        }
        int pattern_length = pEnd - pBegin;
        int[] overlay_value = new int[pattern_length];
        overlay_value[0] = -1;
        int index = 0;

        for (int i = 1; i < pattern_length; ++i)
        {
            index = overlay_value[i - 1];
            while (index >= 0 &&
                    pattern[pBegin + index + 1] != pattern[pBegin + 1]
                    && pattern[pBegin + index + 1] != SINGLE_WILD
                    && pattern[pBegin + 1] != SINGLE_WILD)
            {
                index = overlay_value[index];
            }

            if (pattern[pBegin + index + 1] == pattern[pBegin + 1]
                    || pattern[pBegin + index + 1] == SINGLE_WILD
                    || pattern[pBegin + 1] == SINGLE_WILD)
            {
                overlay_value[i] = index + 1;
            }
            else
            {
                overlay_value[i] = -1;
            }
        }

        MatchWithAllWildcardWithOverlay(s, pattern, overlay_value, sBegin,
                pBegin, sEnd, pEnd, ret);
    }

    // patternInd is array to record the wildcard pos
    // pIndex records the index of the current wildcard
    private static boolean matchPatternWithWildcard(
            char[] str,
            char[] pattern, List<Integer> wild_pos,
            int pIndex, int sBegin
    )
    {
        int startIndex = wild_pos.get(pIndex) + 1;

        // The last character of the pattern is wildcard
        if (startIndex == pattern.length)
        {
            return true;
        }

        int endIndex = 0;
        // if the current pIndex is not the last wildcard in the pattern
        if (pIndex != wild_pos.size() - 1)
        {
            endIndex = wild_pos.get(pIndex + 1);
        }
        else
        {
            // if it is the last one
            endIndex = pattern.length;
        }

        ArrayList<Integer> matchPos = new ArrayList<Integer>();
        AllPatternMatchWithAllWildcard(str, pattern, sBegin,
                startIndex, endIndex, matchPos);
        if (endIndex == pattern.length)
        {
            return matchPos.size() > 0;
        }

        for (Integer i : matchPos)
        {
            if (matchPatternWithWildcard(str, pattern, wild_pos,
                    pIndex + 1, i + endIndex - startIndex))
            {
                return true;
            }
        }
        return false;
    }

    private static boolean allWildCardMatch(String s, String pattern)
    {
        ArrayList<Integer> wild_pos = new ArrayList<Integer>();
        int index = -1;
        while ((index = pattern.indexOf(ALL_WILD, index + 1)) != -1)
        {
            wild_pos.add(index);
        }
        int firstAllWind = wild_pos.size() == 0 ? -1 : wild_pos.get(0);
        if (firstAllWind < 0)
        {
            firstAllWind = pattern.length();
        }

        char[] sa = s.toCharArray();
        char[] pa = pattern.toCharArray();
        for (int i = 0; i < firstAllWind; ++i)
        {
            if (pa[i] != SINGLE_WILD
                    && pa[i] != sa[i])
            {
                return false;
            }
        }
        if (firstAllWind == pa.length)
        {
            return true;
        }

        // From now on, we have *XXX*XX or XXX*XXX pattern
        return matchPatternWithWildcard(sa, pa, wild_pos, 0, firstAllWind);
    }

    private static boolean singleWildCardMatch(char[] s, char[] pattern)
    {
        if (s.length == pattern.length)
        {
            for (int i = 0; i < s.length; ++i)
            {
                if (pattern[i] != SINGLE_WILD &&
                        pattern[i] != s[i])
                {
                    return false;
                }
            }
            return true;
        }
        return false;
    }

    public static boolean containsSingleWildCard(String s)
    {
        return s.contains(String.valueOf(SINGLE_WILD));
    }

    public static boolean containsAllWildCard(String s)
    {
        return s.contains(String.valueOf(ALL_WILD));
    }


    public static boolean wildMatch(String s, String pattern)
    {
        if (pattern.equals(String.valueOf(ALL_WILD)))
        {
            return true;
        }
        else if (containsAllWildCard(pattern))
        {
            return allWildCardMatch(s, pattern);
        }
        else if (containsSingleWildCard(pattern))
        {
            return singleWildCardMatch(s.toCharArray(), pattern.toCharArray());
        }
        return s.equals(pattern);
    }


    @Override
    public boolean match(String value, String pattern)
    {
        return wildMatch(value, pattern);
    }

    private List<String> matchIterator(Iterator<String> it, String p)
    {
        ArrayList<String> ret = new ArrayList<String>();
        while(it.hasNext())
        {
            String s = it.next();
            if(match(s,p))
            {
                ret.add(s);
            }
        }
        return ret;
    }

    @Override
    public List<String> match(Set<String> values, String pattern)
    {
        if(values.contains(pattern))
        {
            ArrayList<String> ret = new ArrayList<String>();
            ret.add(pattern);
            return ret;
        }
        return matchIterator(values.iterator(), pattern);
    }

    @Override
    public Vector<String> parseKeyComponents(String s) throws KeyParseException
    {
        if(s.contains(INVALID_REG) ||
                containsAllWildCard(s) || containsSingleWildCard(s))
        {
            throw new KeyParseException(s);
        }
        return parse(s);
    }

    @Override
    public Vector<String> parseQueryComponents(String s) throws KeyParseException
    {
        if(s.contains(INVALID_REG))
        {
            throw new KeyParseException(s);
        }
        return parse(s);
    }

    private static Vector<String> parse(String s) throws KeyParseException
    {
        Vector<String> v = new Vector<String>();
        for(String com : s.split(KEY_DELIM))
        {
            if(com == null || com.isEmpty())
            {
                throw new KeyParseException(s);
            }
            v.add(com.trim());
        }
        if(v.size() == 0)
        {
            throw new KeyParseException(s);
        }
        return v;
    }
}
