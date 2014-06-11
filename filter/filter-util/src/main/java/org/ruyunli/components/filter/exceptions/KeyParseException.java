package org.ruyunli.components.filter.exceptions;

/**
 * Created by Roy on 2014/6/10.
 */
public class KeyParseException extends Exception
{
    private String invalidKey;
    public KeyParseException(String invalid)
    {
        invalidKey = invalid;
    }

    @Override
    public String getMessage()
    {
        return "[Invalid Filter Key = " + invalidKey + "] " + super.getMessage();
    }
}
