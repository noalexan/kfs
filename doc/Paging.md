# Paging

## How does it works ?

### Directory Tables

A directory table is a 1024 entries array of uint32_t, it is 4096 bytes sized.

It has to be define like it:
```
__attribute__((aligned(PAGE_SIZE))) uint32_t page_directory[0x400];
```

`__attribute__` is used to align array on 4096 byte to fit with pages.

Each entries is formatted:
