package org.ruyunli.components.filter.map;

import org.ruyunli.components.filter.keys.AbstractKey;

import java.util.Vector;
import java.util.concurrent.locks.ReentrantReadWriteLock;

/**
 * Created by Roy on 2014/6/15.
 */
public class ConcurrentCacheMap <C, K extends AbstractKey<C>, V>
        extends AbstractCacheMapDecorator<C, K, V>
{
    private final Vector<ReentrantReadWriteLock> locks
            = new Vector<ReentrantReadWriteLock>();
    public ConcurrentCacheMap(CacheMapInterface<C, K, V> cacheMap, long threadNum)
    {
        super(cacheMap);
        for(int i = 0 ; i < threadNum; ++i)
        {
            locks.add(new ReentrantReadWriteLock());
        }
    }

    public ReentrantReadWriteLock.ReadLock getReadLock(K k)
    {
        return locks.get(k.hashCode() % locks.size()).readLock();
    }

    public ReentrantReadWriteLock.WriteLock getWriteLock(K k)
    {
        return locks.get(k.hashCode() % locks.size()).writeLock();
    }

    @Override
    public V get(K k)
    {
        getReadLock(k).lock();
        V v =  getImpl().get(k);
        getReadLock(k).unlock();
        return v;
    }

    @Override
    public V put(K k, V v)
    {
        getWriteLock(k).lock();
        V ret =  getImpl().put(k, v);
        getWriteLock(k).unlock();
        return ret;
    }

    @Override
    public V insert(K k, V v)
    {
        getWriteLock(k).lock();
        V ret =  getImpl().insert(k, v);
        getWriteLock(k).unlock();
        return ret;
    }

    @Override
    public V remove(K k)
    {
        getWriteLock(k).lock();
        V ret =  getImpl().remove(k);
        getWriteLock(k).unlock();
        return ret;
    }

    @Override
    public boolean contains(K k)
    {
        getReadLock(k).lock();
        boolean ret = getImpl().contains(k);
        getReadLock(k).unlock();
        return ret;
    }
}
