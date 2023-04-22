# Data Encryption Standard
It's a piece of history! Quite possibly the most heavily-scrutinised cipher so far.

My B. Tech. thesis (2018–9) was on side-channel attacks, and I thereby became acquainted with DES. At that time, my
team put together a poorly-written copy-pasted implementation and messed around with it until we got it to work on an
ATmega328P.

Several years later, with more programming experience and a better understanding of cryptographic specifications, I
decided to revisit the horrible code and rewrite it properly. And here we are.

### Download
```
git clone --recurse-submodules https://github.com/tfpf/data-encryption-standard.git
```

### Compile
```
make
```

### Test
```
./des test
```

The test vectors are from [sci.crypt](https://groups.google.com/g/sci.crypt/c/F6hVxM6RC7Q/m/kKjaRA-mCB4J).

### Encrypt
```
./des [key] [data]
```

`[key]` is the secret key; `[data]` is the data to encrypt. They must contain only hexadecimal digits. Otherwise, they
will be set to zero. If not provided, they will be generated randomly using MT19937, which I have included my
implementation of as a submodule here.

### Decrypt
```
./des [key] [data] decrypt
```
As above, except that `[key]` and `[data]` must be provided.
