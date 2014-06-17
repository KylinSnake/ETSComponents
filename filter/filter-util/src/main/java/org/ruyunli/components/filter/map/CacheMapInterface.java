package org.ruyunli.components.filter.map;

import org.ruyunli.components.filter.exceptions.KeyParseException;
import org.ruyunli.components.filter.keys.AbstractKey;
import org.ruyunli.components.filter.keys.KeyQueryInterface;

import java.util.Map;
import java.util.Set;

/**
 * Created by Roy on 2014/6/11.
 */
public  interface CacheMapInterface<C, K extends AbstractKey<C>, V>
{
    Map<K, V> query(KeyQueryInterface<C,K> queryInterface);
    V get(K k);
    V put(K k, V v);
    V insert(K k, V v);
    V remove(K k);
    Set<K> keySet();
    boolean contains(K k);
    void clear();
    K generateKeyFromString(String s) throws KeyParseException;
    KeyQueryInterface<C, K> generateQueryFromString(String s) throws KeyParseException;
}
