# Data Encryption Standard
It's a piece of history! Quite possibly the most heavily-scrutinised cipher so far.

My B. Tech. thesis (2018–9) was on side-channel attacks, and I thereby became acquainted with DES. At that time, my
team put together a poorly-written copy-pasted implementation and messed around with it until we got it to work on an
ATmega328P.

Several years later, I decided to revisit the horibble code and rewrite it properly. And here we are.

# Compile
```
make
```

# Test
```
./des test
```

The test vectors are from [sci.crypt](https://groups.google.com/g/sci.crypt/c/F6hVxM6RC7Q/m/kKjaRA-mCB4J).

# Run
```
./des [key] [plaintext]
```

`[key]` is the secret key; `[plaintext]` is the data to encrypt. Both must contain only hexadecimal digits. Otherwise,
they will be randomly generated using MT19937.
