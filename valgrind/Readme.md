# Install valgrind

```
sudo dnf in valgrind
```

# Compile `example.c`

```
gcc -g -o example example.c
```

# Run example program with valgrind

```
valgrind -v --leak-check=full --show-reachable=yes --log-file=example.memchk ./example
```

# Additional information

This example was taken from: [Understanding memory leaks - using Valgrind (memcheck)](https://access.redhat.com/articles/17774)
