package org.ruyunli.components.filter.util;

import org.ruyunli.components.filter.exceptions.KeyParseException;

import java.util.Vector;

/**
 * Created by Roy on 2014/6/11.
 */
public interface KeyParserInterface<C,T>
{
    Vector<C> parseKeyComponents(T t) throws KeyParseException;
    Vector<C> parseQueryComponents(T t) throws KeyParseException;
}
